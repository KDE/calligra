/*

	Copyright (C) 1998 Simon Hausmann
                       <tronical@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
//
// $Id$
//

#include "edithtmldlg.h"
#include "edithtmldlg.moc"

#include <qlayout.h>
#include <qpushbutton.h>

#include <kapp.h>

HTMLEditDlg::HTMLEditDlg(QString data)
:QDialog(0L, "htmlEditDlg", true)
{
  setCaption(i18n("KoHTML: HTML Editor"));
  
  QVBoxLayout *layout = new QVBoxLayout(this, 4);
  
  e = new QMultiLineEdit(this, "multiLineEdit");
  e->setReadOnly(false);
  layout->addWidget(e, 100);
  
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  layout->addLayout(buttonLayout);
  
  QPushButton *ok = new QPushButton(i18n("OK"), this);
  ok->setMinimumSize( ok->sizeHint() );
  ok->adjustSize();
  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  buttonLayout->addWidget(ok);
  
  QPushButton *cancel = new QPushButton(i18n("Cancel"), this);
  cancel->setMinimumSize( cancel->sizeHint() );
  cancel->adjustSize();
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));
  buttonLayout->addWidget(cancel);

  e->setText(data);
  
  layout->activate();
}

HTMLEditDlg::~HTMLEditDlg()
{
  if (e) delete e;
}

QString HTMLEditDlg::getText()
{
  return e->text();
}
