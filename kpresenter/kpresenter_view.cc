/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.1.0                                                 */
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

#include <qprinter.h>
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
KPresenterView_impl::KPresenterView_impl(QWidget *_parent,const char *_name)
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
  rotateDia = 0;
  searchDia = 0;
  replaceDia = 0;
  shadowDia = 0;
  presStructView = 0;
  xOffset = 0;
  yOffset = 0;
  v_ruler = 0;
  h_ruler = 0;
  pen = QPen(black,1,SolidLine);
  brush = QBrush(white,SolidPattern);
  lineBegin = L_NORMAL;
  lineEnd = L_NORMAL;
  gColor1 = red;
  gColor2 = green;
  gType = BCT_GHORZ;
  fillType = FT_BRUSH;
  setMouseTracking(true);
  m_bShowGUI = true;
  m_bRectSelection = false;
  presStarted = false;
  searchFirst = true;
  continuePres = false;
  exitPres = false;
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

/*=========================== file print =======================*/
CORBA::Boolean KPresenterView_impl::printDlg()
{
  QPrinter prt;
  prt.setMinMax(1,m_pKPresenterDoc->getPageNums());
  bool makeLandscape = false;

  switch (m_pKPresenterDoc->pageLayout().format)
    {
#if QT_VERSION >= 141
    case PG_DIN_A3: prt.setPageSize(QPrinter::A3);
      break;
#endif
    case PG_DIN_A4: prt.setPageSize(QPrinter::A4);
      break;
#if QT_VERSION >= 141
    case PG_DIN_A5: prt.setPageSize(QPrinter::A5);
      break;
#endif
    case PG_US_LETTER: prt.setPageSize(QPrinter::Letter);
      break;
    case PG_US_LEGAL: prt.setPageSize(QPrinter::Legal);
      break;
    case PG_US_EXECUTIVE: prt.setPageSize(QPrinter::Executive);
      break;
    case PG_DIN_B5: prt.setPageSize(QPrinter::B5);
      break;
    case PG_SCREEN:
      {
	warning(i18n("You use the page layout SCREEN. I print it in DIN A4 LANDSCAPE!"));
	prt.setPageSize(QPrinter::A4);
	makeLandscape = true;
      }	break;
    default:
      {
	warning(i18n("The used page layout is not supported by QPrinter. I set it to DIN A4."));
	prt.setPageSize(QPrinter::A4);
      } break;
    }

  switch (m_pKPresenterDoc->pageLayout().orientation)
    {
    case PG_PORTRAIT: prt.setOrientation(QPrinter::Portrait);
      break;
    case PG_LANDSCAPE: prt.setOrientation(QPrinter::Landscape);
      break;
    }

  float left_margin = 0.0;
  float top_margin = 0.0;

  if (makeLandscape) 
    {
      prt.setOrientation(QPrinter::Landscape);
      left_margin = 28.5;
      top_margin = 15.0;
    }

  if (prt.setup(this))
    {    
      page->deSelectAllObj();
      QPainter painter;
      painter.begin(&prt);
      page->print(&painter,&prt,left_margin,top_margin);
      painter.end();
    }
  return true;
}

/*========================== edit undo ==========================*/
void KPresenterView_impl::editUndo()
{
  m_pKPresenterDoc->commands()->undo();
}

/*========================== edit redo ==========================*/
void KPresenterView_impl::editRedo()
{
  m_pKPresenterDoc->commands()->redo();
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
  page->deSelectAllObj();
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
  page->selectAllObj();
}

/*========================== edit find ==========================*/
void KPresenterView_impl::editFind()
{
  if (searchDia)
    {
      QObject::disconnect(searchDia,SIGNAL(doSearch(QString,bool,bool)),this,SLOT(search(QString,bool,bool)));
      searchDia->close();
      delete searchDia;
      searchDia = 0;
    }

  if (page->kTxtObj())
    {
      searchDia = new KSearchDialog(0,"SearchDia");
      searchDia->setMaximumSize(searchDia->width(),searchDia->height());
      searchDia->setMinimumSize(searchDia->width(),searchDia->height());
      QObject::connect(searchDia,SIGNAL(doSearch(QString,bool,bool)),this,SLOT(search(QString,bool,bool)));
      searchDia->show();
      
      searchFirst = true;
    }
}

/*========================== edit find and replace ==============*/
void KPresenterView_impl::editFindReplace()
{
  if (replaceDia)
    {
      QObject::disconnect(replaceDia,SIGNAL(doSearchReplace(QString,QString,bool,bool)),this,SLOT(replace(QString,QString,bool,bool)));
      QObject::disconnect(replaceDia,SIGNAL(doSearchReplaceAll(QString,QString,bool)),this,SLOT(replaceAll(QString,QString,bool)));
      replaceDia->close();
      delete replaceDia;
      replaceDia = 0;
    }

  if (page->kTxtObj())
    {
      replaceDia = new KSearchReplaceDialog(0,"ReplaceDia");
      replaceDia->setMaximumSize(replaceDia->width(),replaceDia->height());
      replaceDia->setMinimumSize(replaceDia->width(),replaceDia->height());
      QObject::connect(replaceDia,SIGNAL(doSearchReplace(QString,QString,bool,bool)),this,SLOT(replace(QString,QString,bool,bool)));
      QObject::connect(replaceDia,SIGNAL(doSearchReplaceAll(QString,QString,bool)),this,SLOT(replaceAll(QString,QString,bool)));
      replaceDia->show();
      
      searchFirst = true;
    }
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

  QString file = KFileDialog::getOpenFileName(0,
					      i18n("*.gif *GIF *.bmp *.BMP *.xbm *.XBM *.xpm *.XPM *.pnm *.PNM "
					      "*.PBM *.PGM *.PPM *.PBMRAW *.PGMRAW *.PPMRAW "
					      "*.pbm *.pgm *.ppm *.pbmdraw *.pgmdraw *.ppmdraw|All pictures\n"
					      "*.gif *.GIF|GIF-Pictures\n"
					      "*.jpg *.JPG *.jpeg *.JPEG|JPEG-Pictures\n"
					      "*.bmp *.BMP|Windows Bitmaps\n"
					      "*.xbm *.XBM|XWindow Pitmaps\n"
					      "*.xpm *.XPM|Pixmaps\n"
					      "*.pnm *.PNM *.PBM *.PGM *.PPM *.PBMRAW *.PGMRAW *.PPMRAW "
					      "*.pbm *.pgm *.ppm *.pbmdraw *.pgmdraw *.ppmdraw|PNM-Pictures"),0);

  if (!file.isEmpty()) m_pKPresenterDoc->insertPicture(file.data(),xOffset,yOffset);

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
  QString file = KFileDialog::getOpenFileName(0,i18n("*.WMF *.wmf|Windows Metafiles"),0);
  if (!file.isEmpty()) m_pKPresenterDoc->insertClipart(file.data(),xOffset,yOffset);

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
  m_pKPresenterDoc->insertCircleOrEllipse(pen,brush,fillType,gColor1,gColor2,gType,xOffset,yOffset);
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
  afChoose = new AFChoose(0,i18n("Autoform-Choose"));
  afChoose->setCaption(i18n("KPresenter - Insert an Autoform"));
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
  
  m_pKPresenterDoc->insertObject(QRect(40,40,150,150),pe->name(),xOffset,yOffset);
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
  styleDia->setLineBegin(m_pKPresenterDoc->getLineBegin(lineBegin));
  styleDia->setLineEnd(m_pKPresenterDoc->getLineEnd(lineEnd));
  styleDia->setFillType(m_pKPresenterDoc->getFillType(fillType));
  styleDia->setGradient(m_pKPresenterDoc->getGColor1(gColor1),
			m_pKPresenterDoc->getGColor2(gColor2),
			m_pKPresenterDoc->getGType(gType));
  styleDia->setCaption(i18n("KPresenter - Pen and Brush"));
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
  if (rotateDia)
    {
      QObject::disconnect(rotateDia,SIGNAL(rotateDiaOk()),this,SLOT(rotateOk()));
      rotateDia->close();
      delete rotateDia;
      rotateDia = 0;
    }

  if (m_pKPresenterDoc->numSelected() > 0)
    {
      rotateDia = new RotateDia(0,"Rotate");
      rotateDia->setMaximumSize(rotateDia->width(),rotateDia->height());
      rotateDia->setMinimumSize(rotateDia->width(),rotateDia->height());
      rotateDia->setCaption(i18n("KPresenter - Rotate"));
      QObject::connect(rotateDia,SIGNAL(rotateDiaOk()),this,SLOT(rotateOk()));
      rotateDia->setAngle(m_pKPresenterDoc->getSelectedObj()->getAngle());
      rotateDia->show();
    }
}

/*========================== extra shadow =======================*/
void KPresenterView_impl::extraShadow()
{
  if (shadowDia)
    {
      QObject::disconnect(shadowDia,SIGNAL(shadowDiaOk()),this,SLOT(shadowOk()));
      shadowDia->close();
      delete shadowDia;
      shadowDia = 0;
    }

  if (m_pKPresenterDoc->numSelected() > 0)
    {
      shadowDia = new ShadowDia(0,"Shadow");
      shadowDia->setMaximumSize(shadowDia->width(),shadowDia->height());
      shadowDia->setMinimumSize(shadowDia->width(),shadowDia->height());
      shadowDia->setCaption(i18n("KPresenter - Shadow"));
      QObject::connect(shadowDia,SIGNAL(shadowDiaOk()),this,SLOT(shadowOk()));
      shadowDia->setShadowDirection(m_pKPresenterDoc->getSelectedObj()->getShadowDirection());
      shadowDia->setShadowDistance(m_pKPresenterDoc->getSelectedObj()->getShadowDistance());
      shadowDia->setShadowColor(m_pKPresenterDoc->getSelectedObj()->getShadowColor());
      shadowDia->show();
    }
}

/*========================== extra align obj ====================*/
void KPresenterView_impl::extraAlignObj()
{
  QPoint pnt(QCursor::pos());

  rb_oalign->popup(pnt);

//   QEvent ev(Event_Leave);
//   QMouseEvent mev(Event_MouseButtonRelease,
// 		  QCursor::pos(),LeftButton,LeftButton);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Line),&ev);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Line),&mev);
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
  backDia = new BackDia(0,"InfoDia",m_pKPresenterDoc->getBackType(getCurrPgNum() - 1),
			m_pKPresenterDoc->getBackColor1(getCurrPgNum() - 1),
			m_pKPresenterDoc->getBackColor2(getCurrPgNum() - 1),
			m_pKPresenterDoc->getBackColorType(getCurrPgNum() - 1),
			m_pKPresenterDoc->getBackPixFilename(getCurrPgNum() - 1),
			m_pKPresenterDoc->getBackClipFilename(getCurrPgNum() - 1),
			m_pKPresenterDoc->getBackView(getCurrPgNum() - 1));
  backDia->setMaximumSize(backDia->width(),backDia->height());
  backDia->setMinimumSize(backDia->width(),backDia->height());
  backDia->setCaption(i18n("KPresenter - Page Background"));
  QObject::connect(backDia,SIGNAL(backOk(bool)),this,SLOT(backOk(bool)));
  backDia->show();
}

