/* This file is part of the KDE project
   Copyright (C) 2005-2015 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIDBDRIVERCOMBOBOX_H
#define KEXIDBDRIVERCOMBOBOX_H

#include "kexiextwidgets_export.h"

#include <KComboBox>

//! \brief A combo box widget for selecting a database driver
//! @todo move to a KDbWidgets lib within KDb
class KEXIEXTWIDGETS_EXPORT KexiDBDriverComboBox : public KComboBox
{
    Q_OBJECT
public:
    enum Option {
        ShowFileDrivers = 1,
        ShowServerDrivers = 2,
        ShowAllDrivers = ShowFileDrivers|ShowServerDrivers
    };
    Q_DECLARE_FLAGS(Options, Option)

    /*! Constructs a new KexiDBDriverComboBox object.

        The combobox is populated with user-visible names of drivers obtained from
        the KDbDriverManager.
        If @a options include ShowFileDrivers, file-based drivers are included.
        If @a options include ShowServerDrivers, setver-based drivers are included.
    */
    explicit KexiDBDriverComboBox(QWidget* parent = 0,
                                  Options options = ShowAllDrivers);

    ~KexiDBDriverComboBox();

    //! @return ID of the currently selected driver
    QString currentDriverId() const;

    //! Set the ID of currently selected driver to @a driverId.
    void setCurrentDriverId(const QString& driverId);

private:
    class Private;
    Private * const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KexiDBDriverComboBox::Options)

#endif
