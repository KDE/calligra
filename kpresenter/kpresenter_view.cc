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
#include "opUIUtils.h"

#define DEBUG

// Qt bug
char *getenv(const char *name);    

/*****************************************************************/
/* class KPresenterFrame                                         */
/*****************************************************************/

/*======================= constructor ===========================*/
KPresenterFrame::KPresenterFrame( KPresenterView* _view, KPresenterChild* _child )
  : KoFrame(_view)
{
  m_pKPresenterView = _view;
  m_pKPresenterChild = _child;
}

/*****************************************************************/
/* class KPresenterView                                     */
/*****************************************************************/

/*======================= constructor ===========================*/
KPresenterView::KPresenterView( QWidget *_parent, const char *_name, KPresenterDoc* _doc )
  : QWidget(_parent,_name), KoViewIf( _doc ), OPViewIf( _doc ), KPresenter::KPresenterView_skel()
{
  setWidget(this);

  OPPartIf::setFocusPolicy(OpenParts::Part::ClickFocus);
 
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
  delPageDia = 0;
  insPageDia = 0;
  confPieDia = 0;
  confRectDia = 0;
  spacingDia = 0;
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
  pieType = PT_PIE;
  pieLength = 90 * 16;
  pieAngle = 45 * 16;
  setMouseTracking(true);
  m_bShowGUI = true;
  m_bRectSelection = false;
  presStarted = false;
  searchFirst = true;
  continuePres = false;
  exitPres = false;
  rndX = 0;
  rndY = 0;

  m_pKPresenterDoc = _doc;
  m_bKPresenterModified = true;

  QObject::connect(m_pKPresenterDoc,SIGNAL(sig_KPresenterModified()),this,SLOT(slotKPresenterModified()));
  QObject::connect(m_pKPresenterDoc,SIGNAL(sig_insertObject(KPresenterChild*)),
		   this,SLOT(slotInsertObject(KPresenterChild*)));
  QObject::connect(m_pKPresenterDoc,SIGNAL(sig_updateChildGeometry(KPresenterChild*)),
		   this,SLOT(slotUpdateChildGeometry(KPresenterChild*)));

  createGUI();
}

/*======================= init ============================*/
void KPresenterView::init()
{
  /****
   * Menu
   ****/

  cerr << "Registering menu as " << id() << endl;
  
  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->registerClient( id(), this );
  else
    cerr << "Did not get a menu bar manager" << endl;

  /****
   * Toolbar
   ****/

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->registerClient( id(), this );
  else
    cerr << "Did not get a tool bar manager" << endl;  

  // Show every embedded object
  QListIterator<KPresenterChild> it = m_pKPresenterDoc->childIterator();
  for(;it.current();++it)
    slotInsertObject(it.current());
}

/*======================= destructor ============================*/
KPresenterView::~KPresenterView()
{
  sdeb("KPresenterView::~KPresenterView()\n");
  cleanUp();
  edeb("...KPresenterView::~KPresenterView() %i\n",_refcnt());
}

/*======================= clean up ==============================*/
void KPresenterView::cleanUp()
{
  if (m_bIsClean) return;
  
  cerr << "1a) Deactivate Frames" << endl;
  
  QListIterator<KPresenterFrame> it(m_lstFrames);
  for(;it.current() != 0L;++it)
    {
      it.current()->detach();
    }
  
  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if (!CORBA::is_nil(menu_bar_manager))
    menu_bar_manager->unregisterClient(id());

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if (!CORBA::is_nil(tool_bar_manager))
    tool_bar_manager->unregisterClient(id());

  m_pKPresenterDoc->removeView(this);

  KoViewIf::cleanUp();
}

/*=========================== file print =======================*/
CORBA::Boolean KPresenterView::printDlg()
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
void KPresenterView::editUndo()
{
  page->setToolEditMode(TEM_MOUSE);
  m_pKPresenterDoc->commands()->undo();
}

/*========================== edit redo ==========================*/
void KPresenterView::editRedo()
{
  page->setToolEditMode(TEM_MOUSE);
  m_pKPresenterDoc->commands()->redo();
}

/*========================== edit cut ===========================*/
void KPresenterView::editCut()
{
  page->setToolEditMode(TEM_MOUSE);
  m_pKPresenterDoc->copyObjs(xOffset,yOffset);
  m_pKPresenterDoc->deleteObjs();
}

/*========================== edit copy ==========================*/
void KPresenterView::editCopy()
{
  page->setToolEditMode(TEM_MOUSE);
  m_pKPresenterDoc->copyObjs(xOffset,yOffset);
}

/*========================== edit paste =========================*/
void KPresenterView::editPaste()
{
  page->setToolEditMode(TEM_MOUSE);
  page->deSelectAllObj();
  m_pKPresenterDoc->pasteObjs(xOffset,yOffset);
}

/*========================== edit delete ========================*/
void KPresenterView::editDelete()
{
  page->setToolEditMode(TEM_MOUSE);
  m_pKPresenterDoc->deleteObjs();
}

/*========================== edit select all ====================*/
void KPresenterView::editSelectAll()
{
  page->setToolEditMode(TEM_MOUSE);
  page->selectAllObj();
}

/*========================== edit delete page ===================*/
void KPresenterView::editDelPage()
{
  if (delPageDia)
    {
      QObject::disconnect(delPageDia,SIGNAL(deletePage(int,DelPageMode)),this,SLOT(delPageOk(int,DelPageMode)));
      delPageDia->close();
      delete delPageDia;
      delPageDia = 0;
    }
  
  if (m_pKPresenterDoc->getPageNums() < 2)
    {
      QMessageBox::critical((QWidget*)0L,i18n("KPresenter Error"),
			  i18n("Every document has to have at least one page. Because this document \n"
			       "has not more that one page you can't delete this one."),
			  i18n("OK"));
    }
  else
    {
      delPageDia = new DelPageDia(0,"",m_pKPresenterDoc,getCurrPgNum());
      delPageDia->setCaption(i18n("KPresenter - Delete Page"));
      delPageDia->setMaximumSize(delPageDia->width(),delPageDia->height());
      delPageDia->setMinimumSize(delPageDia->width(),delPageDia->height());
      QObject::connect(delPageDia,SIGNAL(deletePage(int,DelPageMode)),this,SLOT(delPageOk(int,DelPageMode)));
      delPageDia->show();
    }
}

/*========================== edit find ==========================*/
void KPresenterView::editFind()
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
void KPresenterView::editFindReplace()
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
void KPresenterView::newView()
{
  assert((m_pKPresenterDoc != 0L));

  KPresenterShell *shell = new KPresenterShell;
  shell->show();
  shell->setDocument(m_pKPresenterDoc);
}

/*====================== insert a new page ======================*/
void KPresenterView::insertPage()
{
  if (insPageDia)
    {
      QObject::disconnect(insPageDia,SIGNAL(insertPage(int,InsPageMode,InsertPos)),this,SLOT(insPageOk(int,InsPageMode,InsertPos)));
      insPageDia->close();
      delete insPageDia;
      insPageDia = 0;
    }
  
  insPageDia = new InsPageDia(0,"",m_pKPresenterDoc,getCurrPgNum());
  insPageDia->setCaption(i18n("KPresenter - Insert Page"));
  insPageDia->setMaximumSize(insPageDia->width(),insPageDia->height());
  insPageDia->setMinimumSize(insPageDia->width(),insPageDia->height());
  QObject::connect(insPageDia,SIGNAL(insertPage(int,InsPageMode,InsertPos)),this,SLOT(insPageOk(int,InsPageMode,InsertPos)));
  insPageDia->show();
}

/*==============================================================*/
void KPresenterView::toolsMouse()
{
  page->setToolEditMode(TEM_MOUSE);
  page->deSelectAllObj();
}

/*====================== insert a picture =======================*/
void KPresenterView::insertPicture()
{
  page->setToolEditMode(TEM_MOUSE);
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
void KPresenterView::insertClipart()
{
  page->setToolEditMode(TEM_MOUSE);
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
void KPresenterView::toolsLine()
{
  page->setToolEditMode(INS_LINE);
  page->deSelectAllObj();
}

/*===================== insert rectangle ========================*/
void KPresenterView::toolsRectangle()
{
  page->deSelectAllObj();
  page->setToolEditMode(INS_RECT);
}

/*===================== insert circle or ellipse ================*/
void KPresenterView::toolsCircleOrEllipse()
{
  page->deSelectAllObj();
  page->setToolEditMode(INS_ELLIPSE);
  //m_pKPresenterDoc->insertCircleOrEllipse(pen,brush,fillType,gColor1,gColor2,gType,xOffset,yOffset);
}

/*==============================================================*/
void KPresenterView::toolsPie()
{
  page->deSelectAllObj();
  page->setToolEditMode(INS_PIE);
  //m_pKPresenterDoc->insertPie(pen,brush,fillType,gColor1,gColor2,gType,pieType,pieAngle,pieLength,lineBegin,lineEnd,xOffset,yOffset);
}

/*===================== insert a textobject =====================*/
void KPresenterView::toolsText()
{
  page->deSelectAllObj();
  page->setToolEditMode(INS_TEXT);
  //m_pKPresenterDoc->insertText(xOffset,yOffset);
}

/*======================== insert autoform ======================*/
void KPresenterView::insertAutoform()
{
  if (afChoose)
    {
      QObject::disconnect(afChoose,SIGNAL(formChosen(const char*)),this,SLOT(afChooseOk(const char*)));
      afChoose->close();
      delete afChoose;
      afChoose = 0;
    }
  afChoose = new AFChoose(0,i18n("Autoform-Choose"));
  afChoose->resize(400,300);
  afChoose->setCaption(i18n("KPresenter - Insert an Autoform"));
  afChoose->setMaximumSize(afChoose->width(),afChoose->height());
  afChoose->setMinimumSize(afChoose->width(),afChoose->height());
  QObject::connect(afChoose,SIGNAL(formChosen(const char*)),this,SLOT(afChooseOk(const char*)));
  page->setToolEditMode(TEM_MOUSE);
  afChoose->show();
}

/*======================== insert object ========================*/
void KPresenterView::toolsObject()
{
  page->deSelectAllObj();
  KoPartEntry* pe = KoPartSelectDia::selectPart();
  if (!pe) return;
  
  page->setToolEditMode(INS_OBJECT);
  page->setPartEntry(pe);
}

/*===================== extra pen and brush =====================*/
void KPresenterView::extraPenBrush()
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
  page->setToolEditMode(TEM_MOUSE);
  styleDia->show();
}

/*===============================================================*/
void KPresenterView::extraConfigPie()
{
  if (confPieDia)
    {
      QObject::disconnect(confPieDia,SIGNAL(confPieDiaOk()),this,SLOT(confPieOk()));
      confPieDia->close();
      delete confPieDia;
      confPieDia = 0;
    }

  confPieDia = new ConfPieDia(0,"ConfPageDia");
  confPieDia->setMaximumSize(confPieDia->width(),confPieDia->height());
  confPieDia->setMinimumSize(confPieDia->width(),confPieDia->height());
  confPieDia->setType(m_pKPresenterDoc->getPieType(pieType));
  confPieDia->setAngle(m_pKPresenterDoc->getPieAngle(pieAngle));
  confPieDia->setLength(m_pKPresenterDoc->getPieLength(pieLength));
  confPieDia->setPenBrush(m_pKPresenterDoc->getPen(pen),m_pKPresenterDoc->getBrush(brush));
  confPieDia->setCaption(i18n("KPresenter - Configure Pie/Arc/Chord"));
  QObject::connect(confPieDia,SIGNAL(confPieDiaOk()),this,SLOT(confPieOk()));
  page->setToolEditMode(TEM_MOUSE);
  confPieDia->show();
}

/*===============================================================*/
void KPresenterView::extraConfigRect()
{
  if (confRectDia)
    {
      QObject::disconnect(confRectDia,SIGNAL(confRectDiaOk()),this,SLOT(confRectOk()));
      confRectDia->close();
      delete confRectDia;
      confRectDia = 0;
    }

  confRectDia = new ConfRectDia(0,"ConfRectDia");
  confRectDia->setMaximumSize(confRectDia->width(),confRectDia->height());
  confRectDia->setMinimumSize(confRectDia->width(),confRectDia->height());
  confRectDia->setRnds(m_pKPresenterDoc->getRndX(rndX),m_pKPresenterDoc->getRndY(rndY));
  confRectDia->setCaption(i18n("KPresenter - Configure Rectangle"));
  QObject::connect(confRectDia,SIGNAL(confRectDiaOk()),this,SLOT(confRectOk()));
  page->setToolEditMode(TEM_MOUSE);
  confRectDia->show();
}

/*========================== extra raise ========================*/
void KPresenterView::extraRaise()
{
  page->setToolEditMode(TEM_MOUSE);
  m_pKPresenterDoc->raiseObjs(xOffset,yOffset);
}

/*========================== extra lower ========================*/
void KPresenterView::extraLower()
{
  page->setToolEditMode(TEM_MOUSE);
  m_pKPresenterDoc->lowerObjs(xOffset,yOffset);
}

/*========================== extra rotate =======================*/
void KPresenterView::extraRotate()
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
      page->setToolEditMode(TEM_MOUSE);
      rotateDia->show();
    }
}

