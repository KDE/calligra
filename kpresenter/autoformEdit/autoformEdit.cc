/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Autoform Editor                                                */
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
/* Module: Mainwidget                                             */
/******************************************************************/

#include "autoformEdit.h"
#include "autoformEdit.moc"

/*================================================================*/
/* Class: AEditWin                                                */
/*================================================================*/

/*====================== constructor =============================*/
AEditWin::AEditWin(const char *name=0)
  : KTopLevelWidget(name)
{
  groupName = "";
  fileName = "";
  atfInterpret = new ATFInterpreter(this,"");
  setupMenu();
  setupToolbar1();
  setupStatusbar();
  setupPanner();
  setCaption("KPresenter Autoform Editor");
  setMinimumSize(350,350);
  relation = true;
  pntInsDia = 0;
  afChoose = 0;
  saveAsDia = 0;
}

/*======================= destrcutor =============================*/
AEditWin::~AEditWin()
{
  delete file;
  delete edit;
  delete extra;
  delete help;
  delete menu;
  delete toolbar1;
  delete statBar;
  delete drawWid;
  delete editWid;
  delete panner;
  delete atfInterpret;
}

/*====================== setup menu ==============================*/
void AEditWin::setupMenu()
{
  QString pixdir,tmp;
  QPixmap pixmap;

  /* file menu */
  file = new QPopupMenu();
  CHECK_PTR(file);
  pixdir = KApplication::kde_toolbardir();  
  pixmap.load(pixdir+"/filenew.xpm");
  M_FNEW = file->insertItem(pixmap,"&New",this,SLOT(fileNew()),CTRL+Key_N);
  pixmap.load(pixdir+"/fileopen.xpm");
  M_FOPEN = file->insertItem(pixmap,"&Open...",this,SLOT(fileOpen()),CTRL+Key_O);
  file->insertSeparator();
  pixmap.load(pixdir+"/filefloppy.xpm");
  M_FSAVE = file->insertItem(pixmap,"&Save",this,SLOT(fileSave()),CTRL+Key_S);
  M_FSAS = file->insertItem("S&ave as...",this,SLOT(fileSaveAs()),ALT+Key_S);
  file->insertSeparator();
  M_FWIN = file->insertItem("New &Window",this,SLOT(fileNewWin()),ALT+Key_N);
  file->insertSeparator();
  M_FCLOSE = file->insertItem("&Close",this,SLOT(fileClose()),ALT+Key_C);
  M_FEXIT = file->insertItem("&Quit",this,SLOT(fileQuit()),CTRL+Key_Q);
  file->setMouseTracking(true);
  /* edit menu */
  edit = new QPopupMenu();
  CHECK_PTR(edit);
  pixdir = KApplication::kde_toolbardir();  
  pixmap.load(pixdir+"/editcut.xpm");
  M_ECUT = edit->insertItem(pixmap,"&Cut",this,SLOT(editCut()),CTRL+Key_X);
  pixmap.load(pixdir+"/editcopy.xpm");
  M_ECOPY = edit->insertItem(pixmap,"C&opy",this,SLOT(editCopy()),CTRL+Key_C);
  pixmap.load(pixdir+"/editpaste.xpm");
  M_EPASTE = edit->insertItem(pixmap,"&Paste",this,SLOT(editPaste()),CTRL+Key_V);
  pixdir = KApplication::kde_datadir();  
  pixmap.load(pixdir+"/kpresenter/toolbar/delete.xpm");
  M_EDELETE = edit->insertItem(pixmap,"&Delete",this,SLOT(editDelete()),CTRL+Key_Delete);
  edit->setMouseTracking(true);
  /* extra menu */
  extra = new QPopupMenu();
  CHECK_PTR(extra);
  pixdir = KApplication::kde_datadir();  
  pixmap.load(pixdir+"/kpresenter/toolbar/newPoint.xpm");
  M_EINSPNT = extra->insertItem(pixmap,"&Insert Point...",this,SLOT(extraInsertPoint()),ALT+Key_I);
  extra->insertSeparator();
  pixmap.load(pixdir+"/kpresenter/toolbar/121.xpm");
  M_E121 = extra->insertItem(pixmap,"&View autoform in 1:1",this,SLOT(extraOne2One()),CTRL+Key_R);
  extra->setItemChecked(M_E121,true);
  extra->setMouseTracking(true);
  extra->setCheckable(true);
  /* help menu */
  help = new QPopupMenu();
  CHECK_PTR(help);
  M_HHELP = help->insertItem("&Contents...",this,SLOT(helpHelp()),Key_F1);
  help->insertSeparator();
  M_HABOUTKPA = help->insertItem("&About KAutoformEdit...",this,SLOT(helpAbout()));
  M_HABOUTKOFFICE = help->insertItem("About &KOffice...",this,SLOT(helpAboutKOffice()));
  M_HABOUTKDE = help->insertItem("About &KDE...",this,SLOT(helpAboutKDE()));
  help->setMouseTracking(true);
  /* menuber */
  menu = new KMenuBar(this);
  M_FILE = menu->insertItem("&File",file);
  M_EDIT = menu->insertItem("&Edit",edit);
  M_EXTRA = menu->insertItem("E&xtra",extra);
  menu->insertSeparator();
  M_HELP = menu->insertItem("&Help",help);
  menu->setMouseTracking(true);
  setMenu(menu);
  /* connect events */
  timer = new QTimer();
  connect(menu,SIGNAL(highlighted(int)),this,SLOT(setInfoText(int)));
  connect(file,SIGNAL(highlighted(int)),this,SLOT(setInfoText(int)));
  connect(edit,SIGNAL(highlighted(int)),this,SLOT(setInfoText(int)));
  connect(extra,SIGNAL(highlighted(int)),this,SLOT(setInfoText(int)));
  connect(help,SIGNAL(highlighted(int)),this,SLOT(setInfoText(int)));
  connect(timer,SIGNAL(timeout()),this,SLOT(checkMenu()));
}

