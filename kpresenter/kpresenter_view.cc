/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter View                                        */
/******************************************************************/

#include <kfiledialog.h>
#include "kpresenter_view.h"
#include "kpresenter_view.moc"
#include "page.h"

#define DEBUG

// Qt bug
char *getenv(const char *name);    

/*****************************************************************/
/* class KPresenterFrame                                         */
/*****************************************************************/

/*======================= constructor ===========================*/
KPresenterFrame::KPresenterFrame(KPresenterView_impl* _view,KPresenterChild* _child)
  : PartFrame_impl(_view)
{
  m_pKPresenterView = _view;
  m_pKPresenterChild = _child;
}

/*****************************************************************/
/* class KPresenterView_impl                                     */
/*****************************************************************/

/*======================= constructor ===========================*/
KPresenterView_impl::KPresenterView_impl(QWidget *_parent = 0L,const char *_name = 0L)
  : QWidget(_parent,_name), View_impl(), KPresenter::KPresenterView_skel()
{
  setWidget(this);

  Control_impl::setFocusPolicy(OPControls::Control::ClickFocus);
 
  m_pKPresenterDoc = 0L;
  m_bKPresenterModified = false;
  m_bUnderConstruction = true;
  
  m_lstFrames.setAutoDelete(true);  

  // init
  backDia = 0;
  afChoose = 0;
  styleDia = 0;
  optionDia = 0;
  pgConfDia = 0;
  effectDia = 0;
  xOffset = 0;
  yOffset = 0;
  pen.operator=(QPen(black,1,SolidLine));
  brush.operator=(QBrush(white,SolidPattern));
  setMouseTracking(true);
  m_bShowGUI = true;
  m_bRectSelection = false;
  presStarted = false;
  origFramesList.setAutoDelete(true);
  origPartsList.setAutoDelete(true);
}

/*======================= destructor ============================*/
KPresenterView_impl::~KPresenterView_impl()
{
  sdeb("KPresenterView_impl::~KPresenterView_impl()\n");
  cleanUp();
  edeb("...KPresenterView_impl::~KPresenterView_impl() %i\n",_refcnt());
}

/*======================= clean up ==============================*/
void KPresenterView_impl::cleanUp()
{
  if (m_bIsClean) return;
  
  m_pKPresenterDoc->removeView(this);
  
  m_lstFrames.clear();

  mdeb("rMenuBar %i %i\n",m_rMenuBar->refCount(),m_rMenuBar->_refcnt());
  
  m_rMenuBar = 0L;
  m_vMenuBarFactory = 0L;

  m_rToolBarEdit = 0L;
  m_vToolBarFactory = 0L;

  View_impl::cleanUp();
}

/*========================== edit cut ===========================*/
void KPresenterView_impl::editCut()
{
  m_pKPresenterDoc->copyObjs(xOffset,yOffset);
  m_pKPresenterDoc->deleteObjs();
}

/*========================== edit copy ==========================*/
void KPresenterView_impl::editCopy()
{
  m_pKPresenterDoc->copyObjs(xOffset,yOffset);
}

/*========================== edit paste =========================*/
void KPresenterView_impl::editPaste()
{
  m_pKPresenterDoc->pasteObjs(xOffset,yOffset);
}

/*========================== edit delete ========================*/
void KPresenterView_impl::editDelete()
{
  m_pKPresenterDoc->deleteObjs();
}

/*========================== edit select all ====================*/
void KPresenterView_impl::editSelectAll()
{
}

/*========================= view new view =======================*/
void KPresenterView_impl::newView()
{
  assert((m_pKPresenterDoc != 0L));

  shell = new KPresenterShell_impl;
  shell->enableMenuBar();
  shell->PartShell_impl::enableStatusBar();
  shell->enableToolBars();
  shell->show();
  shell->setDocument(m_pKPresenterDoc);
  
  CORBA::release(shell);
}

/*====================== insert a new page ======================*/
void KPresenterView_impl::insertPage()
{
  m_pKPresenterDoc->insertNewTemplate(xOffset,yOffset);
  setRanges();
}

/*====================== insert a picture =======================*/
void KPresenterView_impl::insertPicture()
{
  page->deSelectAllObj();
  QString file = KFileDialog::getOpenFileName(getenv("HOME"),
					      "*.gif *GIF|GIF-Pictures\n"
					      "*.jpg *.JPG *.jpeg *.JPEG|JPEG-Pictures\n"
					      "*.bmp *.BMP|Windows Bitmaps\n"
					      "*.xbm *.XBM|XWindow Pitmaps\n"
					      "*.xpm *.XPM|Pixmaps\n"
					      "*.pnm *.PNM|PNM-Pictures\n"
					      "*.gif *GIF *.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP *.xbm *.XBM *.xpm *.XPM *.pnm *.PNM|All pictures",0);
  if (!file.isEmpty()) m_pKPresenterDoc->insertPicture((const char*)file,xOffset,yOffset);


//   QEvent ev(Event_Leave);
//   QMouseEvent mev(Event_MouseButtonRelease,
// 		  QCursor::pos(),LeftButton,LeftButton);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Picture),&ev);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Picture),&mev);
}

/*====================== insert a clipart =======================*/
void KPresenterView_impl::insertClipart()
{
  page->deSelectAllObj();
  QString file = KFileDialog::getOpenFileName(getenv("HOME"),"*.WMF *.wmf|Windows Metafiles",0);
  if (!file.isEmpty()) m_pKPresenterDoc->insertClipart((const char*)file,xOffset,yOffset);

//   QEvent ev(Event_Leave);
//   QMouseEvent mev(Event_MouseButtonRelease,
// 		  QCursor::pos(),LeftButton,LeftButton);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Clipart),&ev);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Clipart),&mev);
}

/*=========================== insert line =======================*/
void KPresenterView_impl::insertLine()
{
  QPoint pnt(QCursor::pos());

  rb_line->popup(pnt);

//   QEvent ev(Event_Leave);
//   QMouseEvent mev(Event_MouseButtonRelease,
// 		  QCursor::pos(),LeftButton,LeftButton);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Line),&ev);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Line),&mev);
}

/*===================== insert rectangle ========================*/
void KPresenterView_impl::insertRectangle()
{
  QPoint pnt(QCursor::pos());

  rb_rect->popup(pnt);

//   QEvent ev(Event_Leave);
//   QMouseEvent mev(Event_MouseButtonRelease,
// 		  QCursor::pos(),LeftButton,LeftButton);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Rectangle),&ev);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Rectangle),&mev);
}

/*===================== insert circle or ellipse ================*/
void KPresenterView_impl::insertCircleOrEllipse()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertCircleOrEllipse(pen,brush,xOffset,yOffset);
}

/*===================== insert a textobject =====================*/
void KPresenterView_impl::insertText()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertText(xOffset,yOffset);
}

/*======================== insert autoform ======================*/
void KPresenterView_impl::insertAutoform()
{
  if (afChoose)
    {
      QObject::disconnect(afChoose,SIGNAL(formChosen(const char*)),this,SLOT(afChooseOk(const char*)));
      afChoose->close();
      delete afChoose;
      afChoose = 0;
    }
  afChoose = new AFChoose(0,"Autoform-Choose");
  afChoose->setCaption(klocale->translate("KPresenter - Insert an Autoform"));
  afChoose->setMaximumSize(afChoose->width(),afChoose->height());
  afChoose->setMinimumSize(afChoose->width(),afChoose->height());
  QObject::connect(afChoose,SIGNAL(formChosen(const char*)),this,SLOT(afChooseOk(const char*)));
  afChoose->show();
}

/*======================== insert object ========================*/
void KPresenterView_impl::insertObject()
{
  KoPartEntry* pe = KoPartSelectDia::selectPart();
  if (!pe) return;
  
  m_pKPresenterDoc->insertObject(QRect(10,10,150,150),pe->name());
  //  startRectSelection(pe->name());
}

/*===================== extra pen and brush =====================*/
void KPresenterView_impl::extraPenBrush()
{
  if (styleDia)
    {
      QObject::disconnect(styleDia,SIGNAL(styleOk()),this,SLOT(styleOk()));
      styleDia->close();
      delete styleDia;
      styleDia = 0;
    }
  styleDia = new StyleDia(0,"StyleDia");
  styleDia->setMaximumSize(styleDia->width(),styleDia->height());
  styleDia->setMinimumSize(styleDia->width(),styleDia->height());
  styleDia->setPen(m_pKPresenterDoc->getPen(pen));
  styleDia->setBrush(m_pKPresenterDoc->getBrush(brush));
  styleDia->setCaption(klocale->translate("KPresenter - Pen and Brush"));
  QObject::connect(styleDia,SIGNAL(styleOk()),this,SLOT(styleOk()));
  styleDia->show();
}

/*========================== extra raise ========================*/
void KPresenterView_impl::extraRaise()
{
  m_pKPresenterDoc->raiseObjs(xOffset,yOffset);
}

/*========================== extra lower ========================*/
void KPresenterView_impl::extraLower()
{
  m_pKPresenterDoc->lowerObjs(xOffset,yOffset);
}

/*========================== extra rotate =======================*/
void KPresenterView_impl::extraRotate()
{
}

