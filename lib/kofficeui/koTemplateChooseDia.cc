/******************************************************************/
/* KoTemplateChooseDia - (c) by Reginald Stadlbauer 1997-1998     */
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
/* Module: Template Choose Dialog                                 */
/******************************************************************/

#include "koTemplateChooseDia.h"
#include "koTemplateChooseDia.moc"

#include <klocale.h>
#include <kbuttonbox.h>
#include <kfiledialog.h>

#include <qhbox.h>

/******************************************************************/
/* Class: KoTemplateChooseDia                                     */
/******************************************************************/

/*================================================================*/
KoTemplateChooseDia::KoTemplateChooseDia(QWidget *parent,const char *name,QString _globalTemplatePath,
					 QString _personalTemplatePath,bool _hasCancel,bool _onlyTemplates)
  : QDialog(parent,name,true), globalTemplatePath(_globalTemplatePath), 
    personalTemplatePath(_personalTemplatePath), onlyTemplates(_onlyTemplates)
{
  groupList.setAutoDelete(true);
  getGroups();
  setupTabs();

  KButtonBox *bb = new KButtonBox(back);
  bb->addStretch();
  ok = bb->addButton(i18n("OK"));
  connect(ok,SIGNAL(clicked()),this,SLOT(chosen()));
  ok->setDefault(true);
  if (_hasCancel)
  connect(bb->addButton(i18n("Cancel")),SIGNAL(clicked()),this,SLOT(reject()));
  bb->layout();
  bb->setMaximumHeight(bb->sizeHint().height());
  
  templateName = "";
  fullTemplateName = "";
  returnType = Cancel;
}

/*================================================================*/
KoTemplateChooseDia::ReturnType KoTemplateChooseDia::chooseTemplate(QString _globalTemplatePath,QString _personalTemplatePath,
								    QString &_template,bool _hasCancel,bool _onlyTemplates)
{
  bool res = false;
  KoTemplateChooseDia *dlg = new KoTemplateChooseDia(0,"Template",_globalTemplatePath,
						     _personalTemplatePath,_hasCancel,_onlyTemplates);

  dlg->resize(500,400);
  dlg->setCaption(i18n("Choose a Template"));
  
  if (dlg->exec() == QDialog::Accepted)
    {
      res = true;
      _template = dlg->getFullTemplate();
    }

  ReturnType rt = dlg->getReturnType();
  delete dlg;

  return res ? rt : KoTemplateChooseDia::Cancel;
}

