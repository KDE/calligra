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
#include "XmlHelper.h"
#include <QDebug>

QDomElement kcollaborate::Message::XmlHelper::parse( const QString &string )
{
    QDomDocument domDocument;
    QString errorStr;
    int errorLine;
    int errorColumn;
    if ( !domDocument.setContent( string, true, &errorStr, &errorLine,
                                  &errorColumn ) ) {
        qWarning() << "Parse error in the recieved message at line "
        << errorLine << ", column " << errorColumn << ": " << errorStr;
        //return NULL;//TODO: make this better
    }

    return domDocument.documentElement();
}