/*====================== extra background =======================*/
void KPresenterView_impl::extraBackground()
{
  if (backDia)
    {
      QObject::disconnect(backDia,SIGNAL(backOk(bool)),this,SLOT(backOk(bool)));
      backDia->close();
      delete backDia;
      backDia = 0;
    }
  backDia = new BackDia(0,"InfoDia",m_pKPresenterDoc->getBackType(getCurrPgNum()),
			m_pKPresenterDoc->getBackColor1(getCurrPgNum()),
			m_pKPresenterDoc->getBackColor2(getCurrPgNum()),
			m_pKPresenterDoc->getBackColorType(getCurrPgNum()),
			m_pKPresenterDoc->getBackPic(getCurrPgNum()),
			m_pKPresenterDoc->getBackClip(getCurrPgNum()),
			m_pKPresenterDoc->getBPicView(getCurrPgNum()));
  backDia->setMaximumSize(backDia->width(),backDia->height());
  backDia->setMinimumSize(backDia->width(),backDia->height());
  backDia->setCaption(klocale->translate("KPresenter - Page Background"));
  QObject::connect(backDia,SIGNAL(backOk(bool)),this,SLOT(backOk(bool)));
  backDia->show();
}

/*======================= extra layout ==========================*/
void KPresenterView_impl::extraLayout()
{
  KoPageLayout pgLayout = m_pKPresenterDoc->pageLayout();
  KoHeadFoot hf;
  
  if (KoPageLayoutDia::pageLayout(pgLayout,hf,FORMAT_AND_BORDERS)) 
    m_pKPresenterDoc->setPageLayout(pgLayout,xOffset,yOffset);

  setRanges();
}

/*========================== extra options ======================*/
void KPresenterView_impl::extraOptions()
{
  if (optionDia)
    {
      QObject::disconnect(optionDia,SIGNAL(applyButtonPressed()),this,SLOT(optionOk()));
      optionDia->close();
      delete optionDia;
      optionDia = 0;
    }
  optionDia = new OptionDia(0,"OptionDia");
  optionDia->setCaption("KPresenter - Options");
  optionDia->setMaximumSize(optionDia->width(),optionDia->height());
  optionDia->setMinimumSize(optionDia->width(),optionDia->height());
  QObject::connect(optionDia,SIGNAL(applyButtonPressed()),this,SLOT(optionOk()));
  optionDia->setRastX(KPresenterDoc()->getRastX());
  optionDia->setRastY(KPresenterDoc()->getRastY());
  optionDia->setBackCol(KPresenterDoc()->getTxtBackCol());
  optionDia->setSelCol(KPresenterDoc()->getTxtSelCol());
  optionDia->setRndX(KPresenterDoc()->getRndX());
  optionDia->setRndY(KPresenterDoc()->getRndY());
  optionDia->show();
}

/*========================== screen config pages ================*/
void KPresenterView_impl::screenConfigPages()
{
  if (pgConfDia)
    {
      QObject::disconnect(pgConfDia,SIGNAL(pgConfDiaOk()),this,SLOT(pgConfOk()));
      pgConfDia->close();
      delete pgConfDia;
      pgConfDia = 0;
    }
  pgConfDia = new PgConfDia(0,"PageConfig",KPresenterDoc()->spInfinitLoop(),
			    KPresenterDoc()->spManualSwitch());
  pgConfDia->setMaximumSize(pgConfDia->width(),pgConfDia->height());
  pgConfDia->setMinimumSize(pgConfDia->width(),pgConfDia->height());
  pgConfDia->setCaption("KPresenter - Page Configuration for Screenpresentations");
  QObject::connect(pgConfDia,SIGNAL(pgConfDiaOk()),this,SLOT(pgConfOk()));
  pgConfDia->show();
}

/*========================== screen assign effect ===============*/
void KPresenterView_impl::screenAssignEffect()
{
  int _pNum,_oNum;

  if (effectDia)
    {
      QObject::disconnect(effectDia,SIGNAL(effectDiaOk()),this,SLOT(effectOk()));
      effectDia->close();
      delete effectDia;
      effectDia = 0;
    }

  if (page->canAssignEffect(_pNum,_oNum) && _pNum >= 1)
    {
      effectDia = new EffectDia(0,"Effect",_pNum,_oNum,(KPresenterView_impl*)this);
      effectDia->setMaximumSize(effectDia->width(),effectDia->height());
      effectDia->setMinimumSize(effectDia->width(),effectDia->height());
      effectDia->setCaption("KPresenter - Assign effects");
      QObject::connect(effectDia,SIGNAL(effectDiaOk()),this,SLOT(effectOk()));
      effectDia->show();
      page->deSelectAllObj();
      page->selectObj(_oNum);
    }
  else
    QMessageBox::critical((QWidget*)0L,i18n("KPresenter Error"),i18n("I can't assign an effect. You have to select EXACTLY one object!"),
			  i18n("OK"));
}

/*========================== screen start =======================*/
void KPresenterView_impl::screenStart()
{
  if (page && !presStarted) 
    {
      page->deSelectAllObj();
      page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
      presStarted = true;

      float _presFaktW = (float)page->width() / (float)KPresenterDoc()->getPageSize(1,0,0).width() > 1.0 ? 
	(float)page->width() / (float)KPresenterDoc()->getPageSize(1,0,0).width() : 1.0;
      float _presFaktH = (float)page->height() / (float)KPresenterDoc()->getPageSize(1,0,0).height() > 1.0 ? 
	(float)page->height() / (float)KPresenterDoc()->getPageSize(1,0,0).height() : 1.0;
      float _presFakt = min(_presFaktW,_presFaktH);
      page->setPresFakt(_presFakt);
      zoomParts(_presFakt);

      _xOffset = xOffset;
      _yOffset = yOffset;
      xOffset = 10;
      yOffset = 10;
      if (page->width() > KPresenterDoc()->getPageSize(1,0,0,page->presFakt()).width())
 	xOffset -= (page->width() - KPresenterDoc()->getPageSize(1,0,0,page->presFakt()).width()) / 2;
      if (page->height() > KPresenterDoc()->getPageSize(1,0,0,page->presFakt()).height())
 	yOffset -= (page->height() - KPresenterDoc()->getPageSize(1,0,0,page->presFakt()).height()) / 2;

      vert->setEnabled(false);
      horz->setEnabled(false);
      m_bShowGUI = false;
      page->setBackgroundColor(black);
      oldSize = widget()->size();
      widget()->resize(page->size());
      setSize(page->size().width(),page->size().height());
      page->startScreenPresentation();

      page->recreate((QWidget*)0L,WStyle_Customize | WStyle_NoBorder | WType_Popup,QPoint(0,0),true);
      page->topLevelWidget()->move(0,0);
      page->topLevelWidget()->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
      page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
      page->topLevelWidget()->setBackgroundColor(black);
      page->setFocusPolicy(QWidget::StrongFocus);
      page->setFocus();
    }
}

/*========================== screen stop ========================*/
void KPresenterView_impl::screenStop()
{
  if (presStarted)
    {
      page->close(false);
      page->recreate((QWidget*)this,0,QPoint(0,0),true);
      zoomBackParts();
      xOffset = _xOffset;
      yOffset = _yOffset;
      page->stopScreenPresentation();
      presStarted = false;
      vert->setEnabled(true);
      horz->setEnabled(true);
      m_bShowGUI = true;
      page->setMouseTracking(true);
      page->setBackgroundColor(white);
      setSize(oldSize.width(),oldSize.height());
      widget()->resize(oldSize);
      resizeEvent(0L);
    }
}

/*========================== screen pause =======================*/
void KPresenterView_impl::screenPause()
{
}

/*========================== screen first =======================*/
void KPresenterView_impl::screenFirst()
{
}

/*========================== screen pevious =====================*/
void KPresenterView_impl::screenPrev()
{
  if (presStarted)
    {
      if (page->pPrev(true))
	{
	  yOffset -= KPresenterDoc()->getPageSize(1,0,0,page->presFakt()).height()+10; 
	  page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
	  page->repaint(true);
	  page->setFocus();
	}
      else
	{
	  page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
	  page->setFocus();
	}
    }
}

/*========================== screen next ========================*/
void KPresenterView_impl::screenNext()
{
  if (presStarted)
    {
      if (page->pNext(true))
	{
	  yOffset += KPresenterDoc()->getPageSize(1,0,0,page->presFakt()).height()+10; 
	  page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
	  page->repaint(true);
	  page->setFocus();
	}
      else
	{
	  page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
	  page->setFocus();
	}
    }
}

/*========================== screen last ========================*/
void KPresenterView_impl::screenLast()
{
}

/*========================== screen skip =======================*/
void KPresenterView_impl::screenSkip()
{
}

/*========================== screen full screen ================*/
void KPresenterView_impl::screenFullScreen()
{
}

/*========================== screen pen/marker =================*/
void KPresenterView_impl::screenPen()
{
}

/*======================= help contents ========================*/
void KPresenterView_impl::helpContents()
{
}

/*======================= help about ===========================*/
void KPresenterView_impl::helpAbout()
{
  KoAboutDia::about(KoAboutDia::KPresenter,"0.0.1");
}

/*======================= help about koffice ====================*/
void KPresenterView_impl::helpAboutKOffice()
{
  KoAboutDia::about(KoAboutDia::KOffice,"0.0.1");
}

/*======================= help about kde ========================*/
void KPresenterView_impl::helpAboutKDE()
{
  KoAboutDia::about(KoAboutDia::KDE);
}