/*======================= extra layout ==========================*/
void KPresenterView_impl::extraLayout()
{
  KoPageLayout pgLayout = m_pKPresenterDoc->pageLayout();
  KoPageLayout oldLayout = m_pKPresenterDoc->pageLayout();
  KoHeadFoot hf;
  
  if (KoPageLayoutDia::pageLayout(pgLayout,hf,FORMAT_AND_BORDERS)) 
    {
      PgLayoutCmd *pgLayoutCmd = new PgLayoutCmd(i18n("Set Pagelayout"),pgLayout,oldLayout,this);
      pgLayoutCmd->execute();
      KPresenterDoc()->commands()->addCommand(pgLayoutCmd);
    }
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
  optionDia->setCaption(i18n("KPresenter - Options"));
  optionDia->setMaximumSize(optionDia->minimumSize());
  optionDia->resize(optionDia->minimumSize());
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
			    KPresenterDoc()->spManualSwitch(),getCurrPgNum(),
			    KPresenterDoc()->backgroundList()->at(getCurrPgNum() - 1)->getPageEffect(),
			    KPresenterDoc()->getPresSpeed());
  pgConfDia->setMaximumSize(pgConfDia->width(),pgConfDia->height());
  pgConfDia->setMinimumSize(pgConfDia->width(),pgConfDia->height());
  pgConfDia->setCaption(i18n("KPresenter - Page Configuration for Screenpresentations"));
  QObject::connect(pgConfDia,SIGNAL(pgConfDiaOk()),this,SLOT(pgConfOk()));
  pgConfDia->show();
}

/*========================== screen presStructView  =============*/
void KPresenterView_impl::screenPresStructView()
{
  if (!presStructView)
    {
      page->deSelectAllObj();
      
      presStructView = new PresStructViewer(0,"",KPresenterDoc(),this);
      presStructView->setCaption(i18n("KPresenter - Presentation structure viewer"));
      QObject::connect(presStructView,SIGNAL(presStructViewClosed()),this,SLOT(psvClosed()));
      presStructView->show();
    }
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
      effectDia->setCaption(i18n("KPresenter - Assign effects"));
      QObject::connect(effectDia,SIGNAL(effectDiaOk()),this,SLOT(effectOk()));
      effectDia->show();
      page->deSelectAllObj();
      page->selectObj(_oNum);
    }
  else
    QMessageBox::critical((QWidget*)0L,i18n("KPresenter Error"),
			  i18n("I can't assign an effect. You have to select EXACTLY one object!"),
			  i18n("OK"));
}

/*========================== screen start =======================*/
void KPresenterView_impl::screenStart()
{
  bool fullScreen = true; //m_rToolBarScreen->isButtonOn(m_idButtonScreen_Full);

  if (page && !presStarted) 
    {
      // disable screensaver
      QString pidFile;
      pidFile = getenv("HOME");
      pidFile += "/.kss.pid";
      FILE *fp;
      if ((fp = fopen(pidFile,"r")) != NULL)
	{
	  fscanf(fp,"%d",&screensaver_pid);
	  fclose(fp);
	  kill(screensaver_pid,SIGSTOP);
	}

      hideParts();

      page->deSelectAllObj();
      presStarted = true;
      if (fullScreen)
	{
	  page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());

	  float _presFaktW = static_cast<float>(page->width()) / 
	    static_cast<float>(KPresenterDoc()->getPageSize(0,0,0,1.0,false).width()) > 1.0 ? 
	    static_cast<float>(page->width()) / static_cast<float>(KPresenterDoc()->getPageSize(0,0,0,1.0,false).width()) : 1.0;
	  float _presFaktH = static_cast<float>(page->height()) / 
	    static_cast<float>(KPresenterDoc()->getPageSize(0,0,0,1.0,false).height()) > 
	    1.0 ? static_cast<float>(page->height()) / static_cast<float>(KPresenterDoc()->getPageSize(0,0,0,1.0,false).height()) : 1.0;
	  float _presFakt = min(_presFaktW,_presFaktH);
	  page->setPresFakt(_presFakt);
	}
      else
	{
	  float _presFakt = 1.0;
	  page->setPresFakt(_presFakt);
	}
	  
      _xOffset = xOffset;
      _yOffset = yOffset;
      xOffset = 0;
      yOffset = 0;

      if (page->width() > KPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).width())
 	xOffset -= (page->width() - KPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).width()) / 2;
      if (page->height() > KPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).height())
 	yOffset -= (page->height() - KPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).height()) / 2;

      vert->setEnabled(false);
      horz->setEnabled(false);
      m_bShowGUI = false;
      page->setBackgroundColor(black);
      oldSize = widget()->size();
      widget()->resize(page->size());
      setSize(page->size().width(),page->size().height());
      page->startScreenPresentation(fullScreen);

      if (fullScreen)
	{
	  page->recreate((QWidget*)0L,WStyle_Customize | WStyle_NoBorder | WType_Popup,QPoint(0,0),true);
	  page->topLevelWidget()->move(0,0);
	  page->topLevelWidget()->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
	  page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
	  page->topLevelWidget()->setBackgroundColor(black);
	  page->setFocusPolicy(QWidget::StrongFocus);
	  page->setFocus();
	}
      else
	{
	  page->setBackgroundColor(black);
	  page->setFocusPolicy(QWidget::StrongFocus);
	  page->setFocus();
	}

      if (!KPresenterDoc()->spManualSwitch())
	{
	  continuePres = true;
	  exitPres = false;
	  doAutomaticScreenPres();
	}
    }
}

/*========================== screen stop ========================*/
void KPresenterView_impl::screenStop()
{
  if (presStarted)
    {
      continuePres = false;
      exitPres = true;
      if (true) //m_rToolBarScreen->isButtonOn(m_idButtonScreen_Full))
	{
	  page->close(false);
	  page->recreate((QWidget*)this,0,QPoint(0,0),true);
	}
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

      showParts();

      // start screensaver again
      QString pidFile;
      pidFile = getenv("HOME");
      pidFile += "/.kss.pid";
      FILE *fp;
      if ((fp = fopen(pidFile,"r")) != NULL)
	{
	  fscanf(fp,"%d",&screensaver_pid);
	  fclose(fp);
	  kill(screensaver_pid,SIGCONT);
	}
    }
}

/*========================== screen pause =======================*/
void KPresenterView_impl::screenPause()
{
}

/*========================== screen first =======================*/
void KPresenterView_impl::screenFirst()
{
  vert->setValue(0); 
}

/*========================== screen pevious =====================*/
void KPresenterView_impl::screenPrev()
{
  if (presStarted)
    {
      if (page->pPrev(true))
	{
	  yOffset -= KPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).height(); 
	  page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
	  page->repaint(false);
	  page->setFocus();
	}
      else
	{
	  page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
	  page->setFocus();
	}
      QPainter p(page);
      presentParts(page->presFakt(),&p,QRect(0,0,0,0),xOffset,yOffset);
      p.end();
    }
  else
    vert->setValue(yOffset - KPresenterDoc()->getPageSize(0,0,0,1.0,false).height()); 
}

/*========================== screen next ========================*/
void KPresenterView_impl::screenNext()
{
  if (presStarted)
    {
      if (page->pNext(true))
	{
	  yOffset += KPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).height(); 
	  page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
	  //page->repaint(false);
	  page->setFocus();
	}
      else
	{
	  page->resize(QApplication::desktop()->width(),QApplication::desktop()->height());
	  page->setFocus();
	}
      QPainter p(page);
      presentParts(page->presFakt(),&p,QRect(0,0,0,0),xOffset,yOffset);
      p.end();
    }
  else
    vert->setValue(yOffset + KPresenterDoc()->getPageSize(0,0,0,1.0,false).height()); 
}

/*========================== screen last ========================*/
void KPresenterView_impl::screenLast()
{
  vert->setValue(vert->maxValue());
}

/*========================== screen skip =======================*/
void KPresenterView_impl::screenSkip()
{
}

/*========================== screen full screen ================*/
void KPresenterView_impl::screenFullScreen()
{
  warning("Screenpresentations only work in FULLSCREEN mode at the moment!");
}

/*========================== screen pen/marker =================*/
void KPresenterView_impl::screenPen()
{
  QPoint pnt(QCursor::pos());

  rb_pen->popup(pnt);
}

/*======================= help contents ========================*/
void KPresenterView_impl::helpContents()
{
}

/*======================= help about ===========================*/
void KPresenterView_impl::helpAbout()
{
  KoAboutDia::about(KoAboutDia::KPresenter,"0.1.0");
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

  m_rToolBarText->setButton(m_idButtonText_ALeft,false);
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

  m_rToolBarText->setButton(m_idButtonText_ACenter,false);
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

  m_rToolBarText->setButton(m_idButtonText_ARight,false);
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
      
      if (KEnumListDia::enumListDia(_type,_font,_color,_before,_after,_start,fontList))
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
      
      if (KCharSelectDia::selectChar(_font,_color,_c,fontList))
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

/*======================= align object left =====================*/
void KPresenterView_impl::extraAlignObjLeftidl()
{
  KPresenterDoc()->alignObjsLeft();
}

/*======================= align object center h =================*/
void KPresenterView_impl::extraAlignObjCenterHidl()
{
  KPresenterDoc()->alignObjsCenterH();
}

/*======================= align object right ====================*/
void KPresenterView_impl::extraAlignObjRightidl()
{
  KPresenterDoc()->alignObjsRight();
}

/*======================= align object top ======================*/
void KPresenterView_impl::extraAlignObjTopidl()
{
  KPresenterDoc()->alignObjsTop();
}

/*======================= align object center v =================*/
void KPresenterView_impl::extraAlignObjCenterVidl()
{
  KPresenterDoc()->alignObjsCenterV();
}

/*======================= align object bottom ===================*/
void KPresenterView_impl::extraAlignObjBottomidl()
{
  KPresenterDoc()->alignObjsBottom();
}

/*===============================================================*/
void KPresenterView_impl::newPageLayout(KoPageLayout _layout)
{
  KoPageLayout oldLayout = m_pKPresenterDoc->pageLayout();

  PgLayoutCmd *pgLayoutCmd = new PgLayoutCmd(i18n("Set Pagelayout"),_layout,oldLayout,this);
  pgLayoutCmd->execute();
  KPresenterDoc()->commands()->addCommand(pgLayoutCmd);
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
  setupAccelerators();
  setupRulers();
  m_rMenuBar->setItemChecked(m_idMenuExtra_TAlign_Left,true);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW3,true);
 
  if (m_pKPresenterDoc && page)
    QObject::connect(page,SIGNAL(stopPres()),this,SLOT(stopPres()));

  resizeEvent(0L);

  // Show every embedded object
  QListIterator<KPresenterChild> it = m_pKPresenterDoc->childIterator();
  for(;it.current();++it)
    slotInsertObject(it.current());

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
  m_pKPresenterDoc->changeChildGeometry(f->child(),_frame->partGeometry(),xOffset,yOffset);
}

