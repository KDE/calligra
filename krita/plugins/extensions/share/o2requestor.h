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
#ifndef O2REQUESTOR_H
#define O2REQUESTOR_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QByteArray>

#include "o2reply.h"

class O2;

/// Makes authenticated requests.
class O2Requestor: public QObject {
    Q_OBJECT

public:
    explicit O2Requestor(QNetworkAccessManager *manager, O2 *authenticator, QObject *parent = 0);
    ~O2Requestor();

public slots:
    /// Make a GET request.
    /// @return Request ID or -1 if there are too many requests in the queue.
    int get(const QNetworkRequest &req);

    /// Make a POST request.
    /// @return Request ID or -1 if there are too many requests in the queue.
    int post(const QNetworkRequest &req, const QByteArray &data);

    /// Make a PUT request.
    /// @return Request ID or -1 if there are too many requests in the queue.
    int put(const QNetworkRequest &req, const QByteArray &data);

signals:
    /// Emitted when a request has been completed or failed.
    void finished(int id, QNetworkReply::NetworkError error, QByteArray data);

    /// Emitted when an upload has progressed.
    void uploadProgress(int id, qint64 bytesSent, qint64 bytesTotal);

protected slots:
    /// Handle refresh completion.
    void onRefreshFinished(QNetworkReply::NetworkError error);

    /// Handle request finished.
    void onRequestFinished();

    /// Handle request error.
    void onRequestError(QNetworkReply::NetworkError error);

    /// Re-try request (after successful token refresh).
    void retry();

    /// Finish the request, emit finished() signal.
    void finish();

    /// Handle upload progress.
    void onUploadProgress(qint64 uploaded, qint64 total);

protected:
    int setup(const QNetworkRequest &request, QNetworkAccessManager::Operation operation);

    enum Status {
        Idle, Requesting, ReRequesting
    };

    QNetworkAccessManager *manager_;
    O2 *authenticator_;
    QNetworkRequest request_;
    QByteArray data_;
    QNetworkReply *reply_;
    Status status_;
    int id_;
    QNetworkAccessManager::Operation operation_;
    QUrl url_;
    O2ReplyList timedReplies_;
    QNetworkReply::NetworkError error_;
};

#endif // O2REQUESTOR_H
