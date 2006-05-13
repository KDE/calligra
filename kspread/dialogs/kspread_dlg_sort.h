/* This file is part of the KDE project
   Copyright (C) 2006 Robert Knight <robertknight@gmail.com>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Werner Trobin <trobin@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2000 David Faure <faure@kde.org>
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

#ifndef __kspread_dlg_sort__
#define __kspread_dlg_sort__


#include <kdialogbase.h>
#include <QStringList>
//Added by qt3to4:
#include <QLabel>

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QTabWidget;
class QWidget;

namespace KSpread
{
class Sheet;
class View;

class SortDialog : public KDialogBase
{
  Q_OBJECT

 public:
  enum Orientation
  {
    SortColumns=0,
    SortRows=1
  };
  
  SortDialog( View * parent, const char * name = 0,
                  bool modal = false );
  ~SortDialog();
 
 protected:
    Orientation guessDataOrientation();
    QRect       sourceArea();
 
 private slots:
  void sortKey2textChanged( int );
  void useCustomListsStateChanged( int );
  void firstRowHeaderChanged( int );
  virtual void slotOk();
  void slotOrientationChanged(int id);

 private:
  void init();

  View  * m_pView;

  QStringList    m_listColumn;
  QStringList    m_listRow;

  QWidget      * m_page1;
  QWidget      * m_page2;

  QTabWidget   * m_tabWidget;

  QComboBox    * m_sortKey1;
  QComboBox    * m_sortOrder1;
  QComboBox    * m_sortKey2;
  QComboBox    * m_sortOrder2;
  QComboBox    * m_sortKey3;
  QComboBox    * m_sortOrder3;

  QCheckBox    * m_useCustomLists;
  QComboBox    * m_customList;

  QRadioButton * m_sortColumn;
  QRadioButton * m_sortRow;

  QCheckBox    * m_copyLayout;
  QCheckBox    * m_firstRowOrColHeader;
  QCheckBox    * m_respectCase;

  /*QComboBox    * m_outputSheet;
  QLineEdit    * m_outputCell;*/
};

} // namespace KSpread

#endif