/*======================= text size selected  ===================*/
void KPresenterView_impl::sizeSelected(const char *size)
{
  tbFont.setPointSize(atoi(size));
  page->setTextFont(&tbFont);
}

/*======================= text font selected  ===================*/
void KPresenterView_impl::fontSelected(const char *font)
{
  tbFont.setFamily(qstrdup(font));
  page->setTextFont(&tbFont);
}

/*========================= text bold ===========================*/
void KPresenterView_impl::textBold()
{
  tbFont.setBold(!tbFont.bold());
  page->setTextFont(&tbFont);
}

/*========================== text italic ========================*/
void KPresenterView_impl::textItalic()
{
  tbFont.setItalic(!tbFont.italic());
  page->setTextFont(&tbFont);
}

/*======================== text underline =======================*/
void KPresenterView_impl::textUnderline()
{
  tbFont.setUnderline(!tbFont.underline());
  page->setTextFont(&tbFont);
}

/*=========================== text color ========================*/
void KPresenterView_impl::textColor()
{
  if (KColorDialog::getColor(tbColor))
    {
      m_rToolBarText->setButtonPixmap(m_idButtonText_Color,CORBA::string_dup(colorToPixString(tbColor)));
      page->setTextColor(&tbColor);
    }
}

/*======================= text align left =======================*/
void KPresenterView_impl::textAlignLeft()
{
  tbAlign = TxtParagraph::LEFT;
  page->setTextAlign(tbAlign);

  m_rToolBarText->setButton(m_idButtonText_ACenter,false);
  m_rToolBarText->setButton(m_idButtonText_ARight,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Left,true);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Center,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Right,false);
}

/*======================= text align center =====================*/
void KPresenterView_impl::textAlignCenter()
{
  tbAlign = TxtParagraph::CENTER;
  page->setTextAlign(TxtParagraph::CENTER);

  m_rToolBarText->setButton(m_idButtonText_ALeft,false);
  m_rToolBarText->setButton(m_idButtonText_ARight,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Left,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Center,true);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Right,false);
}

/*======================= text align right ======================*/
void KPresenterView_impl::textAlignRight()
{
  tbAlign = TxtParagraph::RIGHT;
  page->setTextAlign(TxtParagraph::RIGHT);

  m_rToolBarText->setButton(m_idButtonText_ALeft,false);
  m_rToolBarText->setButton(m_idButtonText_ACenter,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Left,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Center,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Right,true);
}

/*======================= text align left =======================*/
void KPresenterView_impl::mtextAlignLeft()
{
  tbAlign = TxtParagraph::LEFT;
  page->setTextAlign(tbAlign);

  m_rToolBarText->setButton(m_idButtonText_ALeft,true);
  m_rToolBarText->setButton(m_idButtonText_ACenter,false);
  m_rToolBarText->setButton(m_idButtonText_ARight,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Left,true);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Center,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Right,false);
}

/*======================= text align center =====================*/
void KPresenterView_impl::mtextAlignCenter()
{
  tbAlign = TxtParagraph::CENTER;
  page->setTextAlign(TxtParagraph::CENTER);

  m_rToolBarText->setButton(m_idButtonText_ALeft,false);
  m_rToolBarText->setButton(m_idButtonText_ACenter,true);
  m_rToolBarText->setButton(m_idButtonText_ARight,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Left,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Center,true);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Right,false);
}

/*======================= text align right ======================*/
void KPresenterView_impl::mtextAlignRight()
{
  tbAlign = TxtParagraph::RIGHT;
  page->setTextAlign(TxtParagraph::RIGHT);

  m_rToolBarText->setButton(m_idButtonText_ALeft,false);
  m_rToolBarText->setButton(m_idButtonText_ACenter,false);
  m_rToolBarText->setButton(m_idButtonText_ARight,true);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Left,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Center,false);
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Right,true);
}

/*====================== font selected ==========================*/
void KPresenterView_impl::mtextFont()
{
  QFont tmpFont = tbFont;

  if (KFontDialog::getFont(tmpFont))
    {
      fontChanged(&tmpFont);
      tbFont = tmpFont;
      page->setTextFont(&tbFont);
    }
}

/*====================== enumerated list ========================*/
void KPresenterView_impl::textEnumList()
{
  if (page->kTxtObj())
    {
      int _type = page->kTxtObj()->enumListType().type;
      QFont _font = page->kTxtObj()->enumListType().font;
      QColor _color = page->kTxtObj()->enumListType().color;
      QString _before = page->kTxtObj()->enumListType().before;
      QString _after = page->kTxtObj()->enumListType().after;
      int _start = page->kTxtObj()->enumListType().start;
      
      if (KEnumListDia::enumListDia(_type,_font,_color,_before,_after,_start))
	{
	  KTextObject::EnumListType elt;
	  elt.type = _type;
	  elt.font = _font;
	  elt.color = _color;
	  elt.before = _before;
	  elt.after = _after;
	  elt.start = _start;
	  page->kTxtObj()->setEnumListType(elt);
	}
      
      page->kTxtObj()->setObjType(KTextObject::ENUM_LIST);
    }
}

/*====================== unsorted list ==========================*/
void KPresenterView_impl::textUnsortList()
{
  if (page->kTxtObj())
    {
      QFont _font = page->kTxtObj()->unsortListType().font;
      QColor _color = page->kTxtObj()->unsortListType().color;
      int _c = page->kTxtObj()->unsortListType().chr;
      
      if (KCharSelectDia::selectChar(_font,_color,_c))
	{
	  KTextObject::UnsortListType ult;
	  ult.font = _font;
	  ult.color = _color;
	  ult.chr = _c;
	  page->kTxtObj()->setUnsortListType(ult);
	}
      
      page->kTxtObj()->setObjType(KTextObject::UNSORT_LIST);
    }
}

/*====================== normal text ============================*/
void KPresenterView_impl::textNormalText()
{
  if (page->kTxtObj()) page->kTxtObj()->setObjType(KTextObject::PLAIN);
}

/*======================= set document ==========================*/
void KPresenterView_impl::setDocument(KPresenterDocument_impl *_doc)
{
  if (m_pKPresenterDoc) m_pKPresenterDoc->removeView(this);

  View_impl::setDocument(_doc);
  
  m_pKPresenterDoc = _doc;
  m_bKPresenterModified = true;

  m_pKPresenterDoc->addView(this);

  QObject::connect(m_pKPresenterDoc,SIGNAL(sig_KPresenterModified()),this,SLOT(slotKPresenterModified()));
  QObject::connect(m_pKPresenterDoc,SIGNAL(sig_insertObject(KPresenterChild*)),
		   this,SLOT(slotInsertObject(KPresenterChild*)));
  QObject::connect(m_pKPresenterDoc,SIGNAL(sig_updateChildGeometry(KPresenterChild*)),
		   this,SLOT(slotUpdateChildGeometry(KPresenterChild*)));
}
  
/*======================== create GUI ==========================*/
void KPresenterView_impl::createGUI()
{
  sdeb("void KPresenterView_impl::createGUI() %i | %i\n",refCount(),_refcnt());

  // setup page
  page = new Page(this,"Page",(KPresenterView_impl*)this);
  QObject::connect(page,SIGNAL(fontChanged(QFont*)),this,SLOT(fontChanged(QFont*)));
  QObject::connect(page,SIGNAL(colorChanged(QColor*)),this,SLOT(colorChanged(QColor*)));
  QObject::connect(page,SIGNAL(alignChanged(TxtParagraph::HorzAlign)),this,SLOT(alignChanged(TxtParagraph::HorzAlign)));
  widget()->setFocusProxy(page);

  // setup GUI
  setupMenu();
  setupPopupMenus();
  setupEditToolbar();
  setupInsertToolbar();
  setupTextToolbar();
  setupExtraToolbar();
  setupScreenToolbar();
  setupScrollbars();
  setRanges();
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Left,true);
 
  if (m_pKPresenterDoc && page)
    {
      QObject::connect(m_pKPresenterDoc,SIGNAL(restoreBackColor(unsigned int)),page,SLOT(restoreBackColor(unsigned int)));
      for (unsigned int i = 0;i < KPresenterDoc()->pageList()->count();i++)
	page->restoreBackColor(i);
    }

  resizeEvent(0L);

  // Show every embedded object
  QListIterator<KPresenterChild> it = m_pKPresenterDoc->childIterator();
  for( ; it.current(); ++it )
    slotInsertObject( it.current() );

  edeb("...void KPresenterView_impl::createGUI() %i | %i\n",refCount(),_refcnt());
}

/*====================== construct ==============================*/
void KPresenterView_impl::construct()
{
  if (m_pKPresenterDoc == 0L && !m_bUnderConstruction) return;
  
  assert(m_pKPresenterDoc != 0L);
  
  m_bUnderConstruction = false;

  m_lstFrames.clear();
  
  QListIterator<KPresenterChild> it = m_pKPresenterDoc->childIterator();
  for(;it.current();++it)
    slotInsertObject(it.current());

  // We are now in sync with the document
  m_bKPresenterModified = false;

  resizeEvent(0L);
}

/*======================== document modified ===================*/
void KPresenterView_impl::slotKPresenterModified()
{
  m_bKPresenterModified = true;
  update();
}