/*========================== extra shadow =======================*/
void KPresenterView::extraShadow()
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
      page->setToolEditMode(TEM_MOUSE);
      shadowDia->show();
    }
}

/*========================== extra align obj ====================*/
void KPresenterView::extraAlignObj()
{
  page->setToolEditMode(TEM_MOUSE);

  KPoint pnt(QCursor::pos());

  rb_oalign->popup(pnt);

//   QEvent ev(Event_Leave);
//   QMouseEvent mev(Event_MouseButtonRelease,
// 		  QCursor::pos(),LeftButton,LeftButton);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Line),&ev);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Line),&mev);
}

/*====================== extra background =======================*/
void KPresenterView::extraBackground()
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
void KPresenterView::extraLayout()
{
  KoPageLayout pgLayout = m_pKPresenterDoc->pageLayout();
  KoPageLayout oldLayout = m_pKPresenterDoc->pageLayout();
  KoHeadFoot hf;
  
  if (KoPageLayoutDia::pageLayout(pgLayout,hf,FORMAT_AND_BORDERS)) 
    {
      PgLayoutCmd *pgLayoutCmd = new PgLayoutCmd(i18n("Set Pagelayout"),pgLayout,oldLayout,this);
      pgLayoutCmd->execute();
      kPresenterDoc()->commands()->addCommand(pgLayoutCmd);
    }
}

/*========================== extra options ======================*/
void KPresenterView::extraOptions()
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
  optionDia->setRastX(kPresenterDoc()->getRastX());
  optionDia->setRastY(kPresenterDoc()->getRastY());
  optionDia->setBackCol(kPresenterDoc()->getTxtBackCol());
  optionDia->show();
}

/*===============================================================*/
void KPresenterView::extraLineBegin()
{
  page->setToolEditMode(TEM_MOUSE);

  KPoint pnt(QCursor::pos());

  rb_lbegin->popup(pnt);

//   QEvent ev(Event_Leave);
//   QMouseEvent mev(Event_MouseButtonRelease,
// 		  QCursor::pos(),LeftButton,LeftButton);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Line),&ev);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Line),&mev);
}

/*===============================================================*/
void KPresenterView::extraLineEnd()
{
  page->setToolEditMode(TEM_MOUSE);

  KPoint pnt(QCursor::pos());

  rb_lend->popup(pnt);

//   QEvent ev(Event_Leave);
//   QMouseEvent mev(Event_MouseButtonRelease,
// 		  QCursor::pos(),LeftButton,LeftButton);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Line),&ev);
//   QApplication::sendEvent(m_rToolBarInsert->getButton(m_idButtonInsert_Line),&mev);
}

/*========================== screen config pages ================*/
void KPresenterView::screenConfigPages()
{
  if (pgConfDia)
    {
      QObject::disconnect(pgConfDia,SIGNAL(pgConfDiaOk()),this,SLOT(pgConfOk()));
      pgConfDia->close();
      delete pgConfDia;
      pgConfDia = 0;
    }
  pgConfDia = new PgConfDia(0,"PageConfig",kPresenterDoc()->spInfinitLoop(),
			    kPresenterDoc()->spManualSwitch(),getCurrPgNum(),
			    kPresenterDoc()->backgroundList()->at(getCurrPgNum() - 1)->getPageEffect(),
			    kPresenterDoc()->getPresSpeed());
  pgConfDia->setMaximumSize(pgConfDia->width(),pgConfDia->height());
  pgConfDia->setMinimumSize(pgConfDia->width(),pgConfDia->height());
  pgConfDia->setCaption(i18n("KPresenter - Page Configuration for Screenpresentations"));
  QObject::connect(pgConfDia,SIGNAL(pgConfDiaOk()),this,SLOT(pgConfOk()));
  pgConfDia->show();
}

/*========================== screen presStructView  =============*/
void KPresenterView::screenPresStructView()
{
  if (!presStructView)
    {
      page->deSelectAllObj();
      page->setToolEditMode(TEM_MOUSE);
      
      presStructView = new PresStructViewer(0,"",kPresenterDoc(),this);
      presStructView->setCaption(i18n("KPresenter - Presentation structure viewer"));
      QObject::connect(presStructView,SIGNAL(presStructViewClosed()),this,SLOT(psvClosed()));
      presStructView->show();
    }
}

/*========================== screen assign effect ===============*/
void KPresenterView::screenAssignEffect()
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
      effectDia = new EffectDia(0,"Effect",_pNum,_oNum,(KPresenterView*)this);
      effectDia->setMaximumSize(effectDia->width(),effectDia->height());
      effectDia->setMinimumSize(effectDia->width(),effectDia->height());
      effectDia->setCaption(i18n("KPresenter - Assign effects"));
      QObject::connect(effectDia,SIGNAL(effectDiaOk()),this,SLOT(effectOk()));
      effectDia->show();
      page->deSelectAllObj();
      page->setToolEditMode(TEM_MOUSE);
      page->selectObj(_oNum);
    }
  else
    QMessageBox::critical((QWidget*)0L,i18n("KPresenter Error"),
			  i18n("I can't assign an effect. You have to select EXACTLY one object!"),
			  i18n("OK"));
}

/*========================== screen start =======================*/
void KPresenterView::screenStart()
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
	    static_cast<float>(kPresenterDoc()->getPageSize(0,0,0,1.0,false).width()) > 1.0 ? 
	    static_cast<float>(page->width()) / static_cast<float>(kPresenterDoc()->getPageSize(0,0,0,1.0,false).width()) : 1.0;
	  float _presFaktH = static_cast<float>(page->height()) / 
	    static_cast<float>(kPresenterDoc()->getPageSize(0,0,0,1.0,false).height()) > 
	    1.0 ? static_cast<float>(page->height()) / static_cast<float>(kPresenterDoc()->getPageSize(0,0,0,1.0,false).height()) : 1.0;
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

      if (page->width() > kPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).width())
 	xOffset -= (page->width() - kPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).width()) / 2;
      if (page->height() > kPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).height())
 	yOffset -= (page->height() - kPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).height()) / 2;

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
	  page->recreate((QWidget*)0L,WStyle_Customize | WStyle_NoBorder | WType_Popup,KPoint(0,0),true);
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

      if (!kPresenterDoc()->spManualSwitch())
	{
	  continuePres = true;
	  exitPres = false;
	  doAutomaticScreenPres();
	}
    }
}

/*========================== screen stop ========================*/
void KPresenterView::screenStop()
{
  if (presStarted)
    {
      continuePres = false;
      exitPres = true;
      if (true) //m_rToolBarScreen->isButtonOn(m_idButtonScreen_Full))
	{
	  page->close(false);
	  page->recreate((QWidget*)this,0,KPoint(0,0),true);
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
void KPresenterView::screenPause()
{
}

/*========================== screen first =======================*/
void KPresenterView::screenFirst()
{
  vert->setValue(0); 
}

/*========================== screen pevious =====================*/
void KPresenterView::screenPrev()
{
  if (presStarted)
    {
      if (page->pPrev(true))
	{
	  yOffset -= kPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).height(); 
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
      presentParts(page->presFakt(),&p,KRect(0,0,0,0),xOffset,yOffset);
      p.end();
    }
  else
    vert->setValue(yOffset - kPresenterDoc()->getPageSize(0,0,0,1.0,false).height()); 
}

/*========================== screen next ========================*/
void KPresenterView::screenNext()
{
  if (presStarted)
    {
      if (page->pNext(true))
	{
	  yOffset += kPresenterDoc()->getPageSize(0,0,0,page->presFakt(),false).height(); 
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
      presentParts(page->presFakt(),&p,KRect(0,0,0,0),xOffset,yOffset);
      p.end();
    }
  else
    vert->setValue(yOffset + kPresenterDoc()->getPageSize(0,0,0,1.0,false).height()); 
}

/*========================== screen last ========================*/
void KPresenterView::screenLast()
{
  vert->setValue(vert->maxValue());
}

/*========================== screen skip =======================*/
void KPresenterView::screenSkip()
{
}

/*========================== screen full screen ================*/
void KPresenterView::screenFullScreen()
{
  warning("Screenpresentations only work in FULLSCREEN mode at the moment!");
}

/*========================== screen pen/marker =================*/
void KPresenterView::screenPen()
{
  KPoint pnt(QCursor::pos());

  rb_pen->popup(pnt);
}

/*======================= help contents ========================*/
void KPresenterView::helpContents()
{
  KoAboutDia::about(KoAboutDia::KOffice,"0.0.1");
}

/*======================= help about kde ========================*/
// void KPresenterView::helpAboutKDE()
// {
//   KoAboutDia::about(KoAboutDia::KDE);
// }

/*======================= text size selected  ===================*/
void KPresenterView::sizeSelected(const char *size)
{
  tbFont.setPointSize(atoi(size));
  page->setTextFont(&tbFont);
}

/*======================= text font selected  ===================*/
void KPresenterView::fontSelected(const char *font)
{
  tbFont.setFamily(qstrdup(font));
  page->setTextFont(&tbFont);
}

/*========================= text bold ===========================*/
void KPresenterView::textBold()
{
  tbFont.setBold(!tbFont.bold());
  page->setTextFont(&tbFont);
}

/*========================== text italic ========================*/
void KPresenterView::textItalic()
{
  tbFont.setItalic(!tbFont.italic());
  page->setTextFont(&tbFont);
}

/*======================== text underline =======================*/
void KPresenterView::textUnderline()
{
  tbFont.setUnderline(!tbFont.underline());
  page->setTextFont(&tbFont);
}

/*=========================== text color ========================*/
void KPresenterView::textColor()
{
  if (KColorDialog::getColor(tbColor))
    {
      OpenPartsUI::Pixmap pix;
      pix.data = CORBA::string_dup( colorToPixString( tbColor ) );
      m_vToolBarText->setButtonPixmap(ID_TEXT_COLOR, pix );
      page->setTextColor(&tbColor);
    }
}

/*======================= text align left =======================*/
void KPresenterView::textAlignLeft()
{
  tbAlign = TxtParagraph::LEFT;
  page->setTextAlign(tbAlign);

  m_vToolBarText->setButton(ID_ALEFT,true);
  m_vToolBarText->setButton(ID_ACENTER,false);
  m_vToolBarText->setButton(ID_ARIGHT,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Left,true);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Center,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Right,false);
}

/*======================= text align center =====================*/
void KPresenterView::textAlignCenter()
{
  tbAlign = TxtParagraph::CENTER;
  page->setTextAlign(TxtParagraph::CENTER);

  m_vToolBarText->setButton(ID_ALEFT,false);
  m_vToolBarText->setButton(ID_ACENTER,true);
  m_vToolBarText->setButton(ID_ARIGHT,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Left,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Center,true);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Right,false);
}

/*======================= text align right ======================*/
void KPresenterView::textAlignRight()
{
  tbAlign = TxtParagraph::RIGHT;
  page->setTextAlign(TxtParagraph::RIGHT);

  m_vToolBarText->setButton(ID_ALEFT,false);
  m_vToolBarText->setButton(ID_ACENTER,false);
  m_vToolBarText->setButton(ID_ARIGHT,true);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Left,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Center,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Right,true);
}

/*======================= text align left =======================*/
void KPresenterView::mtextAlignLeft()
{
  tbAlign = TxtParagraph::LEFT;
  page->setTextAlign(tbAlign);

  m_vToolBarText->setButton(ID_ALEFT,true);
  m_vToolBarText->setButton(ID_ACENTER,false);
  m_vToolBarText->setButton(ID_ARIGHT,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Left,true);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Center,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Right,false);
}

