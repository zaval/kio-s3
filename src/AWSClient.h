#ifndef KIO_AWS_S3_AWSCLIENT_H
#define KIO_AWS_S3_AWSCLIENT_H
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <QDir>
#include <KIO/UDSEntry>


class AWSClient{

public:
    explicit AWSClient();
    [[nodiscard]] QList<KIO::UDSEntry> list(const QString &bucket, const QString& folder) const;
    [[nodiscard]] QList<KIO::UDSEntry> buckets() const;
//    [[nodiscard]] QByteArray readFile(const QString &bucket, const QString &path) const;
    Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error> openFile(const QString &bucket, const QString &path);
//    inline QByteArray readDataFromFile(Aws::IOStream& body);

private:
    std::unique_ptr<class Aws::S3::S3Client> m_client;
    const QString separator = QDir::separator();
};


#endif //KIO_AWS_S3_AWSCLIENT_H