/*======================= insert object ========================*/
void KPresenterView_impl::slotInsertObject(KPresenterChild *_child)
{ 
  OPParts::Document_var doc = _child->document();
  OPParts::View_var v;

  try
    { 
      v = doc->createView();
    }
  catch (OPParts::Document::MultipleViewsNotSupported &_ex)
    {
      // HACK
      printf("void KPresenterView_impl::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n");
      printf("Could not create view\n");
      exit(1);
    }
  
  if (CORBA::is_nil(v))
    {
      printf("void KPresenterView_impl::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n");
      printf("return value is 0L\n");
      exit(1);
    }

  v->setMode(OPParts::Part::ChildMode);
  v->setPartShell(partShell());

  KPresenterFrame *p = new KPresenterFrame(this,_child);
  p->attach(v);
  p->setGeometry(_child->geometry());
  p->show();
  m_lstFrames.append(p);
  page->insertChild(p);
  vert->raise();
  horz->raise();
  //CORBA::release(p);
  
  QObject::connect(p,SIGNAL(sig_geometryEnd(PartFrame_impl*)),
		   this,SLOT(slotGeometryEnd(PartFrame_impl*)));
  QObject::connect(p,SIGNAL(sig_moveEnd(PartFrame_impl*)),
		   this,SLOT(slotMoveEnd(PartFrame_impl*)));  
} 

/*========================== update child geometry =============*/
void KPresenterView_impl::slotUpdateChildGeometry(KPresenterChild *_child)
{
  // Find frame for child
  KPresenterFrame *f = 0L;
  QListIterator<KPresenterFrame> it(m_lstFrames);
  for (;it.current() && !f;++it)
    if (it.current()->child() == _child)
      f = it.current();
  
  assert(f != 0L);
  
  // Are we already up to date ?
  if (_child->geometry() == f->partGeometry()) return;
  
  // TODO scaling
  f->setPartGeometry(_child->geometry());
}

/*======================= slot geometry end ====================*/
void KPresenterView_impl::slotGeometryEnd(PartFrame_impl* _frame)
{
  KPresenterFrame *f = (KPresenterFrame*)_frame;
  // TODO scaling
  m_pKPresenterDoc->changeChildGeometry(f->child(),_frame->partGeometry());
}

/*==================== slot move end ===========================*/
void KPresenterView_impl::slotMoveEnd(PartFrame_impl* _frame)
{
  KPresenterFrame *f = (KPresenterFrame*)_frame;
  // TODO scaling
  m_pKPresenterDoc->changeChildGeometry(f->child(),_frame->partGeometry());
}

/*=========== take changes for backgr dialog =====================*/
void KPresenterView_impl::backOk(bool takeGlobal)
{
  unsigned int i;

  if (!takeGlobal)
    {
      m_pKPresenterDoc->setBackColor(getCurrPgNum(),backDia->getBackColor1(),
				     backDia->getBackColor2(),
				     backDia->getBackColorType());
      m_pKPresenterDoc->setBackType(getCurrPgNum(),backDia->getBackType());
      m_pKPresenterDoc->setBPicView(getCurrPgNum(),backDia->getBPicView());
      m_pKPresenterDoc->setBackPic(getCurrPgNum(),backDia->getBackPic());
      m_pKPresenterDoc->setBackClip(getCurrPgNum(),backDia->getBackClip());
      m_pKPresenterDoc->setBPicView(getCurrPgNum(),backDia->getBPicView());
      page->restoreBackColor(getCurrPgNum()-1);
    }
  else
    {
      for (i = 1;i <= m_pKPresenterDoc->getPageNums();i++)
	{
	  m_pKPresenterDoc->setBackColor(i,backDia->getBackColor1(),
					 backDia->getBackColor2(),
					 backDia->getBackColorType());
	  m_pKPresenterDoc->setBPicView(i,backDia->getBPicView());
	  m_pKPresenterDoc->setBackType(i,backDia->getBackType());
	  m_pKPresenterDoc->setBackPic(i,backDia->getBackPic());
	  m_pKPresenterDoc->setBackClip(i,backDia->getBackClip());
	  m_pKPresenterDoc->setBPicView(i,backDia->getBPicView());
	}

      for (i = 0;i < m_pKPresenterDoc->getPageNums();i++)
	page->restoreBackColor(i);

    }

  KPresenterDoc()->repaint(true);
}

/*================== autoform chosen =============================*/
void KPresenterView_impl::afChooseOk(const char* c)
{
  QString afDir = kapp->kde_datadir();
  QFileInfo fileInfo(c);
  QString fileName(afDir + "/kpresenter/autoforms/" + fileInfo.dirPath(false) + "/" + fileInfo.baseName() + ".atf");
  
  page->deSelectAllObj();
  m_pKPresenterDoc->insertAutoform(pen,brush,(const char*)fileName,xOffset,yOffset);
}

/*=========== take changes for style dialog =====================*/
void KPresenterView_impl::styleOk()
{
  if (!m_pKPresenterDoc->setPenBrush(styleDia->getPen(),styleDia->getBrush(),xOffset,yOffset))
    {
      pen.operator=(styleDia->getPen());
      brush.operator=(styleDia->getBrush());
    }
}

/*=========== take changes for option dialog ====================*/
void KPresenterView_impl::optionOk()
{
  if (optionDia->getRastX() < 1)
    optionDia->setRastX(1);
  if (optionDia->getRastY() < 1)
    optionDia->setRastY(1);
  KPresenterDoc()->setRasters(optionDia->getRastX(),optionDia->getRastY());

  KPresenterDoc()->setTxtBackCol(optionDia->getBackCol());
  KPresenterDoc()->setTxtSelCol(optionDia->getSelCol());
  if (optionDia->getRndX() < 1)
    optionDia->setRndX(1);
  if (optionDia->getRndY() < 1)
    optionDia->setRndY(1);
  KPresenterDoc()->setRnds(optionDia->getRndX(),optionDia->getRndY());

  KPresenterDoc()->repaint(false);
}

/*=================== page configuration ok ======================*/
void KPresenterView_impl::pgConfOk()
{
  KPresenterDoc()->setManualSwitch(pgConfDia->getManualSwitch());
  KPresenterDoc()->setInfinitLoop(pgConfDia->getInfinitLoop());
}

/*=================== effect dialog ok ===========================*/
void KPresenterView_impl::effectOk()
{
}

/*================== scroll horizontal ===========================*/
void KPresenterView_impl::scrollH(int _value)
{
  if (!presStarted)
    {
      int xo = xOffset;
      
      xOffset = _value;
      page->scroll(xo - _value,0);
    }
}

/*===================== scroll vertical ==========================*/
void KPresenterView_impl::scrollV(int _value)
{
  if (!presStarted)
    {
      int yo = yOffset;
      
      yOffset = _value;
      page->scroll(0,yo - _value);
    }
}

/*====================== font changed ===========================*/
void KPresenterView_impl::fontChanged(QFont* font)
{
  if (font->operator!=(tbFont)) 
    {
      tbFont.setFamily(font->family());
      tbFont.setBold(font->bold());
      tbFont.setItalic(font->italic());
      tbFont.setUnderline(font->underline());
      tbFont.setPointSize(font->pointSize());
      m_rToolBarText->setButton(m_idButtonText_Bold,tbFont.bold());
      m_rToolBarText->setButton(m_idButtonText_Italic,tbFont.italic());
      m_rToolBarText->setButton(m_idButtonText_Underline,tbFont.underline());
      fontList.find(tbFont.family());
      m_rToolBarText->setCurrentComboItem(m_idComboText_FontList,fontList.at());
      m_rToolBarText->setCurrentComboItem(m_idComboText_FontSize,tbFont.pointSize()-4);
    }
}

/*====================== color changed ==========================*/
void KPresenterView_impl::colorChanged(QColor* color)
{
  if (color->operator!=(tbColor))
    {
      tbColor.setRgb(color->rgb());
      m_rToolBarText->setButtonPixmap(m_idButtonText_Color,CORBA::string_dup(colorToPixString(tbColor)));
    }      
}

/*====================== align changed ==========================*/
void KPresenterView_impl::alignChanged(TxtParagraph::HorzAlign align)
{
  if (align != tbAlign)
    {
      tbAlign = align;
      m_rToolBarText->setButton(m_idButtonText_ALeft,false);
      m_rToolBarText->setButton(m_idButtonText_ACenter,false);
      m_rToolBarText->setButton(m_idButtonText_ARight,false);
      m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Left,false);
      m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Center,false);
      m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Right,false);
      switch (tbAlign)
	{
	case TxtParagraph::LEFT: 
	  {
	    m_rToolBarText->setButton(m_idButtonText_ALeft,true); 
	    m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Left,true);
	  } break;
	case TxtParagraph::CENTER:
	  {
	    m_rToolBarText->setButton(m_idButtonText_ACenter,true); 
	    m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Center,true);
	  } break;
	case TxtParagraph::RIGHT:
	  {
	    m_rToolBarText->setButton(m_idButtonText_ARight,true); 
	    m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Right,true);
	  } break;
	}
    }
}

/*======================= insert line (-) =======================*/
void KPresenterView_impl::insertLineH()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,LT_HORZ,xOffset,yOffset);
}

/*======================= insert line (|) =======================*/
void KPresenterView_impl::insertLineV()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,LT_VERT,xOffset,yOffset);
}

/*======================= insert line (\) =======================*/
void KPresenterView_impl::insertLineD1()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,LT_LU_RD,xOffset,yOffset);
}

/*======================= insert line (/) =======================*/
void KPresenterView_impl::insertLineD2()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,LT_LD_RU,xOffset,yOffset);
}

