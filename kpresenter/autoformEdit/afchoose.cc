/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Autoform Choose Dialog                                 */
/******************************************************************/

#include "afchoose.h"
#include <klocale.h>
#include "afchoose.moc"
#include <qvbox.h>

/******************************************************************/
/* class AFChoose                                                 */
/******************************************************************/

/*==================== constructor ===============================*/
AFChoose::AFChoose(QWidget *parent=0,const char *name=0)
  :QTabDialog(parent,name,true)
{
  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("OK"));
  groupList.setAutoDelete(true);
  getGroups();
  setupTabs();
  connect(this,SIGNAL(applyButtonPressed()),this,SLOT(chosen()));
}

/*===================== destrcutor ===============================*/
AFChoose::~AFChoose()
{
}

/*======================= get Groups =============================*/
void AFChoose::getGroups()
{
  // global autoforms
  QString afDir = qstrdup(KApplication::kde_datadir());
  afDir += "/kpresenter/autoforms/";
  QString str;
  char* c = new char[256];

  QFile afInf(afDir + ".autoforms");

  if (afInf.open(IO_ReadOnly))
    {
      while (!afInf.atEnd())
	{
	  afInf.readLine(c,256);
	  str = c;
	  str = str.stripWhiteSpace();
	  if (!str.isEmpty())
	    {
	      grpPtr = new Group;
	      grpPtr->dir.setFile(afDir + QString(c).stripWhiteSpace() + "/");
	      grpPtr->name = QString(qstrdup(c)).stripWhiteSpace();
	      groupList.append(grpPtr);
	    }
	  strcpy(c,"");
	}

      afInf.close();
    }

  delete c;
}

/*======================= setup Tabs =============================*/
void AFChoose::setupTabs()
{
  if (!groupList.isEmpty())
    {
      for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
	{
	  grpPtr->tab = new QVBox(this);
 	  grpPtr->loadWid = new KIconLoaderCanvas(grpPtr->tab);
  	  grpPtr->loadWid->loadDir(grpPtr->dir.absFilePath(),"*.xpm");
	  grpPtr->loadWid->setBackgroundColor(colorGroup().base());
 	  connect(grpPtr->loadWid,SIGNAL(nameChanged(const QString &)),
 		  this,SLOT(nameChanged(const QString &)));
// 	  connect(grpPtr->loadWid,SIGNAL(doubleClicked()),
// 		  this,SLOT(chosen()));
//  	  connect(grpPtr->loadWid,SIGNAL(doubleClicked()),
//  		  this,SLOT(accept()));
	  grpPtr->label = new QLabel(grpPtr->tab);
	  grpPtr->label->setText(" ");
	  grpPtr->label->setMaximumHeight(grpPtr->label->sizeHint().height());
	  //grpPtr->tab->setMinimumSize(400,300);
 	  addTab(grpPtr->tab,grpPtr->name);
	}
    }
}

/*====================== resize event ============================*/
void AFChoose::resizeEvent(QResizeEvent *e)
{
  QTabDialog::resizeEvent(e);
//   if (!groupList.isEmpty())
//     {
//       for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
// 	{
// 	  grpPtr->loadWid->resize(grpPtr->tab->width(),grpPtr->tab->height()-30);
// 	  grpPtr->label->setGeometry(10,grpPtr->tab->height()-30,
// 				     grpPtr->tab->width()-10,30);
// 	}
//     }
}

/*====================== name changed ===========================*/
void AFChoose::nameChanged(const QString & name)
{
  QFileInfo fi(name);

  if (!groupList.isEmpty())
    {
      for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
	{
	  grpPtr->label->setText(fi.baseName());
	  if (grpPtr->label->text().isEmpty())
	    grpPtr->label->setText(" ");
	}
    }
}

/*======================= form chosen ==========================*/
void AFChoose::chosen()
{
  if (!groupList.isEmpty())
    {
      for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
	{
	  if (grpPtr->tab->isVisible() && !grpPtr->loadWid->getCurrent().isEmpty())
	    emit formChosen(QString(grpPtr->name + "/" + grpPtr->loadWid->getCurrent()));
	}
    }
}