/*======================= text align center =====================*/
void KPresenterView::mtextAlignCenter()
{
  tbAlign = TxtParagraph::CENTER;
  page->setTextAlign(TxtParagraph::CENTER);

  m_vToolBarText->setButton(ID_ALEFT,false);
  m_vToolBarText->setButton(ID_ACENTER,true);
  m_vToolBarText->setButton(ID_ARIGHT,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Left,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Center,true);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Right,false);
}

/*======================= text align right ======================*/
void KPresenterView::mtextAlignRight()
{
  tbAlign = TxtParagraph::RIGHT;
  page->setTextAlign(TxtParagraph::RIGHT);

  m_vToolBarText->setButton(ID_ALEFT,false);
  m_vToolBarText->setButton(ID_ACENTER,false);
  m_vToolBarText->setButton(ID_ARIGHT,true);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Left,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Center,false);
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Right,true);
}

/*====================== font selected ==========================*/
void KPresenterView::mtextFont()
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
void KPresenterView::textEnumList()
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
void KPresenterView::textUnsortList()
{
  if (page->kTxtObj())
    {
      QList<QFont> *_font = page->kTxtObj()->unsortListType().font;
      QList<QColor> *_color = page->kTxtObj()->unsortListType().color;
      QList<int> *_c = page->kTxtObj()->unsortListType().chr;
      
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
void KPresenterView::textNormalText()
{
  if (page->kTxtObj()) page->kTxtObj()->setObjType(KTextObject::PLAIN);
}

/*===============================================================*/
void KPresenterView::textDepthPlus()
{
  if (page->kTxtObj()) page->kTxtObj()->incDepth();
}

/*===============================================================*/
void KPresenterView::textDepthMinus()
{
  if (page->kTxtObj()) page->kTxtObj()->decDepth();
}

/*===============================================================*/
void KPresenterView::textSpacing()
{
  KTextObject *obj = 0L;

  if (page->kTxtObj()) obj = page->kTxtObj();
  else if (page->haveASelectedTextObj()) obj = page->haveASelectedTextObj();

  if (obj)
    {
      if (spacingDia)
	{
	  QObject::disconnect(spacingDia,SIGNAL(spacingDiaOk()),this,SLOT(spacingOk()));
	  spacingDia->close();
	  delete spacingDia;
	  spacingDia = 0;
	}
      
      spacingDia = new SpacingDia(0,obj->getLineSpacing(),obj->getDistBefore(),obj->getDistAfter());
      spacingDia->setMaximumSize(spacingDia->width(),spacingDia->height());
      spacingDia->setMinimumSize(spacingDia->width(),spacingDia->height());
      spacingDia->setCaption(i18n("KPresenter - Spacings"));
      QObject::connect(spacingDia,SIGNAL(spacingDiaOk(int,int,int)),this,SLOT(spacingOk(int,int,int)));
      spacingDia->show();
    }
}

/*======================= align object left =====================*/
void KPresenterView::extraAlignObjLeftidl()
{
  kPresenterDoc()->alignObjsLeft();
}

/*======================= align object center h =================*/
void KPresenterView::extraAlignObjCenterHidl()
{
  kPresenterDoc()->alignObjsCenterH();
}

/*======================= align object right ====================*/
void KPresenterView::extraAlignObjRightidl()
{
  kPresenterDoc()->alignObjsRight();
}

/*======================= align object top ======================*/
void KPresenterView::extraAlignObjTopidl()
{
  kPresenterDoc()->alignObjsTop();
}

/*======================= align object center v =================*/
void KPresenterView::extraAlignObjCenterVidl()
{
  kPresenterDoc()->alignObjsCenterV();
}

/*======================= align object bottom ===================*/
void KPresenterView::extraAlignObjBottomidl()
{
  kPresenterDoc()->alignObjsBottom();
}

/*===============================================================*/
void KPresenterView::extraLineBeginNormal()
{
  if (!m_pKPresenterDoc->setLineBegin(L_NORMAL))
    lineBegin = L_NORMAL;
}

/*===============================================================*/
void KPresenterView::extraLineBeginArrow()
{
  if (!m_pKPresenterDoc->setLineBegin(L_ARROW))
    lineBegin = L_ARROW;
}

/*===============================================================*/
void KPresenterView::extraLineBeginRect()
{
  if (!m_pKPresenterDoc->setLineBegin(L_SQUARE))
    lineBegin = L_SQUARE;
}

/*===============================================================*/
void KPresenterView::extraLineBeginCircle()
{
  if (!m_pKPresenterDoc->setLineBegin(L_CIRCLE))
    lineBegin = L_CIRCLE;
}

/*===============================================================*/
void KPresenterView::extraLineEndNormal()
{
  if (!m_pKPresenterDoc->setLineEnd(L_NORMAL))
    lineEnd = L_NORMAL;
}

/*===============================================================*/
void KPresenterView::extraLineEndArrow()
{
  if (!m_pKPresenterDoc->setLineEnd(L_ARROW))
    lineEnd = L_ARROW;
}

/*===============================================================*/
void KPresenterView::extraLineEndRect()
{
  if (!m_pKPresenterDoc->setLineEnd(L_SQUARE))
    lineEnd = L_SQUARE;
}

/*===============================================================*/
void KPresenterView::extraLineEndCircle()
{
  if (!m_pKPresenterDoc->setLineEnd(L_CIRCLE))
    lineEnd = L_CIRCLE;
}

/*===============================================================*/
void KPresenterView::newPageLayout(KoPageLayout _layout)
{
  KoPageLayout oldLayout = m_pKPresenterDoc->pageLayout();

  PgLayoutCmd *pgLayoutCmd = new PgLayoutCmd(i18n("Set Pagelayout"),_layout,oldLayout,this);
  pgLayoutCmd->execute();
  kPresenterDoc()->commands()->addCommand(pgLayoutCmd);
}

/*======================== create GUI ==========================*/
void KPresenterView::createGUI()
{
  sdeb("void KPresenterView::createGUI() %i | %i\n",refCount(),_refcnt());

  // setup page
  page = new Page(this,"Page",(KPresenterView*)this);
  QObject::connect(page,SIGNAL(fontChanged(QFont*)),this,SLOT(fontChanged(QFont*)));
  QObject::connect(page,SIGNAL(colorChanged(QColor*)),this,SLOT(colorChanged(QColor*)));
  QObject::connect(page,SIGNAL(alignChanged(TxtParagraph::HorzAlign)),this,SLOT(alignChanged(TxtParagraph::HorzAlign)));
  widget()->setFocusPolicy(QWidget::StrongFocus);
  widget()->setFocusProxy(page);

  // setup GUI
  setupPopupMenus();
  setupScrollbars();
  setRanges();
  setupRulers();
 
  if (m_pKPresenterDoc && page)
    QObject::connect(page,SIGNAL(stopPres()),this,SLOT(stopPres()));

  resizeEvent(0L);
}

/*====================== construct ==============================*/
void KPresenterView::construct()
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
void KPresenterView::slotKPresenterModified()
{
  m_bKPresenterModified = true;
  update();
}

/*======================= insert object ========================*/
void KPresenterView::slotInsertObject(KPresenterChild *_child)
{ 
  OpenParts::View_var v;

  try
  { 
    v = _child->createView( m_vKoMainWindow );
  }
  catch (OpenParts::Document::MultipleViewsNotSupported &_ex)
  {
    // HACK
    printf("void KPresenterView::slotInsertObject( const KRect& _rect, OPParts::Document_ptr _doc )\n");
    printf("Could not create view\n");
    exit(1);
  }
  
  if (CORBA::is_nil(v))
  {
    printf("void KPresenterView::slotInsertObject( const KRect& _rect, OPParts::Document_ptr _doc )\n");
    printf("return value is 0L\n");
    exit(1);
  }

  KPresenterFrame *p = new KPresenterFrame(this,_child);
  p->setGeometry(_child->geometry());
  m_lstFrames.append(p);

  KOffice::View_var kv = KOffice::View::_narrow( v );
  kv->setMode( KOffice::View::ChildMode );
  assert( !CORBA::is_nil( kv ) );
  p->attachView( kv );

  p->show();

  page->insertChild(p);
  vert->raise();
  horz->raise();
  
  QObject::connect(p,SIGNAL(sig_geometryEnd(KoFrame*)),
		   this,SLOT(slotGeometryEnd(KoFrame*)));
  QObject::connect(p,SIGNAL(sig_moveEnd(KoFrame*)),
		   this,SLOT(slotMoveEnd(KoFrame*)));  
} 

/*========================== update child geometry =============*/
void KPresenterView::slotUpdateChildGeometry(KPresenterChild *_child)
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
void KPresenterView::slotGeometryEnd(KoFrame* _frame)
{
  KPresenterFrame *f = (KPresenterFrame*)_frame;
  // TODO scaling
  m_pKPresenterDoc->changeChildGeometry(f->child(),_frame->partGeometry(),xOffset,yOffset);
}

/*==================== slot move end ===========================*/
void KPresenterView::slotMoveEnd(KoFrame* _frame)
{
  KPresenterFrame *f = (KPresenterFrame*)_frame;
  // TODO scaling
  m_pKPresenterDoc->changeChildGeometry(f->child(),_frame->partGeometry(),xOffset,yOffset);
}

/*=========== take changes for backgr dialog =====================*/
void KPresenterView::backOk(bool takeGlobal)
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
void KPresenterView::afChooseOk(const char* c)
{
  QString afDir = kapp->kde_datadir();
  QFileInfo fileInfo(c);
  QString fileName(afDir + "/kpresenter/autoforms/" + fileInfo.dirPath(false) + "/" + fileInfo.baseName() + ".atf");
  
  page->deSelectAllObj();
  m_pKPresenterDoc->insertAutoform(pen,brush,lineBegin,lineEnd,fillType,gColor1,gColor2,gType,fileName,xOffset,yOffset);
}

/*=========== take changes for style dialog =====================*/
void KPresenterView::styleOk()
{
  if (!m_pKPresenterDoc->setPenBrush(styleDia->getPen(),styleDia->getBrush(),styleDia->getLineBegin(),
				     styleDia->getLineEnd(),styleDia->getFillType(),styleDia->getGColor1(),
				     styleDia->getGColor2(),styleDia->getGType()))
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
void KPresenterView::optionOk()
{
  if (optionDia->getRastX() < 1)
    optionDia->setRastX(1);
  if (optionDia->getRastY() < 1)
    optionDia->setRastY(1);
  kPresenterDoc()->setRasters(optionDia->getRastX(),optionDia->getRastY(),false);

  kPresenterDoc()->setTxtBackCol(optionDia->getBackCol());

  kPresenterDoc()->replaceObjs();
  kPresenterDoc()->repaint(false);
}

/*=================== page configuration ok ======================*/
void KPresenterView::pgConfOk()
{
  PgConfCmd *pgConfCmd = new PgConfCmd(i18n("Configure Page for Screenpresentations"),
				       pgConfDia->getManualSwitch(),pgConfDia->getInfinitLoop(),
				       pgConfDia->getPageEffect(),pgConfDia->getPresSpeed(),
				       kPresenterDoc()->spInfinitLoop(),kPresenterDoc()->spManualSwitch(),
				       kPresenterDoc()->backgroundList()->at(getCurrPgNum() - 1)->getPageEffect(),
				       kPresenterDoc()->getPresSpeed(),kPresenterDoc(),getCurrPgNum() - 1);
  pgConfCmd->execute();
  kPresenterDoc()->commands()->addCommand(pgConfCmd);
}

/*=================== effect dialog ok ===========================*/
void KPresenterView::effectOk()
{
}

/*=================== rotate dialog ok ===========================*/
void KPresenterView::rotateOk()
{
  KPObject *kpobject = 0;
  QList<KPObject> _objects;
  QList<RotateCmd::RotateValues> _oldRotate;
  float _newAngle;
  RotateCmd::RotateValues *tmp;

  _objects.setAutoDelete(false);
  _oldRotate.setAutoDelete(false);

  _newAngle = rotateDia->getAngle();

  for (int i = 0;i < static_cast<int>(kPresenterDoc()->objectList()->count());i++)
    {
      kpobject = kPresenterDoc()->objectList()->at(i);
      tmp = new RotateCmd::RotateValues;
      tmp->angle = kpobject->getAngle();
      _oldRotate.append(tmp);

      if (kpobject->isSelected())
	_objects.append(kpobject);
    }

  if (!_objects.isEmpty())
    {
      RotateCmd *rotateCmd = new RotateCmd(i18n("Change Rotation"),_oldRotate,_newAngle,_objects,kPresenterDoc());
      kPresenterDoc()->commands()->addCommand(rotateCmd);
      rotateCmd->execute();
    }
  else
    {
      _oldRotate.setAutoDelete(true);
      _oldRotate.clear();
    }
}

/*=================== shadow dialog ok ==========================*/
void KPresenterView::shadowOk()
{
  KPObject *kpobject = 0;
  QList<KPObject> _objects;
  QList<ShadowCmd::ShadowValues> _oldShadow;
  ShadowCmd::ShadowValues _newShadow,*tmp;

  _objects.setAutoDelete(false);
  _oldShadow.setAutoDelete(false);

  _newShadow.shadowDirection = shadowDia->getShadowDirection();
  _newShadow.shadowDistance = shadowDia->getShadowDistance();
  _newShadow.shadowColor = shadowDia->getShadowColor();

  for (int i = 0;i < static_cast<int>(kPresenterDoc()->objectList()->count());i++)
    {
      kpobject = kPresenterDoc()->objectList()->at(i);
      tmp = new ShadowCmd::ShadowValues;
      tmp->shadowDirection = kpobject->getShadowDirection();
      tmp->shadowDistance = kpobject->getShadowDistance();
      tmp->shadowColor = kpobject->getShadowColor();
      _oldShadow.append(tmp);

      if (kpobject->isSelected())
	_objects.append(kpobject);
    }

  if (!_objects.isEmpty())
    {
      ShadowCmd *shadowCmd = new ShadowCmd(i18n("Change Shadow"),_oldShadow,_newShadow,_objects,kPresenterDoc());
      kPresenterDoc()->commands()->addCommand(shadowCmd);
      shadowCmd->execute();
    }
  else
    {
      _oldShadow.setAutoDelete(true);
      _oldShadow.clear();
    }
}

/*================================================================*/
void KPresenterView::psvClosed()
{
  QObject::disconnect(presStructView,SIGNAL(presStructViewClosed()),this,SLOT(psvClosed()));
  presStructView = 0;
}

/*================================================================*/
void KPresenterView::delPageOk(int _page,DelPageMode _delPageMode)
{
  m_pKPresenterDoc->deletePage(_page,_delPageMode);
  setRanges();
}

   
/*================================================================*/
void KPresenterView::insPageOk(int _page,InsPageMode _insPageMode,InsertPos _insPos)
{
  m_pKPresenterDoc->insertPage(_page,_insPageMode,_insPos);
  setRanges();
}

/*================================================================*/
void KPresenterView::confPieOk()
{
  if (!m_pKPresenterDoc->setPieSettings(confPieDia->getType(),confPieDia->getAngle(),confPieDia->getLength()))
    {
      pieType = confPieDia->getType();
      pieAngle = confPieDia->getAngle();
      pieLength = confPieDia->getLength();
    }
}

/*================================================================*/
void KPresenterView::confRectOk()
{
  if (!m_pKPresenterDoc->setRectSettings(confRectDia->getRndX(),confRectDia->getRndY()))
    {
      rndX = confRectDia->getRndX();
      rndY = confRectDia->getRndY();
    }
}

/*================================================================*/
void KPresenterView::spacingOk(int _lineSpacing,int _distBefore,int _distAfter)
{
  if (page->kTxtObj())
    {
      page->kTxtObj()->setLineSpacing(_lineSpacing);
      page->kTxtObj()->setDistBefore(_distBefore);
      page->kTxtObj()->setDistAfter(_distAfter);
    }
  else if (page->haveASelectedTextObj())
    {
      KTextObject *obj = page->haveASelectedTextObj();
      obj->setAllLineSpacing(_lineSpacing);
      obj->setAllDistBefore(_distBefore);
      obj->setAllDistAfter(_distAfter);
      repaint(false);
    }
}

/*================== scroll horizontal ===========================*/
void KPresenterView::scrollH(int _value)
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
void KPresenterView::scrollV(int _value)
{
  if (!presStarted)
    {
      int yo = yOffset;
      
      yOffset = _value;
      page->scroll(0,yo - _value);

      if (v_ruler)
	v_ruler->setOffset(0,-kPresenterDoc()->getPageSize(getCurrPgNum() - 1,xOffset,yOffset,1.0,false).y());
    }
}

/*====================== font changed ===========================*/
void KPresenterView::fontChanged(QFont* font)
{
  if (font->operator!=(tbFont)) 
    {
      tbFont.setFamily(font->family());
      tbFont.setBold(font->bold());
      tbFont.setItalic(font->italic());
      tbFont.setUnderline(font->underline());
      tbFont.setPointSize(font->pointSize());
      m_vToolBarText->setButton(ID_BOLD,tbFont.bold());
      m_vToolBarText->setButton(ID_ITALIC,tbFont.italic());
      m_vToolBarText->setButton(ID_UNDERLINE,tbFont.underline());
      int pos = fontList.find(tbFont.family());
      assert( pos != -1 );
      cerr << "Setting to number " << pos << endl;
      m_vToolBarText->setCurrentComboItem(ID_FONT_LIST, pos );
      cerr << "Never reached" << endl;
      m_vToolBarText->setCurrentComboItem(ID_FONT_SIZE,tbFont.pointSize()-4);
    }
}

/*====================== color changed ==========================*/
void KPresenterView::colorChanged(QColor* color)
{
  if (color->operator!=(tbColor))
    {
      OpenPartsUI::Pixmap pix;
      pix.data = CORBA::string_dup( colorToPixString( color->rgb() ) );

      tbColor.setRgb(color->rgb());
      m_vToolBarText->setButtonPixmap(ID_TEXT_COLOR, pix );
    }      
}

/*====================== align changed ==========================*/
void KPresenterView::alignChanged(TxtParagraph::HorzAlign align)
{
  if (align != tbAlign)
    {
      tbAlign = align;
      m_vToolBarText->setButton(ID_ALEFT,false);
      m_vToolBarText->setButton(ID_ARIGHT,false);
      m_vToolBarText->setButton(ID_ACENTER,false);
      m_vMenuText->setItemChecked(m_idMenuText_TAlign_Left,false);
      m_vMenuText->setItemChecked(m_idMenuText_TAlign_Center,false);
      m_vMenuText->setItemChecked(m_idMenuText_TAlign_Right,false);
      switch (tbAlign)
	{
	case TxtParagraph::LEFT: 
	  {
	    m_vToolBarText->setButton(ID_ALEFT,true); 
	    m_vMenuText->setItemChecked(m_idMenuText_TAlign_Left,true);
	  } break;
	case TxtParagraph::CENTER:
	  {
	    m_vToolBarText->setButton(ID_ACENTER,true); 
	    m_vMenuText->setItemChecked(m_idMenuText_TAlign_Center,true);
	  } break;
	case TxtParagraph::RIGHT:
	  {
	    m_vToolBarText->setButton(ID_ARIGHT,true); 
	    m_vMenuText->setItemChecked(m_idMenuText_TAlign_Right,true);
	  } break;
	default: break;
	}
    }
}

/*======================== set pres pen width 1 =================*/
void KPresenterView::presPen1()
{
  QPen p = kPresenterDoc()->presPen();
  p.setWidth(1);
  kPresenterDoc()->setPresPen(p);
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
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW1,true);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW2,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW3,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW4,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW5,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW6,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW7,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW8,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW9,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 2 =================*/
void KPresenterView::presPen2()
{
  QPen p = kPresenterDoc()->presPen();
  p.setWidth(2);
  kPresenterDoc()->setPresPen(p);
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
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW1,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW2,true);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW3,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW4,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW5,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW6,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW7,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW8,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW9,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 3 =================*/
void KPresenterView::presPen3()
{
  QPen p = kPresenterDoc()->presPen();
  p.setWidth(3);
  kPresenterDoc()->setPresPen(p);
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
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW1,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW2,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW3,true);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW4,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW5,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW6,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW7,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW8,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW9,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 4 =================*/
void KPresenterView::presPen4()
{
  QPen p = kPresenterDoc()->presPen();
  p.setWidth(4);
  kPresenterDoc()->setPresPen(p);
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
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW1,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW2,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW3,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW4,true);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW5,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW6,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW7,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW8,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW9,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 5 =================*/
void KPresenterView::presPen5()
{
  QPen p = kPresenterDoc()->presPen();
  p.setWidth(5);
  kPresenterDoc()->setPresPen(p);
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
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW1,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW2,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW3,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW4,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW5,true);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW6,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW7,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW8,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW9,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 6 =================*/
void KPresenterView::presPen6()
{
  QPen p = kPresenterDoc()->presPen();
  p.setWidth(6);
  kPresenterDoc()->setPresPen(p);
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
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW1,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW2,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW3,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW4,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW5,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW6,true);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW7,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW8,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW9,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 7 =================*/
void KPresenterView::presPen7()
{
  QPen p = kPresenterDoc()->presPen();
  p.setWidth(7);
  kPresenterDoc()->setPresPen(p);
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
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW1,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW2,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW3,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW4,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW5,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW6,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW7,true);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW8,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW9,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 8 =================*/
void KPresenterView::presPen8()
{
  QPen p = kPresenterDoc()->presPen();
  p.setWidth(8);
  kPresenterDoc()->setPresPen(p);
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
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW1,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW2,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW3,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW4,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW5,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW6,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW7,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW8,true);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW9,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 9 =================*/
void KPresenterView::presPen9()
{
  QPen p = kPresenterDoc()->presPen();
  p.setWidth(9);
  kPresenterDoc()->setPresPen(p);
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
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW1,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW2,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW3,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW4,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW5,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW6,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW7,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW8,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW9,true);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW10,false);
}

/*======================== set pres pen width 10 ================*/
void KPresenterView::presPen10()
{
  QPen p = kPresenterDoc()->presPen();
  p.setWidth(10);
  kPresenterDoc()->setPresPen(p);
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
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW1,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW2,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW3,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW4,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW5,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW6,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW7,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW8,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW9,false);
  m_vMenuScreen->setItemChecked(m_idMenuScreen_PenW10,true);
}

