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
#ifndef KCOLLABORATE_MESSAGE_GENERIC_H
#define KCOLLABORATE_MESSAGE_GENERIC_H

#include <QObject>
#include <QString>
#include <QDomDocument>
#include <QDomElement>

#include <libcollaboration/kcollaborate_export.h>

namespace kcollaborate
{
namespace Message
{

class KCOLLABORATE_EXPORT Generic : public QObject
{
        Q_OBJECT
    public:
        Generic( QObject *parent = 0 );
        virtual ~Generic();

        virtual QString tagName() const = 0;
        virtual void toXML( QDomDocument &doc, QDomElement &elt ) const = 0;
        virtual void fromXML( QDomElement &elt ) = 0;
        ///mostly for tests and debug
        virtual const QString toString() const;
};

};
};

#endif
