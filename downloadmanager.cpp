/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QFileDialog>
#include <QMessageBox>
#include "downloadmanager.h"
#include "qdebug.h"

 DownloadManager::DownloadManager(QObject *parent) //Constructor
     : QObject(parent)
 {
     connect(&manager, SIGNAL(finished(QNetworkReply*)),
             SLOT(downloadFinished(QNetworkReply*)));
 }

 void DownloadManager::doDownload(const QUrl &url) //Adds QUrl to download queue
 {
     QNetworkRequest request(url);
     QNetworkReply *reply = manager.get(request);

     currentDownloads.append(reply);
 }

 QString DownloadManager::saveFileName(const QUrl &url) //Saves QUrl to hard disk - user specifies file location via QFileDialog
 {
     QString path = url.path();
     QString basename = QFileInfo(path).fileName();
     QWidget widg;
     QString filename = QFileDialog::getSaveFileName(
              &widg,
             "Save Document - " + basename,
             QDir::currentPath(),
             "All files (*.*)");
     return filename;
 }

 bool DownloadManager::saveToDisk(const QString &filename, QIODevice *data) //Downloads and saves file to specified location
 {
     if (filename.isNull())
         return false;

     QFile file(filename);
     if (!file.open(QIODevice::WriteOnly)) {
         fprintf(stderr, "Could not open %s for writing: %s\n",
                 qPrintable(filename),
                 qPrintable(file.errorString()));
         return false;
     }

     file.write(data->readAll());
     file.close();

     return true;
 }

 void DownloadManager::downloadFinished(QNetworkReply *reply) //Notifies user if download finished successfully or not
 {
     QUrl url = reply->url();
     if (reply->error())
         QMessageBox::warning(NULL, "Download Failed", "Download failed: " + reply->errorString(), QMessageBox::Ok);
     else {
         QString fileName = saveFileName(url);
         if (saveToDisk(fileName, reply))
             QMessageBox::information(NULL, "Download Success", "Download succeeded for: " + fileName, QMessageBox::Ok);
     }
     currentDownloads.removeAll(reply);
     reply->deleteLater();
 }
