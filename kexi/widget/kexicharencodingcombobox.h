/* This file is part of the KDE project
   Copyright (C) 2005 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXICHARENCODINGCOMBO_H
#define KEXICHARENCODINGCOMBO_H

#include <QHash>
#include <KComboBox>
#include <kexi_export.h>

/*! @short Combobox widget providing a list of possible character encodings.
*/
class KEXIEXTWIDGETS_EXPORT KexiCharacterEncodingComboBox : public KComboBox
{
public:
    //! Constructs a new combobox. \a selectedEncoding can be provided to preselect encoding.
    //! If it is not provided, default encoding is selected for current system settings.
    KexiCharacterEncodingComboBox(QWidget* parent = 0,
                                  const QString& selectedEncoding = QString());
    ~KexiCharacterEncodingComboBox();

    QString selectedEncoding() const;
    void setSelectedEncoding(const QString& encodingName);
    //! Selects default encoding, if present
    void selectDefaultEncoding();
    bool defaultEncodingSelected() const;

protected:
    QHash<QString, QString> m_encodingDescriptionForName;
bool m_defaultEncodingAdded : 1;
};

#endif