/*===================== insert normal rect  =====================*/
void KPresenterView_impl::insertNormRect()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertRectangle(pen,brush,RT_NORM,xOffset,yOffset);
}

/*===================== insert round rect =======================*/
void KPresenterView_impl::insertRoundRect()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertRectangle(pen,brush,RT_ROUND,xOffset,yOffset);
}

/*======================= insert line (-) =======================*/
void KPresenterView_impl::insertLineHidl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,LT_HORZ,xOffset,yOffset);
}

/*======================= insert line (|) =======================*/
void KPresenterView_impl::insertLineVidl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,LT_VERT,xOffset,yOffset);
}

/*======================= insert line (\) =======================*/
void KPresenterView_impl::insertLineD1idl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,LT_LU_RD,xOffset,yOffset);
}

/*======================= insert line (/) =======================*/
void KPresenterView_impl::insertLineD2idl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,LT_LD_RU,xOffset,yOffset);
}

/*===================== insert normal rect  =====================*/
void KPresenterView_impl::insertNormRectidl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertRectangle(pen,brush,RT_NORM,xOffset,yOffset);
}

/*===================== insert round rect =======================*/
void KPresenterView_impl::insertRoundRectidl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertRectangle(pen,brush,RT_ROUND,xOffset,yOffset);
}

/*====================== paint event ============================*/
void KPresenterView_impl::repaint(bool erase)
{
  QWidget::repaint(erase);
  page->repaint(erase);
}

/*====================== paint event ============================*/
void KPresenterView_impl::repaint(unsigned int x,unsigned int y,unsigned int w,
				  unsigned int h,bool erase)
{
  QWidget::repaint(x,y,w,h,erase);
  page->repaint(x,y,w,h,erase);
}

/*====================== change pciture =========================*/
void KPresenterView_impl::changePicture(unsigned int,const char* filename)
{
  QFileInfo fileInfo(filename);
  QString file = KFileDialog::getOpenFileName(getenv("HOME"),
					      "*.gif *GIF|GIF-Pictures\n"
					      "*.jpg *.JPG *.jpeg *.JPEG|JPEG-Pictures\n"
					      "*.bmp *.BMP|Windows Bitmaps\n"
					      "*.xbm *.XBM|XWindow Pitmaps\n"
					      "*.xpm *.XPM|Pixmaps\n"
					      "*.pnm *.PNM|PNM-Pictures\n"
					      "*.gif *GIF *.jpg *.JPG *.jpeg *.JPEG *.bmp *.BMP *.xbm *.XBM *.xpm *.XPM *.pnm *.PNM|All pictures",0);

  if (!file.isEmpty()) m_pKPresenterDoc->changePicture((const char*)file,xOffset,yOffset);
}

/*====================== change clipart =========================*/
void KPresenterView_impl::changeClipart(unsigned int,const char* filename)
{
  QFileInfo fileInfo(filename);
  QString file = KFileDialog::getOpenFileName(fileInfo.dirPath(false),"*.wmf *.WMF|Windows Metafiles",0);

  if (!file.isEmpty()) m_pKPresenterDoc->changeClipart((const char*)file,xOffset,yOffset);
}

/*====================== resize event ===========================*/
void KPresenterView_impl::resizeEvent(QResizeEvent *e)
{
  if (!presStarted) QWidget::resizeEvent(e);

  if (m_bShowGUI && !presStarted)
    { 
      horz->show();
      vert->show();
      page->resize(widget()->width()-16,widget()->height()-16);
      vert->setGeometry(widget()->width()-16,0,16,widget()->height()-16);
      horz->setGeometry(0,widget()->height()-16,widget()->width()-16,16);
      setRanges();
    }
  else
    {
      horz->hide();
      vert->hide();
      page->resize(widget()->width(),widget()->height());
    }  
}

/*======================= key press event =======================*/
void KPresenterView_impl::keyPressEvent(QKeyEvent *e)
{
  page->keyPressEvent(e);
}

/*========================= zoom Parts ==========================*/
void KPresenterView_impl::zoomParts(float _fakt)
{
  /*******************************
   * this doesn't really work !
   *******************************/

  if (!origFramesList.isEmpty()) origFramesList.clear();
  if (!origPartsList.isEmpty()) origPartsList.clear();
  
  QListIterator<KPresenterFrame> it(m_lstFrames);

  for(;it.current();++it)
    {
      QRect *r = new QRect(it.current()->geometry().x(),it.current()->geometry().y(),
			   it.current()->geometry().width(),it.current()->geometry().height());
      origFramesList.append(r);
      it.current()->hide();
    }

  QListIterator<KPresenterChild> _it(KPresenterDoc()->lstChildren());

  for(;_it.current();++_it)
    {
      QRect *r = new QRect(_it.current()->geometry().x(),_it.current()->geometry().y(),
			   _it.current()->geometry().width(),_it.current()->geometry().height());
      origPartsList.append(r);
    }
}

/*======================= zoom back Parts =======================*/
void KPresenterView_impl::zoomBackParts()
{
  /*******************************
   * this doesn't really work !
   *******************************/
  
  QListIterator<KPresenterFrame> it(m_lstFrames);

  for(unsigned int i = 0;it.current();++it,i++)
    {
      it.current()->show();
      it.current()->setGeometry(origFramesList.at(i)->x(),origFramesList.at(i)->y(),
 				origFramesList.at(i)->width(),origFramesList.at(i)->height());
    }

  QListIterator<KPresenterChild> _it(KPresenterDoc()->lstChildren());

  for(unsigned int i = 0;_it.current();++_it,i++)
    {
      _it.current()->setGeometry(QRect(origPartsList.at(i)->x(),origPartsList.at(i)->y(),
				       origPartsList.at(i)->width(),origPartsList.at(i)->height()));
    }
}

