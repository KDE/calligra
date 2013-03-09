/*
Copyright (c) 2012, Akos Polster
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation i
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" i
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include <QDebug>
#include <QTimer>

#include <kdebug.h>

#include "o2requestor.h"
#include "o2.h"

#define trace() if (1) qDebug()
// define trace() if (0) qDebug()

O2Requestor::O2Requestor(QNetworkAccessManager *manager, O2 *authenticator, QObject *parent): QObject(parent), reply_(NULL), status_(Idle) {
    manager_ = manager;
    authenticator_ = authenticator;
    qRegisterMetaType<QNetworkReply::NetworkError>("QNetworkReply::NetworkError");
    connect(authenticator, SIGNAL(refreshFinished(QNetworkReply::NetworkError)), this, SLOT(onRefreshFinished(QNetworkReply::NetworkError)), Qt::QueuedConnection);
}

O2Requestor::~O2Requestor() {
}

int O2Requestor::get(const QNetworkRequest &req) {
    if (-1 == setup(req, QNetworkAccessManager::GetOperation)) {
        return -1;
    }
    reply_ = manager_->get(request_);
    timedReplies_.add(reply_);
    connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onRequestError(QNetworkReply::NetworkError)), Qt::QueuedConnection);
    connect(reply_, SIGNAL(finished()), this, SLOT(onRequestFinished()), Qt::QueuedConnection);
    return id_;
}

int O2Requestor::post(const QNetworkRequest &req, const QByteArray &data) {
    if (-1 == setup(req, QNetworkAccessManager::PostOperation)) {
        return -1;
    }
    data_ = data;
    reply_ = manager_->post(request_, data_);
    timedReplies_.add(reply_);
    connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onRequestError(QNetworkReply::NetworkError)), Qt::QueuedConnection);
    connect(reply_, SIGNAL(finished()), this, SLOT(onRequestFinished()), Qt::QueuedConnection);
    connect(reply_, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(onUploadProgress(qint64,qint64)));
    return id_;
}

int O2Requestor::put(const QNetworkRequest &req, const QByteArray &data) {
    if (-1 == setup(req, QNetworkAccessManager::PutOperation)) {
        return -1;
    }
    data_ = data;
    reply_ = manager_->put(request_, data_);
    timedReplies_.add(reply_);
    connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onRequestError(QNetworkReply::NetworkError)), Qt::QueuedConnection);
    connect(reply_, SIGNAL(finished()), this, SLOT(onRequestFinished()), Qt::QueuedConnection);
    connect(reply_, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(onUploadProgress(qint64,qint64)));
    return id_;
}

void O2Requestor::onRefreshFinished(QNetworkReply::NetworkError error) {
    if (status_ != Requesting) {
        kWarning() << "O2Requestor::onRefreshFinished: No pending request";
        return;
    }
    if (QNetworkReply::NoError == error) {
        QTimer::singleShot(100, this, SLOT(retry()));
    } else {
        error_ = error;
        QTimer::singleShot(10, this, SLOT(finish()));
    }
}

void O2Requestor::onRequestFinished() {
    QNetworkReply *senderReply = qobject_cast<QNetworkReply *>(sender());
    QNetworkReply::NetworkError error = senderReply->error();
    if (status_ == Idle) {
        return;
    }
    if (reply_ != senderReply) {
        return;
    }
    if (error == QNetworkReply::NoError) {
        QTimer::singleShot(10, this, SLOT(finish()));
    }
}

void O2Requestor::onRequestError(QNetworkReply::NetworkError error) {
    kWarning() << "O2Requestor::onRequestError: Error" << (int)error;
    if (status_ == Idle) {
        return;
    }
    if (reply_ != qobject_cast<QNetworkReply *>(sender())) {
        return;
    }
    int httpStatus = reply_->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    kWarning() << "O2Requestor::onRequestError: HTTP status" << httpStatus << reply_->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    trace() << reply_->readAll();
    if ((status_ == Requesting) && (httpStatus == 401)) {
        // Call O2::refresh. Note the O2 instance might live in a different thread
        if (QMetaObject::invokeMethod(authenticator_, "refresh")) {
            return;
        }
        qCritical() << "O2Requestor::onRequestError: Invoking remote refresh failed";
    }
    error_ = error;
    QTimer::singleShot(10, this, SLOT(finish()));
}

void O2Requestor::onUploadProgress(qint64 uploaded, qint64 total) {
    if (status_ == Idle) {
        kWarning() << "O2Requestor::onUploadProgress: No pending request";
        return;
    }
    if (reply_ != qobject_cast<QNetworkReply *>(sender())) {
        return;
    }
    emit uploadProgress(id_, uploaded, total);
}

int O2Requestor::setup(const QNetworkRequest &req, QNetworkAccessManager::Operation operation) {
    static int currentId;
    QUrl url;

    if (status_ != Idle) {
        kWarning() << "O2Requestor::setup: Another request pending";
        return -1;
    }

    request_ = req;
    operation_ = operation;
    id_ = currentId++;
    url_ = url = req.url();
    url.addQueryItem("access_token", authenticator_->token());
    request_.setUrl(url);
    status_ = Requesting;
    error_ = QNetworkReply::NoError;
    return id_;
}

void O2Requestor::finish() {
    QByteArray data;
    if (status_ == Idle) {
        kWarning() << "O2Requestor::finish: No pending request";
        return;
    }
    data = reply_->readAll();
    status_ = Idle;
    timedReplies_.remove(reply_);
    reply_->disconnect(this);
    reply_->deleteLater();
    emit finished(id_, error_, data);
}

void O2Requestor::retry() {
    if (status_ != Requesting) {
        kWarning() << "O2Requestor::retry: No pending request";
        return;
    }
    timedReplies_.remove(reply_);
    reply_->disconnect(this);
    reply_->deleteLater();
    QUrl url = url_;
    url.addQueryItem("access_token", authenticator_->token());
    request_.setUrl(url);
    status_ = ReRequesting;
    switch (operation_) {
    case QNetworkAccessManager::GetOperation:
        reply_ = manager_->get(request_);
        break;
    case QNetworkAccessManager::PostOperation:
        reply_ = manager_->post(request_, data_);
        break;
    default:
        reply_ = manager_->put(request_, data_);
    }
    timedReplies_.add(reply_);
    connect(reply_, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(onRequestError(QNetworkReply::NetworkError)), Qt::QueuedConnection);
    connect(reply_, SIGNAL(finished()), this, SLOT(onRequestFinished()), Qt::QueuedConnection);
    connect(reply_, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(onUploadProgress(qint64,qint64)));
}
