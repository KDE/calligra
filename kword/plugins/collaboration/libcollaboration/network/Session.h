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

namespace kcollaborate
{
class Url;
class User;

class KCOLLABORATE_EXPORT Session : public QObject
{
        Q_OBJECT
    public:
        Session( const Url &url, QObject *parent = 0 );
        virtual ~Session();

//         virtual void disconnect() = 0;
        const QString& id() const { return id_; };

    signals:
        void connected();
        void disconnected();

        void userAdded( User *user );
        void userRemoved( User *user );

        void newRawCommand(const QString &command);

    protected:
        void setId( const QString &id ) { id_ = id; };
        Url& url() { return url_; }

    private:
        QString id_;
        Url url_;
};

};

#endif