/*======================= setup menu ============================*/
void KPresenterView_impl::setupMenu()
{
  m_vMenuBarFactory = m_vPartShell->menuBarFactory();
  if (!CORBA::is_nil(m_vMenuBarFactory))
    {
      
      // menubar
      m_rMenuBar = m_vMenuBarFactory->createMenuBar(this);
      // printf("rMenuBar %i %i\n",m_rMenuBar->refCount(),m_rMenuBar->_refcnt());
      
      // edit menu
      m_idMenuEdit = m_rMenuBar->insertMenu(CORBA::string_dup(klocale->translate("&Edit")));
      QString tmp = kapp->kde_toolbardir().copy();
      tmp += "/editcut.xpm";
      QString pix = loadPixmap(tmp);
      m_idMenuEdit_Cut = m_rMenuBar->insertItemP(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("&Cut")),m_idMenuEdit,
						this,CORBA::string_dup("editCut"));
      tmp = kapp->kde_toolbardir().copy();
      tmp += "/editcopy.xpm";
      pix = loadPixmap(tmp);
      m_idMenuEdit_Copy = m_rMenuBar->insertItemP(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("&Copy")),m_idMenuEdit,
						  this,CORBA::string_dup("editCopy"));
      tmp = kapp->kde_toolbardir().copy();
      tmp += "/editpaste.xpm";
      pix = loadPixmap(tmp);
      m_idMenuEdit_Paste = m_rMenuBar->insertItemP(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("&Paste")),m_idMenuEdit,
						   this,CORBA::string_dup("editPaste"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/delete.xpm";
      pix = loadPixmap(tmp);
      m_idMenuEdit_Delete = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(klocale->translate("&Delete")),m_idMenuEdit,
						    this,CORBA::string_dup("editDelete"));
      m_rMenuBar->insertSeparator(m_idMenuEdit);
      m_idMenuEdit_SelectAll = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("Select &all")),m_idMenuEdit,
						      this,CORBA::string_dup("editSelectAll"));

      // view menu
      m_idMenuView = m_rMenuBar->insertMenu(CORBA::string_dup(klocale->translate("&View")));
      m_idMenuView_NewView = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("&New View")),m_idMenuView,
						    this,CORBA::string_dup("newView"));

      // insert menu
      m_idMenuInsert = m_rMenuBar->insertMenu(CORBA::string_dup(klocale->translate("&Insert")));
      m_idMenuInsert_Page = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("Pa&ge...")),m_idMenuInsert,
						   this,CORBA::string_dup("insertPage"));
      m_rMenuBar->insertSeparator(m_idMenuInsert);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/picture.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Picture = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						       CORBA::string_dup(klocale->translate("&Picture...")),m_idMenuInsert,
						       this,CORBA::string_dup("insertPicture"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/clipart.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Clipart = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						       CORBA::string_dup(klocale->translate("&Clipart...")),m_idMenuInsert,
						       this,CORBA::string_dup("insertClipart"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/line.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Line = m_rMenuBar->insertSubMenuP(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("&Line")),
						       m_idMenuInsert);


      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/lineh.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_LineHorz = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							0,m_idMenuInsert_Line,
							this,CORBA::string_dup("insertLineHidl"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/linev.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_LineVert = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							0,m_idMenuInsert_Line,
							this,CORBA::string_dup("insertLineVidl"));

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/lined1.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_LineD1 = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						      0,m_idMenuInsert_Line,
						      this,CORBA::string_dup("insertLineD1idl"));

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/lined2.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_LineD2 = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						      0,m_idMenuInsert_Line,
						      this,CORBA::string_dup("insertLineD2idl"));

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/rectangle.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Rectangle = m_rMenuBar->insertSubMenuP(CORBA::string_dup(pix),
							    CORBA::string_dup(klocale->translate("&Rectangle")),m_idMenuInsert);

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/rectangle2.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_RectangleNormal = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							       0,m_idMenuInsert_Rectangle,
							       this,CORBA::string_dup("insertNormRectidl"));

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/rectangleRound.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_RectangleRound = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							      0,m_idMenuInsert_Rectangle,
							      this,CORBA::string_dup("insertRoundRectidl"));

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/circle.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Circle = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						      CORBA::string_dup(klocale->translate("C&ircle or Ellipse")),m_idMenuInsert,
						      this,CORBA::string_dup("insertCircleOrEllipse"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/text.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Text = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(klocale->translate("&Text")),m_idMenuInsert,
						    this,CORBA::string_dup("insertText"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/autoform.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Autoform = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							CORBA::string_dup(klocale->translate("&Autoform...")),m_idMenuInsert,
							this,CORBA::string_dup("insertAutoform"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/parts.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Part = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(klocale->translate("&Object...")),m_idMenuInsert,
						    this,CORBA::string_dup("insertObject"));

      // extra menu
      m_idMenuExtra = m_rMenuBar->insertMenu(CORBA::string_dup(klocale->translate("&Extra")));
      m_idMenuExtra_TFont = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("&Font...")),m_idMenuExtra,
						   this,CORBA::string_dup("mtextFont"));
      m_idMenuExtra_TColor = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("Text &Color...")),m_idMenuExtra,
						    this,CORBA::string_dup("textColor"));
      m_idMenuExtra_TAlign = m_rMenuBar->insertSubMenu(CORBA::string_dup(klocale->translate("Text &Alignment")),m_idMenuExtra);

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignLeft.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TAlign_Left = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							  CORBA::string_dup(klocale->translate("Align &Left")),m_idMenuExtra_TAlign,
							  this,CORBA::string_dup("mtextAlignLeft"));
      m_rMenuBar->setCheckable(m_idMenuExtra_TAlign_Left,true);
      
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignCenter.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TAlign_Center = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							    CORBA::string_dup(klocale->translate("Align &Center")),m_idMenuExtra_TAlign,
							    this,CORBA::string_dup("mtextAlignCenter"));
      m_rMenuBar->setCheckable(m_idMenuExtra_TAlign_Center,true);

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignRight.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TAlign_Right = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							   CORBA::string_dup(klocale->translate("Align &Right")),m_idMenuExtra_TAlign,
							   this,CORBA::string_dup("mtextAlignRight"));
      m_rMenuBar->setCheckable(m_idMenuExtra_TAlign_Right,true);

      m_idMenuExtra_TType = m_rMenuBar->insertSubMenu(CORBA::string_dup(klocale->translate("Text &Type")),m_idMenuExtra);

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/enumList.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TType_EnumList = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							     CORBA::string_dup(klocale->translate("&Enumerated List")),
							     m_idMenuExtra_TType,
							     this,CORBA::string_dup("textEnumList"));

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/unsortedList.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TType_UnsortList = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							       CORBA::string_dup(klocale->translate("&Unsorted List")),
							       m_idMenuExtra_TType,
							       this,CORBA::string_dup("textUnsortList"));

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/normalText.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TType_NormalText = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							       CORBA::string_dup(klocale->translate("&Normal Text")),
							       m_idMenuExtra_TType,
							       this,CORBA::string_dup("textNormalText"));

      m_rMenuBar->insertSeparator(m_idMenuExtra);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/style.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_PenBrush = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						       CORBA::string_dup(klocale->translate("&Pen and Brush...")),m_idMenuExtra,
						       this,CORBA::string_dup("extraPenBrush"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/raise.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_Raise = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(klocale->translate("&Raise object(s)")),m_idMenuExtra,
						    this,CORBA::string_dup("extraRaise"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/lower.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_Lower = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(klocale->translate("&Lower object(s)")),m_idMenuExtra,
						    this,CORBA::string_dup("extraLower"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/rotate.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_Rotate = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						     CORBA::string_dup(klocale->translate("Rot&ate object(s)...")),m_idMenuExtra,
						     this,CORBA::string_dup("extraRotate"));
      m_rMenuBar->insertSeparator(m_idMenuExtra);
      m_idMenuExtra_Background = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("Page &Background...")),m_idMenuExtra,
							this,CORBA::string_dup("extraBackground"));
      m_idMenuExtra_Layout = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("Pa&ge Layout...")),m_idMenuExtra,
						    this,CORBA::string_dup("extraLayout"));
      m_rMenuBar->insertSeparator(m_idMenuExtra);
      m_idMenuExtra_Options = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("&Options...")),m_idMenuExtra,
						     this,CORBA::string_dup("extraOptions"));
      
      // screenpresentation menu
      m_idMenuScreen = m_rMenuBar->insertMenu(CORBA::string_dup(klocale->translate("&Screen Presentations")));
      m_idMenuScreen_ConfigPage = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("&Configure pages...")),m_idMenuScreen,
							 this,CORBA::string_dup("screenConfigPages"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/effect.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_AssignEffect = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							    CORBA::string_dup(klocale->translate("&Assign effect...")),m_idMenuScreen,
							    this,CORBA::string_dup("screenAssignEffect"));
      m_rMenuBar->insertSeparator(m_idMenuScreen);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/start.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Start = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						     CORBA::string_dup(klocale->translate("&Start")),m_idMenuScreen,
						     this,CORBA::string_dup("screenStart"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/stop.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Stop = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(klocale->translate("St&op")),m_idMenuScreen,
						    this,CORBA::string_dup("screenStop"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/pause.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Pause = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						     CORBA::string_dup(klocale->translate("Pa&use")),m_idMenuScreen,
						     this,CORBA::string_dup("screenPause"));
      m_rMenuBar->insertSeparator(m_idMenuScreen);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/first.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_First = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						     CORBA::string_dup(klocale->translate("&Go to start")),m_idMenuScreen,
						     this,CORBA::string_dup("screenFirst"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/prev.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Prev = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(klocale->translate("&Previous steo")),m_idMenuScreen,
						    this,CORBA::string_dup("screenPrev"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/next.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Next = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(klocale->translate("&Next step")),m_idMenuScreen,
						    this,CORBA::string_dup("screenNext"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/last.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Last = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(klocale->translate("&Go to end")),m_idMenuScreen,
						    this,CORBA::string_dup("screenLast"));
      m_idMenuScreen_Skip = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("Skip &to page")),m_idMenuScreen,
						   this,CORBA::string_dup("screenSkip"));
      m_rMenuBar->insertSeparator(m_idMenuScreen);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/screen.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_FullScreen = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							  CORBA::string_dup(klocale->translate("Full sc&reen")),m_idMenuScreen,
							  this,CORBA::string_dup("screenFullScreen"));
      m_rMenuBar->insertSeparator(m_idMenuScreen);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/pen.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Pen = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						   CORBA::string_dup(klocale->translate("&Marker or Pen")),m_idMenuScreen,
						   this,CORBA::string_dup("screenPen"));

      // help menu
      m_idMenuHelp = m_rMenuBar->insertMenu(CORBA::string_dup(klocale->translate("&Help")));
      m_idMenuHelp_Contents = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("&Contents")),m_idMenuHelp,
						     this,CORBA::string_dup("helpContents"));
      m_rMenuBar->insertSeparator(m_idMenuHelp);
      m_idMenuHelp_About = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("&About KPresenter...")),m_idMenuHelp,
						  this,CORBA::string_dup("helpAbout"));
      m_idMenuHelp_AboutKOffice = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("&About KOffice...")),m_idMenuHelp,
							 this,CORBA::string_dup("helpAboutKOffice"));
      m_idMenuHelp_AboutKDE = m_rMenuBar->insertItem(CORBA::string_dup(klocale->translate("&About KDE...")),m_idMenuHelp,
						     this,CORBA::string_dup("helpAboutKDE"));

     }
}

/*======================== setup popup menus ===================*/
void KPresenterView_impl::setupPopupMenus()
{
  QString pixdir;
  QPixmap pixmap;
  pixdir = KApplication::kde_datadir() + QString("/kpresenter/toolbar/");  

  // create right button line menu
  rb_line = new QPopupMenu();
  CHECK_PTR(rb_line);
  pixmap.load(pixdir+"lineh.xpm");
  rb_line->insertItem(pixmap,this,SLOT(insertLineH()));
  rb_line->insertSeparator();
  pixmap.load(pixdir+"linev.xpm");
  rb_line->insertItem(pixmap,this,SLOT(insertLineV()));
  rb_line->insertSeparator();
  pixmap.load(pixdir+"lined1.xpm");
  rb_line->insertItem(pixmap,this,SLOT(insertLineD1()));
  rb_line->insertSeparator();
  pixmap.load(pixdir+"lined2.xpm");
  rb_line->insertItem(pixmap,this,SLOT(insertLineD2()));
  rb_line->setMouseTracking(true);
  rb_line->setCheckable(false);

  // create right button rectangle menu
  rb_rect = new QPopupMenu();
  CHECK_PTR(rb_rect);
  pixmap.load(pixdir+"rectangle2.xpm");
  rb_rect->insertItem(pixmap,this,SLOT(insertNormRect()));
  rb_rect->insertSeparator();
  pixmap.load(pixdir+"rectangleRound.xpm");
  rb_rect->insertItem(pixmap,this,SLOT(insertRoundRect()));
  rb_rect->setMouseTracking(true);
  rb_rect->setCheckable(false);
}