/*==================== slot move end ===========================*/
void KPresenterView_impl::slotMoveEnd(PartFrame_impl* _frame)
{
  KPresenterFrame *f = (KPresenterFrame*)_frame;
  // TODO scaling
  m_pKPresenterDoc->changeChildGeometry(f->child(),_frame->partGeometry(),xOffset,yOffset);
}

/*=========== take changes for backgr dialog =====================*/
void KPresenterView_impl::backOk(bool takeGlobal)
{
  SetBackCmd *setBackCmd = new SetBackCmd(i18n("Set Background"),backDia->getBackColor1(),
					  backDia->getBackColor2(),backDia->getBackColorType(),
					  backDia->getBackPixFilename(),backDia->getBackClipFilename(),
					  backDia->getBackView(),backDia->getBackType(),
					  m_pKPresenterDoc->getBackColor1(getCurrPgNum() - 1),
					  m_pKPresenterDoc->getBackColor2(getCurrPgNum() - 1),
					  m_pKPresenterDoc->getBackColorType(getCurrPgNum() - 1),
					  m_pKPresenterDoc->getBackPixFilename(getCurrPgNum() - 1),
					  m_pKPresenterDoc->getBackClipFilename(getCurrPgNum() - 1),
					  m_pKPresenterDoc->getBackView(getCurrPgNum() - 1),
					  m_pKPresenterDoc->getBackType(getCurrPgNum() - 1),
					  takeGlobal,getCurrPgNum(),m_pKPresenterDoc);
  setBackCmd->execute();
  m_pKPresenterDoc->commands()->addCommand(setBackCmd);
}

/*================== autoform chosen =============================*/
void KPresenterView_impl::afChooseOk(const char* c)
{
  QString afDir = kapp->kde_datadir();
  QFileInfo fileInfo(c);
  QString fileName(afDir + "/kpresenter/autoforms/" + fileInfo.dirPath(false) + "/" + fileInfo.baseName() + ".atf");
  
  page->deSelectAllObj();
  m_pKPresenterDoc->insertAutoform(pen,brush,lineBegin,lineEnd,fillType,gColor1,gColor2,gType,fileName,xOffset,yOffset);
}

/*=========== take changes for style dialog =====================*/
void KPresenterView_impl::styleOk()
{
  if (!m_pKPresenterDoc->setPenBrush(styleDia->getPen(),styleDia->getBrush(),styleDia->getLineBegin(),
				     styleDia->getLineEnd(),styleDia->getFillType(),styleDia->getGColor1(),
				     styleDia->getGColor2(),styleDia->getGType(),xOffset,yOffset))
    {
      pen = styleDia->getPen();
      brush = styleDia->getBrush();
      lineBegin = styleDia->getLineBegin();
      lineEnd = styleDia->getLineEnd();
      fillType = styleDia->getFillType();
      gColor1 = styleDia->getGColor1();
      gColor2 = styleDia->getGColor2();
      gType = styleDia->getGType();
    }
}

/*=========== take changes for option dialog ====================*/
void KPresenterView_impl::optionOk()
{
  if (optionDia->getRastX() < 1)
    optionDia->setRastX(1);
  if (optionDia->getRastY() < 1)
    optionDia->setRastY(1);
  KPresenterDoc()->setRasters(optionDia->getRastX(),optionDia->getRastY(),false);

  KPresenterDoc()->setTxtBackCol(optionDia->getBackCol());
  KPresenterDoc()->setTxtSelCol(optionDia->getSelCol());
  if (optionDia->getRndX() < 1)
    optionDia->setRndX(1);
  if (optionDia->getRndY() < 1)
    optionDia->setRndY(1);
  KPresenterDoc()->setRnds(optionDia->getRndX(),optionDia->getRndY(),false);

  KPresenterDoc()->replaceObjs();
  KPresenterDoc()->repaint(false);
}

/*=================== page configuration ok ======================*/
void KPresenterView_impl::pgConfOk()
{
  PgConfCmd *pgConfCmd = new PgConfCmd(i18n("Configure Page for Screenpresentations"),
				       pgConfDia->getManualSwitch(),pgConfDia->getInfinitLoop(),
				       pgConfDia->getPageEffect(),pgConfDia->getPresSpeed(),
				       KPresenterDoc()->spInfinitLoop(),KPresenterDoc()->spManualSwitch(),
				       KPresenterDoc()->backgroundList()->at(getCurrPgNum() - 1)->getPageEffect(),
				       KPresenterDoc()->getPresSpeed(),KPresenterDoc(),getCurrPgNum() - 1);
  pgConfCmd->execute();
  KPresenterDoc()->commands()->addCommand(pgConfCmd);
}

/*=================== effect dialog ok ===========================*/
void KPresenterView_impl::effectOk()
{
}

/*=================== rotate dialog ok ===========================*/
void KPresenterView_impl::rotateOk()
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(KPresenterDoc()->objectList()->count());i++)
    {
      kpobject = KPresenterDoc()->objectList()->at(i);
      if (kpobject->isSelected())
	kpobject->rotate(rotateDia->getAngle());
    }

  KPresenterDoc()->repaint(false);
}

/*=================== shadow dialog ok ==========================*/
void KPresenterView_impl::shadowOk()
{
  KPObject *kpobject = 0;
  
  for (int i = 0;i < static_cast<int>(KPresenterDoc()->objectList()->count());i++)
    {
      kpobject = KPresenterDoc()->objectList()->at(i);
      if (kpobject->isSelected())
	{
	  kpobject->setShadowDirection(shadowDia->getShadowDirection());
	  kpobject->setShadowDistance(shadowDia->getShadowDistance());
	  kpobject->setShadowColor(shadowDia->getShadowColor());
	}
    }

  KPresenterDoc()->repaint(false);
}

/*================================================================*/
void KPresenterView_impl::psvClosed()
{
  QObject::disconnect(presStructView,SIGNAL(presStructViewClosed()),this,SLOT(psvClosed()));
  presStructView = 0;
}

/*================== scroll horizontal ===========================*/
void KPresenterView_impl::scrollH(int _value)
{
  if (!presStarted)
    {
      int xo = xOffset;
      
      xOffset = _value;
      page->scroll(xo - _value,0);
      if (h_ruler)
	h_ruler->setOffset(xOffset,0);
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

      if (v_ruler)
	v_ruler->setOffset(0,-KPresenterDoc()->getPageSize(getCurrPgNum() - 1,xOffset,yOffset,1.0,false).y());
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
	default: break;
	}
    }
}

/*======================= insert line (-) =======================*/
void KPresenterView_impl::insertLineH()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,lineBegin,lineEnd,LT_HORZ,xOffset,yOffset);
}

/*======================= insert line (|) =======================*/
void KPresenterView_impl::insertLineV()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,lineBegin,lineEnd,LT_VERT,xOffset,yOffset);
}

/*======================= insert line (\) =======================*/
void KPresenterView_impl::insertLineD1()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,lineBegin,lineEnd,LT_LU_RD,xOffset,yOffset);
}

/*======================= insert line (/) =======================*/
void KPresenterView_impl::insertLineD2()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,lineBegin,lineEnd,LT_LD_RU,xOffset,yOffset);
}

/*===================== insert normal rect  =====================*/
void KPresenterView_impl::insertNormRect()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertRectangle(pen,brush,RT_NORM,fillType,gColor1,gColor2,gType,xOffset,yOffset);
}

/*===================== insert round rect =======================*/
void KPresenterView_impl::insertRoundRect()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertRectangle(pen,brush,RT_ROUND,fillType,gColor1,gColor2,gType,xOffset,yOffset);
}

