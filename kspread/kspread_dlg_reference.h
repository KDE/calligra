/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000 Montel Laurent <montell@club-internet.fr>
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

#ifndef __kspread_dlg_reference__
#define __kspread_dlg_reference__

#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qlabel.h>
#include <qdialog.h>

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;

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
};

class KSpreadEditAreaName : public QDialog
{
    Q_OBJECT

public:
    KSpreadEditAreaName( KSpreadView * parent, const char * name,
                         QString const & areaname );
    ~KSpreadEditAreaName();

public slots:
  void slotOk();

private:
    KSpreadView * m_pView;

    QPushButton * m_buttonOk;
    QPushButton * m_buttonCancel;
    QLineEdit   * m_area;
    QComboBox   * m_sheets;
    QLabel      * m_areaName;
};

#endif
