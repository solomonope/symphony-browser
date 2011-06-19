#ifndef NOSCRIPT_H
#define NOSCRIPT_H

#include <QObject>
#include <QStringList>
#include <QWebView>
#include <QHash>

class NoScript
{
public:
    NoScript();
    void blockHost(QString host, bool block);
    void changeDefaultSecure(bool b);
    void saveAll();
    QHash<QString, bool> getHosts();
    int checkStatus(QString host);
    bool *isEnabled;
    QStringList scriptsOn;
    QStringList scriptsOff;
    QStringList scriptsCurr;
    bool defaultSecure;

private:
    bool m_isEnabled;
};

#endif // NOSCRIPT_H
