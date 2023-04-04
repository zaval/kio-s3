//
// Created by zaval on 04.04.23.
//

#include "S3FileSystem.h"

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
    return m_client->list(url.host(), path);
}

Aws::Utils::Outcome<Aws::S3::Model::GetObjectResult, Aws::S3::S3Error> S3FileSystem::open(const QString &bucket, const QString &path){
    auto resp = m_client->openFile(bucket, path);
    return resp;
}

long long S3FileSystem::size(const QString &bucket, const QString &path) const {
    return m_client->size(bucket, path);
}
