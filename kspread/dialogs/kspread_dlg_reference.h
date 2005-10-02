/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 Harri Porten <porten@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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

#ifndef __kspread_dlg_reference__
#define __kspread_dlg_reference__

#include <kdialogbase.h>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QListBox;
class QListBoxItem;

class KSpreadView;

class KSpreadreference: public QDialog
{
  Q_OBJECT
public:
  KSpreadreference( KSpreadView* parent, const char* name );


public slots:
  void slotOk();
  void slotCancel();
  void slotDoubleClicked(QListBoxItem *);
  void slotRemove();
  void slotEdit();
  void slotHighlighted(QListBoxItem *);
protected:
  KSpreadView * m_pView;

  QListBox    * m_list;
  QPushButton * m_pOk;
  QPushButton * m_pCancel;
  QPushButton * m_pEdit;
  QPushButton * m_pRemove;
  QLabel      * m_rangeName;

private:
  void displayAreaValues(QString const & areaName);
};

class KSpreadEditAreaName : public KDialogBase
{
    Q_OBJECT

public:
    KSpreadEditAreaName( KSpreadView * parent, const char * name,
                         QString const & areaname );
    ~KSpreadEditAreaName();

public slots:
  virtual void slotOk();

private:
    KSpreadView * m_pView;

    QLineEdit   * m_area;
    QComboBox   * m_sheets;
    QLabel      * m_areaName;
};

#endif