/*====================== setup toolbar1 ==========================*/
void AEditWin::setupToolbar1()
{
  QString pixdir,tmp;
  QPixmap pixmap;
  QStrList comboList;
  int i;
  
  /* create toolbar */
  toolbar1 = new KToolBar(this);
  /* insert buttons */
  pixdir = KApplication::kde_toolbardir();  
  pixmap.load(pixdir+"/filenew.xpm");
  toolbar1->insertButton(pixmap,0,
			 SIGNAL(clicked()),this,
			 SLOT(fileNew()),true,"Create a new autoform in that window");
  pixmap.load(pixdir+"/fileopen.xpm");
  toolbar1->insertButton(pixmap,0,
			 SIGNAL(clicked()),this,
			 SLOT(fileOpen()),true,"Open an autoform in that window");
  pixmap.load(pixdir+"/filefloppy.xpm");
  toolbar1->insertButton(pixmap,0,
			 SIGNAL(clicked()),this,
			 SLOT(fileSave()),true,"Save current autoform");
  toolbar1->insertSeparator();
  pixmap.load(pixdir+"/editcut.xpm");
  toolbar1->insertButton(pixmap,0,
			 SIGNAL(clicked()),this,
			 SLOT(editCut()),true,"Cut to the clipboard");
  pixmap.load(pixdir+"/editcopy.xpm");
  toolbar1->insertButton(pixmap,0,
			 SIGNAL(clicked()),this,
			 SLOT(editCopy()),true,"Copy to the clipboard");
  pixmap.load(pixdir+"/editpaste.xpm");
  toolbar1->insertButton(pixmap,0,
			 SIGNAL(clicked()),this,
			 SLOT(editPaste()),true,"Paste from the clipboard");
  pixdir = KApplication::kde_datadir() + QString("/kpresenter/toolbar/");  
  pixmap.load(pixdir+"/delete.xpm");
  toolbar1->insertButton(pixmap,0,
			 SIGNAL(clicked()),this,
			 SLOT(editDelete()),true,"Delete selected point");
  toolbar1->insertSeparator();
  pixmap.load(pixdir+"newPoint.xpm");
  toolbar1->insertButton(pixmap,0,
			 SIGNAL(clicked()),this,
			 SLOT(extraInsertPoint()),true,"Insert a new point");
  pixmap.load(pixdir+"121.xpm");
  toolbar1->insertButton(pixmap,T_RELATION,
			 SIGNAL(clicked()),this,
			 SLOT(extraOne2One()),true,"Switch relation of the autoform viewer");
  toolbar1->setToggle(T_RELATION,true);
  toolbar1->setButton(T_RELATION,true);
  toolbar1->insertSeparator();
  for (i=1;i <= 10;i++)
    comboList.append(tmp.setNum(i));
  toolbar1->insertCombo(&comboList,99,false,
			SIGNAL(activated(int)),this,
			SLOT(setPenWidth(int)),true,"Set pen width for the autoform viewer",70);
  toolbar1->setBarPos(KToolBar::Top);
  toolbar1->setFullWidth(true);
  addToolBar(toolbar1);
}

