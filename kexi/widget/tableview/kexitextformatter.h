/* This file is part of the KDE project
   Copyright (C) 2007 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXITEXTFORMATTER_H
#define KEXITEXTFORMATTER_H

#include <kexi_export.h>
#include <kexidb/field.h>

//! @short Text formatter used to format QVariant values to text for displaying and back to QVariant
/*! Used by KexiInputTableEdit, KexiDateTableEdit, KexiTimeTableEdit, KexiDateTimeTableEdit,
 KexiDBLineEdit (forms), etc. */
class KEXIDATATABLE_EXPORT KexiTextFormatter
{
public:
    KexiTextFormatter();
    ~KexiTextFormatter();

    //! Assigns \a field to the formatter. This affects its behaviour.
    void setField(KexiDB::Field* field);

    /*! \return text for \a value.
     A field schema set using setField() is used to perform the formatting.
     \a add is a text that should be added to the value if possible.
     Used in KexiInputTableEdit::setValueInternal(), by form widgets and for reporting/printing. */
    QString valueToText(const QVariant& value, const QString& add) const;

    /*! \return value cnverted from \a text
     A field schema set using setField() is used to perform the formatting.
     Used in KexiInputTableEdit::setValueInternal(), by form widgets and for reporting/printing. */
    QVariant textToValue(const QString& text) const;

    /*! \return true if value formatted as \a text is empty.
     A field schema set using setField() is used to perform the calculation. */
    bool valueIsEmpty(const QString& text) const;

    /*! \return true if value formatted as \a text is valid.
     A field schema set using setField() is used to perform the calculation. */
    bool valueIsValid(const QString& text) const;

    /*! \return input mask for intering values related to a field schema
     which has been set using setField(). */
    QString inputMask() const;

    class Private;
    Private * const d;
};

#endif