/*================================================================*/
void KoTemplateChooseDia::getGroups()
{
  QString str;
  char* c = new char[256];
  QString templateDir = qstrdup(globalTemplatePath);

  QFile templateInf(globalTemplatePath + ".templates");

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

  templateDir = qstrdup(personalTemplatePath);

  QFile templateInf2(personalTemplatePath + ".templates");

  if (templateInf2.open(IO_ReadOnly))
    {
      while (!templateInf2.atEnd())
	{
	  templateInf2.readLine(c,256);
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

      templateInf2.close();
    }

  delete c;
}

/*================================================================*/
void KoTemplateChooseDia::setupTabs()
{
  back = new QVBox(this);
  back->setMargin(10);
      
  QFrame *line;
  
  if (!onlyTemplates)
    {
      line = new QFrame(back);
      line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
      line->setMaximumHeight(20);

      rbTemplates = new QRadioButton(i18n("Create new document from a &Template"),back);
    }
    
  
  
  if (!groupList.isEmpty())
    {
      tabs = new QTabWidget(back);
      
      for (grpPtr = groupList.first();grpPtr != 0;grpPtr = groupList.next())
	{
	  grpPtr->tab = new QVBox(tabs);
	  grpPtr->loadWid = new MyIconCanvas(grpPtr->tab);
  	  grpPtr->loadWid->loadDir(grpPtr->dir.absFilePath(),"*.xpm");
	  grpPtr->loadWid->setBackgroundColor(colorGroup().base());
	  grpPtr->loadWid->show();
 	  connect(grpPtr->loadWid,SIGNAL(nameChanged(const QString &)),
 		  this,SLOT(nameChanged(const QString &)));
	  connect(grpPtr->loadWid,SIGNAL(currentChanged(const QString &)),
		  this,SLOT(currentChanged(const QString &)));
	  grpPtr->label = new QLabel(grpPtr->tab);
	  grpPtr->label->setText(" ");
	  grpPtr->label->setMaximumHeight(grpPtr->label->sizeHint().height());
 	  tabs->addTab(grpPtr->tab,grpPtr->name);
	}
      connect(tabs,SIGNAL(selected(const QString &)),this,SLOT(tabsChanged(const QString &)));
    }

  line = new QFrame(back);
  line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  line->setMaximumHeight(20);

  if (!onlyTemplates)
    {
      rbFile = new QRadioButton(i18n("&Open an existing document"),back);
      connect(rbFile,SIGNAL(clicked()),this,SLOT(openFile()));

      QHBox *row = new QHBox(back);
      row->setMargin(5);
      lFile = new QLabel(i18n("No File"),row);
      lFile->setFrameStyle(QFrame::Panel | QFrame::Sunken);
      bFile = new QPushButton(i18n("Choose..."),row);
      lFile->setMaximumHeight(bFile->sizeHint().height());
      bFile->setMaximumSize(bFile->sizeHint());
      row->setMaximumHeight(bFile->sizeHint().height() + 10);
      connect(bFile,SIGNAL(clicked()),this,SLOT(chooseFile()));

      line = new QFrame(back);
      line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
      line->setMaximumHeight(20);

      rbEmpty = new QRadioButton(i18n("Start with an &empty document"),back);
      connect(rbEmpty,SIGNAL(clicked()),this,SLOT(openEmpty()));

      line = new QFrame(back);
      line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
      line->setMaximumHeight(20);

      connect(rbTemplates,SIGNAL(clicked()),this,SLOT(openTemplate()));
      rbTemplates->setChecked(true);
    }
}

/*================================================================*/
void KoTemplateChooseDia::nameChanged(const QString &name)
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

/*================================================================*/
void KoTemplateChooseDia::currentChanged(const QString &name)
{
  openTemplate();
}

/*================================================================*/
void KoTemplateChooseDia::chosen()
{
  if (onlyTemplates || !onlyTemplates && rbTemplates->isChecked())
    {
      returnType = Template;
		   
      if (!groupList.isEmpty())
	{
	  for (grpPtr = groupList.first();grpPtr != 0;grpPtr = groupList.next())
	    {
	      if (grpPtr->tab->isVisible() && !grpPtr->loadWid->getCurrent().isEmpty())
		{
		  emit templateChosen(QString(grpPtr->name + "/" + grpPtr->loadWid->getCurrent()));
		  templateName = QString(grpPtr->name + "/" + grpPtr->loadWid->getCurrent());
		  fullTemplateName = QString(grpPtr->dir.dirPath(true) + "/" + grpPtr->name + "/" + grpPtr->loadWid->getCurrent());
		  accept();
		}
	    }
	}
    }
  else if (!onlyTemplates && rbFile->isChecked())
    {
      returnType = File;
      
      fullTemplateName = templateName = lFile->text();
      accept();
    }
  else if (!onlyTemplates && rbEmpty->isChecked())
    {
      returnType = Empty;
      accept();
    }
  else
    {
      returnType = Cancel;
      reject();
    }
}

/*================================================================*/
void KoTemplateChooseDia::openTemplate()
{
  if (!onlyTemplates)
    {
      rbTemplates->setChecked(true);
      rbFile->setChecked(false);
      rbEmpty->setChecked(false);
    }
  
  if (isVisible())
    ok->setEnabled(false);
  
  if (!groupList.isEmpty())
    {
      for (grpPtr = groupList.first();grpPtr != 0;grpPtr = groupList.next())
	{
	  if (grpPtr->tab->isVisible() && grpPtr->loadWid->isCurrentValid())
	    ok->setEnabled(true);
	}
    }
}

/*================================================================*/
void KoTemplateChooseDia::openFile()
{
  rbTemplates->setChecked(false);
  rbFile->setChecked(true);
  rbEmpty->setChecked(false);

  ok->setEnabled(QFile::exists(lFile->text()));
}

/*================================================================*/
void KoTemplateChooseDia::openEmpty()
{
  rbTemplates->setChecked(false);
  rbFile->setChecked(false);
  rbEmpty->setChecked(true);

  ok->setEnabled(true);
}

/*================================================================*/
void KoTemplateChooseDia::chooseFile()
{
  openFile();

  QString dir = QString::null;
  if (QFile::exists(lFile->text()))
    dir = QFileInfo(lFile->text()).absFilePath();
  
  QString filename = KFileDialog::getOpenFileName(dir);
  if (!filename.isEmpty() && QFileInfo(filename).isFile() ||
      (QFileInfo(filename).isSymLink() && !QFileInfo(filename).readLink().isEmpty() &&
       QFileInfo(QFileInfo(filename).readLink()).isFile()))
      lFile->setText(filename);
  
  openFile();
}
