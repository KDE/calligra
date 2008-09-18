/* This file is part of the KDE project
   Copyright (C) 2004 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXI_STARTUPHANDLER_P_H
#define KEXI_STARTUPHANDLER_P_H

#include <qobject.h>
#include <qstring.h>

#include <sys/stat.h>

#include <kexiutils/tristate.h>

class KProcess;
class KProgressDialog;

class SQLite2ToSQLite3Migration : public QObject
{
    Q_OBJECT
public:
    SQLite2ToSQLite3Migration(const QString& filePath);
    ~SQLite2ToSQLite3Migration();

    tristate run();

public slots:
    void processExited(KProcess*);
    void receivedStderr(KProcess*, char*, int);
    void cancelClicked();

protected:
    QString m_filePath;
    KProcess *m_process;
    KProgressDialog* m_dlg;

    struct stat m_st;
bool m_restoreStat : 1;
bool m_run : 1;

    tristate result;
};

#endif
