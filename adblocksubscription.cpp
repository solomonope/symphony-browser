/**
 * Copyright (c) 2009, Benjamin C. Meyer <ben@meyerhome.net>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Benjamin Meyer nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "adblocksubscription.h"

#include "browser.h"

#include "networkaccessmanager.h"

#include <qcryptographichash.h>
#include <qdebug.h>
#include <qfile.h>
#include <qnetworkreply.h>
#include <qtextstream.h>
#include <boost/bind.hpp>
#include <QSslConfiguration>

AdBlockSubscription::AdBlockSubscription(QObject *parent, const QUrl &url)
    : QObject(parent)
    , m_url(url.toEncoded())
    , m_enabled(false)
    , m_downloading(0)
{
    parseUrl(url);
    updating = false;
    updateWhitelist();
}

AdBlockSubscription::~AdBlockSubscription()
{
    //delete whitelist;
}

void AdBlockSubscription::load()
{
    QString fileName = "data/adblock.dat";
    QFile tmpFile(fileName);
    if (tmpFile.exists())
        loadRules();
    update();
}

void AdBlockSubscription::update()
{
    updating = true;
    QNetworkAccessManager *accessMgr = new QNetworkAccessManager(this);
    QSslConfiguration sslConf = QSslConfiguration::defaultConfiguration();;
    sslConf.setProtocol(QSsl::AnyProtocol);
    QSslConfiguration::setDefaultConfiguration(sslConf);
    connect(accessMgr, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), SLOT(handleSslErrors(QNetworkReply*,QList<QSslError>)));
    accessMgr->get(QNetworkRequest(m_uri));
    connect(accessMgr, SIGNAL(finished(QNetworkReply*)), SLOT(continueParseUrl(QNetworkReply*)));
}

void AdBlockSubscription::updateWhitelist()
{
    QSettings settings("data/whitelist.dat", QSettings::IniFormat);
    if (settings.contains("adblock"))
        whitelist = settings.value("adblock").toStringList();
}

void AdBlockSubscription::setEnabled(bool enabled)
{
    if (m_enabled == enabled)
        return;
    m_enabled = enabled;
    emit changed();
}

void AdBlockSubscription::setTitle(const QString &title)
{
    if (m_title == title)
        return;
    m_title = title;
    emit changed();
}

void AdBlockSubscription::setLocation(const QUrl &url)
{
    if (url == location())
        return;
    m_location = url.toEncoded();
    m_lastUpdate = QDateTime();
    emit changed();
}

bool AdBlockSubscription::block(const QString &urlString) const
{
    if (whitelist.contains(QUrl(urlString).host()))
        return false;
    if (allRules().count() < 1)
        return false;

    foreach(QRegExp rule, allRules()) {
        bool matched = urlString.contains(rule);
        if (matched)
            return true;
    }
    return false;
}

bool AdBlockSubscription::getMatch(QString encodedUrl, QRegExp m_regExp) const
{
    return encodedUrl.contains(m_regExp);
}

QString AdBlockSubscription::convertPatternToRegExp(const QString &wildcardPattern) {
    QString pattern = wildcardPattern;
    return pattern.replace(QRegExp(QLatin1String("\\*+")), QLatin1String("*"))   // remove multiple wildcards
        .replace(QRegExp(QLatin1String("\\^\\|$")), QLatin1String("^"))        // remove anchors following separator placeholder
        .replace(QRegExp(QLatin1String("^(\\*)")), QLatin1String(""))          // remove leading wildcards
        .replace(QRegExp(QLatin1String("(\\*)$")), QLatin1String(""))          // remove trailing wildcards
        .replace(QRegExp(QLatin1String("(\\W)")), QLatin1String("\\\\1"))      // escape special symbols
        .replace(QRegExp(QLatin1String("^\\\\\\|\\\\\\|")),
                 QLatin1String("^[\\w\\-]+:\\/+(?!\\/)(?:[^\\/]+\\.)?"))       // process extended anchor at expression start
        .replace(QRegExp(QLatin1String("\\\\\\^")),
                 QLatin1String("(?:[^\\w\\d\\-.%]|$)"))                        // process separator placeholders
        .replace(QRegExp(QLatin1String("^\\\\\\|")), QLatin1String("^"))       // process anchor at expression start
        .replace(QRegExp(QLatin1String("\\\\\\|$")), QLatin1String("$"))       // process anchor at expression end
        .replace(QRegExp(QLatin1String("\\\\\\*")), QLatin1String(".*"))       // replace wildcards by .*
        ;
}

QString AdBlockSubscription::title() const
{
    return m_title;
}

QUrl AdBlockSubscription::url() const
{
    QUrl url;
    return url;
}

QUrl AdBlockSubscription::location() const
{
    return QUrl::fromEncoded(m_location);
}

bool AdBlockSubscription::isEnabled() const
{
    return m_enabled;
}

QDateTime AdBlockSubscription::lastUpdate() const
{
    return m_lastUpdate;
}

QList<QRegExp> AdBlockSubscription::allRules() const
{
    return *m_rules;
}

QRegExp AdBlockSubscription::handleRule(QString filter)
{
     if (filter.startsWith(QLatin1String("!")) || (filter.contains(QLatin1String("##"))))
     {
         return QRegExp();
     }
     if (filter.startsWith(QLatin1Char('/'))) {
         if (filter.endsWith(QLatin1Char('/'))) {
             filter = filter.mid(1);
             filter = filter.left(filter.size() - 1);
         }
     }
     return QRegExp(convertPatternToRegExp(filter));
}


void AdBlockSubscription::parseUrl(const QUrl &url)
{
    started = false;
    m_uri = url;
}

void AdBlockSubscription::loadRules()
{
    started = true;
    QString fileName = "data/adblock.dat";
    QFile file(fileName);
    if (file.exists()) {
        if (file.open(QFile::ReadOnly)) {
            QTextStream textStream(&file);
            QString header = textStream.readLine();
            if (!header.startsWith(QLatin1String("[Adblock"))) {
                started = false;
                file.close();
                if (!updating)
                    update();
                return;
            }
            else {
                m_rules = new QList<QRegExp>();
                m_rules->clear();
                while (!textStream.atEnd()) {
                    QString line = textStream.readLine();
                    QRegExp rule = handleRule(line);
                    if (!rule.isEmpty())
                        m_rules->insert(m_rules->count(), rule);
                }
            }
        }
    }
    emit loadRulesFinished();
}

void AdBlockSubscription::continueParseUrl(QNetworkReply *reply)
{
    QIODevice *data = reply;
    QString fileName = "data/adblock.dat";
    QFile tmpFile(fileName);
    tmpFile.open(QIODevice::WriteOnly);
    tmpFile.write(data->readAll());
    tmpFile.close();
    updating = false;
    if (!started)
        loadRules();
    reply->deleteLater();
}

void AdBlockSubscription::handleSslErrors(QNetworkReply* reply, QList<QSslError> error)
{
   reply->ignoreSslErrors();
}