/*======================== setup Statusbar =======================*/
void AEditWin::setupStatusbar()
{
  statBar = new KStatusBar(this);
  statBar->setInsertOrder(KStatusBar::RightToLeft);
  statBar->insertItem("1234567890ABZDEFGHIJKLMNOPQRSTUVWXYZ1234567890AAAAAAAAAAAAAAA",ST_INFO);
  statBar->changeItem("",ST_INFO);
  setStatusBar(statBar);
}

/*======================== setup drawWidget ======================*/
void AEditWin::setupPanner()
{
  panner = new KNewPanner(this);
  drawWid = new DrawWidget(panner);
  editWid = new EditWidget(panner);
  panner->activate(drawWid,editWid);
  panner->resize(size());
  setView(panner);
  connect(editWid,SIGNAL(getSource()),this,SLOT(sendSource()));
  connect(drawWid,SIGNAL(getPntArry(int,int)),this,SLOT(sendPntArry(int,int)));
  connect(editWid,SIGNAL(changeVar(int,int,int,QString)),this,SLOT(chnVar(int,int,int,QString)));
  connect(editWid,SIGNAL(delPnt(int)),this,SLOT(delPoint(int)));
  sendSource();
  sendPntArry(drawWid->aW(),drawWid->aH());
}

/*======================= file new ===============================*/
void AEditWin::fileNew()
{
  if (editWid->isChanged())
    {
      switch(QMessageBox::warning(this,"KPresenter Autoformedit",
                                  "The current autoform has been changed but\n"
                                  "not saved. Do you really want to reject the\n"
                                  "changes and open a new autoform?\n\n",
                                  "Yes", "No",
                                  0,1))
	{
	case 0: break;
	case 1: return; break;
	}
    }
  groupName = "";
  fileName = "";
  if (atfInterpret) atfInterpret->newAutoform();
  sendSource();
  sendPntArry(drawWid->aW(),drawWid->aH());  
}

/*======================= file open ==============================*/
void AEditWin::fileOpen()
{
  if (editWid->isChanged())
    {
      switch(QMessageBox::warning(this,"KPresenter Autoformedit",
                                  "The current autoform has been changed but\n"
                                  "not saved. Do you really want to reject the\n"
                                  "changes and open another autoform?\n\n",
                                  "Yes", "No",
                                  0,1))
	{
	case 0: break;
	case 1: return; break;
	}
    }
  if (afChoose)
    {
      disconnect(afChoose,SIGNAL(formChosen(const char*)),this,SLOT(afChooseOk(const char*)));
      afChoose->close();
      delete afChoose;
      afChoose = 0;
    }
  afChoose = new AFChoose(0,"Autoform-Choose");
  afChoose->setCaption("Open an Autoform");
  afChoose->setMaximumSize(afChoose->width(),afChoose->height());
  afChoose->setMinimumSize(afChoose->width(),afChoose->height());
  connect(afChoose,SIGNAL(formChosen(const char*)),this,SLOT(afChooseOk(const char*)));
  afChoose->show();
}

/*======================= file save ==============================*/
void AEditWin::fileSave()
{
  if (!groupName.isEmpty() && !fileName.isEmpty())
    {
      QString afDir = qstrdup(KApplication::kde_datadir());
      afDir += "/kpresenter/autoforms/";
      QString atfName(afDir + groupName + "/" + fileName);
      if (atfInterpret) atfInterpret->save(atfName);
      editWid->saved();
      QFileInfo fi(atfName);
      QString pixName(afDir + groupName + "/" + fi.baseName() + ".xpm"); 
      if (drawWid) drawWid->createPixmap(pixName);
    }
  else fileSaveAs();
}