/*======================== set pres pen width 1 =================*/
void KPresenterView_impl::presPen1()
{
  QPen p = KPresenterDoc()->presPen();
  p.setWidth(1);
  KPresenterDoc()->setPresPen(p);
  rb_pen_width->setItemChecked(W1,true);
  rb_pen_width->setItemChecked(W2,false);
  rb_pen_width->setItemChecked(W3,false);
  rb_pen_width->setItemChecked(W4,false);
  rb_pen_width->setItemChecked(W5,false);
  rb_pen_width->setItemChecked(W6,false);
  rb_pen_width->setItemChecked(W7,false);
  rb_pen_width->setItemChecked(W8,false);
  rb_pen_width->setItemChecked(W9,false);
  rb_pen_width->setItemChecked(W10,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW1,true);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW2,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW3,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW4,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW5,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW6,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW7,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW8,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW9,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 2 =================*/
void KPresenterView_impl::presPen2()
{
  QPen p = KPresenterDoc()->presPen();
  p.setWidth(2);
  KPresenterDoc()->setPresPen(p);
  rb_pen_width->setItemChecked(W1,false);
  rb_pen_width->setItemChecked(W2,true);
  rb_pen_width->setItemChecked(W3,false);
  rb_pen_width->setItemChecked(W4,false);
  rb_pen_width->setItemChecked(W5,false);
  rb_pen_width->setItemChecked(W6,false);
  rb_pen_width->setItemChecked(W7,false);
  rb_pen_width->setItemChecked(W8,false);
  rb_pen_width->setItemChecked(W9,false);
  rb_pen_width->setItemChecked(W10,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW1,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW2,true);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW3,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW4,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW5,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW6,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW7,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW8,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW9,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 3 =================*/
void KPresenterView_impl::presPen3()
{
  QPen p = KPresenterDoc()->presPen();
  p.setWidth(3);
  KPresenterDoc()->setPresPen(p);
  rb_pen_width->setItemChecked(W1,false);
  rb_pen_width->setItemChecked(W2,false);
  rb_pen_width->setItemChecked(W3,true);
  rb_pen_width->setItemChecked(W4,false);
  rb_pen_width->setItemChecked(W5,false);
  rb_pen_width->setItemChecked(W6,false);
  rb_pen_width->setItemChecked(W7,false);
  rb_pen_width->setItemChecked(W8,false);
  rb_pen_width->setItemChecked(W9,false);
  rb_pen_width->setItemChecked(W10,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW1,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW2,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW3,true);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW4,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW5,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW6,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW7,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW8,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW9,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 4 =================*/
void KPresenterView_impl::presPen4()
{
  QPen p = KPresenterDoc()->presPen();
  p.setWidth(4);
  KPresenterDoc()->setPresPen(p);
  rb_pen_width->setItemChecked(W1,false);
  rb_pen_width->setItemChecked(W2,false);
  rb_pen_width->setItemChecked(W3,false);
  rb_pen_width->setItemChecked(W4,true);
  rb_pen_width->setItemChecked(W5,false);
  rb_pen_width->setItemChecked(W6,false);
  rb_pen_width->setItemChecked(W7,false);
  rb_pen_width->setItemChecked(W8,false);
  rb_pen_width->setItemChecked(W9,false);
  rb_pen_width->setItemChecked(W10,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW1,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW2,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW3,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW4,true);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW5,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW6,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW7,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW8,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW9,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 5 =================*/
void KPresenterView_impl::presPen5()
{
  QPen p = KPresenterDoc()->presPen();
  p.setWidth(5);
  KPresenterDoc()->setPresPen(p);
  rb_pen_width->setItemChecked(W1,false);
  rb_pen_width->setItemChecked(W2,false);
  rb_pen_width->setItemChecked(W3,false);
  rb_pen_width->setItemChecked(W4,false);
  rb_pen_width->setItemChecked(W5,true);
  rb_pen_width->setItemChecked(W6,false);
  rb_pen_width->setItemChecked(W7,false);
  rb_pen_width->setItemChecked(W8,false);
  rb_pen_width->setItemChecked(W9,false);
  rb_pen_width->setItemChecked(W10,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW1,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW2,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW3,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW4,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW5,true);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW6,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW7,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW8,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW9,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 6 =================*/
void KPresenterView_impl::presPen6()
{
  QPen p = KPresenterDoc()->presPen();
  p.setWidth(6);
  KPresenterDoc()->setPresPen(p);
  rb_pen_width->setItemChecked(W1,false);
  rb_pen_width->setItemChecked(W2,false);
  rb_pen_width->setItemChecked(W3,false);
  rb_pen_width->setItemChecked(W4,false);
  rb_pen_width->setItemChecked(W5,false);
  rb_pen_width->setItemChecked(W6,true);
  rb_pen_width->setItemChecked(W7,false);
  rb_pen_width->setItemChecked(W8,false);
  rb_pen_width->setItemChecked(W9,false);
  rb_pen_width->setItemChecked(W10,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW1,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW2,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW3,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW4,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW5,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW6,true);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW7,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW8,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW9,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 7 =================*/
void KPresenterView_impl::presPen7()
{
  QPen p = KPresenterDoc()->presPen();
  p.setWidth(7);
  KPresenterDoc()->setPresPen(p);
  rb_pen_width->setItemChecked(W1,false);
  rb_pen_width->setItemChecked(W2,false);
  rb_pen_width->setItemChecked(W3,false);
  rb_pen_width->setItemChecked(W4,false);
  rb_pen_width->setItemChecked(W5,false);
  rb_pen_width->setItemChecked(W6,false);
  rb_pen_width->setItemChecked(W7,true);
  rb_pen_width->setItemChecked(W8,false);
  rb_pen_width->setItemChecked(W9,false);
  rb_pen_width->setItemChecked(W10,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW1,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW2,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW3,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW4,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW5,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW6,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW7,true);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW8,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW9,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 8 =================*/
void KPresenterView_impl::presPen8()
{
  QPen p = KPresenterDoc()->presPen();
  p.setWidth(8);
  KPresenterDoc()->setPresPen(p);
  rb_pen_width->setItemChecked(W1,false);
  rb_pen_width->setItemChecked(W2,false);
  rb_pen_width->setItemChecked(W3,false);
  rb_pen_width->setItemChecked(W4,false);
  rb_pen_width->setItemChecked(W5,false);
  rb_pen_width->setItemChecked(W6,false);
  rb_pen_width->setItemChecked(W7,false);
  rb_pen_width->setItemChecked(W8,true);
  rb_pen_width->setItemChecked(W9,false);
  rb_pen_width->setItemChecked(W10,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW1,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW2,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW3,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW4,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW5,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW6,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW7,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW8,true);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW9,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 9 =================*/
void KPresenterView_impl::presPen9()
{
  QPen p = KPresenterDoc()->presPen();
  p.setWidth(9);
  KPresenterDoc()->setPresPen(p);
  rb_pen_width->setItemChecked(W1,false);
  rb_pen_width->setItemChecked(W2,false);
  rb_pen_width->setItemChecked(W3,false);
  rb_pen_width->setItemChecked(W4,false);
  rb_pen_width->setItemChecked(W5,false);
  rb_pen_width->setItemChecked(W6,false);
  rb_pen_width->setItemChecked(W7,false);
  rb_pen_width->setItemChecked(W8,false);
  rb_pen_width->setItemChecked(W9,true);
  rb_pen_width->setItemChecked(W10,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW1,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW2,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW3,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW4,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW5,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW6,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW7,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW8,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW9,true);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 10 ================*/
void KPresenterView_impl::presPen10()
{
  QPen p = KPresenterDoc()->presPen();
  p.setWidth(10);
  KPresenterDoc()->setPresPen(p);
  rb_pen_width->setItemChecked(W1,false);
  rb_pen_width->setItemChecked(W2,false);
  rb_pen_width->setItemChecked(W3,false);
  rb_pen_width->setItemChecked(W4,false);
  rb_pen_width->setItemChecked(W5,false);
  rb_pen_width->setItemChecked(W6,false);
  rb_pen_width->setItemChecked(W7,false);
  rb_pen_width->setItemChecked(W8,false);
  rb_pen_width->setItemChecked(W9,false);
  rb_pen_width->setItemChecked(W10,true);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW1,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW2,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW3,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW4,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW5,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW6,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW7,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW8,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW9,false);
  m_rMenuBar->setItemChecked(m_idMenuScreen_PenW10,true);
}

/*======================== set pres pen color ===================*/
void KPresenterView_impl::presPenColor()
{
  QColor c = KPresenterDoc()->presPen().color();
  
  if (KColorDialog::getColor(c))
    {
      QPen p = KPresenterDoc()->presPen();
      p.setColor(c);
      KPresenterDoc()->setPresPen(p);
      QPixmap pix(16,16);
      pix.fill(c);
      rb_pen->changeItem(pix,i18n("Pen color..."),P_COL);
    }
}

/*======================== set pres pen width 1 =================*/
void KPresenterView_impl::presPen1idl()
{
  presPen1();
}

/*======================== set pres pen width 2 =================*/
void KPresenterView_impl::presPen2idl()
{
  presPen2();
}

/*======================== set pres pen width 3 =================*/
void KPresenterView_impl::presPen3idl()
{
  presPen3();
}

/*======================== set pres pen width 4 =================*/
void KPresenterView_impl::presPen4idl()
{
  presPen4();
}

/*======================== set pres pen width 5 =================*/
void KPresenterView_impl::presPen5idl()
{
  presPen5();
}

/*======================== set pres pen width 6 =================*/
void KPresenterView_impl::presPen6idl()
{
  presPen6();
}

/*======================== set pres pen width 7 =================*/
void KPresenterView_impl::presPen7idl()
{
  presPen7();
}

/*======================== set pres pen width 8 =================*/
void KPresenterView_impl::presPen8idl()
{
  presPen8();
}

/*======================== set pres pen width 9 =================*/
void KPresenterView_impl::presPen9idl()
{
  presPen9();
}

/*======================== set pres pen width 10 ================*/
void KPresenterView_impl::presPen10idl()
{
  presPen10();
}

/*======================== set pres pen color ===================*/
void KPresenterView_impl::presPenColoridl()
{
  presPenColor();
}

/*======================= insert line (-) =======================*/
void KPresenterView_impl::insertLineHidl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,lineBegin,lineEnd,LT_HORZ,xOffset,yOffset);
}

/*======================= insert line (|) =======================*/
void KPresenterView_impl::insertLineVidl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,lineBegin,lineEnd,LT_VERT,xOffset,yOffset);
}

/*======================= insert line (\) =======================*/
void KPresenterView_impl::insertLineD1idl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,lineBegin,lineEnd,LT_LU_RD,xOffset,yOffset);
}

/*======================= insert line (/) =======================*/
void KPresenterView_impl::insertLineD2idl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertLine(pen,lineBegin,lineEnd,LT_LD_RU,xOffset,yOffset);
}

/*===================== insert normal rect  =====================*/
void KPresenterView_impl::insertNormRectidl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertRectangle(pen,brush,RT_NORM,fillType,gColor1,gColor2,gType,xOffset,yOffset);
}

/*===================== insert round rect =======================*/
void KPresenterView_impl::insertRoundRectidl()
{
  page->deSelectAllObj();
  m_pKPresenterDoc->insertRectangle(pen,brush,RT_ROUND,fillType,gColor1,gColor2,gType,xOffset,yOffset);
}

/*=========================== search =============================*/
void KPresenterView_impl::search(QString text,bool sensitive,bool direction)
{
  if (page->kTxtObj())
    {
      TxtCursor from,to;
      from.setKTextObject(page->kTxtObj());
      to.setKTextObject(page->kTxtObj());
      bool found = false;
      
      if (!direction)
	{
	  if (searchFirst)
	    found = page->kTxtObj()->searchFirst(text,&from,&to,sensitive);
	  else
	    found = page->kTxtObj()->searchNext(text,&from,&to,sensitive);
      
	  if (found)
	    searchFirst = false;
	  else
	    {
	      searchFirst = false;
	      page->kTxtObj()->setSearchIndexToBegin();
	      QMessageBox::warning(this,i18n("Warning"),
				   i18n("The search string '" + text + "' couldn't be found!"));
	    }
	}
      else
	{
	  if (searchFirst)
	    found = page->kTxtObj()->searchFirstRev(text,&from,&to,sensitive);
	  else
	    found = page->kTxtObj()->searchNextRev(text,&from,&to,sensitive);
	  
	  if (found)
	    searchFirst = false;
	  else
	    {
	      searchFirst = false;
	      page->kTxtObj()->setSearchIndexToEnd();
	      QMessageBox::warning(this,i18n("Warning"),
				   i18n("The search string '" + text + "' couldn't be found!"));
	    }
	}
    }
}

/*=========================== search and replace =================*/
void KPresenterView_impl::replace(QString search,QString replace,bool sensitive,bool direction)
{
  if (page->kTxtObj())
    {
      TxtCursor from,to;
      from.setKTextObject(page->kTxtObj());
      to.setKTextObject(page->kTxtObj());
      bool found = false;
      
      if (!direction)
	{
	  if (searchFirst)
	    found = page->kTxtObj()->replaceFirst(search,replace,&from,&to,sensitive);
	  else
	    found = page->kTxtObj()->replaceNext(search,replace,&from,&to,sensitive);
	  
	  if (found)
	    searchFirst = false;
	  else
	    {
	      searchFirst = false;
	      page->kTxtObj()->setSearchIndexToBegin();
	      QMessageBox::warning(this,i18n("Warning"),
				   i18n("The search string '" + search + "' couldn't be found"
				   " and replaced with '" + replace + "'!"));
	    }
	}
      else
	{
	  if (searchFirst)
	    found = page->kTxtObj()->replaceFirstRev(search,replace,&from,&to,sensitive);
	  else
	    found = page->kTxtObj()->replaceNextRev(search,replace,&from,&to,sensitive);
	  
	  if (found)
	    searchFirst = false;
	  else
	    {
	      searchFirst = false;
	      page->kTxtObj()->setSearchIndexToEnd();
	      QMessageBox::warning(this,i18n("Warning"),
				   i18n("The search string '" + search + "' couldn't be found"
				   " and replaced with '" + replace + "'!"));
	    }
	}
    }
}

