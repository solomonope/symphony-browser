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

#include "adblockpage.h"

#include "adblocksubscription.h"

#include <qwebelement.h>
#include <qwebpage.h>
#include <qwebframe.h>
#include "browser.h"

#include <qdebug.h>

AdBlockPage::AdBlockPage(QObject *parent)
    : QObject(parent)
{}

void AdBlockPage::applyRulesToPage(QWebPage *page)
{
    if (!page || !page->mainFrame())
        return;

    AdBlockSubscription *subscription = browser::subscrip();
    QWebElement document = page->mainFrame()->documentElement();
    QWebElementCollection elements = document.findAll("embed");
    elements.append(document.findAll("img"));
    elements.append(document.findAll("iframe"));
    elements.append(document.findAll("param"));
    foreach (QWebElement element, elements) {
        QString url = element.toOuterXml();
        if (!(url.contains("http")))
            continue;
        bool deleteParent = false;
        int start = 0;
        if (url.contains("src"))
            start = url.indexOf("src=\"") + 5;
        else if (url.contains("href"))
            start = url.indexOf("href=\"") + 6;
        else {
            start = url.indexOf("value=\"") + 7;
            deleteParent = true;
        }
        if (start <= 12)
            continue;
        url = url.mid(start, url.length() - start);
        url.chop(url.length() - url.indexOf("\""));
        if (subscription->block(url)) {
            if (deleteParent)
                element.parent().removeFromDocument();
            else
                element.removeFromDocument();
        }
    }
    foreach(QWebElement element, document.findAll("a")) {
        QString url = element.toOuterXml();
        if (!(url.contains("http")))
            continue;
        int start = 0;
        if (url.contains("src"))
            start = url.indexOf("src=\"") + 5;
        if (start <= 12)
            continue;
        url = url.mid(start, url.length() - start);
        url.chop(url.length() - url.indexOf("\""));
        if (!(url.contains("&")) && !(url.contains("_")) && !(url.contains("-")) && !(url.contains("?")))
            continue;
        if (subscription->block(url)) {
            element.removeFromDocument();
        }
    }

}


