/*
    SPDX-FileCopyrightText: 2023 Dmytrii Zavalnyi <dzavalny@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <QStandardPaths>
#include "S3FileSystem.h"

//const QString S3FileSystem::cachePath = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);

S3FileSystem::S3FileSystem() : m_client(new AWSClient()){

};

S3FileSystem::S3FileSystem(AWSClientAbstract *client): m_client(client) {

}

QList<FSEntry> S3FileSystem::ls(const QUrl &url) {
    QString path = url.path();
    if (path.startsWith(QLatin1Char('/'))) {
        path.remove(0, 1);
    }
    if (url.host().isEmpty()){
        return m_client->buckets();
    }

    QList<FSEntry> entities;

    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (cacheDir.exists(url.host())){
        cacheDir.cd(url.host());
        cacheDir.cd(path);
        for (const auto &entry: cacheDir.entryList()){
            FSEntry fsEntry{
                entry,
                QUrl(QStringLiteral("%1%2/").arg(url.toString(), entry)),
                QStringLiteral("inode/directory"),
                FSType::DIRECTORY
            };
            entities << fsEntry;
        }
    }
    entities += m_client->list(url.host(), path);
    return entities;
}

Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error> S3FileSystem::open(const QString &bucket, const QString &path){
    auto resp = m_client->openFile(bucket, path);
    return resp;
}

long long S3FileSystem::size(const QString &bucket, const QString &path) const {
    return m_client->size(bucket, path);
}

void S3FileSystem::mkdir(const QString &bucket, const QString &path) {
    QDir pathDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (!pathDir.exists(bucket)){
        pathDir.mkdir(bucket);
    }
    pathDir.cd(bucket);
    pathDir.mkpath(path);
}
