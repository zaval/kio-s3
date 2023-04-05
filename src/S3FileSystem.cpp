/*
    SPDX-FileCopyrightText: 2023 Dmytrii Zavalnyi <dzavalny@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include <QStandardPaths>
#include "S3FileSystem.h"
#include <QDebug>

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
    QSet<QUrl> entityUrls;
    for (const auto &elem : m_client->list(url.host(), path)){
        entities << elem;
        entityUrls.insert(elem.url);
    }
    QDir cacheDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (cacheDir.exists(url.host())){
        cacheDir.cd(url.host());
        if (cacheDir.exists(path)){
            cacheDir.cd(path);
            for (const auto &entry: cacheDir.entryList(QDir::NoDotAndDotDot)){

                const auto &u = QUrl(QStringLiteral("%1%2/").arg(url.toString(), entry));

                if (entityUrls.contains(u)){
                    QDir(QStringLiteral("%1/%2").arg(QStandardPaths::writableLocation(QStandardPaths::CacheLocation), entry)).removeRecursively();
                    continue;
                }

                FSEntry fsEntry{
                        entry,
                        u,
                        QStringLiteral("inode/directory"),
                        FSType::DIRECTORY
                };
                entities << fsEntry;
            }
        }

    }

    return entities;
}

Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error> S3FileSystem::open(const QUrl &url){
    const auto &path = normalizePath(url);
    auto resp = m_client->openFile(url.host(), path);
    return resp;
}

long long S3FileSystem::size(const QUrl &url) const {
    const auto &path = normalizePath(url);
    return m_client->size(url.host(), path);
}

void S3FileSystem::mkdir(const QUrl &url) {
    const auto &path = normalizePath(url);
    QDir pathDir(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (!pathDir.exists(url.host())){
        pathDir.mkdir(url.host());
    }
    pathDir.cd(url.host());
    pathDir.mkpath(path);
}

void S3FileSystem::put(const QUrl &url, const QString &fname) {
    const auto &path = normalizePath(url);
    m_client->putFile(url.host(), path, fname);
}

void S3FileSystem::del(const QUrl &url) {
    const auto &path = normalizePath(url);
    m_client->deleteFile(url.host(), path);
}

QString S3FileSystem::normalizePath(const QUrl &url) const {
    QString path = url.path();
    if (path.startsWith(QLatin1Char('/'))) {
        path.remove(0, 1);
    }
    return path;
}

void S3FileSystem::copy(const QUrl &src, const QUrl &dst) {
    const auto &srcPath = normalizePath(src);
    const auto &dstPath = normalizePath(dst);

    m_client->copyFile(
            QStringLiteral("%1/%2").arg(src.host(), srcPath),
            dst.host(),
            dstPath
            );
}
