#include <QObjectUserData>
#include "networkaccessmanager.h"
#include "adblocksubscription.h"
#include "browser.h"
#include "qdebug.h"
#include "cookiejar.h"
#include "cookies.h"
#include "emptynetworkreply.h"
#include <QSslConfiguration>

NetworkAccessManager::NetworkAccessManager(QObject *parent, CookieJar *cookieJar, QNetworkDiskCache *cache)
    : QNetworkAccessManager(parent)
{
    setCookieJar(cookieJar);

    QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();;
    sslConf.setProtocol(QSsl::AnyProtocol);
    QSslConfiguration::setDefaultConfiguration(sslConf);

    setCache(cache);
}

QNetworkReply* NetworkAccessManager::block(const QNetworkRequest &req, QObject *nam)
{
    const QNetworkRequest request = req;
    QUrl url = request.url();

    QNetworkReply *reply = 0;

    if (url.scheme() == QLatin1String("data"))
        return reply;

    QString urlString = QString::fromUtf8(url.toEncoded());
    bool blockedRule = false;

    AdBlockSubscription *subscription = browser::subscrip();

    if (subscription->block(urlString)) {
        blockedRule = true;
    }

    if (blockedRule)
        reply = new EmptyNetworkReply(nam, request);

    return reply;
}

QNetworkReply* NetworkAccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
{
    QNetworkReply *reply = 0;
    QNetworkRequest req = request;
    req.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    if (op == QNetworkAccessManager::GetOperation && *browser::adBlock2Enabled())
            reply = block(req, this);

    if (!reply)
        reply = QNetworkAccessManager::createRequest(op, req, outgoingData);

    return reply;
}
