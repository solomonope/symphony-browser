#include "noscript.h"
#include <QSettings>
#include <QWebElement>
#include <QWebElementCollection>
#include <QWebPage>
#include <QWebFrame>
#include "qdebug.h"

NoScript::NoScript()
{
    QSettings settings("data/noscript.dat", QSettings::IniFormat);
    if (settings.contains("on"))
        scriptsOn = settings.value("on").toStringList();
    if (settings.contains("off"))
        scriptsOff = settings.value("off").toStringList();
    defaultSecure = true;
    QSettings settings2("data/settings.dat", QSettings::IniFormat);
    if (settings2.contains("NoScriptSecureEnabled")) {
        if (settings2.value("NoScriptSecureEnabled").toInt() == 1)
            defaultSecure = false;
    }
    m_isEnabled = false;
    isEnabled = &m_isEnabled;
}

void NoScript::blockHost(QString host, bool block)
{

    if (block == defaultSecure) {
        if (!(scriptsOff.contains(host)) && !(scriptsOn.contains(host)))
            return;
    }
    if (scriptsCurr.contains(host))
        scriptsCurr.removeOne(host);
    if (block) {
        if (scriptsOff.contains(host))
            scriptsOff.removeOne(host);
        if (scriptsOn.contains(host))
            scriptsOn.removeOne(host);
        scriptsOff.insert(scriptsOff.count(), host);
    }
    else {
        if (scriptsOn.contains(host))
            scriptsOn.removeOne(host);
        if (scriptsOff.contains(host))
            scriptsOff.removeOne(host);
        scriptsOn.insert(scriptsOn.count(), host);
    }
}

void NoScript::changeDefaultSecure(bool b) //b = true == noScriptSecure = 1 && b = false == noScriptSecure = 0
{
    defaultSecure = b;
    QSettings settings("data/settings.dat", QSettings::IniFormat);
    if (b)
        settings.setValue("NoScriptSecureEnabled", 1);
    else
        settings.setValue("NoScriptSecureEnabled", 0);
}

void NoScript::saveAll()
{
    QSettings settings("data/noscript.dat", QSettings::IniFormat);
    if (scriptsOn.count() > 0)
        settings.setValue("on", scriptsOn);
    if (scriptsOff.count() > 0)
        settings.setValue("off", scriptsOff);
}

 QHash<QString, bool> NoScript::getHosts()
 {
     QHash<QString, bool> ret;
     QStringList all;
     all.append(scriptsCurr);
     if (defaultSecure) {
         all.append(scriptsOn);
         all.append(scriptsOff);
     }
     else {
         all.append(scriptsOff);
         all.append(scriptsOn);
     }
     foreach(QString host, all) {
         int tmp = checkStatus(host);
         if (tmp == 1 || (tmp == 2 && !defaultSecure))
            ret.insert(host, true);
         else
            ret.insert(host, false);
     }
     return ret;
 }

 int NoScript::checkStatus(QString host)
 {
     if (scriptsOn.contains(host))
         return 1;
     else if (scriptsOff.contains(host))
         return 0;
     else {
         if(!(scriptsCurr.contains(host)))
             scriptsCurr.insert(scriptsCurr.count(), host);
         return 2;
     }
 }
