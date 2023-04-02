#include <QObject>
#include <QTest>
#include "../src/AWSClient.h"
#include <QDebug>

class S3Test: public QObject {
    Q_OBJECT

private Q_SLOTS:

    void testUDSEntry(){
        const auto &client = AWSClient();
        QHash<QString, KIO::UDSEntry> entries;

        const auto &root = client.buckets();
        for (const auto &entry: root){
            entries.insert(entry.stringValue(KIO::UDSEntry::UDS_URL), entry);
        }

        const auto &url = QUrl(QStringLiteral("s3://assetlinkglobal-dev-raw/dce66825-74a1-411b-831b-260395c2cfa1/raw_data/realtime_moments/2020/11/19/"));
        QString path = url.path();
        if (path.startsWith(QLatin1Char('/'))) {
            path.remove(0, 1);
        }

        const auto &files = client.list(url.host(), path);
        for (const auto &f: files){
            entries.insert(f.stringValue(KIO::UDSEntry::UDS_URL), f);
        }

        for (const auto &k: entries){
            qDebug() << k.stringValue(KIO::UDSEntry::UDS_NAME);
            qDebug() << k.stringValue(KIO::UDSEntry::UDS_URL);
        }

    }
};

QTEST_MAIN(S3Test)
#include "s3_test.moc"