/*=========================== search and replace all =============*/
void KPresenterView_impl::replaceAll(QString search,QString replace,bool sensitive)
{
  if (page->kTxtObj())
    {
      TxtCursor from,to;
      from.setKTextObject(page->kTxtObj());
      to.setKTextObject(page->kTxtObj());
      bool found = true;
      
      page->kTxtObj()->setSearchIndexToBegin();
      
      while (found)
	found = page->kTxtObj()->replaceNext(search,replace,&from,&to,sensitive);
    }
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

/*====================== paint event ============================*/
void KPresenterView_impl::repaint(QRect r,bool erase)
{
  QWidget::repaint(r,erase);
  page->repaint(r,erase);
}

/*====================== change pciture =========================*/
void KPresenterView_impl::changePicture(unsigned int,const char* filename)
{
  QFileInfo fileInfo(filename);

  QString file = KFileDialog::getOpenFileName(0,
					      i18n("*.gif *GIF *.bmp *.BMP *.xbm *.XBM *.xpm *.XPM *.pnm *.PNM "
					      "*.PBM *.PGM *.PPM *.PBMRAW *.PGMRAW *.PPMRAW "
					      "*.pbm *.pgm *.ppm *.pbmdraw *.pgmdraw *.ppmdraw|All pictures\n"
					      "*.gif *.GIF|GIF-Pictures\n"
					      "*.jpg *.JPG *.jpeg *.JPEG|JPEG-Pictures\n"
					      "*.bmp *.BMP|Windows Bitmaps\n"
					      "*.xbm *.XBM|XWindow Pitmaps\n"
					      "*.xpm *.XPM|Pixmaps\n"
					      "*.pnm *.PNM *.PBM *.PGM *.PPM *.PBMRAW *.PGMRAW *.PPMRAW "
					      "*.pbm *.pgm *.ppm *.pbmdraw *.pgmdraw *.ppmdraw|PNM-Pictures"),0);

  if (!file.isEmpty()) m_pKPresenterDoc->changePicture(file,xOffset,yOffset);
}

/*====================== change clipart =========================*/
void KPresenterView_impl::changeClipart(unsigned int,QString filename)
{
  QFileInfo fileInfo(filename);
  QString file = KFileDialog::getOpenFileName(fileInfo.dirPath(false),i18n("*.WMF *.wmf|Windows Metafiles"),0);

  if (!file.isEmpty()) m_pKPresenterDoc->changeClipart(file,xOffset,yOffset);
}

/*====================== resize event ===========================*/
void KPresenterView_impl::resizeEvent(QResizeEvent *e)
{
  if (!presStarted) QWidget::resizeEvent(e);

  if (m_bShowGUI && !presStarted)
    { 
      horz->show();
      vert->show();
      h_ruler->show();
      v_ruler->show();
      page->resize(widget()->width() - 36,widget()->height() - 36);
      page->move(20,20);
      vert->setGeometry(widget()->width() - 16,0,16,widget()->height() - 16);
      horz->setGeometry(0,widget()->height() - 16,widget()->width() - 16,16);
      h_ruler->setGeometry(20,0,page->width(),20);
      v_ruler->setGeometry(0,20,20,page->height());
      setRanges();
    }
  else
    {
      horz->hide();
      vert->hide();
      h_ruler->hide();
      v_ruler->hide();
      page->move(0,0);
      page->resize(widget()->width(),widget()->height());
    }  
}

/*======================= key press event =======================*/
void KPresenterView_impl::keyPressEvent(QKeyEvent *e)
{
  page->keyPressEvent(e);
}

/*====================== do automatic screenpresentation ========*/
void KPresenterView_impl::doAutomaticScreenPres()
{
  page->repaint(false);

  while (continuePres && !exitPres)
    screenNext();

  if (!exitPres && KPresenterDoc()->spInfinitLoop())
    {
      screenStop();
      screenStart();
    }

  screenStop();
}

/*======================= hide parts ============================*/
void KPresenterView_impl::hideParts()
{
  QListIterator<KPresenterFrame> it(m_lstFrames);

  for(;it.current();++it)
    it.current()->hide();
}

/*====================== present parts ==========================*/
void KPresenterView_impl::presentParts(float _presFakt,QPainter* _painter,QRect _rect,int _diffx,int _diffy)
{
  QListIterator<KPresenterChild> chl = m_pKPresenterDoc->childIterator();
  QRect child_geometry;
  float scale_w,scale_h;

  for(;chl.current();++chl)
    {
      child_geometry.setLeft(static_cast<int>(static_cast<float>(chl.current()->_geometry().left()) * _presFakt));
      child_geometry.setTop(static_cast<int>(static_cast<float>(chl.current()->_geometry().top()) * _presFakt));

      child_geometry.setRight(chl.current()->_geometry().right());
      child_geometry.setBottom(chl.current()->_geometry().bottom());

      scale_w = static_cast<float>(chl.current()->_geometry().width()) * _presFakt / 
	static_cast<float>(chl.current()->_geometry().width());

      scale_h = static_cast<float>(chl.current()->_geometry().height()) * _presFakt / 
	static_cast<float>(chl.current()->_geometry().height());

      _painter->translate(static_cast<float>(child_geometry.left()) - static_cast<float>(_diffx),
			  static_cast<float>(child_geometry.top()) - static_cast<float>(_diffy));
      _painter->scale(scale_w,scale_h);

      QPicture* pic;
      pic = chl.current()->draw();

      if (pic && !pic->isNull())
	_painter->drawPicture(*pic);
  
      _painter->resetXForm();
    }
}

/*==================== show parts again =========================*/
void KPresenterView_impl::showParts()
{
  QListIterator<KPresenterFrame> it(m_lstFrames);

  for(;it.current();++it)
    it.current()->show();
}

/*========================= change undo =========================*/
void KPresenterView_impl::changeUndo(QString _text,bool _enable)
{
  if (_enable)
    {
      m_rMenuBar->setItemEnabled(m_idMenuEdit_Undo,true);
      QString str;
      str.sprintf(i18n("Undo: %s"),_text.data());
      m_rMenuBar->changeItem(str,m_idMenuEdit_Undo);
      m_rToolBarEdit->setItemEnabled(m_idButtonEdit_Undo,true);
    }
  else
    {    
      m_rMenuBar->changeItem(i18n("No Undo possible"),m_idMenuEdit_Undo);
      m_rMenuBar->setItemEnabled(m_idMenuEdit_Undo,false);
      m_rToolBarEdit->setItemEnabled(m_idButtonEdit_Undo,false);
    }
}

/*========================= change redo =========================*/
void KPresenterView_impl::changeRedo(QString _text,bool _enable)
{
  if (_enable)
    {
      m_rMenuBar->setItemEnabled(m_idMenuEdit_Redo,true);
      QString str;
      str.sprintf(i18n("Redo: %s"),_text.data());
      m_rMenuBar->changeItem(str,m_idMenuEdit_Redo);
      m_rToolBarEdit->setItemEnabled(m_idButtonEdit_Redo,true);
    }
  else
    {
      m_rMenuBar->changeItem(i18n("No Redo possible"),m_idMenuEdit_Redo);
      m_rMenuBar->setItemEnabled(m_idMenuEdit_Redo,false);
      m_rToolBarEdit->setItemEnabled(m_idButtonEdit_Redo,false);
    }
}

/*================ color of pres-pen changed ====================*/
void KPresenterView_impl::presColorChanged()
{
  QPen p = KPresenterDoc()->presPen();
  QColor c = p.color();
  QPixmap pix(16,16);
  pix.fill(c);
  rb_pen->changeItem(pix,i18n("Pen color..."),P_COL);
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
      m_idMenuEdit = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&Edit")));
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/undo.xpm";
      QString pix = loadPixmap(tmp);
      m_idMenuEdit_Undo = m_rMenuBar->insertItemP(CORBA::string_dup(pix),CORBA::string_dup(i18n("No Undo possible")),m_idMenuEdit,
						  this,CORBA::string_dup("editUndo"));
      m_rMenuBar->setItemEnabled(m_idMenuEdit_Undo,false);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/redo.xpm";
      pix = loadPixmap(tmp);
      m_idMenuEdit_Redo = m_rMenuBar->insertItemP(CORBA::string_dup(pix),CORBA::string_dup(i18n("No Redo possible")),m_idMenuEdit,
						  this,CORBA::string_dup("editRedo"));
      m_rMenuBar->setItemEnabled(m_idMenuEdit_Redo,false);
      m_rMenuBar->insertSeparator(m_idMenuEdit);
      tmp = kapp->kde_toolbardir().copy();
      tmp += "/editcut.xpm";
      pix = loadPixmap(tmp);
      m_idMenuEdit_Cut = m_rMenuBar->insertItemP(CORBA::string_dup(pix),CORBA::string_dup(i18n("&Cut")),m_idMenuEdit,
						this,CORBA::string_dup("editCut"));
      tmp = kapp->kde_toolbardir().copy();
      tmp += "/editcopy.xpm";
      pix = loadPixmap(tmp);
      m_idMenuEdit_Copy = m_rMenuBar->insertItemP(CORBA::string_dup(pix),CORBA::string_dup(i18n("&Copy")),m_idMenuEdit,
						  this,CORBA::string_dup("editCopy"));
      tmp = kapp->kde_toolbardir().copy();
      tmp += "/editpaste.xpm";
      pix = loadPixmap(tmp);
      m_idMenuEdit_Paste = m_rMenuBar->insertItemP(CORBA::string_dup(pix),CORBA::string_dup(i18n("&Paste")),m_idMenuEdit,
						   this,CORBA::string_dup("editPaste"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/delete.xpm";
      pix = loadPixmap(tmp);
      m_idMenuEdit_Delete = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(i18n("&Delete")),m_idMenuEdit,
						    this,CORBA::string_dup("editDelete"));
      m_rMenuBar->insertSeparator(m_idMenuEdit);
      m_idMenuEdit_Find = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Find...")),m_idMenuEdit,
						 this,CORBA::string_dup("editFind"));
      m_idMenuEdit_FindReplace = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Replace...")),m_idMenuEdit,
							this,CORBA::string_dup("editFindReplace"));

      // view menu
      m_idMenuView = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&View")));
      m_idMenuView_NewView = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&New View")),m_idMenuView,
						    this,CORBA::string_dup("newView"));

      // insert menu
      m_idMenuInsert = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&Insert")));

      tmp = kapp->kde_toolbardir().copy();
      tmp += "/filenew.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Page = m_rMenuBar->insertItemP(CORBA::string_dup(pix),CORBA::string_dup(i18n("Pa&ge...")),m_idMenuInsert,
						    this,CORBA::string_dup("insertPage"));
      m_rMenuBar->insertSeparator(m_idMenuInsert);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/picture.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Picture = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						       CORBA::string_dup(i18n("&Picture...")),m_idMenuInsert,
						       this,CORBA::string_dup("insertPicture"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/clipart.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Clipart = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						       CORBA::string_dup(i18n("&Clipart...")),m_idMenuInsert,
						       this,CORBA::string_dup("insertClipart"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/line.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Line = m_rMenuBar->insertSubMenuP(CORBA::string_dup(pix),CORBA::string_dup(i18n("&Line")),
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
							    CORBA::string_dup(i18n("&Rectangle")),m_idMenuInsert);

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
						      CORBA::string_dup(i18n("C&ircle or Ellipse")),m_idMenuInsert,
						      this,CORBA::string_dup("insertCircleOrEllipse"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/text.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Text = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(i18n("&Text")),m_idMenuInsert,
						    this,CORBA::string_dup("insertText"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/autoform.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Autoform = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							CORBA::string_dup(i18n("&Autoform...")),m_idMenuInsert,
							this,CORBA::string_dup("insertAutoform"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/parts.xpm";
      pix = loadPixmap(tmp);
      m_idMenuInsert_Part = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(i18n("&Object...")),m_idMenuInsert,
						    this,CORBA::string_dup("insertObject"));

      // extra menu
      m_idMenuExtra = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&Extra")));
      m_idMenuExtra_TFont = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Font...")),m_idMenuExtra,
						   this,CORBA::string_dup("mtextFont"));
      m_idMenuExtra_TColor = m_rMenuBar->insertItem(CORBA::string_dup(i18n("Text &Color...")),m_idMenuExtra,
						    this,CORBA::string_dup("textColor"));
      m_idMenuExtra_TAlign = m_rMenuBar->insertSubMenu(CORBA::string_dup(i18n("Text &Alignment")),m_idMenuExtra);

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignLeft.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TAlign_Left = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							  CORBA::string_dup(i18n("Align &Left")),m_idMenuExtra_TAlign,
							  this,CORBA::string_dup("mtextAlignLeft"));
      m_rMenuBar->setCheckable(m_idMenuExtra_TAlign_Left,true);
      
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignCenter.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TAlign_Center = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							    CORBA::string_dup(i18n("Align &Center")),m_idMenuExtra_TAlign,
							    this,CORBA::string_dup("mtextAlignCenter"));
      m_rMenuBar->setCheckable(m_idMenuExtra_TAlign_Center,true);

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignRight.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TAlign_Right = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							   CORBA::string_dup(i18n("Align &Right")),m_idMenuExtra_TAlign,
							   this,CORBA::string_dup("mtextAlignRight"));
      m_rMenuBar->setCheckable(m_idMenuExtra_TAlign_Right,true);

      m_idMenuExtra_TType = m_rMenuBar->insertSubMenu(CORBA::string_dup(i18n("Text &Type")),m_idMenuExtra);

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/enumList.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TType_EnumList = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("&Enumerated List")),
							     m_idMenuExtra_TType,
							     this,CORBA::string_dup("textEnumList"));

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/unsortedList.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TType_UnsortList = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							       CORBA::string_dup(i18n("&Unsorted List")),
							       m_idMenuExtra_TType,
							       this,CORBA::string_dup("textUnsortList"));

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/normalText.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_TType_NormalText = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							       CORBA::string_dup(i18n("&Normal Text")),
							       m_idMenuExtra_TType,
							       this,CORBA::string_dup("textNormalText"));

      m_rMenuBar->insertSeparator(m_idMenuExtra);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/style.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_PenBrush = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						       CORBA::string_dup(i18n("&Pen and Brush...")),m_idMenuExtra,
						       this,CORBA::string_dup("extraPenBrush"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/raise.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_Raise = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(i18n("&Raise object(s)")),m_idMenuExtra,
						    this,CORBA::string_dup("extraRaise"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/lower.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_Lower = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(i18n("&Lower object(s)")),m_idMenuExtra,
						    this,CORBA::string_dup("extraLower"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/rotate.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_Rotate = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						     CORBA::string_dup(i18n("Rot&ate object(s)...")),m_idMenuExtra,
						     this,CORBA::string_dup("extraRotate"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/shadow.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_Shadow = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						     CORBA::string_dup(i18n("&Shadow object(s)...")),m_idMenuExtra,
						     this,CORBA::string_dup("extraShadow"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignobjs.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_AlignObj = m_rMenuBar->insertSubMenuP(CORBA::string_dup(pix),CORBA::string_dup(i18n("Align object(s)")),
							  m_idMenuExtra);

      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/aoleft.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_AlignObj_Left = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							    CORBA::string_dup(i18n("Align &Left")),m_idMenuExtra_AlignObj,
							    this,CORBA::string_dup("extraAlignObjLeftidl"));
      m_rMenuBar->insertSeparator(m_idMenuExtra_AlignObj);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/aocenterh.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_AlignObj_CenterH = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							       CORBA::string_dup(i18n("Align Center (&horizontal)")),
							       m_idMenuExtra_AlignObj,
							       this,CORBA::string_dup("extraAlignObjCenterHidl"));
      m_rMenuBar->insertSeparator(m_idMenuExtra_AlignObj);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/aoright.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_AlignObj_Right = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Align &Right")),m_idMenuExtra_AlignObj,
							     this,CORBA::string_dup("extraAlignObjRightidl"));
      m_rMenuBar->insertSeparator(m_idMenuExtra_AlignObj);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/aotop.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_AlignObj_Top = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							   CORBA::string_dup(i18n("Align &Top")),m_idMenuExtra_AlignObj,
							   this,CORBA::string_dup("extraAlignObjTopidl"));
      m_rMenuBar->insertSeparator(m_idMenuExtra_AlignObj);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/aocenterv.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_AlignObj_CenterV = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							       CORBA::string_dup(i18n("Align Center (&vertical)")),m_idMenuExtra_AlignObj,
							       this,CORBA::string_dup("extraAlignObjCenterVidl"));
      m_rMenuBar->insertSeparator(m_idMenuExtra_AlignObj);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/aobottom.xpm";
      pix = loadPixmap(tmp);
      m_idMenuExtra_AlignObj_Bottom = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							      CORBA::string_dup(i18n("Align &Bottom")),m_idMenuExtra_AlignObj,
							      this,CORBA::string_dup("extraAlignObjBottomidl"));
      m_rMenuBar->insertSeparator(m_idMenuExtra);

      m_idMenuExtra_Background = m_rMenuBar->insertItem(CORBA::string_dup(i18n("Page &Background...")),m_idMenuExtra,
							this,CORBA::string_dup("extraBackground"));
      m_idMenuExtra_Layout = m_rMenuBar->insertItem(CORBA::string_dup(i18n("Pa&ge Layout...")),m_idMenuExtra,
						    this,CORBA::string_dup("extraLayout"));
      m_rMenuBar->insertSeparator(m_idMenuExtra);
      m_idMenuExtra_Options = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Options...")),m_idMenuExtra,
						     this,CORBA::string_dup("extraOptions"));
      
      // screenpresentation menu
      m_idMenuScreen = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&Screen Presentations")));
      m_idMenuScreen_ConfigPage = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Configure pages...")),m_idMenuScreen,
							 this,CORBA::string_dup("screenConfigPages"));
      m_idMenuScreen_PresStructView = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Open presentation structure viewer...")),
							     m_idMenuScreen,this,CORBA::string_dup("screenPresStructView"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/effect.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_AssignEffect = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
							    CORBA::string_dup(i18n("&Assign effect...")),m_idMenuScreen,
							    this,CORBA::string_dup("screenAssignEffect"));
      m_rMenuBar->insertSeparator(m_idMenuScreen);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/start.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Start = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						     CORBA::string_dup(i18n("&Start")),m_idMenuScreen,
						     this,CORBA::string_dup("screenStart"));
