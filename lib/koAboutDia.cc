/******************************************************************/
/* KOffice Library - (c) by Reginald Stadlbauer 1998              */
/* Version: 1.0                                                   */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: About Dialog                                           */
/******************************************************************/

#include "koAboutDia.h"
#include "koAboutDia.moc"

/******************************************************************/
/* class KoAboutDia                                               */
/******************************************************************/

/*==================== constructor ===============================*/
KoAboutDia::KoAboutDia(QWidget* parent=0,const char* name=0,KoApplication koapp=KDE)
  :QDialog(parent,name,true)
{
  switch (koapp)
    {
    case KPresenter: 
      {
	setCaption("KPresenter - About"); 
	pLogo.load(KApplication::kde_datadir()+"/koffice/pics/koKPresenter.xpm");
	author = "Reginald Stadlbauer";
	email = "<reggie@kde.org>";
	add = "WMF Clipart Support (c) by Stefan Taferner <taferner@kde.org>";
      } break;
    case KSpread: 
      {
	setCaption("KSpread - About"); 
	pLogo.load(KApplication::kde_datadir()+"/koffice/pics/koKSpread.xpm");
	author = "Torben Weis";
	email = "<weis@kde.org>";
	add = "";
      } break;
    case KCharts: 
      {
	setCaption("KCharts - About");
	pLogo.load(KApplication::kde_datadir()+"/koffice/pics/koKCharts.xpm");
	author = "Torben Weis";
	email = "<weis@kde.org>";
	add = "";
      } break;
    case KImage:
      {
	setCaption("KImage - About");
	pLogo.load(KApplication::kde_datadir()+"/koffice/pics/koKImage.xpm");
	author = "Torben Weis";
	email = "<weis@kde.org>";
	add = "";
      } break;
    case KAutoformEdit: 
      {
	setCaption("KAutoformEdit - About");
	pLogo.load(KApplication::kde_datadir()+"/koffice/pics/koKAutoformEdit.xpm");
	author = "Reginald Stadlbauer";
	email = "<reggie@kde.org>";
	add = "";
      } break;
    case KOffice: 
      {
	setCaption("KOffice - About");
	pLogo.load(KApplication::kde_datadir()+"/koffice/pics/koKOffice.xpm");
	author = "Torben Weis and Reginald Stadlbauer";
	email = "<weis@kde.org> and <reggie@kde.org>";
	add = "";
      } break;
    }
  
  grid = new QGridLayout(this,2,2);

  lLogo = new QLabel(this);
  lLogo->setPixmap(pLogo);
  lLogo->resize(lLogo->sizeHint());
  grid->addWidget(lLogo,0,0);
  
  lInfo = new QLabel(this);
  lInfo->setText("(c) by " + author + " 1997 - 1998\n\n"
		 "E-Mail: " + email + "\n\n"
		 "The KOffice is under GNU GPL");
  if (!add.isEmpty())
    lInfo->setText(QString(lInfo->text()) + "\n\n\n" + add);
  lInfo->resize(lInfo->sizeHint());
  grid->addWidget(lInfo,0,1);

  bbox = new KButtonBox(this,KButtonBox::HORIZONTAL,7);
  bbox->addStretch(20);
  bOk = bbox->addButton("OK");
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
void KoAboutDia::about(KoApplication koapp)
{
  if (koapp != KDE)
    {
      KoAboutDia *dlg = new KoAboutDia(0,"About",koapp);
      dlg->exec();
      delete dlg;
    }
  else
    {
      QMessageBox::about(0,klocale->translate(klocale->translate("About KDE")),
			 klocale->translate("\nThe KDE Desktop Environment was written by the KDE Team,\n"
					    "a world-wide network of software engineers commited to\n"
					    "free software development.\n\n"
					    "Visit http://www.kde.org for more information on the KDE\n"
					    "Project. Please consider joining and supporting KDE.\n\n"
					    "Please report bugs at http://buglist.kde.org.\n"));
    }
}
