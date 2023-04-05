#ifndef KIO_AWS_S3_AWSCLIENT_H
#define KIO_AWS_S3_AWSCLIENT_H
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <QDir>
#include <KIO/UDSEntry>
#include "AWSClientAbstract.h"


class AWSClient: public AWSClientAbstract{

public:
    explicit AWSClient();
    [[nodiscard]] QList<FSEntry> list(const QString &bucket, const QString& folder) const override;
    [[nodiscard]] QList<FSEntry> buckets() const override;
    Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error> openFile(const QString &bucket, const QString &path) override;
    virtual long long size(const QString &bucket, const QString& folder) const override;
    void putFile(const QString &bucket, const QString &path, const QString &fname) override;

    void deleteFile(const QString &bucket, const QString &path) override;

    void copyFile(const QString &src, const QString &dstBucket, const QString &dstKey) override;

private:
    std::unique_ptr<class Aws::S3::S3Client> m_client;
    const QString separator = QDir::separator();
};


#endif //KIO_AWS_S3_AWSCLIENT_H
