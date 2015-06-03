/* This file is part of the KDE project
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIDBPASSWORDDIALOG_H
#define KEXIDBPASSWORDDIALOG_H

#include "kexiextwidgets_export.h"
#include <kpassworddialog.h>
#include <db/tristate.h>

class KexiProjectData;
class KexiProjectData;
namespace KexiDB
{
class ConnectionData;
}

//! Database password dialog
class KEXIEXTWIDGETS_EXPORT KexiDBPasswordDialog : public KPasswordDialog
{
    Q_OBJECT
public:
    enum Flag
    {
        NoFlags = 0x00,
        ShowDetailsButton = 0x01, //!< If set, the details button will be displayed
        ServerReadOnly = 0x02     //!< If set, the server line edit will be read-only
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    //! Constructs a new password dialog.
    //! cdata.password is modified only when user provided the password.
    //! If @a showDetailsButton is true, the dialog displays connection details.
    KexiDBPasswordDialog(QWidget *parent, KexiDB::ConnectionData& cdata, Flags flags = NoFlags);
    virtual ~KexiDBPasswordDialog();

    bool showConnectionDetailsRequested() const;

    //! Asks the user for password if needed and sets it to data.
    //! @return true if password is needed and user provided the password, cancelled is password
    //! is needed but user pressed Cancel and false if password is not needed.
    //! data->password is modified only when user provided the password.
    static tristate getPasswordIfNeeded(KexiDB::ConnectionData *data, QWidget *parent = 0);

protected Q_SLOTS:
    virtual void slotOkOrDetailsButtonClicked();
    void slotShowConnectionDetails();

protected:
    class Private;
    Private* const d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KexiDBPasswordDialog::Flags)

#endif
