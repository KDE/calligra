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
#ifndef O2TIMEDREPLYLIST_H
#define O2TIMEDREPLYLIST_H

#include <QList>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QByteArray>

/// A network request/reply pair that can time out.
class O2Reply: public QTimer {
    Q_OBJECT

public:
    O2Reply(QNetworkReply *reply, int timeOut = 60 * 1000, QObject *parent = 0);

signals:
    void error(QNetworkReply::NetworkError);

public slots:
    /// When time out occurs, the QNetworkReply's error() signal is triggered.
    void onTimeOut();

public:
    QNetworkReply *reply;
};

/// List of O2Replies.
class O2ReplyList {
public:
    /// Destructor.
    /// Deletes all O2Reply instances in the list.
    virtual ~O2ReplyList();

    /// Create a new O2Reply from a QNetworkReply, and add it to this list.
    void add(QNetworkReply *reply);

    /// Add an O2Reply to the list, while taking ownership of it.
    void add(O2Reply *reply);

    /// Remove item from the list that corresponds to a QNetworkReply.
    void remove(QNetworkReply *reply);

    /// Find an O2Reply in the list, corresponding to a QNetworkReply.
    /// @return Matching O2Reply or NULL.
    O2Reply *find(QNetworkReply *reply);

protected:
    QList<O2Reply *> replies_;
};

#endif // O2TIMEDREPLYLIST_H
