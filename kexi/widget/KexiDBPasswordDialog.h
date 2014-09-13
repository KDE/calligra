/* This file is part of the KDE project
   Copyright (C) 2003-2013 Jarosław Staniek <staniek@kde.org>

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

#include <kexi_export.h>
#include <kpassworddialog.h>
#include <db/tristate.h>

class KexiProjectData;
class KexiProjectData;
class KCmdLineArgs;
namespace KexiDB
{
class ConnectionData;
}

//! Database password dialog
class KEXIEXTWIDGETS_EXPORT KexiDBPasswordDialog : public KPasswordDialog
{
    Q_OBJECT
public:
    KexiDBPasswordDialog(QWidget *parent, KexiDB::ConnectionData& cdata, bool showDetailsButton = false);
    virtual ~KexiDBPasswordDialog();

    bool showConnectionDetailsRequested() const;

    //! Asks the user for password if needed and sets it to data.
    //! @return true if password is needed and user provided the password, cancelled is password
    //! is needed but user pressed Cancel and false if password is not needed.
    static tristate getPasswordIfNeeded(KexiDB::ConnectionData *data, QWidget *parent = 0);

protected slots:
    virtual void slotButtonClicked(int button);
    void slotShowConnectionDetails();

protected:
    class Private;
    Private* const d;
};

#endif
