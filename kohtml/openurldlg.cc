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

#include "openurldlg.h"
#include "openurldlg.moc"

#include <stdlib.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <kapp.h>
#include <kseparator.h>
#include <kfiledialog.h>
#include <klocale.h>

OpenURLDlg::OpenURLDlg()
:QDialog(0L, "openURLDlg", true)
{
  setCaption(i18n("KoHTML: Open URL"));
  
  QVBoxLayout *layout = new QVBoxLayout(this, 8);

  QLabel *l = new QLabel(i18n(""
  "Enter the World Wide Web location (URL) or specify\n"
  "the local file you would like to open:"
  ""), this);
  l->setMinimumSize( l->sizeHint() );
  layout->addWidget( l );
  
  QHBoxLayout *hLayout = new QHBoxLayout();
  layout->addLayout( hLayout );
  
  urlEdit = new KLined(this, "lineEdit");
  urlEdit->setMinimumSize( urlEdit->sizeHint().width()*4, urlEdit->sizeHint().height() );
  hLayout->addWidget( urlEdit, 5 );
  
  completion = new KURLCompletion();
  connect(urlEdit, SIGNAL(completion()),
          completion, SLOT(make_completion()));
  connect(urlEdit, SIGNAL(rotation()),
          completion, SLOT(make_rotation()));
  connect(urlEdit, SIGNAL(textChanged(const char *)),
          completion, SLOT(edited(const char *)));
  connect(completion, SIGNAL(setText(const char *)),
          urlEdit, SLOT(setText(const char *)));	  	  	  
  
  QPushButton *fileSelect = new QPushButton(i18n("Choose File..."), this);
  connect(fileSelect, SIGNAL(clicked()), SLOT(chooseFile()));
  fileSelect->setMinimumSize( fileSelect->sizeHint() );
  fileSelect->adjustSize();
  hLayout->addWidget( fileSelect, 2 );
  
  KSeparator *separator = new KSeparator(this);
  separator->setMinimumHeight( 10 );
  layout->addWidget( separator );
  
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  layout->addLayout( buttonLayout );
  
  QPushButton *ok = new QPushButton(i18n("Open URL"), this);
  ok->setMinimumSize( ok->sizeHint() );
  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  buttonLayout->addWidget(ok);
  
  QPushButton *cancel = new QPushButton(i18n("Cancel"), this);
  cancel->setMinimumSize( cancel->sizeHint() );
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));
  buttonLayout->addWidget(cancel);
  
  QPushButton *clear = new QPushButton(i18n("Clear"), this);
  clear->setMinimumSize( clear->sizeHint() );
  connect(clear, SIGNAL(clicked()), urlEdit, SLOT(clear()));
  buttonLayout->addWidget(clear);
  
  layout->activate();

  resize( sizeHint() );
}

OpenURLDlg::~OpenURLDlg()
{
  if (urlEdit) delete urlEdit;
  if (completion) delete completion;
}

QString OpenURLDlg::url()
{
  QString url = urlEdit->text();
  
  url = url.stripWhiteSpace();
  
  if (url.find("www") == 0)
    url.prepend("http://");
  else if (url.find("ftp.") == 0)
    url.prepend("ftp://");
    
  return url;    
}

void OpenURLDlg::chooseFile()
{
  QString file = KFileDialog::getOpenFileName( getenv("HOME") );
  
  if (file.isNull())
    return;
    
  urlEdit->setText( file );    
}