/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 2001 

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef PageNameDialog_h
#define PageNameDialog_h

#include <qdialog.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qstringlist.h>
#include <qlistbox.h>
#include <qrect.h>
#include <qcheckbox.h>

class QApplication;
class KIllustratorView;
class GPage;

class PageNameDialog : public QDialog
{
  Q_OBJECT
public:
  PageNameDialog( KIllustratorView *parent, const char *name, QString &_pageName );

  QString pageName() { return m_pPageName->text(); }

public slots:
  void slotOk();
  void slotClose();

protected:
  KIllustratorView* m_pView;

  QLineEdit* m_pPageName;
  QPushButton* m_pOk;
  QPushButton* m_pClose;

  QString m_PageName;
};

#endif
