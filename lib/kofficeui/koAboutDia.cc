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

// Description: About Dialog

/******************************************************************/

#include "koAboutDia.h"
#include <klocale.h>
#include <koApplication.h>
#include <kiconloader.h>
#include <kglobal.h>

/******************************************************************/
/* class KoAboutDia                                               */
/******************************************************************/

/*==================== constructor ===============================*/
KoAboutDia::KoAboutDia(QWidget* parent,
		       const char* name,
		       KoApplication koapp,
		       QString version)
  :QDialog(parent,name,true)
{
  switch (koapp)
    {
    case KPresenter:
      {
	setCaption(i18n("KPresenter - About"));
	pLogo = BarIcon("koKPresenter");
	author = "Reginald Stadlbauer";
	email = "<reggie@kde.org>";
	add = i18n("WMF Clipart Support (c) by Stefan Taferner <taferner@kde.org>");
      } break;
    case KSpread:
      {
	setCaption(i18n("KSpread - About"));
	pLogo = BarIcon("koKSpread");
	author = "Torben Weis";
	email = "<weis@kde.org>";
	add = "";
      } break;
    case KCharts:
      {
	setCaption(i18n("KCharts - About"));
	pLogo = BarIcon("koKCharts");
	author = "Kalle Dalheimer and Torben Weis";
	email = "<weis@kde.org>";
	add = "";
      } break;
    case KImage:
      {
	setCaption(i18n("KImage - About"));
	pLogo = BarIcon("koKImage");
	author = "Torben Weis";
	email = "<weis@kde.org>";
	add = "";
      } break;
    case KAutoformEdit:
      {
	setCaption(i18n("KAutoformEdit - About"));
	pLogo = BarIcon("koKAutoformEdit");
	author = "Reginald Stadlbauer";
	email = "<reggie@kde.org>";
	add = "";
      } break;
    case KOffice:
      {
	setCaption(i18n("KOffice - About"));
	pLogo = BarIcon("koKOffice");
	author = i18n("the KOffice Team");
	email = i18n("<koffice@kde.org>");
	add = "";
      } break;
    default:
	warning("KoAboutDia can not handle case %d", koapp);
    }

  grid = new QGridLayout(this,2,2);

  lLogo = new QLabel(this);
  lLogo->setPixmap(pLogo);
  lLogo->resize(lLogo->sizeHint());
  grid->addWidget(lLogo,0,0);

  lInfo = new QLabel(this);
  QString infoText;

  infoText = i18n("Version: %1\n\n"
			"(c) by %1 1997 - 1998\n\n"
			"E-mail: %2 \n\n"
		  "The KOffice is under GNU GPL").arg(version).
                  arg(author).arg(email);
  lInfo->setText(infoText);

  if (!add.isEmpty())
    lInfo->setText(QString(lInfo->text()) + "\n\n" + add);
  lInfo->resize(lInfo->sizeHint());
  grid->addWidget(lInfo,0,1);

  bbox = new KButtonBox(this,KButtonBox::HORIZONTAL,7);
  bbox->addStretch(20);
  bOk = bbox->addButton(i18n("OK"));
  bOk->setAutoRepeat(false);
  bOk->setAutoResize(false);
  bOk->setAutoDefault(true);
  bOk->setDefault(true);
  connect(bOk,SIGNAL(clicked()),SLOT(accept()));
  bbox->layout();
  grid->addWidget(bbox,1,1);

  grid->addColSpacing(0,lLogo->width()+10);
  grid->addColSpacing(1,lInfo->width()+10);
  grid->addRowSpacing(0,lLogo->height()+10);
  grid->addRowSpacing(0,lInfo->height()+10);
  grid->addRowSpacing(1,bOk->height()+10);
  grid->activate();

  resize(0,0);
  setMaximumSize(size());
  setMinimumSize(size());
}

/*===================== destructor ===============================*/
KoAboutDia::~KoAboutDia()
{
}

/*======================= about application ======================*/
void KoAboutDia::about(KoApplication koapp,QString version)
{
  if (koapp != KDE)
    {
      KoAboutDia *dlg = new KoAboutDia(0,i18n("About"),koapp,version);
      dlg->exec();
      delete dlg;
    }
  else {
    ::KoApplication *app = dynamic_cast< ::KoApplication* >(KApplication::kApplication());
    ASSERT(app);
    app->aboutKDE();
  }
}

#include "koAboutDia.moc"
