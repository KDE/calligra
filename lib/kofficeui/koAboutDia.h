/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
 
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

// Description: About Dialog (header)                                  

#ifndef KOABOUTDIA_H
#define KOABOUTDIA_H

#include <qdialog.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qpushbt.h>
#include <qmsgbox.h>
#include <qlayout.h>

#include <kbuttonbox.h>
#include <kapp.h>

/**
 * class KoAboutDia
 */
class KoAboutDia : public QDialog
{
  Q_OBJECT

public:

  // enums
  enum KoApplication {KPresenter,KSpread,KCharts,KImage,KAutoformEdit,KOffice,KDE};

  // constructor - destructor
  KoAboutDia(QWidget* parent=0,const char* name=0,KoApplication koapp=KDE,QString version = QString::null);
  ~KoAboutDia();

  // show about dialog
  static void about(KoApplication koapp,QString version = QString::null);

protected:

  // dialog objects
  QPixmap pLogo;
  QLabel *lLogo;
  QLabel *lInfo;
  QString author;
  QString email;
  QString add;
  QGridLayout *grid;
  QPushButton *bOk;
  KButtonBox *bbox;

};

#endif
