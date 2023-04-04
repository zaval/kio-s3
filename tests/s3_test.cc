#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/AWSClientAbstract.h"
#include "../src/S3FileSystem.h"
#include <QCoreApplication>

#define qCDebug(X) qDebug() << "X"
using ::testing::Return;

class MockAWSClient: public AWSClientAbstract {
public:
    MOCK_METHOD(QList<FSEntry>, list, (const QString &bucket, const QString& folder), (const, override));
    MOCK_METHOD(QList<FSEntry>, buckets, (), (const, override));
    MOCK_METHOD(long long, size, (const QString &bucket, const QString& folder), (const, override));
    MOCK_METHOD((Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error>), openFile, (const QString &bucket, const QString &path), (override));
};

TEST(kio_aws_s3, buckets) {

    MockAWSClient mock;
    QList<FSEntry> entries;
    entries
        << FSEntry{QStringLiteral("name1"), QUrl(QStringLiteral("s3://name1/")), QStringLiteral("inode/directory"), FSType::DIRECTORY}
        << FSEntry{QStringLiteral("name2"), QUrl(QStringLiteral("s3://name2/")), QStringLiteral("inode/directory"), FSType::DIRECTORY}
        << FSEntry{QStringLiteral("name3"), QUrl(QStringLiteral("s3://name3/")), QStringLiteral("inode/directory"), FSType::DIRECTORY}
        << FSEntry{QStringLiteral("name4"), QUrl(QStringLiteral("s3://name4/")), QStringLiteral("inode/directory"), FSType::DIRECTORY};
    EXPECT_CALL(mock, buckets())
        .WillOnce(Return(entries));

    S3FileSystem fs(&mock);
    const auto &buckets = fs.ls(QUrl(QStringLiteral("s3:///")));
    EXPECT_TRUE(buckets.size() == entries.size());

}

TEST(kio_aws_s3, ls){
    MockAWSClient mock;
    QList<FSEntry> entries;
    entries
            << FSEntry{QStringLiteral("file.txt"), QUrl(QStringLiteral("s3://name1/folder/file.txt")), QStringLiteral("text/plain"), FSType::REGULAR_FILE}
            << FSEntry{QStringLiteral("folder"), QUrl(QStringLiteral("s3://name1/folder/folder")), QStringLiteral("inode/directory"), FSType::DIRECTORY};
    EXPECT_CALL(mock, list(QStringLiteral("name1"), QStringLiteral("folder/")))
            .WillOnce(Return(entries));

    S3FileSystem fs(&mock);
    const auto &folders = fs.ls(QUrl(QStringLiteral("s3://name1/folder/")));
    EXPECT_TRUE(folders.size() == entries.size());
}


