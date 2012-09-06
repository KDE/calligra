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
#include <QTimer>
#include <QNetworkReply>

#include "o2reply.h"

O2Reply::O2Reply(QNetworkReply *r, int timeOut, QObject *parent): QTimer(parent), reply(r) {
    setSingleShot(true);
    connect(this, SIGNAL(error(QNetworkReply::NetworkError)), reply, SIGNAL(error(QNetworkReply::NetworkError)), Qt::QueuedConnection);
    connect(this, SIGNAL(timeout()), this, SLOT(onTimeOut()), Qt::QueuedConnection);
    start(timeOut);
}

void O2Reply::onTimeOut() {
    emit error(QNetworkReply::TimeoutError);
}

O2ReplyList::~O2ReplyList() {
    foreach (O2Reply *timedReply, replies_) {
        delete timedReply;
    }
}

void O2ReplyList::add(QNetworkReply *reply) {
    add(new O2Reply(reply));
}

void O2ReplyList::add(O2Reply *reply) {
    replies_.append(reply);
}

void O2ReplyList::remove(QNetworkReply *reply) {
    O2Reply *o2Reply = find(reply);
    if (o2Reply) {
        o2Reply->stop();
        (void)replies_.removeOne(o2Reply);
    }
}

O2Reply *O2ReplyList::find(QNetworkReply *reply) {
    foreach (O2Reply *timedReply, replies_) {
        if (timedReply->reply == reply) {
            return timedReply;
        }
    }
    return 0;
}