/*===================== file save as =============================*/
void AEditWin::fileSaveAs()
{
  if (saveAsDia)
    {
      disconnect(saveAsDia,SIGNAL(saveATFAs(const char*,const char*)),
		 this,SLOT(saveAsDiaOk(const char*,const char*)));
      saveAsDia->close();
      delete saveAsDia;
      saveAsDia = 0;
    }
  saveAsDia = new SaveAsDia(0,"Save-As");
  connect(saveAsDia,SIGNAL(saveATFAs(const char*,const char*)),
	  this,SLOT(saveAsDiaOk(const char*,const char*)));
  saveAsDia->setCaption("Save autoform as");
  saveAsDia->setMaximumSize(saveAsDia->width(),saveAsDia->height());
  saveAsDia->setMinimumSize(saveAsDia->width(),saveAsDia->height());
  saveAsDia->show();
 }

/*======================= file newWin ============================*/
void AEditWin::fileNewWin()
{
}

/*======================= file close =============================*/
void AEditWin::fileClose()
{
  close();
}

/*======================= file exit ==============================*/
void AEditWin::fileQuit()
{
  exit(0);
}

/*======================= edit cut ===============================*/
void AEditWin::editCut()
{
}

/*======================= edit copy ==============================*/
void AEditWin::editCopy()
{
}

/*======================= edit paste =============================*/
void AEditWin::editPaste()
{
}

/*======================= edit delete ============================*/
void AEditWin::editDelete()
{
  editWid->deletePoint();
}

/*======================= edit sel all ===========================*/
void AEditWin::editSelAll()
{
}

/*======================= extra insert point =====================*/
void AEditWin::extraInsertPoint()
{
  if (atfInterpret->getNumOfPoints() > 0)
    {
      if (pntInsDia)
	{
	  disconnect(pntInsDia,SIGNAL(insPoint(int,bool)),this,SLOT(insPntOk(int,bool)));
	  pntInsDia->close();
	  delete pntInsDia;
	  pntInsDia = 0;
	}
      pntInsDia = new PntInsDia(0,"Insert Point",atfInterpret->getNumOfPoints());
      pntInsDia->setCaption("Insert a point");
      pntInsDia->setMaximumSize(pntInsDia->width(),pntInsDia->height());
      pntInsDia->setMinimumSize(pntInsDia->width(),pntInsDia->height());
      connect(pntInsDia,SIGNAL(insPoint(int,bool)),this,SLOT(insPntOk(int,bool)));
      pntInsDia->show();
    }
  else insPntOk(-1,true);
}

/*======================= extra switch relation ==================*/
void AEditWin::extraOne2One()
{
  relation = !relation;
  drawWid->setRelation(relation);
  if (relation)
    {
      toolbar1->setButton(T_RELATION,true);
      extra->setItemChecked(M_E121,true);
    }
  else
    {
      toolbar1->setButton(T_RELATION,false);
      extra->setItemChecked(M_E121,false);
    }
}

/*======================= help help ==============================*/
void AEditWin::helpHelp()
{
}

/*======================= help about =============================*/
void AEditWin::helpAbout()
{
  KoAboutDia::about(KoAboutDia::KAutoformEdit,"0.0.1");
}

/*======================= help about kofficr =====================*/
void AEditWin::helpAboutKOffice()
{
  KoAboutDia::about(KoAboutDia::KOffice,"0.0.1");
}

/*======================= help about kde =========================*/
void AEditWin::helpAboutKDE()
{
  KoAboutDia::about(KoAboutDia::KDE);
}

