/*
 * Copyright (C) 2007 Igor Stepin <igor_for_os@stepin.name>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KCOLLABORATE_SESSION_H
#define KCOLLABORATE_SESSION_H

#include <QObject>
#include <QString>
#include <libcollaboration/kcollaborate_export.h>
#include <libcollaboration/network/Url.h>
#include <libcollaboration/network/messages/Update.h>
#include <libcollaboration/network/messages/UpdateAnswer.h>

namespace kcollaborate
{
class Url;

namespace Message
{
class User;
};

class KCOLLABORATE_EXPORT Session : public QObject
{
        Q_OBJECT
    public:
        Session( const Url &url, QObject *parent = 0 );
        virtual ~Session();

        const QString& id() const { return id_; };
        Url& url() { return url_; }

    signals:
        void connected();
        void disconnected();

        void messageUpdate( const kcollaborate::Message::Update &msg );
        void messageUpdateAnswer( const kcollaborate::Message::UpdateAnswer &msg );

//         void userAdded( const kcollaborate::Message::Update &user );
//         void userRemoved( const kcollaborate::Message::Update &user );

    protected:
        void setId( const QString &id ) { id_ = id; };

    private:
        QString id_;
        Url url_;
};

};

#endif
