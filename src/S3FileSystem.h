#ifndef KIO_AWS_S3_S3FILESYSTEM_H
#define KIO_AWS_S3_S3FILESYSTEM_H


#include "AWSClientAbstract.h"
#include "AWSClient.h"


using namespace std;

class S3FileSystem {
public:
    S3FileSystem();
    explicit S3FileSystem(AWSClientAbstract *client);
    QList<FSEntry> ls(const QUrl &url);
    Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error> open(const QString &bucket, const QString &path);
    long long size(const QString &bucket, const QString &path) const;


private:
    AWSClientAbstract *m_client;
};


#endif //KIO_AWS_S3_S3FILESYSTEM_H
