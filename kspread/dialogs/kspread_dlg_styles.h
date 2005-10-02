/* This file is part of the KDE project
   Copyright (C) 2003 Laurent Montel <montel@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>  

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

#ifndef __kspread_dlg_styles__
#define __kspread_dlg_styles__

#include <kdialogbase.h>
#include <qwidget.h>

class KComboBox;
class KListView;

class KSpreadStyleManager;
class KSpreadView;

class QListViewItem;

class StyleWidget : public QWidget
{
  Q_OBJECT

 public:
  StyleWidget( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
  ~StyleWidget();

  KListView * m_styleList;
  KComboBox * m_displayBox;
signals:
    void modifyStyle();
};

class KSpreadStyleDlg : public KDialogBase
{
  Q_OBJECT
 public:
  KSpreadStyleDlg( KSpreadView * parent, KSpreadStyleManager * manager,
                   const char * name = "KSpreadStyleDlg" );
  ~KSpreadStyleDlg();

 protected slots:
  void slotOk();
  void slotUser1();
  void slotUser2();
  void slotUser3();
  void slotDisplayMode( int mode );
  void slotSelectionChanged( QListViewItem * );

 private:
  KSpreadView         * m_view;
  KSpreadStyleManager * m_styleManager;
  StyleWidget         * m_dlg;

  void fillComboBox();
};

#endif
