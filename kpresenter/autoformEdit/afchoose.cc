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
#include <kglobal.h>
#include <kstddirs.h>

/******************************************************************/
/* class AFChoose                                                 */
/******************************************************************/

/*==================== constructor ===============================*/
AFChoose::AFChoose(QWidget *parent,const char *name)
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
  QString str;
  char c[256];

  QStringList autoformDirs = KGlobal::dirs()->getResourceDirs("autoforms");
  for (QStringList::ConstIterator it = autoformDirs.begin();
       it != autoformDirs.end(); it++) {

    QFile afInf(*it + ".autoforms");

    if (afInf.open(IO_ReadOnly))
    {
      while (!afInf.atEnd())
	{
	  afInf.readLine(c,256);
	  str = QString(c).stripWhiteSpace();
	  if (!str.isEmpty())
	    {
	      grpPtr = new Group;
	      grpPtr->dir.setFile(*it + str + "/");
	      grpPtr->name = str;
	      groupList.append(grpPtr);
	    }
	  strcpy(c,"");
	}

      afInf.close();
    }
  }
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
	  grpPtr->loadWid->show();
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