//       tmp = kapp->kde_datadir().copy();
//       tmp += "/kpresenter/toolbar/stop.xpm";
//       pix = loadPixmap(tmp);
//       m_idMenuScreen_Stop = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
// 						    CORBA::string_dup(i18n("St&op")),m_idMenuScreen,
// 						    this,CORBA::string_dup("screenStop"));
//       tmp = kapp->kde_datadir().copy();
//       tmp += "/kpresenter/toolbar/pause.xpm";
//       pix = loadPixmap(tmp);
//       m_idMenuScreen_Pause = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
// 						     CORBA::string_dup(i18n("Pa&use")),m_idMenuScreen,
// 						     this,CORBA::string_dup("screenPause"));
      m_rMenuBar->insertSeparator(m_idMenuScreen);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/first.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_First = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						     CORBA::string_dup(i18n("&Go to start")),m_idMenuScreen,
						     this,CORBA::string_dup("screenFirst"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/prev.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Prev = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(i18n("&Previous step")),m_idMenuScreen,
						    this,CORBA::string_dup("screenPrev"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/next.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Next = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(i18n("&Next step")),m_idMenuScreen,
						    this,CORBA::string_dup("screenNext"));
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/last.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Last = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						    CORBA::string_dup(i18n("&Go to end")),m_idMenuScreen,
						    this,CORBA::string_dup("screenLast"));
