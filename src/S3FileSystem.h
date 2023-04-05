/*
    SPDX-FileCopyrightText: 2023 Dmytrii Zavalnyi <dzavalny@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KIO_AWS_S3_S3FILESYSTEM_H
#define KIO_AWS_S3_S3FILESYSTEM_H


#include "AWSClientAbstract.h"
#include "AWSClient.h"
#include <QSet>


using namespace std;

class S3FileSystem {
public:
    S3FileSystem();
    explicit S3FileSystem(AWSClientAbstract *client);
    QList<FSEntry> ls(const QUrl &url);
    Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error> open(const QUrl &url);
    [[nodiscard]] long long size(const QUrl &url) const;
    void mkdir(const QUrl &url);
    void put(const QUrl &url, const QString &fname);
    void del(const QUrl &url);

//    static const QString cachePath;


private:
    AWSClientAbstract *m_client;
    [[nodiscard]] QString normalizePath(const QUrl &url) const;
//    const QString &cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
};


#endif //KIO_AWS_S3_S3FILESYSTEM_H