/*======================= setup edit toolbar ===================*/
void KPresenterView_impl::setupEditToolbar()
{
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if (!CORBA::is_nil(m_vToolBarFactory))
    {
      // toolbar
      m_rToolBarEdit = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(klocale->translate("Edit")));
 
      // cut
      QString tmp = kapp->kde_toolbardir().copy();
      tmp += "/editcut.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonEdit_Cut = m_rToolBarEdit->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Cut")),
							this,CORBA::string_dup("editCut"));

      // copy
      tmp = kapp->kde_toolbardir().copy();
      tmp += "/editcopy.xpm";
      pix = loadPixmap(tmp);
      m_idButtonEdit_Copy = m_rToolBarEdit->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Copy")),
							 this,CORBA::string_dup("editCopy"));

      // paste
      tmp = kapp->kde_toolbardir().copy();
      tmp += "/editpaste.xpm";
      pix = loadPixmap(tmp);
      m_idButtonEdit_Paste = m_rToolBarEdit->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Paste")),
							  this,CORBA::string_dup("editPaste"));
      m_rToolBarEdit->insertSeparator();

      // delete
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/delete.xpm";
      pix = loadPixmap(tmp);
      m_idButtonEdit_Delete = m_rToolBarEdit->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Delete")),
							   this,CORBA::string_dup("editDelete"));
    }
}

/*======================= setup insert toolbar =================*/
void KPresenterView_impl::setupInsertToolbar()
{
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if (!CORBA::is_nil(m_vToolBarFactory))
    {
      // toolbar
      m_rToolBarInsert = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(klocale->translate("Insert")));
 
      // picture
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/picture.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonInsert_Picture = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
								CORBA::string_dup(klocale->translate("Insert Picture")),
								this,CORBA::string_dup("insertPicture"));
      
      // clipart
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/clipart.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Clipart = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
								CORBA::string_dup(klocale->translate("Insert Clipart")),
								this,CORBA::string_dup("insertClipart"));

      // line
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/line.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Line = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(klocale->translate("Insert Line")),
							     this,CORBA::string_dup("insertLine"));

      // rectangle
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/rectangle.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Rectangle = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
								  CORBA::string_dup(klocale->translate("Insert Rectangle")),
								  this,CORBA::string_dup("insertRectangle"));

      // circle or ellipse
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/circle.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Circle = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
							       CORBA::string_dup(klocale->translate("Insert Circle or Ellipse")),
							       this,CORBA::string_dup("insertCircleOrEllipse"));

      // text
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/text.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Text = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(klocale->translate("Insert Text")),
							     this,CORBA::string_dup("insertText"));

      // autoform
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/autoform.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Autoform = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
								 CORBA::string_dup(klocale->translate("Insert Autoform")),
								 this,CORBA::string_dup("insertAutoform"));

      // parts
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/parts.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Part = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(klocale->translate("Insert Object")),
							     this,CORBA::string_dup("insertObject"));
   }
}

/*======================= setup text toolbar ===================*/
void KPresenterView_impl::setupTextToolbar()
{
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if (!CORBA::is_nil(m_vToolBarFactory))
    {
      // toolbar
      m_rToolBarText = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(klocale->translate("Text")));
 
      // size combobox
      m_idComboText_FontSize = m_rToolBarText->insertCombo(false,CORBA::string_dup(klocale->translate("Font Size")),60,
							   this,CORBA::string_dup("sizeSelected"));
      for(unsigned int i = 4;i <= 100;i++)
	{
	  char buffer[10];
	  sprintf(buffer,"%i",i);
	  m_rToolBarText->insertComboItem(m_idComboText_FontSize,CORBA::string_dup(buffer),-1);
	}
      m_rToolBarText->setCurrentComboItem(m_idComboText_FontSize,16);
      tbFont.setPointSize(20);

      // fonts combobox
      getFonts();
      m_idComboText_FontList = m_rToolBarText->insertCombo(false,CORBA::string_dup(klocale->translate("Font List")),200,
							   this,CORBA::string_dup("fontSelected"));
      for(unsigned int i = 0;i <= fontList.count()-1;i++)
 	m_rToolBarText->insertComboItem(m_idComboText_FontList,CORBA::string_dup(fontList.at(i)),-1);
      m_rToolBarText->setCurrentComboItem(m_idComboText_FontList,1);
      m_rToolBarText->setCurrentComboItem(m_idComboText_FontList,0);
      m_rToolBarText->insertSeparator();
      tbFont.setFamily(fontList.at(0));

      // bold
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/bold.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonText_Bold = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Bold")),
							 this,CORBA::string_dup("textBold"));
      m_rToolBarText->setToggle(m_idButtonText_Bold,true);
      m_rToolBarText->setButton(m_idButtonText_Bold,false);
      tbFont.setBold(false);

      // italic
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/italic.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_Italic = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Italic")),
							   this,CORBA::string_dup("textItalic"));
      m_rToolBarText->setToggle(m_idButtonText_Italic,true);
      m_rToolBarText->setButton(m_idButtonText_Italic,false);
      tbFont.setItalic(false);

      // underline
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/underl.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_Underline = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Underline")),
							      this,CORBA::string_dup("textUnderline"));
      m_rToolBarText->setToggle(m_idButtonText_Underline,true);
      m_rToolBarText->setButton(m_idButtonText_Underline,false);
      tbFont.setUnderline(false);

      // color
      m_idButtonText_Color = m_rToolBarText->insertButton(CORBA::string_dup(""),CORBA::string_dup(klocale->translate("Color")),
							  this,CORBA::string_dup("textColor"));
      m_rToolBarText->setButtonPixmap(m_idButtonText_Color,CORBA::string_dup(colorToPixString(black)));
      tbColor = black;
      m_rToolBarText->insertSeparator();

      // align left
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignLeft.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_ALeft = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Align Left")),
							  this,CORBA::string_dup("textAlignLeft"));
      m_rToolBarText->setToggle(m_idButtonText_ALeft,true);
      m_rToolBarText->setButton(m_idButtonText_ALeft,true);
      tbAlign = TxtParagraph::LEFT;

      // align center
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignCenter.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_ACenter = m_rToolBarText->insertButton(CORBA::string_dup(pix),
							    CORBA::string_dup(klocale->translate("Align Center")),
							    this,CORBA::string_dup("textAlignCenter"));
      m_rToolBarText->setToggle(m_idButtonText_ACenter,true);
      m_rToolBarText->setButton(m_idButtonText_ACenter,false);

      // align right
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignRight.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_ARight = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Align Right")),
							   this,CORBA::string_dup("textAlignRight"));
      m_rToolBarText->setToggle(m_idButtonText_ARight,true);
      m_rToolBarText->setButton(m_idButtonText_ARight,false);
      m_rToolBarText->insertSeparator();

      // enum list
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/enumList.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_EnumList = m_rToolBarText->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(klocale->translate("Enumerated List")),
							     this,CORBA::string_dup("textEnumList"));

      // unsorted list
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/unsortedList.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_EnumList = m_rToolBarText->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(klocale->translate("Unsorted List")),
							     this,CORBA::string_dup("textUnsortList"));

      // normal text
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/normalText.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_EnumList = m_rToolBarText->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(klocale->translate("Normal Text")),
							     this,CORBA::string_dup("textNormalText"));
    }
}      

/*==================== setup extra toolbar ====================*/
void KPresenterView_impl::setupExtraToolbar()
{
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if (!CORBA::is_nil(m_vToolBarFactory))
    {
      // toolbar
      m_rToolBarExtra = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(klocale->translate("Extra")));
 
      // pen and brush
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/style.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonExtra_Style = m_rToolBarExtra->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Pen & Brush")),
							   this,CORBA::string_dup("extraPenBrush"));
      m_rToolBarExtra->insertSeparator();

      // raise
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/raise.xpm";
      pix = loadPixmap(tmp);
      m_idButtonExtra_Raise = m_rToolBarExtra->insertButton(CORBA::string_dup(pix),
							    CORBA::string_dup(klocale->translate("Raise object(s)")),
							    this,CORBA::string_dup("extraRaise"));

      // lower
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/lower.xpm";
      pix = loadPixmap(tmp);
      m_idButtonExtra_Lower = m_rToolBarExtra->insertButton(CORBA::string_dup(pix),
							    CORBA::string_dup(klocale->translate("Lower object(s)")),
							    this,CORBA::string_dup("extraLower"));
      m_rToolBarExtra->insertSeparator();

      // rotate
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/rotate.xpm";
      pix = loadPixmap(tmp);
      m_idButtonExtra_Rotate = m_rToolBarExtra->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(klocale->translate("Rotate object(s)")),
							     this,CORBA::string_dup("extraRotate"));
    }
}

