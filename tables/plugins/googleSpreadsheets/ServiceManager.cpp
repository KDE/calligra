/* This file is part of the KDE project
   Copyright (C) 2010-2011 Christoph Goerlich <chgoerlich@gmx.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "ServiceManager.h"
#include "GoogleSpreadsheetsPlugin.h"
#include "LoginDialog.h"
#include "DocumentsListService.h"
#include "SpreadsheetService.h"

#include <KoView.h>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QApplication>

#include <QDir>
#include <QFile>
#include <QDebug>

class ServiceManager::Private
{
    ServiceManager *q;
    
public:
    explicit Private (QObject *parent, ServiceManager *qq)
        : plugin(0),
        docListSrv(new DocumentsListService(qq)),
        spreadsheetSrv(new SpreadsheetService(qq))
    {
        q = qq;
        plugin = dynamic_cast<GoogleSpreadsheetsPlugin*>(parent);
        
        netManager = new QNetworkAccessManager(q);
        connect(netManager, SIGNAL(finished(QNetworkReply*)), q, SLOT(slotReplyFinished(QNetworkReply*)));
    }
    
    GoogleSpreadsheetsPlugin *plugin;
    DocumentsListService *docListSrv;
    SpreadsheetService *spreadsheetSrv;
    QNetworkAccessManager *netManager;
    
    QString username;
    QString password;
    
    struct RequestMetadata {
        GoogleServices::Service service;
        NetworkActions::Action netAction;
        RequestMetadata(GoogleServices::Service srv, NetworkActions::Action n) 
            : service (srv), netAction(n) {}
        RequestMetadata() : service(GoogleServices::none), netAction(NetworkActions::Nothing) {}
    };
    QHash<QNetworkReply*, RequestMetadata> requests;
    
    QString handleAuthenticationReply(QNetworkReply *reply);
};

QString ServiceManager::Private::handleAuthenticationReply(QNetworkReply* reply)
{
    int httpStatusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString dataStr(reply->readAll());
    
    QApplication::restoreOverrideCursor();
    
    if (httpStatusCode == 200) {
        return QString("GoogleLogin auth=") + dataStr.right(dataStr.length() - dataStr.indexOf("Auth=") - 5).trimmed();
    }
    else if (httpStatusCode == 403) {
        QString err = dataStr.right(dataStr.length() - dataStr.indexOf("Error=") - 6).trimmed();
        if (err.contains("BadAuthentication")) {
            username.clear();
            password.clear();
            return QString("Error=Authentication failed! Incorrect Username or Password!\nPlease try to login again.");
        }
        else if (err.contains("CaptchaRequired")) {
            QString captchaToken = dataStr.right(dataStr.length() - dataStr.indexOf("CaptchaToken=") - 13).trimmed();
qDebug() << "###G_Serv_Mgr::captchaToken=" << captchaToken;
            QString captchaUrl = dataStr.right(dataStr.length() - dataStr.indexOf("CaptchaUrl=") - 11);//trimmed();
qDebug() << "###G_Serv_Mgr::captchaUrl=" << captchaUrl;
            //TODO CAPTCHA challenge (separate CAPTCHA challenge for Spreadsheets-Srv)
        }
        else {
            return QString("Error=Error during authentication!\nGoogle-Loginservice response:\n" + 
                    err + "\nwith HTTPStatusCode=" + QString::number(httpStatusCode) + 
                    "\nPlease try to login again.");
        }
    }
    else {
        return QString("Error=An unknown error occured during user authentication!\nHTTPStatusCode="
                + QString::number(httpStatusCode) + ": \"" 
                + reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray()
                + "\"" + "\nPlease try to login again.");
    }

    return QString();
}


/********************************
    ServiceManager-Defs    
********************************/
ServiceManager::ServiceManager(QObject *parent)
    : QObject(parent),
    d(new Private(parent, this))
{
}

ServiceManager::~ServiceManager()
{
    delete d;
}

