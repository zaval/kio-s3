#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/AWSClientAbstract.h"
#include "../src/S3FileSystem.h"
#include <QCoreApplication>
#include <QStandardPaths>

#define qCDebug(X) qDebug() << "X"
using ::testing::Return;

class MockAWSClient: public AWSClientAbstract {
public:
    MOCK_METHOD(QList<FSEntry>, list, (const QString &bucket, const QString& folder), (const, override));
    MOCK_METHOD(QList<FSEntry>, buckets, (), (const, override));
    MOCK_METHOD(long long, size, (const QString &bucket, const QString& folder), (const, override));
    MOCK_METHOD((Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error>), openFile, (const QString &bucket, const QString &path), (override));
    MOCK_METHOD(void, putFile, (const QString &bucket, const QString &path, const QString &fname), (override));
    MOCK_METHOD(void, deleteFile, (const QString &bucket, const QString &path), (override));
    MOCK_METHOD(void, copyFile, (const QString &src, const QString &dstBucket, const QString &dstKey), (override));
};

TEST(kio_aws_s3, buckets) {
    QStandardPaths::setTestModeEnabled(true);
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
    QStandardPaths::setTestModeEnabled(true);
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


TEST(kio_aws_s3, open) {
    QStandardPaths::setTestModeEnabled(true);
    MockAWSClient mock;
    Aws::S3::Model::GetObjectResult result;
    auto *ss = new std::istringstream("hello");
    result.ReplaceBody(reinterpret_cast<Aws::IOStream *>(ss));
    Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error> outcome(std::move(result));

    EXPECT_CALL(mock, openFile(QStringLiteral("name1"), QStringLiteral("folder/file.txt")))
        .WillOnce(Return(std::move(outcome)));
    S3FileSystem fs(&mock);
    auto res = fs.open(QUrl(QStringLiteral("s3://name1/folder/file.txt")));
    char data[6];
    res.GetResult().GetBody().read(data, 10);

    EXPECT_TRUE(std::strcmp(data, "hello") == 0);

}

TEST(kio_aws_s3, size) {
    QStandardPaths::setTestModeEnabled(true);
    MockAWSClient mock;
    EXPECT_CALL(mock, size(QStringLiteral("name1"), QStringLiteral("folder/file.txt")))
            .WillOnce(Return(10));
    S3FileSystem fs(&mock);
    auto res = fs.size(QUrl(QStringLiteral("s3://name1/folder/file.txt")));
    EXPECT_EQ(res, 10);

}

TEST(kio_aws_s3, mkdir) {
    QStandardPaths::setTestModeEnabled(true);
    MockAWSClient mock;
    S3FileSystem fs(&mock);
    const QUrl &url = QUrl(QStringLiteral("s3://name1/folder"));
    fs.mkdir(url);

    QDir d(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    EXPECT_TRUE(d.exists(url.host() + url.path()));
    d.cd(url.host() + url.path());
    d.removeRecursively();

}

TEST(kio_aws_s3, copy){
    QStandardPaths::setTestModeEnabled(true);
    MockAWSClient mock;
    EXPECT_CALL(mock, copyFile(QStringLiteral("bucket1/folder/file1.txt"), QStringLiteral("bucket2"), QStringLiteral("folder2/file2.txt")))
            .WillOnce(Return());

    S3FileSystem fs(&mock);
    fs.copy(QUrl(QStringLiteral("s3://bucket1/folder/file1.txt")), QUrl(QStringLiteral("s3://bucket2/folder2/file2.txt")));
}