/*==================== setup screen toolbar ====================*/
void KPresenterView_impl::setupScreenToolbar()
{
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if (!CORBA::is_nil(m_vToolBarFactory))
    {
      // toolbar
      m_rToolBarScreen = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(klocale->translate("Screen Presentations")));
 
      // stop
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/stop.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonScreen_Stop = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Stop")),
							     this,CORBA::string_dup("screenStop"));

      // pause
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/pause.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Pause = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Pause")),
							      this,CORBA::string_dup("screenPause"));

      // start
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/start.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Start = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Start")),
							      this,CORBA::string_dup("screenStart"));
      m_rToolBarScreen->insertSeparator();

      // first
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/first.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_First = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("First")),
							      this,CORBA::string_dup("screenFirst"));

      // previous
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/prev.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Prev = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Previous")),
							     this,CORBA::string_dup("screenPrev"));

      // next
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/next.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Next = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Next")),
							     this,CORBA::string_dup("screenNext"));

      // last
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/last.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Last = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Last")),
							     this,CORBA::string_dup("screenLast"));
      m_rToolBarScreen->insertSeparator();

      // effect
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/effect.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Effect = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),
							       CORBA::string_dup(klocale->translate("Assign Effect")),
							       this,CORBA::string_dup("screenAssignEffect"));
      m_rToolBarScreen->insertSeparator();

      // full screen
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/screen.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Full = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(klocale->translate("Full Screen")),
							     this,CORBA::string_dup("screenFullScreen"));

      // pen
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/pen.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Pen = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(klocale->translate("Merker/Pen")),
							    this,CORBA::string_dup("screenPen"));
    }
}

/*======================= setup scrollbars =====================*/
void KPresenterView_impl::setupScrollbars()
{
  vert = new QScrollBar(QScrollBar::Vertical,this);
  horz = new QScrollBar(QScrollBar::Horizontal,this);
  vert->show(); horz->show();
  QObject::connect(vert,SIGNAL(valueChanged(int)),this,SLOT(scrollV(int)));
  QObject::connect(horz,SIGNAL(valueChanged(int)),this,SLOT(scrollH(int)));
  vert->setValue(vert->maxValue());
  horz->setValue(horz->maxValue());
  vert->setValue(vert->minValue());
  horz->setValue(horz->minValue());
  if (page && !presStarted) page->resize(widget()->width()-16,widget()->height()-16);
  vert->setGeometry(widget()->width()-16,0,16,widget()->height()-16);
  horz->setGeometry(0,widget()->height()-16,widget()->width()-16,16);
}

/*===================== set ranges of scrollbars ===============*/
void KPresenterView_impl::setRanges()
{
  if (vert && horz && page && m_pKPresenterDoc)
    {
      int range;
      
      vert->setSteps(10,m_pKPresenterDoc->getPageSize(1,xOffset,yOffset).height()+20);
      range = (m_pKPresenterDoc->getPageSize(1,xOffset,yOffset).height()+10)*m_pKPresenterDoc->getPageNums()-page->height()+26 < 0 ? 0 :
	(m_pKPresenterDoc->getPageSize(1,xOffset,yOffset).height()+10)*m_pKPresenterDoc->getPageNums()-page->height()+26;
      vert->setRange(0,range);
      horz->setSteps(10,m_pKPresenterDoc->getPageSize(1,xOffset,yOffset).width()+36-page->width());
      range = m_pKPresenterDoc->getPageSize(1,xOffset,yOffset).width()+36-page->width() < 0 ? 0 :
	m_pKPresenterDoc->getPageSize(1,xOffset,yOffset).width()+36-page->width();
      horz->setRange(0,range);
    }
}

/*============== create a pixmapstring from a color ============*/
char* KPresenterView_impl::colorToPixString(QColor c)
{
  int r,g,b;
  char pix[1500];
  char line[40];

  c.rgb(&r,&g,&b);

  qstrcpy(pix,"/* XPM */\n");
  
  strcat(pix,"static char * text_xpm[] = {\n");

  sprintf(line,"%c 20 20 1 1 %c,\n",34,34);
  strcat(pix,qstrdup(line));

  sprintf(line,"%c c #%2X%2X%2X %c,\n",34,r,g,b,34);
  strcat(pix,qstrdup(line));

  sprintf(line,"%c                    %c,\n",34,34);
  for (unsigned int i = 1;i <= 20;i++)
    strcat(pix,qstrdup(line));
    
  sprintf(line,"%c                    %c};\n",34,34);
  strcat(pix,qstrdup(line));
  
  return (char*)&pix;
}

/*===================== load not KDE installed fonts =============*/
void KPresenterView_impl::getFonts()
{
  int numFonts;
  Display *kde_display;
  char** fontNames;
  char** fontNames_copy;
  QString qfontname;
  
  kde_display = XOpenDisplay(0L);

  // now try to load the KDE fonts
  bool have_installed = kapp->getKDEFonts(&fontList);
  
  // if available we are done, the kde fonts are now in the family_combo
  if (have_installed)
    return;

  fontNames = XListFonts(kde_display,"*",32767,&numFonts);
  fontNames_copy = fontNames;
  
  for(int i = 0; i < numFonts; i++){
    
    if (**fontNames != '-')
      { 
      // The font name doesn't start with a dash -- an alias
      // so we ignore it. It is debatable whether this is the right
      // behaviour so I leave the following snippet of code around.
      // Just uncomment it if you want those aliases to be inserted as well.
      
      /*
	qfontname = "";
      qfontname = *fontNames;
      if(fontlist.find(qfontname) == -1)
          fontlist.inSort(qfontname);
      */

      fontNames ++;
      continue;
    };
      
    qfontname = "";
    qfontname = *fontNames;
    int dash = qfontname.find ('-', 1, TRUE); // find next dash

    if (dash == -1)  // No such next dash -- this shouldn't happen.
                      // but what do I care -- lets skip it.
      {
	fontNames ++;
	continue;
      }

    // the font name is between the second and third dash so:
    // let's find the third dash:
    int dash_two = qfontname.find ('-', dash + 1 , TRUE); 

    if (dash == -1)  // No such next dash -- this shouldn't happen.
                      // But what do I care -- lets skip it.
      {
	fontNames ++;
	continue;
      }

    // fish the name of the font info string
    qfontname = qfontname.mid(dash +1, dash_two - dash -1);

    if( !qfontname.contains("open look", TRUE))
      {
	if(qfontname != "nil"){
	  if(fontList.find(qfontname) == -1)
	    fontList.inSort(qfontname);
	}
      }
    
    
    fontNames ++;

  }
  
  XFreeFontNames(fontNames_copy);
  XCloseDisplay(kde_display);
}

/*========================== start rect selection ================*/
void KPresenterView_impl::startRectSelection(const char *_part_name)
{
  m_strNewPart = _part_name;
  m_bRectSelection = true;
}

/*========================== cancel rect selection ===============*/
void KPresenterView_impl::cancelRectSelection()
{
  m_bRectSelection = false;
  update();
}
 
/*========================== paint rect selection ================*/
void KPresenterView_impl::paintRectSelection()
{
  printf("paintRect\n");
  QPainter painter;
  painter.begin(page);
  
  painter.setRasterOp(NotROP);
  painter.drawRect(m_rctRectSelection);
  painter.end();
}

/*======================== mouse press event =====================*/
void KPresenterView_impl::mousePressEvent(QMouseEvent *_ev)
{
  cout << "Mouse pressed" << endl;
  
  if ( !m_bRectSelection )
    return;

  m_rctRectSelection.setTop(_ev->pos().y());
  m_rctRectSelection.setLeft(_ev->pos().x());
  m_rctRectSelection.setBottom(_ev->pos().y());
  m_rctRectSelection.setRight(_ev->pos().x());

  paintRectSelection();
}

/*========================= mouse move event =====================*/
void KPresenterView_impl::mouseMoveEvent(QMouseEvent *_ev)
{
  if (!m_bRectSelection)
    return;
  
  paintRectSelection();

  if (_ev->pos().y() < m_rctRectSelection.top())
    m_rctRectSelection.setBottom(m_rctRectSelection.top());
  else
    m_rctRectSelection.setBottom(_ev->pos().y());

  if (_ev->pos().x() < m_rctRectSelection.left())
    m_rctRectSelection.setRight(m_rctRectSelection.left());
  else
    m_rctRectSelection.setRight(_ev->pos().x());

  paintRectSelection();
}

/*======================= mouse release event ====================*/
void KPresenterView_impl::mouseReleaseEvent(QMouseEvent *_ev)
{
  if (!m_bRectSelection)
    return;

  paintRectSelection();
  
  if (_ev->pos().y() < m_rctRectSelection.top())
    m_rctRectSelection.setBottom(m_rctRectSelection.top());
  else
    m_rctRectSelection.setBottom(_ev->pos().y());

  if (_ev->pos().x() < m_rctRectSelection.left())
    m_rctRectSelection.setRight(m_rctRectSelection.left());
  else
    m_rctRectSelection.setRight(_ev->pos().x());
  
  m_bRectSelection = false;
  m_pKPresenterDoc->insertObject(m_rctRectSelection,m_strNewPart);
}

/*======================== set mode ==============================*/
void KPresenterView_impl::setMode(OPParts::Part::Mode _mode)
{
  Part_impl::setMode(_mode);
  
  if (mode() == OPParts::Part::ChildMode && !m_bFocus)
    m_bShowGUI = false;
  else
    m_bShowGUI = true;
}

/*===================== set focus ================================*/
void KPresenterView_impl::setFocus(CORBA::Boolean _mode)
{
  Part_impl::setFocus(_mode);

  bool old = m_bShowGUI;
  
  if (mode() == OPParts::Part::ChildMode && !m_bFocus)
    m_bShowGUI = false;
  else
    m_bShowGUI = true;

  if (old != m_bShowGUI)
    resizeEvent(0L);
}
