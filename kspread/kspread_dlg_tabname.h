/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __kspread_dlg_tabname__
#define __kspread_dlg_tabname__

#include <qdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcombobox.h>

class QApplication;
class KSpreadView;
class KSpreadTable;
class QPushButton;

class KSpreadTableName : public QDialog
{
    Q_OBJECT
public:
    KSpreadTableName( KSpreadView* parent, const char* name, QString &_tableName );

    QString tableName() { return m_pTableName->text(); }

public slots:
    void slotOk();
    void slotClose();

protected:
    KSpreadView* m_pView;

    QLineEdit* m_pTableName;
    QPushButton* m_pOk;
    QPushButton* m_pClose;

    QString m_TableName;
};

#endif