/*======================== set pres pen color ===================*/
void KPresenterView::presPenColor()
{
  QColor c = kPresenterDoc()->presPen().color();
  
  if (KColorDialog::getColor(c))
    {
      QPen p = kPresenterDoc()->presPen();
      p.setColor(c);
      kPresenterDoc()->setPresPen(p);
      QPixmap pix(16,16);
      pix.fill(c);
      rb_pen->changeItem(pix,i18n("Pen color..."),P_COL);
    }
}

/*======================== set pres pen width 1 =================*/
void KPresenterView::presPen1idl()
{
  presPen1();
}

/*======================== set pres pen width 2 =================*/
void KPresenterView::presPen2idl()
{
  presPen2();
}

/*======================== set pres pen width 3 =================*/
void KPresenterView::presPen3idl()
{
  presPen3();
}

/*======================== set pres pen width 4 =================*/
void KPresenterView::presPen4idl()
{
  presPen4();
}

/*======================== set pres pen width 5 =================*/
void KPresenterView::presPen5idl()
{
  presPen5();
}

/*======================== set pres pen width 6 =================*/
void KPresenterView::presPen6idl()
{
  presPen6();
}

/*======================== set pres pen width 7 =================*/
void KPresenterView::presPen7idl()
{
  presPen7();
}

