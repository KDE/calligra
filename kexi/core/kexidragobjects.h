/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
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

#ifndef KEXI_DRAGOBJECTS_H_
#define KEXI_DRAGOBJECTS_H_

#include <QDragMoveEvent>
#include <QDropEvent>

#include <kexi_export.h>

class QString;
class QStringList;
class QWidget;

//! Drag object containing information about field(s).
class KEXICORE_EXPORT KexiFieldDrag
{
public:
    /*! \return true if event \a e (of class QDragMoveEvent or QDropEvent)
     can be decoded as "kexi/field" data */
    static bool canDecodeSingle(QMimeSource* e);

    /*! \return true if event \a e (of class QDragMoveEvent or QDropEvent)
     can be decoded as "kexi/fields" data. If decoding of "kexi/field"
     type is supported, decoding of "kexi/fields" is always supported.
     */
    static bool canDecodeMultiple(QMimeSource* e);

    /*! Decodes data of single-field drag ("kexi/field" mime type) coming with event \a e.
     Sets \a sourceMimeType, \a sourceName and \a field.
     \return true on successful decoding (\a e will be accepted in such case). */
    static bool decodeSingle(QDropEvent* e, QString& sourceMimeType,
                             QString& sourceName, QString& field);

    /*! Decodes data of multiple-field drag ("kexi/fields" mime type) coming with event \a e.
     Sets \a sourceMimeType, \a sourceName and \a fields. Also works with "kexi/field" data.
     \return true on successful decoding (\a e will be accepted in such case). */
    static bool decodeMultiple(QDropEvent* e, QString& sourceMimeType,
                               QString& sourceName, QStringList& fields);
};

class KEXICORE_EXPORT KexiDataProviderDrag : public QDrag
{
public:
    KexiDataProviderDrag(const QString& sourceMimeType, const QString& sourceName,
                         QWidget *parent = 0, const char *name = 0);
    ~KexiDataProviderDrag() { }

    static bool canDecode(QDragMoveEvent* e);
    static bool decode(QDropEvent* e, QString& sourceMimeType, QString& sourceName);

};

#endif
