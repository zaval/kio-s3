/*
    SPDX-FileCopyrightText: 2023 Dmytrii Zavalnyi <dzavalny@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "kio_aws_s3.h"

#include "kio_aws_s3_log.h"
// KF
#include <KIO/UDSEntry>
// Qt
#include <QCoreApplication>
#include <aws/s3/model/ListObjectsV2Request.h>
#include <QMimeDatabase>
#include <QMimeType>

// Pseudo plugin class to embed meta data
class KIOPluginForMetaData : public QObject
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.kio.worker.s3" FILE "s3.json")
};

extern "C" {
int Q_DECL_EXPORT kdemain(int argc, char **argv);
}

int kdemain(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("kio_aws_s3"));

    kio_aws_s3 worker(argv[2], argv[3]);
    worker.dispatchLoop();

    return 0;
}

kio_aws_s3::kio_aws_s3(const QByteArray &pool_socket, const QByteArray &app_socket)
    : KIO::WorkerBase("s3", pool_socket, app_socket)
{
    qCDebug(KIO_AWS_S3_LOG) << "kio_aws_s3 starting up";
}

kio_aws_s3::~kio_aws_s3()
{
//    ShutdownAPI(options);
    qCDebug(KIO_AWS_S3_LOG) << "kio_aws_s3 shutting down";
}


KIO::WorkerResult kio_aws_s3::get(const QUrl &url)
{
    qCDebug(KIO_AWS_S3_LOG) << "kio_aws_s3 starting get" << url;
    QString path = url.path();
    if (path.startsWith(QLatin1Char('/'))) {
        path.remove(0, 1);
    }

    const auto sz = m_fs.size(url.host(), path);
    qCDebug(KIO_AWS_S3_LOG) << "totalSize" << sz;
    totalSize(sz);

    QMimeDatabase db;
    const auto &mime = db.mimeTypeForFile(url.path());

    mimeType(mime.name());
    auto response = m_fs.open(url.host(), path);
    if (!response.IsSuccess()){
        return KIO::WorkerResult::fail();
    }


    auto body = &response.GetResult().GetBody();

    char buf[1024];
    while(!body->eof()){
        long i = body->read(buf, std::size(buf)).gcount();
//        qCDebug(KIO_AWS_S3_LOG) << "read" << i << "bytes";
        data(QByteArray(buf, static_cast<int>(i)));
    }



    // now emit the data...
//    data(m_client.readFile(url.host(), path));

    // and we are done
    return KIO::WorkerResult::pass();
}

KIO::WorkerResult kio_aws_s3::stat(const QUrl &url)
{
    qCDebug(KIO_AWS_S3_LOG) << "kio_aws_s3 starting stat" << url;

    if (m_filesystem.contains(url.toString())){
        statEntry(m_filesystem[url.toString()]);
        return KIO::WorkerResult::pass();
    }
    return KIO::WorkerResult::fail();

}

KIO::WorkerResult kio_aws_s3::listDir(const QUrl &url)
{
    qCDebug(KIO_AWS_S3_LOG) << "kio_aws_s3 starting listDir" << url;
    KIO::UDSEntry currentFolderEntry;
    currentFolderEntry.reserve(4);
    currentFolderEntry.fastInsert(KIO::UDSEntry::UDS_NAME, QStringLiteral("."));
    currentFolderEntry.fastInsert(KIO::UDSEntry::UDS_MIME_TYPE, QStringLiteral("inode/directory"));
    currentFolderEntry.fastInsert(KIO::UDSEntry::UDS_FILE_TYPE, S_IFDIR);
    currentFolderEntry.fastInsert(KIO::UDSEntry::UDS_ACCESS, S_IRUSR | S_IRGRP | S_IROTH);
    listEntry(currentFolderEntry);

    const auto &entries = m_fs.ls(url);
    for (const auto &e: entries){
        KIO::UDSEntry entry;
        entry.reserve(6);
        entry.fastInsert(KIO::UDSEntry::UDS_NAME, e.name);
        entry.fastInsert(KIO::UDSEntry::UDS_MIME_TYPE, e.mime);
        entry.fastInsert(KIO::UDSEntry::UDS_FILE_TYPE, e.type == FSType::REGULAR_FILE ? S_IFREG : S_IFDIR);
        entry.fastInsert(KIO::UDSEntry::UDS_SIZE, e.size);
        entry.fastInsert(KIO::UDSEntry::UDS_ACCESS, S_IRUSR | S_IRGRP | S_IROTH);
        entry.fastInsert(KIO::UDSEntry::UDS_MODIFICATION_TIME, e.time);
        entry.fastInsert(KIO::UDSEntry::UDS_URL, e.url.toString());

        m_filesystem.insert(entry.stringValue(KIO::UDSEntry::UDS_URL), entry);

        listEntry(entry);
    }

    return KIO::WorkerResult::pass();

}

#include "kio_aws_s3.moc"


/*
 *
import boto3
s3 = boto3.client('s3')
s3.list_buckets()
{'ResponseMetadata': {'RequestId': '20XP9CNY5F7S2SEA', 'HostId': 'Qt8HydpSWNcHMqc8bk/i2HOI8u6WBAL+k8Yu61UWGLurL+Q/E9MEA/nNiUTG9QI1UxFNMv0Lw64=', 'HTTPStatusCode': 200, 'HTTPHeaders': {'x-amz-id-2': 'Qt8HydpSWNcHMqc8bk/i2HOI8u6WBAL+k8Yu61UWGLurL+Q/E9MEA/nNiUTG9QI1UxFNMv0Lw64=', 'x-amz-request-id': '20XP9CNY5F7S2SEA', 'date': 'Thu, 23 Mar 2023 22:15:26 GMT', 'content-type': 'application/xml', 'transfer-encoding': 'chunked', 'server': 'AmazonS3'}, 'RetryAttempts': 0}, 'Buckets': [{'Name': 'assetlinkglobal-dev-raw', 'CreationDate': datetime.datetime(2020, 11, 18, 15, 3, 53, tzinfo=tzlocal())}, {'Name': 'assetlinkglobal-staging-raw', 'CreationDate': datetime.datetime(2021, 3, 11, 16, 0, 57, tzinfo=tzlocal())}, {'Name': 'atlas1-registeratlasresources-1g-lambdazipsbucket-ya9c8s1znjyy', 'CreationDate': datetime.datetime(2022, 3, 2, 16, 26, 36, tzinfo=tzlocal())}, {'Name': 'auth0-lock-branding', 'CreationDate': datetime.datetime(2020, 11, 6, 5, 46, 29, tzinfo=tzlocal())}, {'Name': 'auth0-lock-branding-with-cloudfront-cdn', 'CreationDate': datetime.datetime(2020, 11, 19, 16, 7, 32, tzinfo=tzlocal())}, {'Name': 'aws-cloudtrail-logs-335962503776-886edb44', 'CreationDate': datetime.datetime(2022, 4, 12, 14, 1, 38, tzinfo=tzlocal())}, {'Name': 'aws-logs-335962503776-us-west-2', 'CreationDate': datetime.datetime(2022, 2, 11, 16, 6, 45, tzinfo=tzlocal())}, {'Name': 'azuga-dev-raw', 'CreationDate': datetime.datetime(2020, 7, 28, 12, 37, 26, tzinfo=tzlocal())}, {'Name': 'azuga-stage-raw', 'CreationDate': datetime.datetime(2020, 7, 28, 13, 43, 39, tzinfo=tzlocal())}, {'Name': 'backup-livlet-rds-test', 'CreationDate': datetime.datetime(2022, 12, 7, 5, 46, 30, tzinfo=tzlocal())}, {'Name': 'cdktoolkit-stagingbucket-9p5bsjsae6vg', 'CreationDate': datetime.datetime(2022, 12, 7, 5, 43, 54, tzinfo=tzlocal())}, {'Name': 'cf-templates-1lmbcp9axqc2q-us-west-2', 'CreationDate': datetime.datetime(2020, 7, 23, 14, 35, 16, tzinfo=tzlocal())}, {'Name': 'config-bucket-335962503776', 'CreationDate': datetime.datetime(2022, 1, 28, 9, 38, 53, tzinfo=tzlocal())}, {'Name': 'cw-syn-results-335962503776-us-west-2', 'CreationDate': datetime.datetime(2020, 8, 23, 16, 58, 4, tzinfo=tzlocal())}, {'Name': 'data-webhook', 'CreationDate': datetime.datetime(2020, 10, 19, 11, 7, 25, tzinfo=tzlocal())}, {'Name': 'datadog-forwarderstack-1n7axj-forwarderzipsbucket-ji3dqw8du2by', 'CreationDate': datetime.datetime(2020, 4, 21, 7, 35, 47, tzinfo=tzlocal())}, {'Name': 'datapipe.logging', 'CreationDate': datetime.datetime(2020, 6, 27, 0, 4, 40, tzinfo=tzlocal())}, {'Name': 'dataretriever-staging', 'CreationDate': datetime.datetime(2021, 3, 11, 16, 1, 52, tzinfo=tzlocal())}, {'Name': 'dataretriever-test', 'CreationDate': datetime.datetime(2021, 1, 6, 15, 55, 59, tzinfo=tzlocal())}, {'Name': 'dev-apps-elb-logs', 'CreationDate': datetime.datetime(2020, 12, 10, 15, 18, 32, tzinfo=tzlocal())}, {'Name': 'dmytrii-test', 'CreationDate': datetime.datetime(2022, 8, 4, 14, 49, 57, tzinfo=tzlocal())}, {'Name': 'events-app', 'CreationDate': datetime.datetime(2021, 2, 4, 21, 52, 21, tzinfo=tzlocal())}, {'Name': 'gpstrackit-dev-raw', 'CreationDate': datetime.datetime(2021, 5, 7, 14, 22, 38, tzinfo=tzlocal())}, {'Name': 'gpstrackit-staging-raw', 'CreationDate': datetime.datetime(2021, 11, 5, 23, 58, 31, tzinfo=tzlocal())}, {'Name': 'infinit-dev-raw', 'CreationDate': datetime.datetime(2020, 12, 8, 14, 14, 48, tzinfo=tzlocal())}, {'Name': 'infinit-staging-raw', 'CreationDate': datetime.datetime(2021, 3, 11, 16, 2, 22, tzinfo=tzlocal())}, {'Name': 'intouch-dev-raw', 'CreationDate': datetime.datetime(2021, 11, 23, 13, 44, 10, tzinfo=tzlocal())}, {'Name': 'intouchgps-staging-raw', 'CreationDate': datetime.datetime(2021, 12, 16, 16, 7, 56, tzinfo=tzlocal())}, {'Name': 'mock-data1', 'CreationDate': datetime.datetime(2020, 10, 28, 11, 40, 6, tzinfo=tzlocal())}, {'Name': 'nextraq-dev-raw', 'CreationDate': datetime.datetime(2020, 7, 28, 15, 25, 6, tzinfo=tzlocal())}, {'Name': 'nextraq-staging-raw', 'CreationDate': datetime.datetime(2021, 3, 11, 16, 4, 32, tzinfo=tzlocal())}, {'Name': 'protective-analytics-data-processing-pipeline', 'CreationDate': datetime.datetime(2021, 5, 7, 8, 15, 23, tzinfo=tzlocal())}, {'Name': 'rdz-static-content', 'CreationDate': datetime.datetime(2022, 11, 22, 16, 48, 48, tzinfo=tzlocal())}, {'Name': 'roadz-airflow-test', 'CreationDate': datetime.datetime(2022, 8, 23, 12, 35, 39, tzinfo=tzlocal())}, {'Name': 'roadz-druid-test', 'CreationDate': datetime.datetime(2022, 3, 18, 22, 22, 37, tzinfo=tzlocal())}, {'Name': 'rum-hosting', 'CreationDate': datetime.datetime(2021, 2, 23, 23, 57, 39, tzinfo=tzlocal())}, {'Name': 'solution-assets-glacier', 'CreationDate': datetime.datetime(2020, 6, 30, 15, 33, tzinfo=tzlocal())}, {'Name': 'solution-uploads', 'CreationDate': datetime.datetime(2020, 10, 1, 11, 13, 26, tzinfo=tzlocal())}, {'Name': 'spark-cluster-poc', 'CreationDate': datetime.datetime(2022, 2, 10, 19, 59, 30, tzinfo=tzlocal())}, {'Name': 'swagger-bucket-azuga-stage', 'CreationDate': datetime.datetime(2022, 4, 11, 18, 1, 29, tzinfo=tzlocal())}, {'Name': 'temp-ziv', 'CreationDate': datetime.datetime(2020, 11, 28, 13, 50, 31, tzinfo=tzlocal())}, {'Name': 'template.sources', 'CreationDate': datetime.datetime(2020, 6, 28, 1, 53, 51, tzinfo=tzlocal())}], 'Owner': {'DisplayName': 'ziv.rosen', 'ID': '1f9f0b4330f4bc72c4acaa5bc7c1b6423863c7c2ee3281445062fdec3723b241'}}
a = s3.list_buckets()
 b = s3.list_objects_v2(Bucket=a['Buckets'][0]['Name'], Prefix='/')
b = s3.list_objects_v2(Bucket=a['Buckets'][0]['Name'], Prefix='')
b = s3.list_objects_v2(Bucket=a['Buckets'][0]['Name'], Prefix='^/')
b = s3.list_objects_v2(Bucket=a['Buckets'][0]['Name'], Prefix='', Delimiter='/')
b = s3.list_objects_v2(Bucket=a['Buckets'][0]['Name'], Prefix='/dce66825-74a1-411b-831b-260395c2cfa1/', Delimiter='/')
b = s3.list_objects_v2(Bucket=a['Buckets'][0]['Name'], Prefix='/dce66825-74a1-411b-831b-260395c2cfa1', Delimiter='/')
b = s3.list_objects_v2(Bucket=a['Buckets'][0]['Name'], Prefix='dce66825-74a1-411b-831b-260395c2cfa1', Delimiter='/')
b = s3.list_objects_v2(Bucket=a['Buckets'][0]['Name'], Prefix='dce66825-74a1-411b-831b-260395c2cfa1/', Delimiter='/')
b = s3.list_objects_v2(Bucket=a['Buckets'][0]['Name'], Prefix='dce66825-74a1-411b-831b-260395c2cfa1/cache/', Delimiter='/')
 */