/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0alpha                                            */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Template Choose Dialog                                 */
/******************************************************************/

#include "koTemplateChooseDia.h"
#include "koTemplateChooseDia.moc"

/******************************************************************/
/* Class: KoTemplateChooseDia                                     */
/******************************************************************/

/*==================== constructor ===============================*/
KoTemplateChooseDia::KoTemplateChooseDia(QWidget *parent,const char *name,QString _templatePath)
  : QTabDialog(parent,name,true), templatePath(_templatePath)
{
  setOKButton(i18n("OK"));
  groupList.setAutoDelete(true);
  getGroups();
  setupTabs();
  connect(this,SIGNAL(applyButtonPressed()),this,SLOT(chosen()));
  templateName = "";
}

/*================================================================*/
bool KoTemplateChooseDia::chooseTemplate(QString _templatePath,QString &_template)
{
  bool res = false;
  KoTemplateChooseDia *dlg = new KoTemplateChooseDia(0,"Template",_templatePath);

  dlg->resize(400,300);
  dlg->setCaption(i18n("Choose a Template"));

  if (dlg->exec() == QDialog::Accepted)
    {
      res = true;
      _template = dlg->getTemplate();
    }

  delete dlg;

  return res;
}

/*======================= get Groups =============================*/
void KoTemplateChooseDia::getGroups()
{
  QString str;
  char* c = new char[256];
  QString templateDir = qstrdup(templatePath);
  
  QFile templateInf(templatePath + ".templates");

  if (templateInf.open(IO_ReadOnly))
    {
      while (!templateInf.atEnd())
	{
	  templateInf.readLine(c,256);
	  str = c;
	  str = str.stripWhiteSpace();
	  if (!str.isEmpty())
	    {
	      grpPtr = new Group;
	      grpPtr->dir.setFile(templateDir + QString(c).stripWhiteSpace() + "/");
	      grpPtr->name = QString(qstrdup(c)).stripWhiteSpace();
	      groupList.append(grpPtr);
	    }
	  strcpy(c,"");
	}
      
      templateInf.close();
    }

  delete c;
}

/*======================= setup Tabs =============================*/
void KoTemplateChooseDia::setupTabs()
{
  if (!groupList.isEmpty())
    {
      for (grpPtr = groupList.first();grpPtr != 0;grpPtr = groupList.next())
	{
	  grpPtr->tab = new QWidget(this);
 	  grpPtr->loadWid = new KIconLoaderCanvas(grpPtr->tab);
  	  grpPtr->loadWid->loadDir(grpPtr->dir.absFilePath(),"*.xpm");
 	  grpPtr->loadWid->move(0,0);
	  grpPtr->loadWid->setBackgroundColor(kapp->windowColor);
 	  connect(grpPtr->loadWid,SIGNAL(nameChanged(const char*)),
 		  this,SLOT(nameChanged(const char*)));
	  connect(grpPtr->loadWid,SIGNAL(doubleClicked()),
		  this,SLOT(chosen()));
	  connect(grpPtr->loadWid,SIGNAL(doubleClicked()),
		  this,SLOT(accept()));
	  grpPtr->label = new QLabel(grpPtr->tab);
	  grpPtr->tab->setMinimumSize(400,300);
 	  addTab(grpPtr->tab,grpPtr->name);
	}
    }
}  

/*====================== resize event ============================*/
void KoTemplateChooseDia::resizeEvent(QResizeEvent *e)
{
  QTabDialog::resizeEvent(e);
  if (!groupList.isEmpty())
    {
      for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
	{
	  grpPtr->loadWid->resize(grpPtr->tab->width(),grpPtr->tab->height()-30);
	  grpPtr->label->setGeometry(10,grpPtr->tab->height()-30,
				     grpPtr->tab->width()-10,30);
	}
    }
}

/*====================== name changed ===========================*/
void KoTemplateChooseDia::nameChanged(const char* name)
{
  QFileInfo fi(name);

  if (!groupList.isEmpty())
    {
      for (grpPtr=groupList.first();grpPtr != 0;grpPtr=groupList.next())
	grpPtr->label->setText(fi.baseName());
    }
}

/*======================= template chosen =======================*/
void KoTemplateChooseDia::chosen()
{
  if (!groupList.isEmpty())
    {
      for (grpPtr = groupList.first();grpPtr != 0;grpPtr = groupList.next())
	{
	  if (grpPtr->tab->isVisible() && !grpPtr->loadWid->getCurrent().isEmpty()) 
	    {
	      emit templateChosen(static_cast<const char*>(QString(grpPtr->name + "/" + grpPtr->loadWid->getCurrent())));
	      templateName = QString(grpPtr->name + "/" + grpPtr->loadWid->getCurrent());
	    }
	}
    }
}