//       m_idMenuScreen_Skip = m_rMenuBar->insertItem(CORBA::string_dup(i18n("Goto &page")),m_idMenuScreen,
// 						   this,CORBA::string_dup("screenSkip"));
      m_rMenuBar->insertSeparator(m_idMenuScreen);
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/pen.xpm";
      pix = loadPixmap(tmp);
      m_idMenuScreen_Pen = m_rMenuBar->insertSubMenuP(CORBA::string_dup(pix),
						      CORBA::string_dup(i18n("&Choose Pen")),m_idMenuScreen);
      m_idMenuScreen_PenWidth = m_rMenuBar->insertSubMenu(CORBA::string_dup(i18n("Pen width")),m_idMenuScreen_Pen);
      m_idMenuScreen_PenColor = m_rMenuBar->insertItem(CORBA::string_dup(i18n("Pen color")),m_idMenuScreen_Pen,
						       this,CORBA::string_dup("presPenColoridl"));
      m_idMenuScreen_PenW1 = m_rMenuBar->insertItem(CORBA::string_dup(i18n("1")),m_idMenuScreen_PenWidth,
						    this,CORBA::string_dup("presPen1idl"));
      m_idMenuScreen_PenW2 = m_rMenuBar->insertItem(CORBA::string_dup(i18n("2")),m_idMenuScreen_PenWidth,
						    this,CORBA::string_dup("presPen2idl"));
      m_idMenuScreen_PenW3 = m_rMenuBar->insertItem(CORBA::string_dup(i18n("3")),m_idMenuScreen_PenWidth,
						    this,CORBA::string_dup("presPen3idl"));
      m_idMenuScreen_PenW4 = m_rMenuBar->insertItem(CORBA::string_dup(i18n("4")),m_idMenuScreen_PenWidth,
						    this,CORBA::string_dup("presPen4idl"));
      m_idMenuScreen_PenW5 = m_rMenuBar->insertItem(CORBA::string_dup(i18n("5")),m_idMenuScreen_PenWidth,
						    this,CORBA::string_dup("presPen5idl"));
      m_idMenuScreen_PenW6 = m_rMenuBar->insertItem(CORBA::string_dup(i18n("6")),m_idMenuScreen_PenWidth,
						    this,CORBA::string_dup("presPen6idl"));
      m_idMenuScreen_PenW7 = m_rMenuBar->insertItem(CORBA::string_dup(i18n("7")),m_idMenuScreen_PenWidth,
						    this,CORBA::string_dup("presPen7idl"));
      m_idMenuScreen_PenW8 = m_rMenuBar->insertItem(CORBA::string_dup(i18n("8")),m_idMenuScreen_PenWidth,
						    this,CORBA::string_dup("presPen8idl"));
      m_idMenuScreen_PenW9 = m_rMenuBar->insertItem(CORBA::string_dup(i18n("9")),m_idMenuScreen_PenWidth,
						    this,CORBA::string_dup("presPen9idl"));
      m_idMenuScreen_PenW10 = m_rMenuBar->insertItem(CORBA::string_dup(i18n("10")),m_idMenuScreen_PenWidth,
						     this,CORBA::string_dup("presPen10idl"));
      m_rMenuBar->setCheckable(m_idMenuScreen_PenW1,true);
      m_rMenuBar->setCheckable(m_idMenuScreen_PenW2,true);
      m_rMenuBar->setCheckable(m_idMenuScreen_PenW3,true);
      m_rMenuBar->setCheckable(m_idMenuScreen_PenW4,true);
      m_rMenuBar->setCheckable(m_idMenuScreen_PenW5,true);
      m_rMenuBar->setCheckable(m_idMenuScreen_PenW6,true);
      m_rMenuBar->setCheckable(m_idMenuScreen_PenW7,true);
      m_rMenuBar->setCheckable(m_idMenuScreen_PenW8,true);
      m_rMenuBar->setCheckable(m_idMenuScreen_PenW9,true);
      m_rMenuBar->setCheckable(m_idMenuScreen_PenW10,true);

      // help menu
      m_idMenuHelp = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&Help")));
      m_idMenuHelp_Contents = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Contents")),m_idMenuHelp,
						     this,CORBA::string_dup("helpContents"));
      m_rMenuBar->insertSeparator(m_idMenuHelp);
      m_idMenuHelp_About = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&About KPresenter...")),m_idMenuHelp,
						  this,CORBA::string_dup("helpAbout"));
      m_idMenuHelp_AboutKOffice = m_rMenuBar->insertItem(CORBA::string_dup(i18n("About K&Office...")),m_idMenuHelp,
							 this,CORBA::string_dup("helpAboutKOffice"));
      m_idMenuHelp_AboutKDE = m_rMenuBar->insertItem(CORBA::string_dup(i18n("About &KDE...")),m_idMenuHelp,
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

  // create right button pen menu
  rb_pen_width = new QPopupMenu();
  CHECK_PTR(rb_pen_width);
  W1 = rb_pen_width->insertItem("1",this,SLOT(presPen1()));
  W2 = rb_pen_width->insertItem("2",this,SLOT(presPen2()));
  W3 = rb_pen_width->insertItem("3",this,SLOT(presPen3()));
  W4 = rb_pen_width->insertItem("4",this,SLOT(presPen4()));
  W5 = rb_pen_width->insertItem("5",this,SLOT(presPen5()));
  W6 = rb_pen_width->insertItem("6",this,SLOT(presPen6()));
  W7 = rb_pen_width->insertItem("7",this,SLOT(presPen7()));
  W8 = rb_pen_width->insertItem("8",this,SLOT(presPen8()));
  W9 = rb_pen_width->insertItem("9",this,SLOT(presPen9()));
  W10 = rb_pen_width->insertItem("10",this,SLOT(presPen10()));
  rb_pen_width->setCheckable(true);
  rb_pen_width->setMouseTracking(true);
  rb_pen_width->setItemChecked(W3,true);

  rb_pen = new QPopupMenu();
  CHECK_PTR(rb_pen);
  rb_pen->insertItem(i18n("Pen width"),rb_pen_width);
  QPixmap pix(16,16);
  pix.fill(red);
  P_COL = rb_pen->insertItem(pix,i18n("Pen color..."),this,SLOT(presPenColor()));
  rb_pen->setMouseTracking(true);
  rb_pen->setCheckable(false);

  // create right button object align menu
  rb_oalign = new QPopupMenu();
  CHECK_PTR(rb_oalign);
  pixmap.load(pixdir + "aoleft.xpm");
  rb_oalign->insertItem(pixmap,this,SLOT(extraAlignObjLeft()));
  rb_oalign->insertSeparator();
  pixmap.load(pixdir + "aocenterh.xpm");
  rb_oalign->insertItem(pixmap,this,SLOT(extraAlignObjCenterH()));
  rb_oalign->insertSeparator();
  pixmap.load(pixdir + "aoright.xpm");
  rb_oalign->insertItem(pixmap,this,SLOT(extraAlignObjRight()));
  rb_oalign->insertSeparator();
  pixmap.load(pixdir + "aotop.xpm");
  rb_oalign->insertItem(pixmap,this,SLOT(extraAlignObjTop()));
  rb_oalign->insertSeparator();
  pixmap.load(pixdir + "aocenterv.xpm");
  rb_oalign->insertItem(pixmap,this,SLOT(extraAlignObjCenterV()));
  rb_oalign->insertSeparator();
  pixmap.load(pixdir + "aobottom.xpm");
  rb_oalign->insertItem(pixmap,this,SLOT(extraAlignObjBottom()));
  rb_oalign->setMouseTracking(true);
  rb_oalign->setCheckable(false);
}

/*======================= setup edit toolbar ===================*/
void KPresenterView_impl::setupEditToolbar()
{
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if (!CORBA::is_nil(m_vToolBarFactory))
    {
      // toolbar
      m_rToolBarEdit = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(i18n("Edit")));
      m_rToolBarEdit->setFullWidth(false);

      // undo
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/undo.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonEdit_Undo = m_rToolBarEdit->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Undo")),
							 this,CORBA::string_dup("editUndo"));
      m_rToolBarEdit->setItemEnabled(m_idButtonEdit_Undo,false);

      // redo
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/redo.xpm";
      pix = loadPixmap(tmp);
      m_idButtonEdit_Redo = m_rToolBarEdit->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Redo")),
							 this,CORBA::string_dup("editRedo"));
      m_rToolBarEdit->setItemEnabled(m_idButtonEdit_Redo,false);

      m_rToolBarEdit->insertSeparator();

      // cut
      tmp = kapp->kde_toolbardir().copy();
      tmp += "/editcut.xpm";
      pix = loadPixmap(tmp);
      m_idButtonEdit_Cut = m_rToolBarEdit->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Cut")),
							this,CORBA::string_dup("editCut"));

      // copy
      tmp = kapp->kde_toolbardir().copy();
      tmp += "/editcopy.xpm";
      pix = loadPixmap(tmp);
      m_idButtonEdit_Copy = m_rToolBarEdit->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Copy")),
							 this,CORBA::string_dup("editCopy"));

      // paste
      tmp = kapp->kde_toolbardir().copy();
      tmp += "/editpaste.xpm";
      pix = loadPixmap(tmp);
      m_idButtonEdit_Paste = m_rToolBarEdit->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Paste")),
							  this,CORBA::string_dup("editPaste"));
      m_rToolBarEdit->insertSeparator();

      // delete
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/delete.xpm";
      pix = loadPixmap(tmp);
      m_idButtonEdit_Delete = m_rToolBarEdit->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Delete")),
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
      m_rToolBarInsert = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(i18n("Insert")));
      m_rToolBarInsert->setFullWidth(false);
 
      // picture
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/picture.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonInsert_Picture = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
								CORBA::string_dup(i18n("Insert Picture")),
								this,CORBA::string_dup("insertPicture"));
      
      // clipart
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/clipart.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Clipart = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
								CORBA::string_dup(i18n("Insert Clipart")),
								this,CORBA::string_dup("insertClipart"));

      // line
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/line.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Line = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Insert Line")),
							     this,CORBA::string_dup("insertLine"));

      // rectangle
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/rectangle.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Rectangle = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
								  CORBA::string_dup(i18n("Insert Rectangle")),
								  this,CORBA::string_dup("insertRectangle"));

      // circle or ellipse
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/circle.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Circle = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
							       CORBA::string_dup(i18n("Insert Circle or Ellipse")),
							       this,CORBA::string_dup("insertCircleOrEllipse"));

      // text
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/text.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Text = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Insert Text")),
							     this,CORBA::string_dup("insertText"));

      // autoform
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/autoform.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Autoform = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
								 CORBA::string_dup(i18n("Insert Autoform")),
								 this,CORBA::string_dup("insertAutoform"));

      // parts
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/parts.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Part = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Insert Object")),
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
      m_rToolBarText = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(i18n("Text")));
      m_rToolBarText->setFullWidth(false);

      // size combobox
      m_idComboText_FontSize = m_rToolBarText->insertCombo(true,CORBA::string_dup(i18n("Font Size")),60,
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
      m_idComboText_FontList = m_rToolBarText->insertCombo(true,CORBA::string_dup(i18n("Font List")),200,
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
      m_idButtonText_Bold = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Bold")),
							 this,CORBA::string_dup("textBold"));
      m_rToolBarText->setToggle(m_idButtonText_Bold,true);
      m_rToolBarText->setButton(m_idButtonText_Bold,false);
      tbFont.setBold(false);

      // italic
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/italic.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_Italic = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Italic")),
							   this,CORBA::string_dup("textItalic"));
      m_rToolBarText->setToggle(m_idButtonText_Italic,true);
      m_rToolBarText->setButton(m_idButtonText_Italic,false);
      tbFont.setItalic(false);

      // underline
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/underl.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_Underline = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Underline")),
							      this,CORBA::string_dup("textUnderline"));
      m_rToolBarText->setToggle(m_idButtonText_Underline,true);
      m_rToolBarText->setButton(m_idButtonText_Underline,false);
      tbFont.setUnderline(false);

      // color
      m_idButtonText_Color = m_rToolBarText->insertButton(CORBA::string_dup(""),CORBA::string_dup(i18n("Color")),
							  this,CORBA::string_dup("textColor"));
      m_rToolBarText->setButtonPixmap(m_idButtonText_Color,CORBA::string_dup(colorToPixString(black)));
      tbColor = black;
      m_rToolBarText->insertSeparator();

      // align left
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignLeft.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_ALeft = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Align Left")),
							  this,CORBA::string_dup("textAlignLeft"));
      m_rToolBarText->setToggle(m_idButtonText_ALeft,true);
      m_rToolBarText->setButton(m_idButtonText_ALeft,true);
      tbAlign = TxtParagraph::LEFT;

      // align center
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignCenter.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_ACenter = m_rToolBarText->insertButton(CORBA::string_dup(pix),
							    CORBA::string_dup(i18n("Align Center")),
							    this,CORBA::string_dup("textAlignCenter"));
      m_rToolBarText->setToggle(m_idButtonText_ACenter,true);
      m_rToolBarText->setButton(m_idButtonText_ACenter,false);

      // align right
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignRight.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_ARight = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Align Right")),
							   this,CORBA::string_dup("textAlignRight"));
      m_rToolBarText->setToggle(m_idButtonText_ARight,true);
      m_rToolBarText->setButton(m_idButtonText_ARight,false);
      m_rToolBarText->insertSeparator();

      // enum list
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/enumList.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_EnumList = m_rToolBarText->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Enumerated List")),
							     this,CORBA::string_dup("textEnumList"));

      // unsorted list
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/unsortedList.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_EnumList = m_rToolBarText->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Unsorted List")),
							     this,CORBA::string_dup("textUnsortList"));

      // normal text
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/normalText.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_EnumList = m_rToolBarText->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Normal Text")),
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
      m_rToolBarExtra = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(i18n("Extra")));
      m_rToolBarExtra->setFullWidth(false);

      // pen and brush
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/style.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonExtra_Style = m_rToolBarExtra->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Pen & Brush")),
							   this,CORBA::string_dup("extraPenBrush"));
      m_rToolBarExtra->insertSeparator();

      // raise
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/raise.xpm";
      pix = loadPixmap(tmp);
      m_idButtonExtra_Raise = m_rToolBarExtra->insertButton(CORBA::string_dup(pix),
							    CORBA::string_dup(i18n("Raise object(s)")),
							    this,CORBA::string_dup("extraRaise"));

      // lower
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/lower.xpm";
      pix = loadPixmap(tmp);
      m_idButtonExtra_Lower = m_rToolBarExtra->insertButton(CORBA::string_dup(pix),
							    CORBA::string_dup(i18n("Lower object(s)")),
							    this,CORBA::string_dup("extraLower"));
      m_rToolBarExtra->insertSeparator();

      // rotate
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/rotate.xpm";
      pix = loadPixmap(tmp);
      m_idButtonExtra_Rotate = m_rToolBarExtra->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Rotate object(s)")),
							     this,CORBA::string_dup("extraRotate"));
      // shadow
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/shadow.xpm";
      pix = loadPixmap(tmp);
      m_idButtonExtra_Shadow = m_rToolBarExtra->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Shadow object(s)")),
							     this,CORBA::string_dup("extraShadow"));
      m_rToolBarExtra->insertSeparator();

      // align
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/alignobjs.xpm";
      pix = loadPixmap(tmp);
      m_idButtonExtra_Align = m_rToolBarExtra->insertButton(CORBA::string_dup(pix),
							    CORBA::string_dup(i18n("Align object(s)")),
							    this,CORBA::string_dup("extraAlignObj"));
    }
}

