/* This file is part of the KDE project
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef DR_PROP_TEST_H
#define DR_PROP_TEST_H

int drPropTest()
{
    QList<QByteArray> names = driver->propertyNames();
    kDebug() << QString("%1 properties found:").arg(names.count());
    foreach(const QByteArray& propertyName, names) {
        kDebug() << " - " << propertyName << ":"
        << " caption=\"" << driver->propertyCaption(propertyName) << "\""
        << " type=" << driver->propertyValue(propertyName).typeName()
        << " value=\"" << driver->propertyValue(propertyName).toString() << "\"";
    }
//  QVariant propertyValue( const QCString& propName ) const;

//  QVariant propertyCaption( const QCString& propName ) const;

    return 0;
}

#endif