void ServiceManager::authenticateService(GoogleServices::Service service, const QString errorMsg)
{
    if (d->username.isEmpty() || d->password.isEmpty()) {
        LoginDialog *dialog;
        if (errorMsg.isEmpty())
            dialog = new LoginDialog(d->plugin->parentView());
        else
            dialog = new LoginDialog(d->plugin->parentView(), errorMsg);

        if (!dialog->exec()) {
            delete dialog;
            return;
        }

        d->username = dialog->username();
        d->password = dialog->password();
        
        delete dialog;
    }

    QString srv;
    NetworkActions::Action na;
    switch (service) {
    case GoogleServices::Writely:
        srv = "writely";
        na = NetworkActions::AuthWritely;
        break;
    case GoogleServices::Wise:
        srv = "wise";
        na = NetworkActions::AuthWise;
        break;
    case GoogleServices::none:
        return; //TODO
    default:
        ;
    }

    QByteArray reqData;
    reqData.append(QString("Email=" + d->username + "&Passwd=" + d->password 
        + "&service=" + srv + "&source=CalligraSuite").toUtf8());

    QNetworkRequest request(QUrl(GoogleServices::GOOGLE_CLIENTLOGIN_URL));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::ContentLengthHeader, reqData.length());

    QNetworkReply *reply = d->netManager->post(request, reqData);
    addRequest(reply, GoogleServices::ClientLogin, na);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    
    //QEventLoop for authorization-requests to make this request executed nearly synchronous
    //because we can't send other requests without being authenticated
    QEventLoop eventLoop;
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();
}

void ServiceManager::addRequest(QNetworkReply* reply, GoogleServices::Service service,
                                             NetworkActions::Action netAction)
{
    ServiceManager::Private::RequestMetadata data(service, netAction);
    d->requests.insert(reply, data);
}

QNetworkAccessManager *ServiceManager::netManager() const
{
    return d->netManager;
}

GoogleSpreadsheetsPlugin *ServiceManager::plugin() const
{
    return d->plugin;
}

DocumentsListService *ServiceManager::documentsListService() const
{
    return d->docListSrv;
}

SpreadsheetService *ServiceManager::spreadsheetService() const
{
    return d->spreadsheetSrv;
}

void ServiceManager::slotProgress(qint64 bytesTransfered, qint64 bytesTotal)
{
    int max = d->plugin->progressMaximum();
    if (max == -1)
        return;
    
    if (max == 0)
        d->plugin->setProgressMaximum(bytesTotal);
    
    d->plugin->setProgress(bytesTransfered);
}

void ServiceManager::slotReplyFinished(QNetworkReply *reply)
{
    ServiceManager::Private::RequestMetadata data = d->requests.take(reply);
    
    switch (data.service) {
    case GoogleServices::ClientLogin:
    {
        QString token = d->handleAuthenticationReply(reply);
        if (token.startsWith("GoogleLogin auth=")) {
            if (data.netAction == NetworkActions::AuthWritely) {
                d->docListSrv->setAuthToken(token);
                d->plugin->enableMenuActions();
            }
            else if (data.netAction == NetworkActions::AuthWise) {
                d->spreadsheetSrv->setAuthToken(token);
            }
        }
        else if (token.startsWith("Error=")) {
            reply->deleteLater();
            int pos = token.indexOf("=");
            QString msg(token.mid(pos + 1));
            if (data.netAction == NetworkActions::AuthWritely) {
                authenticateService(GoogleServices::Writely, msg);
            }
            else if (data.netAction == NetworkActions::AuthWise) {
                authenticateService(GoogleServices::Wise, msg);
            }
        }
        break;
    }
    case GoogleServices::Writely:
        emit docListReplyFinished(reply, data.netAction);
        break;
    case GoogleServices::Wise:
        emit spreadsheetsReplyFinished(reply, data.netAction);
        break;
    case GoogleServices::none:
        //TODO
    default:
        ;
    }

    reply->deleteLater();
    QApplication::restoreOverrideCursor();
}

#include "ServiceManager.moc"
