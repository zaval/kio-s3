/*
    SPDX-FileCopyrightText: 2023 Dmytrii Zavalnyi <dzavalny@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef KIO_AWS_S3_H
#define KIO_AWS_S3_H

// KF
#include <KIO/WorkerBase>
// Std
#include <memory>
#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <sstream>
#include "AWSClient.h"
#include "S3FileSystem.h"

class kio_aws_s3 : public KIO::WorkerBase
{
public:
    kio_aws_s3(const QByteArray &pool_socket, const QByteArray &app_socket);
    ~kio_aws_s3() override;

public:
    KIO::WorkerResult get(const QUrl &url) override;
    KIO::WorkerResult stat(const QUrl &url) override;
    KIO::WorkerResult listDir(const QUrl &url) override;
    KIO::WorkerResult put(const QUrl &url, int permissions, KIO::JobFlags flags) override;
    KIO::WorkerResult mkdir(const QUrl &url, int permissions) override;
    KIO::WorkerResult del(const QUrl &url, bool isfile) override;

private:
    QHash<QString, KIO::UDSEntry> m_filesystem;
    S3FileSystem m_fs;

};

#endif
