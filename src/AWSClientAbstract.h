//
// Created by zaval on 03.04.23.
//

#ifndef KIO_AWS_S3_AWSCLIENTABSTRACT_H
#define KIO_AWS_S3_AWSCLIENTABSTRACT_H


#include <QString>
#include <KIO/UDSEntry>
#include <aws/s3/model/GetObjectResult.h>
#include <aws/s3/S3Errors.h>
#include <QUrl>

using namespace std;

enum FSType {
    REGULAR_FILE,
    DIRECTORY
};

class FSEntry{
public:
    QString name;
    QUrl url;
    QString mime;
    FSType type;
    long long size = 0;
    int64_t time = 0;
};


class AWSClientAbstract {
public:
    virtual ~AWSClientAbstract(){}
    virtual QList<FSEntry> list(const QString &bucket, const QString& folder) const = 0;
    virtual long long size(const QString &bucket, const QString& folder) const = 0;
    virtual QList<FSEntry> buckets() const = 0;
    virtual Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error> openFile(const QString &bucket, const QString &path) = 0;
};


#endif //KIO_AWS_S3_AWSCLIENTABSTRACT_H
