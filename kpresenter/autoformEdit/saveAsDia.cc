/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Autoform Editor                                                */
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
/* Module: save as dialog                                         */
/******************************************************************/

#include "saveAsDia.h"
#include "saveAsDia.moc"

/******************************************************************/
/* class SaveAsDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
SaveAsDia::SaveAsDia(QWidget* parent=0,const char* name=0)
  :QDialog(parent,name,true)
{
  groupList.setAutoDelete(true);
  getGroups();
  
  grpLabel = new QLabel("Choose autoform group",this);
  grpLabel->resize(grpLabel->sizeHint());
  grpLabel->move(20,10);

  groups = new QListBox(this);
  groups->insertStrList(&groupList);
  groups->resize(grpLabel->width(),100);
  groups->move(grpLabel->x(),grpLabel->y()+grpLabel->height()+10);
  
  grpEdit = new QLineEdit(this);
  grpEdit->resize(grpLabel->width(),grpEdit->sizeHint().height());
  grpEdit->move(grpLabel->x(),groups->y()+groups->height()+20);

  addGrp = new QPushButton("Add group",this);
  addGrp->resize(grpLabel->width(),addGrp->sizeHint().height());
  addGrp->move(grpLabel->x(),grpEdit->y()+grpEdit->height()+10);

  nameLabel = new QLabel("Input autoform name (without extension)",this);
  nameLabel->resize(nameLabel->sizeHint());
  nameLabel->move(grpLabel->x()+grpLabel->width()+20,grpLabel->y());

  nameEdit = new QLineEdit(this);
  nameEdit->resize(nameLabel->width(),grpEdit->sizeHint().height());
  nameEdit->move(nameLabel->x(),nameLabel->y()+nameLabel->height()+10);

  cancelBut = new QPushButton(this);
  cancelBut->setText("Cancel");
  cancelBut->resize(cancelBut->sizeHint());
    
  okBut = new QPushButton(this);
  okBut->setText("OK");
  okBut->setAutoRepeat(false);
  okBut->setAutoResize(false);
  okBut->setAutoDefault(true);
  okBut->setDefault(true);
  okBut->resize(cancelBut->width(),okBut->sizeHint().height());
    
  cancelBut->move(nameLabel->x()+nameLabel->width()-cancelBut->width(),addGrp->y()+addGrp->height()+20);
  okBut->move(cancelBut->x()-10-okBut->width(),cancelBut->y());

  connect(okBut,SIGNAL(clicked()),this,SLOT(accept()));
  connect(okBut,SIGNAL(clicked()),this,SLOT(saveAs()));
  connect(cancelBut,SIGNAL(clicked()),this,SLOT(reject()));
  connect(addGrp,SIGNAL(clicked()),this,SLOT(addGroup()));

  resize(cancelBut->x()+cancelBut->width()+20,okBut->y()+okBut->height()+10);
}

/*===================== destructor ===============================*/
SaveAsDia::~SaveAsDia()
{
}

/*======================= get Groups =============================*/
void SaveAsDia::getGroups()
{
  // global autoforms
  QString afDir = qstrdup(KApplication::kde_datadir());
  afDir += "/kpresenter/autoforms/";
  
  char* c = new char[256];
  QString str;
  
  QFile afInf(afDir + ".autoforms");
  afInf.open(IO_ReadOnly);
  
  while (!afInf.atEnd())
    {
      afInf.readLine(c,256);
      str = c;
      str = str.stripWhiteSpace();
      if (!str.isEmpty())
	groupList.append(qstrdup(str));
      strcpy(c,"");
    }

  afInf.close();
  delete c;
}

/*============================= ok clicked ========================*/
void SaveAsDia::saveAs()
{
  QString fileName = nameEdit->text();
  fileName = fileName.stripWhiteSpace();
  QString grpName;

  if (groups->currentItem() != -1 && !fileName.isEmpty())
    { 
      grpName = groups->text(groups->currentItem());
      emit saveATFAs(grpName.data(),fileName.data());
    }
}

/*======================= add a group =============================*/
void SaveAsDia::addGroup()
{
  const char *s = grpEdit->text();
  QString str(s);
  QString str2;
  QString afDir = qstrdup(KApplication::kde_datadir());
  afDir += "/kpresenter/autoforms/";

  QFile afInf(afDir + ".autoforms");

  str = str.stripWhiteSpace();

  if (!str.isEmpty() && groupList.find(str) == -1)
    {
      groups->insertItem(qstrdup(str));
      groupList.append(qstrdup(str));
      afInf.open(IO_WriteOnly);
      for (unsigned int i=0;i < groupList.count();i++)
	{
	  str2 = groupList.at(i);
	  str2 = str2.stripWhiteSpace();
	  str2 += "\n";
	  afInf.writeBlock(str2.data(),str2.length());
	}
      afInf.close();
      afDir += str;
      QString cmd = "mkdir -p " + afDir;
      system(cmd.data());
      grpEdit->setText("");
    }
}
