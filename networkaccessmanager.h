#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkDiskCache>
#include <QStringList>
#include "cookiejar.h"

class CookieJar;

class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    NetworkAccessManager(QObject *parent = 0, CookieJar *cookieJar = 0, QNetworkDiskCache *cache = 0);
    QNetworkReply* block(const QNetworkRequest &request, QObject *nam);
    CookieJar *cookieJar;
    QStringList adBlockRules;

protected:
    QNetworkReply* createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);
};

#endif // NETWORKACCESSMANAGER_H
