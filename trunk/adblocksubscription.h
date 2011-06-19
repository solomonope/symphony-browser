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

#ifndef ADBLOCKSUBSCRIPTION_H
#define ADBLOCKSUBSCRIPTION_H

#include <QObject>
#include <QList>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QObject>
#include <QSslError>
#include <QStringList>

class AdBlockSubscription : public QObject
{
    Q_OBJECT

public:
    AdBlockSubscription(QObject *parent = 0, const QUrl &url = QUrl());
    ~AdBlockSubscription();
    void load();
    void update();
    void updateWhitelist();
    void setEnabled(bool enabled);
    void setTitle(const QString &title);
    void setLocation(const QUrl &url);
    bool block(const QString &urlString) const;
    bool getMatch(QString encodedUrl, QRegExp m_regExp) const;
    QString convertPatternToRegExp(const QString &wildcardPattern);
    QString title() const;
    QUrl url() const;
    QUrl location() const;
    bool isEnabled() const;
    QDateTime lastUpdate() const;
    QList<QRegExp> allRules() const;
    QRegExp handleRule(QString filter);
    QStringList whitelist;
    bool updating;
    bool started;

private:
    void parseUrl(const QUrl &url);
    void loadRules();
    QNetworkReply *m_downloading;
    QList<QRegExp> *m_rules;
    QByteArray m_url;
    QUrl m_uri;
    QString m_title;
    QByteArray m_location;
    QDateTime m_lastUpdate;
    bool m_enabled;

private slots:
    void continueParseUrl(QNetworkReply *reply);
    void handleSslErrors(QNetworkReply*,QList<QSslError>);

signals:
    void changed();
    void rulesChanged();
    void loadRulesFinished();

};

#endif // ADBLOCKSUBSCRIPTION_H
