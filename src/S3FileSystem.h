/*
    SPDX-FileCopyrightText: 2023 Dmytrii Zavalnyi <dzavalny@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

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
    [[nodiscard]] long long size(const QString &bucket, const QString &path) const;
    void mkdir(const QString &bucket, const QString &path);

//    static const QString cachePath;


private:
    AWSClientAbstract *m_client;
//    const QString &cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
};


#endif //KIO_AWS_S3_S3FILESYSTEM_H