/*======================== set pres pen width 8 =================*/
void KPresenterView::presPen8idl()
{
  presPen8();
}

/*======================== set pres pen width 9 =================*/
void KPresenterView::presPen9idl()
{
  presPen9();
}

/*======================== set pres pen width 10 ================*/
void KPresenterView::presPen10idl()
{
  presPen10();
}

/*======================== set pres pen color ===================*/
void KPresenterView::presPenColoridl()
{
  presPenColor();
}

/*=========================== search =============================*/
void KPresenterView::search(QString text,bool sensitive,bool direction)
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
void KPresenterView::replace(QString search,QString replace,bool sensitive,bool direction)
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
void KPresenterView::replaceAll(QString search,QString replace,bool sensitive)
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
void KPresenterView::repaint(bool erase)
{
  QWidget::repaint(erase);
  page->repaint(erase);
}

/*====================== paint event ============================*/
void KPresenterView::repaint(unsigned int x,unsigned int y,unsigned int w,
				  unsigned int h,bool erase)
{
  QWidget::repaint(x,y,w,h,erase);
  page->repaint(x,y,w,h,erase);
}

/*====================== paint event ============================*/
void KPresenterView::repaint(KRect r,bool erase)
{
  QWidget::repaint(r,erase);
  page->repaint(r,erase);
}

/*====================== change pciture =========================*/
void KPresenterView::changePicture(unsigned int,const char* filename)
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
void KPresenterView::changeClipart(unsigned int,QString filename)
{
  QFileInfo fileInfo(filename);
  QString file = KFileDialog::getOpenFileName(fileInfo.dirPath(false),i18n("*.WMF *.wmf|Windows Metafiles"),0);

  if (!file.isEmpty()) m_pKPresenterDoc->changeClipart(file,xOffset,yOffset);
}