/*==================== set info text =============================*/
void AEditWin::setInfoText(int id)
{
  if (!timer->isActive()) timer->start(100,false);
  if (id == M_FILE) statBar->changeItem("File operations",ST_INFO);
  if (id == M_EDIT) statBar->changeItem("Editing operations",ST_INFO);
  if (id == M_EXTRA) statBar->changeItem("Extra operations",ST_INFO);
  if (id == M_HELP) statBar->changeItem("Calling help and about dialog",ST_INFO);

  if (id == M_FNEW) statBar->changeItem("Create a new autoform in that window",ST_INFO);
  if (id == M_FOPEN) statBar->changeItem("Open an autoform in that window",ST_INFO);
  if (id == M_FSAVE) statBar->changeItem("Save current autoform",ST_INFO);
  if (id == M_FSAS) statBar->changeItem("Save current autoform as",ST_INFO);
  if (id == M_FWIN) statBar->changeItem("Open a new KPresenter Autoform Editor window",ST_INFO);
  if (id == M_FCLOSE) statBar->changeItem("Close this window",ST_INFO);
  if (id == M_FEXIT) statBar->changeItem("Exit KPresenter Autoform Editor",ST_INFO);

  if (id == M_ECUT) statBar->changeItem("Cut to the clipboard",ST_INFO);
  if (id == M_ECOPY) statBar->changeItem("Copy to the clipboard",ST_INFO);
  if (id == M_EPASTE) statBar->changeItem("Paste from the clipboard",ST_INFO);
  if (id == M_EDELETE) statBar->changeItem("Delete selected point",ST_INFO);

  if (id == M_EINSPNT) statBar->changeItem("Insert a new point",ST_INFO);
  if (id == M_E121) statBar->changeItem("Switch relation of the autoform viewer",ST_INFO);

  if (id == M_HABOUTKPA) statBar->changeItem("Show about dialog of the KAutoform Editor",ST_INFO);
  if (id == M_HABOUTKPA) statBar->changeItem("Show about dialog of the KOffice",ST_INFO);
  if (id == M_HABOUTKDE) statBar->changeItem("Show information about KDE",ST_INFO);
  if (id == M_HHELP) statBar->changeItem("Show contents of help",ST_INFO);
}

/*==================== clear info text ===========================*/
void AEditWin::clearInfoText()
{
  statBar->changeItem("",ST_INFO);
}

/*============== timer: check if menu is visible =================*/
void AEditWin::checkMenu()
{
  if ((!file) || ((!file->isVisible())) && (!edit->isVisible()) &&
      (!extra->isVisible()) && (!help->isVisible()))
    {
      clearInfoText();
      timer->stop();
    }
} 

/*==================== send source ===============================*/
void AEditWin::sendSource()
{
  if (atfInterpret && editWid) editWid->setSource(atfInterpret->getPoints());
} 

/*================== send point array ============================*/
void AEditWin::sendPntArry(int w,int h)
{
  if (atfInterpret && drawWid)
    drawWid->setPointArray(atfInterpret->getPointArray(w,h),atfInterpret->getAttribList());
}

/*======================= set pen width ==========================*/
void AEditWin::setPenWidth(int w)
{
  if (drawWid) drawWid->setPenWidth(w);
  toolbar1->getCombo(99)->resize(70,24);
}

/*======================= change variable ========================*/
void AEditWin::chnVar(int pnt,int structur,int var,QString str)
{
  if (atfInterpret && drawWid)
    {
      atfInterpret->changeVar(pnt,structur,var,str);
      sendPntArry(drawWid->aW(),drawWid->aH());
    }
}

/*====================== insert new point ========================*/
void AEditWin::insPntOk(int index,bool pos)
{
  if (atfInterpret) atfInterpret->insertPoint(index,pos);
  sendSource();
  sendPntArry(drawWid->aW(),drawWid->aH());
}		      

/*====================== delete Point ============================*/
void AEditWin::delPoint(int pnt)
{
  if (atfInterpret) atfInterpret->deletePoint(pnt);
  sendSource();
  sendPntArry(drawWid->aW(),drawWid->aH());
}		      

/*================== autoform chosen =============================*/
void AEditWin::afChooseOk(const char* c)
{
  QString afDir = qstrdup(KApplication::kde_datadir());
  afDir += "/kpresenter/autoforms/";
  QFileInfo fileInfo(c);
  QString atfName(afDir + fileInfo.dirPath(false) + "/" + fileInfo.baseName() + ".atf");
  
  fileName = fileInfo.baseName() + ".atf";
  groupName = fileInfo.dirPath(false);
  if (atfInterpret) atfInterpret->load(atfName);
  sendSource();
  sendPntArry(drawWid->aW(),drawWid->aH());
}

/*======================== save as ==============================*/
void AEditWin::saveAsDiaOk(const char* grp,const char* nam)
{
  groupName = qstrdup(grp);
  fileName = qstrdup(nam);
  fileName += ".atf";
  fileSave();
}

