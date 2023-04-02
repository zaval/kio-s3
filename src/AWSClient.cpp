#include <aws/s3/model/ListObjectsV2Request.h>
#include <aws/s3/model/GetObjectRequest.h>
#include "AWSClient.h"
#include <QDebug>
#include <QBuffer>
#include <QDataStream>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <fstream>

AWSClient::AWSClient() {

    Aws::SDKOptions options;
    InitAPI(options);
    m_client = std::make_unique<Aws::S3::S3Client>();
}

QList<KIO::UDSEntry> AWSClient::list(const QString &bucket, const QString& folder = QLatin1String()) const {
    const auto &normalizedFolder = (folder.endsWith(separator) || folder.isEmpty()) ? folder : folder + separator;

    qDebug() << "ListObjectsV2 | Bucket=" << bucket << "Prefix=" << normalizedFolder << "Separator=" << separator;
    Aws::S3::Model::ListObjectsV2Request request;
    QList<KIO::UDSEntry> result;
    request
        .WithBucket(bucket.toStdString())
        .WithPrefix(normalizedFolder.toStdString())
        .WithDelimiter(separator.toStdString());

    QMimeDatabase db;
    Aws::S3::Model::ListObjectsV2Outcome response;
    do {
        response = m_client->ListObjectsV2(request);
        if (!response.IsSuccess()){
            break;
        }

        for (auto &&c: response.GetResult().GetContents()){
            qDebug() << "ListObjectsV2 | Content" << QString::fromStdString(c.GetKey());
            KIO::UDSEntry entry;
            entry.reserve(6);
            entry.fastInsert(KIO::UDSEntry::UDS_NAME, QString::fromStdString(c.GetKey()).split(separator, Qt::SkipEmptyParts).last());
//            entry.fastInsert(KIO::UDSEntry::UDS_MIME_TYPE, QStringLiteral("text/plain"));
            entry.fastInsert(KIO::UDSEntry::UDS_MIME_TYPE, db.mimeTypeForFile(QString::fromStdString(c.GetKey())).name());
            entry.fastInsert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFREG);
            entry.fastInsert(KIO::UDSEntry::UDS_SIZE, c.GetSize());
            entry.fastInsert(KIO::UDSEntry::UDS_ACCESS, S_IRUSR | S_IRGRP | S_IROTH);
            entry.fastInsert(KIO::UDSEntry::UDS_MODIFICATION_TIME, c.GetLastModified().Seconds());
            entry.fastInsert(KIO::UDSEntry::UDS_URL, QStringLiteral("s3://%1/%2").arg(bucket, QString::fromStdString(c.GetKey())));
            result << entry;
        }

        for (auto&& c: response.GetResult().GetCommonPrefixes()){
            qDebug() << "ListObjectsV2 | CommonPrefix" << QString::fromStdString(c.GetPrefix());
            qDebug() << "ListObjectsV2 | UDS_URL" << QStringLiteral("s3://%1/%2").arg(bucket, QString::fromStdString(c.GetPrefix()));
            KIO::UDSEntry entry;
            entry.reserve(5);
            entry.fastInsert(KIO::UDSEntry::UDS_NAME, QString::fromStdString(c.GetPrefix()).split(separator, Qt::SkipEmptyParts).last());
            entry.fastInsert(KIO::UDSEntry::UDS_MIME_TYPE, QStringLiteral("inode/directory"));
            entry.fastInsert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR);
            entry.fastInsert(KIO::UDSEntry::UDS_ACCESS, S_IRUSR | S_IRGRP | S_IROTH);
            entry.fastInsert(KIO::UDSEntry::UDS_URL, QStringLiteral("s3://%1/%2").arg(bucket, QString::fromStdString(c.GetPrefix())));
            result << entry;
        }

        request.SetContinuationToken(response.GetResult().GetNextContinuationToken());
    } while (response.GetResult().GetNextContinuationToken().length());
    return result;
}

QList<KIO::UDSEntry> AWSClient::buckets() const {
    QList<KIO::UDSEntry> result;
    const auto &buckets = m_client->ListBuckets();
    if (buckets.IsSuccess()){
        for (auto &&b: buckets.GetResult().GetBuckets()){
            KIO::UDSEntry entry;
            entry.reserve(5);
            entry.fastInsert(KIO::UDSEntry::UDS_NAME, QString::fromStdString(b.GetName()));
            entry.fastInsert(KIO::UDSEntry::UDS_MIME_TYPE, QStringLiteral("inode/directory"));
            entry.fastInsert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR);
            entry.fastInsert(KIO::UDSEntry::UDS_ACCESS, S_IRUSR | S_IRGRP | S_IROTH);
            entry.fastInsert(KIO::UDSEntry::UDS_URL, QStringLiteral("s3://%1/").arg(QString::fromStdString(b.GetName())));
            result << entry;
        }
    }
    return result;
}

//QByteArray AWSClient::readFile(const QString &bucket, const QString &path) const {
//
//    Aws::S3::Model::GetObjectRequest request;
//    request.WithBucket(bucket.toStdString()).WithKey(path.toStdString());
//    const auto &response = m_client->GetObject(request);
//    if (!response.IsSuccess()){
//        return {};
//    }
//
//    QByteArray res;
//
//    auto body = &response.GetResult().GetBody();
//
//    char buf[256];
//    while(!body->eof()){
//        long i = body->read(buf, std::size(buf)).gcount();
//        qDebug() << "read" << i << " bytes";
//        res.append(buf, static_cast<int>(i));
//    }
//
//    return res;
//
//}

Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error> AWSClient::openFile(const QString &bucket, const QString &path) {
    Aws::S3::Model::GetObjectRequest request;
    request.WithBucket(bucket.toStdString()).WithKey(path.toStdString());
    auto response = m_client->GetObject(request);
    return response;
//    if (!response.IsSuccess()){
//        return nullptr;
//    }
//    return std::ref(response.GetResult().GetBody())  ;

}

//QByteArray AWSClient::readDataFromFile(Aws::IOStream &body) {
//    char buf[256];
//    QByteArray res;
//}
