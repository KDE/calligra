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
#ifndef KCOLLABORATE_USER_H
#define KCOLLABORATE_USER_H

#include <QObject>
#include <QString>
#include <QMap>

#include <libcollaboration/kcollaborate_export.h>

namespace kcollaborate
{

class KCOLLABORATE_EXPORT User : public QObject
{
        Q_OBJECT
    public:
        User( const QString &name, const QString &pictureUrl, QObject *parent = 0 );
        virtual ~User();

        void addIm( const QString &imType, const QString &account );

        const QString& name() const;
        const QString& pictureUrl() const;//TODO: will be not string
        const QMap<QString, QString>& im() const;
        const QString im( const QString &imType ) const;

        const QString toMsgPart() const;
    private:
        QString m_name;
        QString m_pictureUrl;//TODO: will be not string
        QMap<QString, QString> m_im;
};

};

#endif
