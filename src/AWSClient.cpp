#include <aws/s3/model/ListObjectsV2Request.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include "AWSClient.h"
#include <QDebug>
#include <QDataStream>
#include <QMimeDatabase>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <fstream>

AWSClient::AWSClient() {

    Aws::SDKOptions options;
    options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    InitAPI(options);
    m_client = std::make_unique<Aws::S3::S3Client>();
}

QList<FSEntry> AWSClient::list(const QString &bucket, const QString& folder = QLatin1String()) const {
    const auto &normalizedFolder = (folder.endsWith(separator) || folder.isEmpty()) ? folder : folder + separator;

    qDebug() << "ListObjectsV2 | Bucket=" << bucket << "Prefix=" << normalizedFolder << "Separator=" << separator;
    Aws::S3::Model::ListObjectsV2Request request;
    QList<FSEntry> result;
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
            FSEntry entry{
                    QString::fromStdString(c.GetKey()).split(separator, Qt::SkipEmptyParts).last(),
                    QUrl(QStringLiteral("s3://%1/%2").arg(bucket, QString::fromStdString(c.GetKey()))),
                    db.mimeTypeForFile(QString::fromStdString(c.GetKey())).name(),
                    FSType::REGULAR_FILE,
                    c.GetSize(),
                    c.GetLastModified().Seconds()
            };
            result << entry;
        }

        for (auto&& c: response.GetResult().GetCommonPrefixes()){
            qDebug() << "ListObjectsV2 | CommonPrefix" << QString::fromStdString(c.GetPrefix());
            qDebug() << "ListObjectsV2 | UDS_URL" << QStringLiteral("s3://%1/%2").arg(bucket, QString::fromStdString(c.GetPrefix()));
            FSEntry entry{
                    QString::fromStdString(c.GetPrefix()).split(separator, Qt::SkipEmptyParts).last(),
                    QUrl(QStringLiteral("s3://%1/%2").arg(bucket, QString::fromStdString(c.GetPrefix()))),
                    QStringLiteral("inode/directory"),
                    FSType::DIRECTORY
            };
            result << entry;
        }

        request.SetContinuationToken(response.GetResult().GetNextContinuationToken());
    } while (response.GetResult().GetNextContinuationToken().length());
    return result;
}

QList<FSEntry> AWSClient::buckets() const {
    QList<FSEntry> result;
    const auto &buckets = m_client->ListBuckets();
    if (buckets.IsSuccess()){
        for (auto &&b: buckets.GetResult().GetBuckets()){
            FSEntry entry{
                    QString::fromStdString(b.GetName()),
                    QUrl(QStringLiteral("s3://%1/").arg(QString::fromStdString(b.GetName()))),
                    QStringLiteral("inode/directory"),
                    FSType::DIRECTORY
            };
            result << entry;
        }
    }
    return result;
}

Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error> AWSClient::openFile(const QString &bucket, const QString &path) {
    Aws::S3::Model::GetObjectRequest request;
    request.WithBucket(bucket.toStdString()).WithKey(path.toStdString());
    auto response = m_client->GetObject(request);
    return response;

}

long long AWSClient::size(const QString &bucket, const QString &folder) const {
    Aws::S3::Model::ListObjectsV2Request request;
    request
        .WithBucket(bucket.toStdString())
        .WithPrefix(folder.toStdString())
        .WithDelimiter(separator.toStdString());
    auto response = m_client->ListObjectsV2(request);
    if (!response.IsSuccess()){
        return 0;
    }
    return response.GetResult().GetContents()[0].GetSize();
}

void AWSClient::putFile(const QString &bucket, const QString &path, const QString &fname) {
    Aws::S3::Model::PutObjectRequest request;
    request
        .WithBucket(bucket.toStdString())
        .WithKey(path.toStdString());

    const auto f_ptr = std::make_shared<Aws::IFStream>(fname.toStdString());

    request.SetBody((const shared_ptr<Aws::IOStream> &) f_ptr);
    auto response = m_client->PutObject(request);
    if (!response.IsSuccess()){
        qDebug() << QString::fromStdString(response.GetError().GetMessage());
    }
    f_ptr->close();
}

void AWSClient::deleteFile(const QString &bucket, const QString &path) {
    Aws::S3::Model::DeleteObjectRequest request;
    request
        .WithBucket(bucket.toStdString())
        .WithKey(path.toStdString());

    m_client->DeleteObject(request);
}