/*====================== resize event ===========================*/
void KPresenterView::resizeEvent(QResizeEvent *e)
{
  if (!presStarted) QWidget::resizeEvent(e);

  if ( ( KoViewIf::hasFocus() || mode() == KOffice::View::RootMode ) && m_bShowGUI )
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
void KPresenterView::keyPressEvent(QKeyEvent *e)
{
  QApplication::sendEvent(page,e);
}

/*====================== do automatic screenpresentation ========*/
void KPresenterView::doAutomaticScreenPres()
{
  page->repaint(false);

  while (continuePres && !exitPres)
    screenNext();

  if (!exitPres && kPresenterDoc()->spInfinitLoop())
    {
      screenStop();
      screenStart();
    }

  screenStop();
}

/*======================= hide parts ============================*/
void KPresenterView::hideParts()
{
  QListIterator<KPresenterFrame> it(m_lstFrames);

  for(;it.current();++it)
    it.current()->hide();
}

/*====================== present parts ==========================*/
void KPresenterView::presentParts(float _presFakt,QPainter* _painter,KRect _rect,int _diffx,int _diffy)
{
  QListIterator<KPresenterChild> chl = m_pKPresenterDoc->childIterator();
  KRect child_geometry;
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
void KPresenterView::showParts()
{
  QListIterator<KPresenterFrame> it(m_lstFrames);

  for(;it.current();++it)
    it.current()->show();
}

/*========================= change undo =========================*/
void KPresenterView::changeUndo(QString _text,bool _enable)
{
  if (_enable)
    {
      m_vMenuEdit->setItemEnabled(m_idMenuEdit_Undo,true);
      QString str;
      str.sprintf(i18n("Undo: %s"),_text.data());
      m_vMenuEdit->changeItemText(str,m_idMenuEdit_Undo);
      m_vToolBarEdit->setItemEnabled(ID_UNDO,true);
    }
  else
    {    
      m_vMenuEdit->changeItemText(i18n("No Undo possible"),m_idMenuEdit_Undo);
      m_vMenuEdit->setItemEnabled(m_idMenuEdit_Undo,false);
      m_vToolBarEdit->setItemEnabled(ID_UNDO,false);
    }
}

/*========================= change redo =========================*/
void KPresenterView::changeRedo(QString _text,bool _enable)
{
  if (_enable)
    {
      m_vMenuEdit->setItemEnabled(m_idMenuEdit_Redo,true);
      QString str;
      str.sprintf(i18n("Redo: %s"),_text.data());
      m_vMenuEdit->changeItemText(str,m_idMenuEdit_Redo);
      m_vToolBarEdit->setItemEnabled(ID_REDO,true);
    }
  else
    {
      m_vMenuEdit->changeItemText(i18n("No Redo possible"),m_idMenuEdit_Redo);
      m_vMenuEdit->setItemEnabled(m_idMenuEdit_Redo,false);
      m_vToolBarEdit->setItemEnabled(ID_REDO,false);
    }
}

/*================ color of pres-pen changed ====================*/
void KPresenterView::presColorChanged()
{
  QPen p = kPresenterDoc()->presPen();
  QColor c = p.color();
  QPixmap pix(16,16);
  pix.fill(c);
  rb_pen->changeItem(pix,i18n("Pen color..."),P_COL);
}

/*======================= event handler ============================*/
bool KPresenterView::event( const char* _event, const CORBA::Any& _value )
{
  EVENT_MAPPER( _event, _value );

  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_var, mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_var, mappingCreateToolbar );

  END_EVENT_MAPPER;
  
  return false;
}

/*======================= setup menu ============================*/
bool KPresenterView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
  if ( CORBA::is_nil( _menubar ) )
  {
    m_vMenuEdit = 0L;
    m_vMenuView = 0L;
    m_vMenuInsert = 0L;
    m_vMenuExtra = 0L;
    m_vMenuText = 0L;
    m_vMenuTools = 0L;
    m_vMenuScreen = 0L;
    m_vMenuHelp = 0L;
    return true;
  }

  // MENU Edit  
  _menubar->insertMenu( i18n( "&Edit" ), m_vMenuEdit, -1, -1 );

  QString tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/undo.xpm";
  OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap( tmp );
  m_idMenuEdit_Undo = m_vMenuEdit->insertItem6( pix, i18n("No Undo possible"), this,"editUndo", 0, -1, -1 );
  m_vMenuEdit->setItemEnabled(m_idMenuEdit_Undo,false);

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/redo.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuEdit_Redo = m_vMenuEdit->insertItem6( pix, i18n("No Redo possible"), this,"editRedo", 0, -1, -1 );
  m_vMenuEdit->setItemEnabled(m_idMenuEdit_Redo,false);
  m_vMenuEdit->insertSeparator( -1 );

  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editcut.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuEdit_Cut = m_vMenuEdit->insertItem6(pix, i18n("&Cut"), this,"editCut", 0, -1, -1 );

  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editcopy.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuEdit_Copy = m_vMenuEdit->insertItem6(pix, i18n("&Copy"), this,"editCopy", 0, -1, -1 );

  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editpaste.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuEdit_Paste = m_vMenuEdit->insertItem6(pix, i18n("&Paste"), this,"editPaste", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/delete.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuEdit_Delete = m_vMenuEdit->insertItem6(pix, i18n("&Delete"), this,"editDelete", 0, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  m_idMenuEdit_SelectAll = m_vMenuEdit->insertItem( i18n("&Select all"), this, "editSelectAll", 0 );

  m_vMenuEdit->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/delslide.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuEdit_DelPage = m_vMenuEdit->insertItem6(pix, i18n("&Delete Page..."), this, "editDelPage", 0, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  m_idMenuEdit_Find = m_vMenuEdit->insertItem( i18n("&Find..."), this,"editFind", 0 );

  m_idMenuEdit_FindReplace = m_vMenuEdit->insertItem( i18n("&Replace..."), this,"editFindReplace", 0 );

  // MENU View
  _menubar->insertMenu( i18n( "&View" ), m_vMenuView, -1, -1 );

  m_idMenuView_NewView = m_vMenuView->insertItem(i18n("&New View"), this,"newView", 0 );

  // MENU Insert
  _menubar->insertMenu( i18n( "&Insert" ), m_vMenuInsert, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/newslide.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuInsert_Page = m_vMenuInsert->insertItem6(pix, i18n("Pa&ge..."), this,"insertPage", 0, -1, -1 );
  m_vMenuInsert->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/picture.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuInsert_Picture = m_vMenuInsert->insertItem6(pix, i18n("&Picture..."), this,"insertPicture", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/clipart.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuInsert_Clipart = m_vMenuInsert->insertItem6(pix, i18n("&Clipart..."), this,"insertClipart", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/autoform.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuInsert_Autoform = m_vMenuInsert->insertItem6(pix, i18n("&Autoform..."), this,"insertAutoform", 0, -1, -1 );

  m_vMenuInsert->setCheckable(true);

  // MENU Tools
  _menubar->insertMenu( i18n( "&Tools" ), m_vMenuTools, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/mouse.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuTools_Mouse = m_vMenuTools->insertItem6(pix, i18n("&Mouse"), this,"toolsMouse", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/line.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuTools_Line = m_vMenuTools->insertItem6(pix, i18n("&Line"), this,"toolsLine", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/rectangle.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuTools_Rectangle = m_vMenuTools->insertItem6(pix, i18n("&Rectangle"), this,"toolsRectangle", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/circle.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuTools_Circle = m_vMenuTools->insertItem6(pix, i18n("C&ircle or Ellipse"), this,"toolsCircleOrEllipse", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/pie.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuTools_Pie = m_vMenuTools->insertItem6(pix, i18n("Pie/&Arc/Chord"), this,"toolsPie", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/text.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuTools_Text = m_vMenuTools->insertItem6(pix, i18n("&Text"), this,"toolsText", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/parts.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuTools_Part = m_vMenuTools->insertItem6(pix, i18n("&Object..."), this,"toolsObject", 0, -1, -1 );

  m_vMenuTools->setCheckable( true );

  // MENU Text
  _menubar->insertMenu( i18n( "T&ext" ), m_vMenuText, -1, -1 );

  m_idMenuText_TFont = m_vMenuText->insertItem(i18n("&Font..."), this, "mtextFont", 0 );

  m_idMenuText_TColor = m_vMenuText->insertItem(i18n("Text &Color..."), this,"textColor", 0 );

  m_vMenuText->insertItem8( i18n("Text &Alignment"), m_vMenuText_TAlign, -1, -1 );
  m_vMenuText_TAlign->setCheckable( true );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/alignLeft.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuText_TAlign_Left = m_vMenuText_TAlign->insertItem6(pix, i18n("Align &Left"), this,"mtextAlignLeft", 0, -1, -1 );
  // m_vMenuText_TAlign->setCheckable(m_idMenuText_TAlign_Left,true);
      
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/alignCenter.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuText_TAlign_Center = m_vMenuText_TAlign->insertItem6(pix, i18n("Align &Center"), this,"mtextAlignCenter", 0, -1, -1 );
  // m_vMenuText_TAlign->setCheckable(m_idMenuText_TAlign_Center,true);

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/alignRight.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuText_TAlign_Right = m_vMenuText_TAlign->insertItem6(pix, i18n("Align &Right"), this,"mtextAlignRight", 0, -1, -1 );
  // m_vMenuText_TAlign->setCheckable(m_idMenuText_TAlign_Right,true);

  m_vMenuText->insertItem8( i18n("Text &Type"), m_vMenuText_TType, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/enumList.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuText_TType_EnumList = m_vMenuText_TType->insertItem6(pix, i18n("&Enumerated List"), this,"textEnumList", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/unsortedList.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuText_TType_UnsortList = m_vMenuText_TType->insertItem6(pix, i18n("&Unsorted List"), this,"textUnsortList", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/normalText.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuText_TType_NormalText = m_vMenuText_TType->insertItem6(pix, i18n("&Normal Text"), this,"textNormalText", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/depth+.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuText_TDepthPlus = m_vMenuText->insertItem6(pix, i18n("&Increase Depth"), this,"textDepthPlus", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/depth-.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuText_TDepthMinus = m_vMenuText->insertItem6(pix, i18n("&Decrease Depth"), this,"textDepthMinus", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/spacing.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuText_TSpacing = m_vMenuText->insertItem6(pix, i18n("Paragraph &Spacing..."), this,"textSpacing", 0, -1, -1 );


  // MENU Extra
  _menubar->insertMenu( i18n( "&Extra" ), m_vMenuExtra, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/style.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_PenBrush = m_vMenuExtra->insertItem6(pix, i18n("&Pen and Brush..."), this,"extraPenBrush", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/edit_pie.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_Pie = m_vMenuExtra->insertItem6(pix, i18n("&Configure Pie/Arc/Chord..."), this,"extraConfigPie", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/rectangle2.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_Rect = m_vMenuExtra->insertItem6(pix, i18n("C&onfigure Rectangle..."), this,"extraConfigRect", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/raise.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_Raise = m_vMenuExtra->insertItem6(pix, i18n("&Raise object(s)"), this,"extraRaise", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/lower.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_Lower = m_vMenuExtra->insertItem6(pix, i18n("&Lower object(s)"), this,"extraLower", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/rotate.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_Rotate = m_vMenuExtra->insertItem6(pix, i18n("Rot&ate object(s)..."), this,"extraRotate", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/shadow.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_Shadow = m_vMenuExtra->insertItem6(pix, i18n("&Shadow object(s)..."), this,"extraShadow", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/alignobjs.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_vMenuExtra->insertItem12( pix, i18n("Text &Alignment"), m_vMenuExtra_AlignObj, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/aoleft.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_AlignObj_Left = m_vMenuExtra_AlignObj->insertItem6(pix, i18n("Align &Left"), this,"extraAlignObjLeftidl", 0, -1, -1 );

  m_vMenuExtra_AlignObj->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/aocenterh.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_AlignObj_CenterH = m_vMenuExtra_AlignObj->insertItem6(pix, i18n("Align Center (&horizontal)"), this,"extraAlignObjCenterHidl", 0, -1, -1);
  m_vMenuExtra_AlignObj->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/aoright.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_AlignObj_Right = m_vMenuExtra_AlignObj->insertItem6(pix, i18n("Align &Right"), this,"extraAlignObjRightidl", 0, -1, -1 );
  m_vMenuExtra_AlignObj->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/aotop.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_AlignObj_Top = m_vMenuExtra_AlignObj->insertItem6(pix, i18n("Align &Top"), this,"extraAlignObjTopidl", 0, -1, -1 );
  m_vMenuExtra_AlignObj->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/aocenterv.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_AlignObj_CenterV = m_vMenuExtra_AlignObj->insertItem6(pix, i18n("Align Center (&vertical)"), this,"extraAlignObjCenterVidl", 0, -1, -1 );
  m_vMenuExtra_AlignObj->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/aobottom.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuExtra_AlignObj_Bottom = m_vMenuExtra_AlignObj->insertItem6(pix, i18n("Align &Bottom"), this,"extraAlignObjBottomidl", 0, -1, -1);

  m_vMenuExtra->insertSeparator( -1 );

  m_idMenuExtra_Background = m_vMenuExtra->insertItem(i18n("Page &Background..."), this,"extraBackground", 0 );

  m_idMenuExtra_Layout = m_vMenuExtra->insertItem(i18n("Pa&ge Layout..."), this,"extraLayout", 0 );

  m_vMenuExtra->insertSeparator( -1 );
  
  m_idMenuExtra_Options = m_vMenuExtra->insertItem(i18n("&Options..."), this,"extraOptions", 0 );
      
  // MENU Screenpresentation
  _menubar->insertMenu( i18n( "&Screen Presentations" ), m_vMenuScreen, -1, -1 );

  m_idMenuScreen_ConfigPage = m_vMenuScreen->insertItem(i18n("&Configure pages..."), this,"screenConfigPages", 0 );
  m_idMenuScreen_PresStructView = m_vMenuScreen->insertItem(i18n("&Open presentation structure viewer..."), this,"screenPresStructView", 0 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/effect.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuScreen_AssignEffect = m_vMenuScreen->insertItem6(pix, i18n("&Assign effect..."), this,"screenAssignEffect", 0, -1, -1);
  m_vMenuScreen->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/start.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuScreen_Start = m_vMenuScreen->insertItem6(pix, i18n("&Start"), this,"screenStart", 0, -1, -1 );

  //       tmp = kapp->kde_datadir().copy();
  //       tmp += "/kpresenter/toolbar/stop.xpm";
  //       pix = OPUIUtils::loadPixmap(tmp);
  //       m_idMenuScreen_Stop = m_vMenuScreen->insertItem6(pix, 
  // 						    i18n("St&op"),m_idMenuScreen,
  // 						    this,"screenStop");
  //       tmp = kapp->kde_datadir().copy();
  //       tmp += "/kpresenter/toolbar/pause.xpm";
  //       pix = OPUIUtils::loadPixmap(tmp);
  //       m_idMenuScreen_Pause = m_vMenuScreen->insertItem6(pix, 
  // 						     i18n("Pa&use"),m_idMenuScreen,
  // 						     this,"screenPause");
  m_vMenuScreen->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/first.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuScreen_First = m_vMenuScreen->insertItem6(pix, i18n("&Go to start"), this,"screenFirst", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/prev.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuScreen_Prev = m_vMenuScreen->insertItem6(pix, i18n("&Previous step"), this,"screenPrev", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/next.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuScreen_Next = m_vMenuScreen->insertItem6(pix, i18n("&Next step"), this,"screenNext", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/last.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuScreen_Last = m_vMenuScreen->insertItem6(pix, i18n("&Go to end"), this,"screenLast", 0, -1, -1 );

//   m_idMenuScreen_Skip = m_vMenuScreen->insertItem(i18n("Goto &page"),m_idMenuScreen,
// 						   this,"screenSkip");

  m_vMenuScreen->insertSeparator( -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/pen.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_vMenuScreen->insertItem12( pix, i18n("&Choose Pen"), m_vMenuScreen_Pen, -1, -1 );
  m_vMenuScreen_Pen->setCheckable( true );

  m_vMenuScreen_Pen->insertItem8( i18n("Pen width") ,m_vMenuScreen_PenWidth, -1, -1 );
  m_vMenuScreen_PenWidth->setCheckable( true );
  
  m_idMenuScreen_PenColor = m_vMenuScreen_Pen->insertItem( i18n("Pen color"), this,"presPenColoridl", 0 );

  m_idMenuScreen_PenW1 = m_vMenuScreen_PenWidth->insertItem(i18n("1"), this,"presPen1idl", 0 );
  
  m_idMenuScreen_PenW2 = m_vMenuScreen_PenWidth->insertItem(i18n("2"), this,"presPen2idl", 0 );

  m_idMenuScreen_PenW3 = m_vMenuScreen_PenWidth->insertItem(i18n("3"), this,"presPen3idl", 0 );

  m_idMenuScreen_PenW4 = m_vMenuScreen_PenWidth->insertItem(i18n("4"), this,"presPen4idl", 0 );

  m_idMenuScreen_PenW5 = m_vMenuScreen_PenWidth->insertItem(i18n("5"), this,"presPen5idl", 0 );

  m_idMenuScreen_PenW6 = m_vMenuScreen_PenWidth->insertItem(i18n("6"), this,"presPen6idl", 0 );

  m_idMenuScreen_PenW7 = m_vMenuScreen_PenWidth->insertItem(i18n("7"), this,"presPen7idl", 0 );

  m_idMenuScreen_PenW8 = m_vMenuScreen_PenWidth->insertItem(i18n("8"), this,"presPen8idl", 0 );

  m_idMenuScreen_PenW9 = m_vMenuScreen_PenWidth->insertItem(i18n("9"), this,"presPen9idl", 0 );

  m_idMenuScreen_PenW10 = m_vMenuScreen_PenWidth->insertItem(i18n("10"), this,"presPen10idl", 0 );

  // MENU Help
  m_vMenuHelp = _menubar->helpMenu();
  if ( CORBA::is_nil( m_vMenuHelp ) )
  {
    _menubar->insertSeparator( -1 );
    _menubar->setHelpMenu( _menubar->insertMenu( i18n( "&Help" ), m_vMenuHelp, -1, -1 ) );
  }
    
  m_idMenuHelp_Contents = m_vMenuHelp->insertItem( i18n( "&Contents" ), this, "helpContents", 0 );

  // Torben: Reggie, Check/uncheck all checked menu items to their actual value in this function
  m_vMenuText_TAlign->setItemChecked(m_idMenuText_TAlign_Left,true);
  m_vMenuScreen_PenWidth->setItemChecked(m_idMenuScreen_PenW3,true);

//   setupAccelerators();

  return true;
}

/*======================== setup popup menus ===================*/
void KPresenterView::setupPopupMenus()
{
  QString pixdir;
  QPixmap pixmap;
  pixdir = KApplication::kde_datadir() + QString("/kpresenter/toolbar/");  

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
  rb_oalign->insertSeparator( -1 );
  pixmap.load(pixdir + "aocenterh.xpm");
  rb_oalign->insertItem(pixmap,this,SLOT(extraAlignObjCenterH()));
  rb_oalign->insertSeparator( -1 );
  pixmap.load(pixdir + "aoright.xpm");
  rb_oalign->insertItem(pixmap,this,SLOT(extraAlignObjRight()));
  rb_oalign->insertSeparator( -1 );
  pixmap.load(pixdir + "aotop.xpm");
  rb_oalign->insertItem(pixmap,this,SLOT(extraAlignObjTop()));
  rb_oalign->insertSeparator( -1 );
  pixmap.load(pixdir + "aocenterv.xpm");
  rb_oalign->insertItem(pixmap,this,SLOT(extraAlignObjCenterV()));
  rb_oalign->insertSeparator( -1 );
  pixmap.load(pixdir + "aobottom.xpm");
  rb_oalign->insertItem(pixmap,this,SLOT(extraAlignObjBottom()));
  rb_oalign->setMouseTracking(true);
  rb_oalign->setCheckable(false);

  // create right button line begin
  rb_lbegin = new QPopupMenu();
  CHECK_PTR(rb_lbegin);
  pixmap.load(pixdir + "line_normal_begin.xpm");
  rb_lbegin->insertItem(pixmap,this,SLOT(extraLineBeginNormal()));
  rb_lbegin->insertSeparator(-1);
  pixmap.load(pixdir + "line_arrow_begin.xpm");
  rb_lbegin->insertItem(pixmap,this,SLOT(extraLineBeginArrow()));
  rb_lbegin->insertSeparator(-1);
  pixmap.load(pixdir + "line_rect_begin.xpm");
  rb_lbegin->insertItem(pixmap,this,SLOT(extraLineBeginRect()));
  rb_lbegin->insertSeparator(-1);
  pixmap.load(pixdir + "line_circle_begin.xpm");
  rb_lbegin->insertItem(pixmap,this,SLOT(extraLineBeginCircle()));
  rb_lbegin->setMouseTracking(true);
  rb_lbegin->setCheckable(false);

  // create right button line end
  rb_lend = new QPopupMenu();
  CHECK_PTR(rb_lend);
  pixmap.load(pixdir + "line_normal_end.xpm");
  rb_lend->insertItem(pixmap,this,SLOT(extraLineEndNormal()));
  rb_lend->insertSeparator(-1);
  pixmap.load(pixdir + "line_arrow_end.xpm");
  rb_lend->insertItem(pixmap,this,SLOT(extraLineEndArrow()));
  rb_lend->insertSeparator(-1);
  pixmap.load(pixdir + "line_rect_end.xpm");
  rb_lend->insertItem(pixmap,this,SLOT(extraLineEndRect()));
  rb_lend->insertSeparator(-1);
  pixmap.load(pixdir + "line_circle_end.xpm");
  rb_lend->insertItem(pixmap,this,SLOT(extraLineEndCircle()));
  rb_lend->setMouseTracking(true);
  rb_lend->setCheckable(false);
}

/*======================= setup toolbar ===================*/
bool KPresenterView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  if ( CORBA::is_nil( _factory ) )
  {
    m_vToolBarEdit = 0L;
    m_vToolBarInsert = 0L;
    m_vToolBarText = 0L;
    m_vToolBarExtra = 0L;
    m_vToolBarScreen = 0L;
    return true;
  }

  cerr << "bool KPresenterView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" << endl;
  
  ///////
  // Edit
  ///////
  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarEdit->setFullWidth(false);

  // undo
  QString tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/undo.xpm";
  OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonEdit_Undo = m_vToolBarEdit->insertButton2( pix, ID_UNDO, SIGNAL( clicked() ), this, "editUndo", true, i18n("Undo"), -1 );
  m_vToolBarEdit->setItemEnabled(ID_UNDO,false);
  
  // redo
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/redo.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonEdit_Redo = m_vToolBarEdit->insertButton2( pix, ID_REDO, SIGNAL( clicked() ), this, "editRedo", true, i18n("Redo"), -1 );
  m_vToolBarEdit->setItemEnabled(ID_REDO,false);

  m_vToolBarEdit->insertSeparator( -1 );

  // cut
  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editcut.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonEdit_Cut = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editCut", true, i18n("Cut"), -1 );

  // copy
  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editcopy.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonEdit_Copy = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editCopy", true, i18n("Copy"), -1 );

  // paste
  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editpaste.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonEdit_Paste = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editPaste", true, i18n("Paste"), -1 );

  m_vToolBarEdit->insertSeparator( -1 );
  
  // delete
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/delete.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonEdit_Delete = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editDelete", true, i18n("Delete"), -1 );

  m_vToolBarEdit->enable( OpenPartsUI::Show );

  ///////
  // Insert
  //////
  m_vToolBarInsert = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarInsert->setFullWidth(false);
 
  // page
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/newslide.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonInsert_Page = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertPage", true, i18n("Insert Page"), -1 );
  m_vToolBarInsert->insertSeparator( -1 );

  // picture
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/picture.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonInsert_Picture = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertPicture", true, i18n("Insert Picture"), -1 );
      
  // clipart
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/clipart.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonInsert_Clipart = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertClipart", true, i18n("Insert Clipart"), -1 );
  
  // autoform
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/autoform.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonInsert_Autoform = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertAutoform", true, i18n("Insert Autoform"), -1 );
  
  m_vToolBarInsert->enable( OpenPartsUI::Show );

  ///////
  // Tools
  //////
  m_vToolBarTools = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarTools->setFullWidth(false);

  // mouse
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/mouse.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonTools_Mouse = m_vToolBarTools->insertButton2( pix, ID_TOOL_MOUSE, SIGNAL( clicked() ), this, "toolsMouse", 
							   true, i18n("Mouse Tool"), -1 );

  m_vToolBarTools->setToggle(ID_TOOL_MOUSE,true);
  m_vToolBarTools->setButton(ID_TOOL_MOUSE,true);

  // line
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/line.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonTools_Line = m_vToolBarTools->insertButton2( pix, ID_TOOL_LINE, SIGNAL( clicked() ), this, "toolsLine", 
							   true, i18n("Create Line"), -1 );

  m_vToolBarTools->setToggle(ID_TOOL_LINE,true);
  m_vToolBarTools->setButton(ID_TOOL_LINE,false);

  // rectangle
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/rectangle.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonTools_Rectangle = m_vToolBarTools->insertButton2( pix, ID_TOOL_RECT, SIGNAL( clicked() ), this, "toolsRectangle", 
								true, i18n("Create Rectangle"), -1 );
  m_vToolBarTools->setToggle(ID_TOOL_RECT,true);
  m_vToolBarTools->setButton(ID_TOOL_RECT,false);

  // circle or ellipse
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/circle.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonTools_Circle = m_vToolBarTools->insertButton2( pix, ID_TOOL_ELLIPSE, SIGNAL( clicked() ), this, "toolsCircleOrEllipse", 
							     true, i18n("Create Circle or Ellipse"), -1 );
  m_vToolBarTools->setToggle(ID_TOOL_ELLIPSE,true);
  m_vToolBarTools->setButton(ID_TOOL_ELLIPSE,false);

  // circle or ellipse
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/pie.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonTools_Pie = m_vToolBarTools->insertButton2( pix, ID_TOOL_PIE, SIGNAL( clicked() ), this, "toolsPie", 
							  true, i18n("Create Pie/Arc/Chord"), -1 );
  m_vToolBarTools->setToggle(ID_TOOL_PIE,true);
  m_vToolBarTools->setButton(ID_TOOL_PIE,false);

  // text
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/text.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonTools_Text = m_vToolBarTools->insertButton2( pix, ID_TOOL_TEXT, SIGNAL( clicked() ), this, "toolsText", 
							   true, i18n("Create Text"), -1 );
  m_vToolBarTools->setToggle(ID_TOOL_TEXT,true);
  m_vToolBarTools->setButton(ID_TOOL_TEXT,false);

  // parts
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/parts.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonTools_Part = m_vToolBarTools->insertButton2( pix, ID_TOOL_OBJECT, SIGNAL( clicked() ), this, "toolsObject", 
							   true, i18n("Create Object"), -1 );
  m_vToolBarTools->setToggle(ID_TOOL_OBJECT,true);
  m_vToolBarTools->setButton(ID_TOOL_OBJECT,false);

  m_vToolBarTools->enable( OpenPartsUI::Show );

  /////////
  // Text
  /////////
  m_vToolBarText = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarText->setFullWidth(false);

  // size combobox
  OpenPartsUI::StrList sizelist;
  sizelist.length( 97 );
  for( int i = 4; i <= 100 ; i++ )
  {
    char buffer[ 10 ];
    sprintf( buffer, "%i", i );
    sizelist[i-4] = CORBA::string_dup( buffer );
  }
  m_idComboText_FontSize = m_vToolBarText->insertCombo( sizelist, ID_FONT_SIZE, true, SIGNAL( activated( const char* ) ),
							this, "sizeSelected", true,
							i18n( "Font Size"  ), 50, -1, OpenPartsUI::AtBottom );
  m_vToolBarText->setCurrentComboItem(ID_FONT_SIZE,16);
  tbFont.setPointSize(20);

  // fonts combobox
  getFonts();
  OpenPartsUI::StrList fonts;
  fonts.length( fontList.count() );
  for(unsigned int i = 0;i < fontList.count(); i++ )
    fonts[i] = CORBA::string_dup( fontList.at(i) );
  m_idComboText_FontList = m_vToolBarText->insertCombo( fonts, ID_FONT_LIST, true, SIGNAL( activated( const char* ) ), this,
							"fontSelected", true, i18n("Font List"),
							200, -1, OpenPartsUI::AtBottom );
  tbFont.setFamily(fontList.at(0));
  m_vToolBarText->setCurrentComboItem(ID_FONT_LIST,0);

  m_vToolBarText->insertSeparator( -1 );

  // bold
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/bold.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_Bold = m_vToolBarText->insertButton2( pix, ID_BOLD, SIGNAL( clicked() ), this, "textBold", true, i18n("Bold"), -1 );
  m_vToolBarText->setToggle(ID_BOLD,true);
  m_vToolBarText->setButton(ID_BOLD,false);
  tbFont.setBold(false);

  // italic
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/italic.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_Italic = m_vToolBarText->insertButton2( pix, ID_ITALIC, SIGNAL( clicked() ), this, "textItalic", true, i18n("Italic"), -1 );
  m_vToolBarText->setToggle(ID_ITALIC,true);
  m_vToolBarText->setButton(ID_ITALIC,false);
  tbFont.setItalic(false);

  // underline
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/underl.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_Underline = m_vToolBarText->insertButton2( pix, ID_UNDERLINE, SIGNAL( clicked() ), this, "textUnderline", 
							    true, i18n("Underline"), -1 );
  m_vToolBarText->setToggle(ID_UNDERLINE,true);
  m_vToolBarText->setButton(ID_UNDERLINE,false);
  tbFont.setUnderline(false);

  // color
  OpenPartsUI::Pixmap* p = new OpenPartsUI::Pixmap;
  p->data = CORBA::string_dup( colorToPixString(black) );
  pix = p;
  m_idButtonText_Color = m_vToolBarText->insertButton2( pix, ID_TEXT_COLOR, SIGNAL( clicked() ), this, "textColor", true, i18n("Color"), -1 );
  tbColor = black;

  m_vToolBarText->insertSeparator( -1 );
  
  // align left
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/alignLeft.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_ALeft = m_vToolBarText->insertButton2( pix, ID_ALEFT, SIGNAL( clicked() ), this, "textAlignLeft", 
							true, i18n("Align Left"), -1 );
  m_vToolBarText->setToggle(ID_ALEFT,true);
  m_vToolBarText->setButton(ID_ALEFT,true);
  tbAlign = TxtParagraph::LEFT;
  
  // align center
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/alignCenter.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_ACenter = m_vToolBarText->insertButton2( pix, ID_ACENTER, SIGNAL( clicked() ), this, "textAlignCenter", 
							  true, i18n("Align Center"), -1 );
  m_vToolBarText->setToggle(ID_ACENTER,true);
  m_vToolBarText->setButton(ID_ACENTER,false);
  
  // align right
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/alignRight.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_ARight = m_vToolBarText->insertButton2( pix, ID_ARIGHT, SIGNAL( clicked() ), this, "textAlignRight", 
							 true, i18n("Align Right"), -1 );
  m_vToolBarText->setToggle(ID_ARIGHT,true);
  m_vToolBarText->setButton(ID_ARIGHT,false);

  m_vToolBarText->insertSeparator( -1 );
  
  // enum list
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/enumList.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_EnumList = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textEnumList", 
							   true, i18n("Enumerated List"), -1 );
  
  // unsorted list
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/unsortedList.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_UnsortList = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textUnsortList", true, i18n("Unsorted List"), -1 );
  
  // normal text
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/normalText.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_NormalText = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textNormalText", true, i18n("Normal Text"), -1 );

  m_vToolBarText->enable( OpenPartsUI::Show );

  // depth plus
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/depth+.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_DepthPlus = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textDepthPlus", true, i18n("Increase Depth"), -1 );

  // depth minus
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/depth-.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_DepthPlus = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textDepthMinus", true, i18n("Decrease Depth"), -1 );

  // spacing
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/spacing.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_Spacing = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textSpacing", true, i18n("Paragraph Spacing"), -1 );

  m_vToolBarText->enable( OpenPartsUI::Show );

  /////////
  // Extra
  /////////
  m_vToolBarExtra = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarExtra->setFullWidth(false);

  // pen and brush
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/style.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonExtra_Style = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraPenBrush", true, i18n("Pen & Brush"), -1 );
  m_vToolBarExtra->insertSeparator( -1 );

  // pie
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/edit_pie.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonExtra_Pie = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraConfigPie", true, i18n("Configure Pie"), -1 );

  // rect
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/rectangle2.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonExtra_Rect = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraConfigRect", true, 
							 i18n("Configure Rectangle"), -1 );
  m_vToolBarExtra->insertSeparator( -1 );

  // raise
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/raise.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonExtra_Raise = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraRaise", true, i18n("Raise object"), -1 );
  
  // lower
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/lower.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonExtra_Lower = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraLower", true, i18n("Lower object"), -1 );
  m_vToolBarExtra->insertSeparator( -1 );
  
  // rotate
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/rotate.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonExtra_Rotate = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraRotate", true, i18n("Rotate object"), -1 );
  
  // shadow
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/shadow.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonExtra_Shadow = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraShadow", true, i18n("Shadow object"), -1 );
  m_vToolBarExtra->insertSeparator( -1 );
  
  // align
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/alignobjs.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonExtra_Align = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraAlignObj", true, i18n("Align object"), -1 );
  m_vToolBarExtra->insertSeparator( -1 );

  // line begin
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/line_begin.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonExtra_LineBegin = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraLineBegin", 
							      true, i18n("Line Begin"), -1 );
  // line end
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/line_end.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonExtra_LineEnd = m_vToolBarExtra->insertButton2( pix, 1, SIGNAL( clicked() ), this, "extraLineEnd", 
							      true, i18n("Line End"), -1 );

  m_vToolBarExtra->enable( OpenPartsUI::Show );

  /////////
  // Screen
  /////////
  m_vToolBarScreen = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarScreen->setFullWidth(false);

  // start
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/start.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonScreen_Start = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenStart", true, i18n("Start"), -1 );
  m_vToolBarScreen->insertSeparator( -1 );
  
  // first
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/first.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonScreen_First = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenFirst", true, i18n("First"), -1 );

  // previous
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/prev.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonScreen_Prev = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenPrev", true, i18n("Previous"), -1 );

  // next
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/next.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonScreen_Next = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenNext", true, i18n("Next"), -1 );

  // last
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/last.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonScreen_Last = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenLast", true, i18n("Last"), -1 );
  m_vToolBarScreen->insertSeparator( -1 );
  
  // effect
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/effect.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonScreen_Effect = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenAssignEffect", true, i18n("Assign Effect"), -1 );
  m_vToolBarScreen->insertSeparator( -1 );

  // pen
  tmp = kapp->kde_datadir().copy();
  tmp += "/kpresenter/toolbar/pen.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonScreen_Pen = m_vToolBarScreen->insertButton2( pix, 1, SIGNAL( clicked() ), this, "screenPen", true, i18n("choose Pen"), -1 );

  m_vToolBarScreen->enable( OpenPartsUI::Show );

  setTool(TEM_MOUSE);

  return true;
}

/*======================= setup scrollbars =====================*/
void KPresenterView::setupScrollbars()
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
void KPresenterView::setupAccelerators()
{
//   // edit menu
//   m_vMenuEdit->setAccel(CTRL + Key_Z,m_idMenuEdit_Undo);
//   m_vMenuEdit->setAccel(CTRL + Key_X,m_idMenuEdit_Cut);
//   m_vMenuEdit->setAccel(CTRL + Key_C,m_idMenuEdit_Copy);
//   m_vMenuEdit->setAccel(CTRL + Key_V,m_idMenuEdit_Paste);
//   m_vMenuEdit->setAccel(CTRL + Key_F,m_idMenuEdit_Find);
//   m_vMenuEdit->setAccel(CTRL + Key_R,m_idMenuEdit_FindReplace);

//   // insert menu
//   m_vMenuInsert->setAccel(ALT + Key_N,m_idMenuInsert_Page);
//   m_vMenuInsert->setAccel(Key_F1,m_idMenuInsert_Picture);
//   m_vMenuInsert->setAccel(Key_F2,m_idMenuInsert_Clipart);
//   m_vMenuInsert->setAccel(Key_F11,m_idMenuInsert_Autoform);

//   // tools menu
//   m_vMenuInsert->setAccel(Key_F3,m_idMenuTools_Line);
//   m_vMenuInsert->setAccel(Key_F4,m_idMenuTools_LineVert);
//   m_vMenuInsert->setAccel(Key_F5,m_idMenuTools_LineD1);
//   m_vMenuInsert->setAccel(Key_F6,m_idMenuTools_LineD2);
//   m_vMenuInsert->setAccel(Key_F7,m_idMenuTools_RectangleNormal);
//   m_vMenuInsert->setAccel(Key_F8,m_idMenuTools_RectangleRound);
//   m_vMenuInsert->setAccel(Key_F9,m_idMenuTools_Circle);
//   m_vMenuInsert->setAccel(Key_F10,m_idMenuTools_Text);
//   m_vMenuInsert->setAccel(Key_F12,m_idMenuTools_Part);

//   // extra menu
//   m_vMenuExtra->setAccel(CTRL + Key_P,m_idMenuExtra_PenBrush);
//   m_vMenuExtra->setAccel(CTRL + Key_Minus,m_idMenuExtra_Lower);
//   m_vMenuExtra->setAccel(CTRL + Key_Plus,m_idMenuExtra_Raise);
//   m_vMenuExtra->setAccel(ALT + Key_R,m_idMenuExtra_Rotate);
//   m_vMenuExtra->setAccel(ALT + Key_S,m_idMenuExtra_Shadow);
 
//   // screen menu
//   m_vMenuScreen->setAccel(ALT + Key_A,m_idMenuScreen_AssignEffect);
//   m_vMenuScreen->setAccel(CTRL + Key_G,m_idMenuScreen_Start);
//   m_vMenuScreen->setAccel(Key_Home,m_idMenuScreen_First);
//   m_vMenuScreen->setAccel(Key_End,m_idMenuScreen_Last);
//   m_vMenuScreen->setAccel(Key_Prior,m_idMenuScreen_Prev);
//   m_vMenuScreen->setAccel(Key_Next,m_idMenuScreen_Next);

//   // help menu
//   m_vMenuHelp->setAccel(CTRL + Key_H,m_idMenuHelp_Contents);
}

/*==============================================================*/
void KPresenterView::setupRulers()
{
  h_ruler = new KoRuler(this,page,KoRuler::HORIZONTAL,kPresenterDoc()->pageLayout(),0);
  v_ruler = new KoRuler(this,page,KoRuler::VERTICAL,kPresenterDoc()->pageLayout(),0);
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
void KPresenterView::setRanges()
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
void KPresenterView::skipToPage(int _num)
{
  vert->setValue(kPresenterDoc()->getPageSize(_num,0,0,1.0,false).y()); 
}

/*==============================================================*/
void KPresenterView::makeRectVisible(KRect _rect)
{
  horz->setValue(_rect.x()); 
  vert->setValue(_rect.y()); 
}

/*==============================================================*/
void KPresenterView::restartPresStructView()
{
  QObject::disconnect(presStructView,SIGNAL(presStructViewClosed()),this,SLOT(psvClosed()));
  presStructView->close();
  delete presStructView;
  presStructView = 0;
  page->deSelectAllObj();
      
  presStructView = new PresStructViewer(0,"",kPresenterDoc(),this);
  presStructView->setCaption(i18n("KPresenter - Presentation structure viewer"));
  QObject::connect(presStructView,SIGNAL(presStructViewClosed()),this,SLOT(psvClosed()));
  presStructView->show();
}

/*==============================================================*/
void KPresenterView::setTool(ToolEditMode toolEditMode)
{
  m_vToolBarTools->setButton(ID_TOOL_MOUSE,false);
  m_vToolBarTools->setButton(ID_TOOL_LINE,false);
  m_vToolBarTools->setButton(ID_TOOL_RECT,false);
  m_vToolBarTools->setButton(ID_TOOL_ELLIPSE,false);
  m_vToolBarTools->setButton(ID_TOOL_PIE,false);
  m_vToolBarTools->setButton(ID_TOOL_TEXT,false);
  m_vToolBarTools->setButton(ID_TOOL_OBJECT,false);

  m_vMenuTools->setItemChecked(m_idMenuTools_Mouse,false);
  m_vMenuTools->setItemChecked(m_idMenuTools_Line,false);
  m_vMenuTools->setItemChecked(m_idMenuTools_Rectangle,false);
  m_vMenuTools->setItemChecked(m_idMenuTools_Circle,false);
  m_vMenuTools->setItemChecked(m_idMenuTools_Pie,false);
  m_vMenuTools->setItemChecked(m_idMenuTools_Text,false);
  m_vMenuTools->setItemChecked(m_idMenuTools_Part,false);

  switch (toolEditMode)
    {
    case TEM_MOUSE:
      {
	m_vMenuTools->setItemChecked(m_idMenuTools_Mouse,true);
	m_vToolBarTools->setButton(ID_TOOL_MOUSE,true);
      } break;
    case INS_LINE: 
      {
	m_vMenuTools->setItemChecked(m_idMenuTools_Line,true);
	m_vToolBarTools->setButton(ID_TOOL_LINE,true);
      } break;
    case INS_RECT: 
      {
	m_vMenuTools->setItemChecked(m_idMenuTools_Rectangle,true);
	m_vToolBarTools->setButton(ID_TOOL_RECT,true);
      } break;
    case INS_ELLIPSE:
      {
	m_vMenuTools->setItemChecked(m_idMenuTools_Circle,true);
	m_vToolBarTools->setButton(ID_TOOL_ELLIPSE,true);
      } break;
    case INS_PIE:
      {      
	m_vMenuTools->setItemChecked(m_idMenuTools_Pie,true);
	m_vToolBarTools->setButton(ID_TOOL_PIE,true);
      } break;
    case INS_OBJECT:
      {
	m_vMenuTools->setItemChecked(m_idMenuTools_Part,true);
	m_vToolBarTools->setButton(ID_TOOL_OBJECT,true);
      } break;
    case INS_TEXT:
      {
	m_vMenuTools->setItemChecked(m_idMenuTools_Text,true);
	m_vToolBarTools->setButton(ID_TOOL_TEXT,true);
      } break;
    }
}

/*============== create a pixmapstring from a color ============*/
char* KPresenterView::colorToPixString(QColor c)
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
void KPresenterView::getFonts()
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