/*==================== setup screen toolbar ====================*/
void KPresenterView_impl::setupScreenToolbar()
{
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if (!CORBA::is_nil(m_vToolBarFactory))
    {
      // toolbar
      m_rToolBarScreen = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(i18n("Screen Presentations"))); 
      m_rToolBarScreen->setFullWidth(false);

      // stop
//       QString tmp = kapp->kde_datadir().copy();
//       tmp += "/kpresenter/toolbar/stop.xpm";
//       QString pix = loadPixmap(tmp);
//       m_idButtonScreen_Stop = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Stop")),
// 							     this,CORBA::string_dup("screenStop"));

      // pause
//       tmp = kapp->kde_datadir().copy();
//       tmp += "/kpresenter/toolbar/pause.xpm";
//       pix = loadPixmap(tmp);
//       m_idButtonScreen_Pause = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Pause")),
// 							      this,CORBA::string_dup("screenPause"));

      // start
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/start.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonScreen_Start = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Start")),
							      this,CORBA::string_dup("screenStart"));
      m_rToolBarScreen->insertSeparator();

      // first
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/first.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_First = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("First")),
							      this,CORBA::string_dup("screenFirst"));

      // previous
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/prev.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Prev = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Previous")),
							     this,CORBA::string_dup("screenPrev"));

      // next
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/next.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Next = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Next")),
							     this,CORBA::string_dup("screenNext"));

      // last
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/last.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Last = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Last")),
							     this,CORBA::string_dup("screenLast"));
      m_rToolBarScreen->insertSeparator();

      // effect
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/effect.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Effect = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),
							       CORBA::string_dup(i18n("Assign Effect")),
							       this,CORBA::string_dup("screenAssignEffect"));
      m_rToolBarScreen->insertSeparator();

      // full screen
//       tmp = kapp->kde_datadir().copy();
//       tmp += "/kpresenter/toolbar/screen.xpm";
//       pix = loadPixmap(tmp);
//       m_idButtonScreen_Full = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),
// 							     CORBA::string_dup(i18n("Full Screen")),
// 							     this,CORBA::string_dup("screenFullScreen"));
//       m_rToolBarScreen->setToggle(m_idButtonScreen_Full,true);
//       m_rToolBarScreen->setButton(m_idButtonScreen_Full,true);

      // pen
      tmp = kapp->kde_datadir().copy();
      tmp += "/kpresenter/toolbar/pen.xpm";
      pix = loadPixmap(tmp);
      m_idButtonScreen_Pen = m_rToolBarScreen->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("choose Pen")),
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

/*======================= setup accellerators ==================*/
void KPresenterView_impl::setupAccelerators()
{
  // edit menu
  m_rMenuBar->setAccel(CTRL + Key_Z,m_idMenuEdit_Undo);
  m_rMenuBar->setAccel(CTRL + Key_X,m_idMenuEdit_Cut);
  m_rMenuBar->setAccel(CTRL + Key_C,m_idMenuEdit_Copy);
  m_rMenuBar->setAccel(CTRL + Key_V,m_idMenuEdit_Paste);
  m_rMenuBar->setAccel(CTRL + Key_F,m_idMenuEdit_Find);
  m_rMenuBar->setAccel(CTRL + Key_R,m_idMenuEdit_FindReplace);

  // insert menu
  m_rMenuBar->setAccel(ALT + Key_N,m_idMenuInsert_Page);
  m_rMenuBar->setAccel(Key_F1,m_idMenuInsert_Picture);
  m_rMenuBar->setAccel(Key_F2,m_idMenuInsert_Clipart);
  m_rMenuBar->setAccel(Key_F3,m_idMenuInsert_LineHorz);
  m_rMenuBar->setAccel(Key_F4,m_idMenuInsert_LineVert);
  m_rMenuBar->setAccel(Key_F5,m_idMenuInsert_LineD1);
  m_rMenuBar->setAccel(Key_F6,m_idMenuInsert_LineD2);
  m_rMenuBar->setAccel(Key_F7,m_idMenuInsert_RectangleNormal);
  m_rMenuBar->setAccel(Key_F8,m_idMenuInsert_RectangleRound);
  m_rMenuBar->setAccel(Key_F9,m_idMenuInsert_Circle);
  m_rMenuBar->setAccel(Key_F10,m_idMenuInsert_Text);
  m_rMenuBar->setAccel(Key_F11,m_idMenuInsert_Autoform);
  m_rMenuBar->setAccel(Key_F12,m_idMenuInsert_Part);

  // extra menu
  m_rMenuBar->setAccel(CTRL + Key_P,m_idMenuExtra_PenBrush);
  m_rMenuBar->setAccel(CTRL + Key_Minus,m_idMenuExtra_Lower);
  m_rMenuBar->setAccel(CTRL + Key_Plus,m_idMenuExtra_Raise);
  m_rMenuBar->setAccel(ALT + Key_R,m_idMenuExtra_Rotate);
  m_rMenuBar->setAccel(ALT + Key_S,m_idMenuExtra_Shadow);
 
  // screen menu
  m_rMenuBar->setAccel(ALT + Key_A,m_idMenuScreen_AssignEffect);
  m_rMenuBar->setAccel(CTRL + Key_G,m_idMenuScreen_Start);
  m_rMenuBar->setAccel(Key_Home,m_idMenuScreen_First);
  m_rMenuBar->setAccel(Key_End,m_idMenuScreen_Last);
  m_rMenuBar->setAccel(Key_Prior,m_idMenuScreen_Prev);
  m_rMenuBar->setAccel(Key_Next,m_idMenuScreen_Next);

  // help menu
  m_rMenuBar->setAccel(CTRL + Key_H,m_idMenuHelp_Contents);
}

/*==============================================================*/
void KPresenterView_impl::setupRulers()
{
  h_ruler = new KoRuler(this,page,KoRuler::HORIZONTAL,KPresenterDoc()->pageLayout(),0);
  v_ruler = new KoRuler(this,page,KoRuler::VERTICAL,KPresenterDoc()->pageLayout(),0);
  page->resize(page->width() - 20,page->height() - 20);
  page->move(20,20);
  h_ruler->setGeometry(20,0,page->width(),20);
  v_ruler->setGeometry(0,20,20,page->height());

  QObject::connect(h_ruler,SIGNAL(newPageLayout(KoPageLayout)),this,SLOT(newPageLayout(KoPageLayout)));
  QObject::connect(h_ruler,SIGNAL(openPageLayoutDia()),this,SLOT(openPageLayoutDia()));
  QObject::connect(v_ruler,SIGNAL(newPageLayout(KoPageLayout)),this,SLOT(newPageLayout(KoPageLayout)));
  QObject::connect(v_ruler,SIGNAL(openPageLayoutDia()),this,SLOT(openPageLayoutDia()));
}

/*===================== set ranges of scrollbars ===============*/
void KPresenterView_impl::setRanges()
{
  if (vert && horz && page && m_pKPresenterDoc)
    {
      int range;
      
      vert->setSteps(10,m_pKPresenterDoc->getPageSize(0,xOffset,yOffset,1.0,false).height() + 20);
      range = (m_pKPresenterDoc->getPageSize(0,xOffset,yOffset,1.0,false).height()) * 
	m_pKPresenterDoc->getPageNums() - page->height() + 16 
	< 0 ? 0 :
	(m_pKPresenterDoc->getPageSize(0,xOffset,yOffset,1.0,false).height()) * 
	m_pKPresenterDoc->getPageNums() - page->height() + 16;
      vert->setRange(0,range);
      horz->setSteps(10,m_pKPresenterDoc->getPageSize(0,xOffset,yOffset,1.0,false).width() + 16 - page->width());
      range = m_pKPresenterDoc->getPageSize(0,xOffset,yOffset,1.0,false).width() + 16 - page->width() < 0 ? 0 :
	m_pKPresenterDoc->getPageSize(0,xOffset,yOffset,1.0,false).width() + 16 - page->width();
      horz->setRange(0,range);
    }
}

/*==============================================================*/
void KPresenterView_impl::skipToPage(int _num)
{
  vert->setValue(KPresenterDoc()->getPageSize(_num,0,0,1.0,false).y()); 
}

/*==============================================================*/
void KPresenterView_impl::makeRectVisible(QRect _rect)
{
  horz->setValue(_rect.x()); 
  vert->setValue(_rect.y()); 
}

/*==============================================================*/
void KPresenterView_impl::restartPresStructView()
{
  QObject::disconnect(presStructView,SIGNAL(presStructViewClosed()),this,SLOT(psvClosed()));
  presStructView->close();
  delete presStructView;
  presStructView = 0;
  page->deSelectAllObj();
      
  presStructView = new PresStructViewer(0,"",KPresenterDoc(),this);
  presStructView->setCaption(i18n("KPresenter - Presentation structure viewer"));
  QObject::connect(presStructView,SIGNAL(presStructViewClosed()),this,SLOT(psvClosed()));
  presStructView->show();
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

  sprintf(line,"%c c #%02X%02X%02X %c,\n",34,r,g,b,34);
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
  
  kde_display = kapp->getDisplay();

  bool have_installed = kapp->getKDEFonts(&fontList);
  
  if (have_installed)
    return;

  fontNames = XListFonts(kde_display,"*",32767,&numFonts);
  fontNames_copy = fontNames;
  
  for(int i = 0; i < numFonts; i++){
    
    if (**fontNames != '-')
      { 
	fontNames ++;
      continue;
      };
    
    qfontname = "";
    qfontname = *fontNames;
    int dash = qfontname.find ('-', 1, TRUE);

    if (dash == -1) 
      {
	fontNames ++;
	continue;
      }

    int dash_two = qfontname.find ('-', dash + 1 , TRUE); 

    if (dash == -1)
      {
	fontNames ++;
	continue;
      }


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
