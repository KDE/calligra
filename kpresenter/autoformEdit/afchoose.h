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

#ifndef AFCHOOSE_H
#define AFCHOOSE_H

#include <qtabdlg.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qstring.h>
#include <qpushbt.h>
#include <qlistbox.h>
#include <qlist.h>
#include <qfileinf.h>
#include <qpixmap.h>
#include <qstring.h>
#include <qevent.h>
#include <qfile.h>
#include <qcolor.h>

#include <kiconloaderdialog.h>
#include <kapp.h>

/******************************************************************/
/* class AFChoose                                                 */
/******************************************************************/

class AFChoose : public QTabDialog
{
  Q_OBJECT

public:

  // constructor - destructor
  AFChoose(QWidget *parent=0,const char *name=0);
  ~AFChoose();

protected:

  // structure of a group
  struct Group
  {
    QFileInfo dir;
    QString name;
    QWidget *tab;
    KIconLoaderCanvas *loadWid;
    QLabel *label;
  };

  // set groups
  void getGroups();

  // setup tabs
  void setupTabs();

  // resize event
  void resizeEvent(QResizeEvent *);

  // ********** variables **********

  // list of groups and a pointer to a group
  QList<Group> groupList;
  Group *grpPtr;

private slots:

  // name changed
  void nameChanged(const QString &);

  // autoform chosen
  void chosen();

signals:

  //autoform chosen
  void formChosen(const QString &);

};
#endif //AFCHOOSE_H

