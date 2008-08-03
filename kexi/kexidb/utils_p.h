/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXIDB_UTILS_P_H
#define KEXIDB_UTILS_P_H

#include <qtimer.h>
#include <qwaitcondition.h>

#include <kprogressdialog.h>

#include "msghandler.h"

class ConnectionTestThread;

class ConnectionTestDialog : protected KProgressDialog
{
    Q_OBJECT
public:
    ConnectionTestDialog(QWidget* parent,
                         const KexiDB::ConnectionData& data, KexiDB::MessageHandler& msgHandler);
    virtual ~ConnectionTestDialog();

    int exec();

    void error(KexiDB::Object *obj);

protected slots:
    void slotTimeout();
    virtual void reject();

protected:
    ConnectionTestThread* m_thread;
    KexiDB::ConnectionData m_connData;
    QTimer m_timer;
    KexiDB::MessageHandler* m_msgHandler;
    uint m_elapsedTime;
    KexiDB::Object *m_errorObj;
    QWaitCondition m_wait;
bool m_stopWaiting : 1;
};

#endif
