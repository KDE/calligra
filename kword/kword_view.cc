/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: View                                                   */
/******************************************************************/

#include <qprinter.h>
#include <kfiledialog.h>

#include "kword_view.h"
#include "kword_doc.h"
#include "kword_main.h"
#include "kword_view.moc"

#include <qpainter.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qkeycode.h>

#include <koPartSelectDia.h>
#include <opUIUtils.h>

#include <kapp.h>

#define DEBUG

// Qt bug
char *getenv(const char *name);    

/******************************************************************/
/* Class: KWordFrame                                              */
/******************************************************************/
KWordFrame::KWordFrame( KWordView* _view,KWordChild* _child ) 
  : KoFrame( _view )
{
  m_pKWordView = _view;
  m_pKWordChild = _child;
}

/******************************************************************/
/* Class: KWordView                                               */
/******************************************************************/

/*================================================================*/
KWordView::KWordView( QWidget *_parent, const char *_name, KWordDocument* _doc )
  : QWidget( _parent, _name ), KoViewIf( _doc ), OPViewIf( _doc ), KWord::KWordView_skel(), format( _doc )
{
  setWidget(this);

  KoViewIf::setFocusPolicy( OpenParts::Part::ClickFocus);
 
  m_pKWordDoc = 0L;
  m_bUnderConstruction = true;
  m_bShowGUI = true;
  m_vMenuTools = 0L;
  m_vToolBarTools = 0L;
  m_lstFrames.setAutoDelete(true);  
  gui = 0;
  flow = KWParagLayout::LEFT;
  paragDia = 0;
  styleManager = 0;
  vertAlign = KWFormat::VA_NORMAL;
  left.color = white;
  left.style = KWParagLayout::SOLID;
  left.ptWidth = 0;
  right.color = white;
  right.style = KWParagLayout::SOLID;
  right.ptWidth = 0;
  top.color = white;
  top.style = KWParagLayout::SOLID;
  top.ptWidth = 0;
  bottom.color = white;
  bottom.style = KWParagLayout::SOLID;
  bottom.ptWidth = 0;
  tmpBrd.color = white;
  tmpBrd.style = KWParagLayout::SOLID;
  tmpBrd.ptWidth = 0;

  m_pKWordDoc = _doc;

  QObject::connect(m_pKWordDoc,SIGNAL(sig_insertObject(KWordChild*)),
		   this,SLOT(slotInsertObject(KWordChild*)));
  QObject::connect(m_pKWordDoc,SIGNAL(sig_updateChildGeometry(KWordChild*)),
		   this,SLOT(slotUpdateChildGeometry(KWordChild*)));
}

/*================================================================*/
void KWordView::init()
{
  /******************************************************
   * Menu
   ******************************************************/

  cerr << "Registering menu as " << id() << endl;
  
  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->registerClient( id(), this );
  else
    cerr << "Did not get a menu bar manager" << endl;

  /******************************************************
   * Toolbar
   ******************************************************/

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->registerClient( id(), this );
  else
    cerr << "Did not get a tool bar manager" << endl;  

  // Create GUI
  gui = new KWordGUI(this,m_bShowGUI,m_pKWordDoc,this);
  gui->setGeometry(0,0,width(),height());
  gui->show();

  gui->getPaperWidget()->formatChanged(format);
  widget()->setFocusProxy(gui);

  setFormat(format,false);

  if (gui)
    gui->setDocument(m_pKWordDoc);

  format.setDefaults(m_pKWordDoc);
  if (gui)
    gui->getPaperWidget()->formatChanged(format);
}

/*================================================================*/
KWordView::~KWordView()
{
  cerr << "KWordView::~KWordView()" << endl;
  cleanUp();
  cerr << "...KWordView::~KWordView()" << endl;
}

/*================================================================*/
void KWordView::cleanUp()
{
  cerr << "void KWordView::cleanUp()" << endl;
 
  if ( m_bIsClean )
    return;
  
  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->unregisterClient( id() );

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->unregisterClient( id() );

  m_pKWordDoc->removeView(this);
  
  m_lstFrames.clear();

  KoViewIf::cleanUp();

  cerr << "... void KWordView::cleanUp()" << endl;
}
    
/*=========================== file print =======================*/
CORBA::Boolean KWordView::printDlg()
{
  QPrinter prt;
  prt.setMinMax(1,m_pKWordDoc->getPages());
  bool makeLandscape = false;

  KoPageLayout pgLayout;
  KoColumns cl;
  m_pKWordDoc->getPageLayout(pgLayout,cl);

  switch (pgLayout.format)
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

  switch (pgLayout.orientation)
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
      QPainter painter;
      painter.begin(&prt);
      m_pKWordDoc->print(&painter,&prt,left_margin,top_margin);
      painter.end();
    }
  return true;
}

/*================================================================*/
void KWordView::setFormat( KWFormat &_format, bool _check = true, bool _update_page = true )
{
  if (_check && format == _format) return;

  format = _format;

  if (_format.getUserFont()->getFontName())
    {
      fontList.find(_format.getUserFont()->getFontName());
      if ( !CORBA::is_nil( m_vToolBarText ) )
	m_vToolBarText->setCurrentComboItem(ID_FONT_LIST,fontList.at());
    }

  if (_format.getPTFontSize() != -1)
      if ( !CORBA::is_nil( m_vToolBarText ) )
	m_vToolBarText->setCurrentComboItem(ID_FONT_SIZE,_format.getPTFontSize() - 4);
  
  if (_format.getWeight() != -1)
    {
      if ( !CORBA::is_nil( m_vToolBarText ) )
	m_vToolBarText->setButton(ID_BOLD,_format.getWeight() == QFont::Bold);
      tbFont.setBold(_format.getWeight() == QFont::Bold);
    }
  if (_format.getItalic() != -1)
    {
      if ( !CORBA::is_nil( m_vToolBarText ) )
	m_vToolBarText->setButton(ID_ITALIC,_format.getItalic() == 1);
      tbFont.setItalic(_format.getItalic() == 1);
    }
  if (_format.getUnderline() != -1)
    {
      if ( !CORBA::is_nil( m_vToolBarText ) )
	m_vToolBarText->setButton(ID_UNDERLINE,_format.getUnderline() == 1);
      tbFont.setUnderline(_format.getUnderline() == 1);
    }

  if (_format.getColor().isValid())
    {
      if ( !CORBA::is_nil( m_vToolBarText ) )
	{  
	  OpenPartsUI::Pixmap pix;
	  pix.data = CORBA::string_dup( colorToPixString(_format.getColor() ) );
	  
	  m_vToolBarText->setButtonPixmap(ID_TEXT_COLOR, pix );
	}
      tbColor = QColor(_format.getColor());
    }

  if ( !CORBA::is_nil( m_vToolBarText ) )
  {
    m_vToolBarText->setButton(ID_SUPERSCRIPT,false);  
    m_vToolBarText->setButton(ID_SUBSCRIPT,false);  
  }
  
  if (_format.getVertAlign() == KWFormat::VA_NORMAL)
    vertAlign = KWFormat::VA_NORMAL;
  else if (_format.getVertAlign() == KWFormat::VA_SUB)
    {
      vertAlign = KWFormat::VA_SUB;
      if ( !CORBA::is_nil( m_vToolBarText ) )
	m_vToolBarText->setButton(ID_SUBSCRIPT,true);  
   }
  else if (_format.getVertAlign() == KWFormat::VA_SUPER)
    {
      vertAlign = KWFormat::VA_SUPER;
      if ( !CORBA::is_nil( m_vToolBarText ) )
	m_vToolBarText->setButton(ID_SUPERSCRIPT,true);  
   }

  format = _format;
  
  if (_update_page)
    gui->getPaperWidget()->formatChanged(format);
}

/*================================================================*/
void KWordView::setFlow(KWParagLayout::Flow _flow)
{
  if (_flow != flow)
    {
      flow = _flow;
      m_vToolBarText->setButton(ID_ALEFT,false);  
      m_vToolBarText->setButton(ID_ACENTER,false);  
      m_vToolBarText->setButton(ID_ARIGHT,false);  
      m_vToolBarText->setButton(ID_ABLOCK,false);  
      
      switch (flow)
	{
	case KWParagLayout::LEFT:
	  m_vToolBarText->setButton(ID_ALEFT,true);  
	  break;
	case KWParagLayout::CENTER:
	  m_vToolBarText->setButton(ID_ACENTER,true);  
	  break;
	case KWParagLayout::RIGHT:
	  m_vToolBarText->setButton(ID_ARIGHT,true);  
	  break;
	case KWParagLayout::BLOCK:
	  m_vToolBarText->setButton(ID_ABLOCK,true);  
	  break;
	}
    }
}

/*================================================================*/
void KWordView::setParagBorders(KWParagLayout::Border _left,KWParagLayout::Border _right,
				     KWParagLayout::Border _top,KWParagLayout::Border _bottom)
{
  if (left != _left || right != _right || top != _top || bottom != _bottom)
    {
      m_vToolBarText->setButton(ID_BRD_LEFT,false);  
      m_vToolBarText->setButton(ID_BRD_RIGHT,false);  
      m_vToolBarText->setButton(ID_BRD_TOP,false);  
      m_vToolBarText->setButton(ID_BRD_BOTTOM,false);  
      
      left = _left;
      right = _right;
      top = _top;
      bottom = _bottom;

      if (left.ptWidth > 0)
	{
	  m_vToolBarText->setButton(ID_BRD_LEFT,true);  
	  tmpBrd = left;
	  setParagBorderValues();
	}
      if (right.ptWidth > 0)
	{
	  m_vToolBarText->setButton(ID_BRD_RIGHT,true);  
	  tmpBrd = right;
	  setParagBorderValues();
	}
      if (top.ptWidth > 0)
	{
	  m_vToolBarText->setButton(ID_BRD_TOP,true);  
	  tmpBrd = top;
	  setParagBorderValues();
	}
      if (bottom.ptWidth > 0)
	{
	  m_vToolBarText->setButton(ID_BRD_BOTTOM,true);  
	  tmpBrd = bottom;
	  setParagBorderValues();
	}
    }
}

bool KWordView::event( const char* _event, const CORBA::Any& _value )
{
  EVENT_MAPPER( _event, _value );

  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_var, mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_var, mappingCreateToolbar );

  END_EVENT_MAPPER;
  
  return false;
}

/*===============================================================*/
void KWordView::uncheckAllTools()
{
  if (m_vMenuTools)
    {
      m_vMenuTools->setItemChecked(m_idMenuTools_Edit,false);
      m_vMenuTools->setItemChecked(m_idMenuTools_EditFrame,false);
      m_vMenuTools->setItemChecked(m_idMenuTools_CreateText,false);
      m_vMenuTools->setItemChecked(m_idMenuTools_CreatePix,false);
    }

  if (m_vToolBarTools)
    {
      m_vToolBarTools->setButton(ID_TOOL_EDIT,false);
      m_vToolBarTools->setButton(ID_TOOL_EDIT_FRAME,false);
      m_vToolBarTools->setButton(ID_TOOL_CREATE_TEXT,false);
      m_vToolBarTools->setButton(ID_TOOL_CREATE_PIX,false);
    }
}

/*===============================================================*/
void KWordView::setTool(MouseMode _mouseMode)
{
  if (m_vMenuTools)
    {
      switch (_mouseMode)
	{
	case MM_EDIT: 
	  m_vMenuTools->setItemChecked(m_idMenuTools_Edit,true);
	  break;
	case MM_EDIT_FRAME: 
	  m_vMenuTools->setItemChecked(m_idMenuTools_EditFrame,true);
	  break;
	case MM_CREATE_TEXT:
	  m_vMenuTools->setItemChecked(m_idMenuTools_CreateText,true);
	  break;
	case MM_CREATE_PIX:
	  m_vMenuTools->setItemChecked(m_idMenuTools_CreatePix,true);
	  break;
	}
    }

  if (m_vToolBarTools)
    {
      switch (_mouseMode)
	{
	case MM_EDIT: 
	  m_vToolBarTools->setButton(ID_TOOL_EDIT,true);
	  break;
	case MM_EDIT_FRAME: 
	  m_vToolBarTools->setButton(ID_TOOL_EDIT_FRAME,true);
	  break;
	case MM_CREATE_TEXT:
	  m_vToolBarTools->setButton(ID_TOOL_CREATE_TEXT,true);
	  break;
	case MM_CREATE_PIX:
	  m_vToolBarTools->setButton(ID_TOOL_CREATE_PIX,true);
	  break;
	}
    }
}

/*===============================================================*/
void KWordView::editUndo()
{
}

/*===============================================================*/
void KWordView::editRedo()
{
}

/*===============================================================*/
void KWordView::editCut()
{
  gui->getPaperWidget()->editCut();
}

/*===============================================================*/
void KWordView::editCopy()
{
  gui->getPaperWidget()->editCopy();
}

/*===============================================================*/
void KWordView::editPaste()
{
  QClipboard *cb = QApplication::clipboard();
  if (cb->text())
    gui->getPaperWidget()->editPaste(cb->text());
}

/*===============================================================*/
void KWordView::editSelectAll()
{
}

/*===============================================================*/
void KWordView::editFind()
{
}

/*===============================================================*/
void KWordView::editFindReplace()
{
}

/*================================================================*/
void KWordView::newView()
{
  assert((m_pKWordDoc != 0L));

  KWordShell* shell = new KWordShell;
  shell->show();
  shell->setDocument( m_pKWordDoc );
}

/*===============================================================*/
void KWordView::insertPicture()
{
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

  if (!file.isEmpty()) m_pKWordDoc->insertPicture(file,gui->getPaperWidget());
}

/*===============================================================*/
void KWordView::insertTable()
{
}

/*===============================================================*/
void KWordView::insertClipart()
{
}

/*===============================================================*/
void KWordView::insertPart()
{
}

/*===============================================================*/
void KWordView::formatFont()
{
}

/*===============================================================*/
void KWordView::formatColor()
{
}

/*===============================================================*/
void KWordView::formatParagraph()
{
  if (paragDia)
    {
      QObject::disconnect(paragDia,SIGNAL(applyButtonPressed()),this,SLOT(paragDiaOk()));
      paragDia->close();
      delete paragDia;
      paragDia = 0;
    }
  paragDia = new KWParagDia(0,"");
  paragDia->setCaption(i18n("KWord - Paragraph settings"));
  QObject::connect(paragDia,SIGNAL(applyButtonPressed()),this,SLOT(paragDiaOk()));
  paragDia->setLeftIndent(gui->getPaperWidget()->getLeftIndent());
  paragDia->setFirstLineIndent(gui->getPaperWidget()->getFirstLineIndent());
  paragDia->setSpaceBeforeParag(gui->getPaperWidget()->getSpaceBeforeParag());
  paragDia->setSpaceAfterParag(gui->getPaperWidget()->getSpaceAfterParag());
  paragDia->setLineSpacing(gui->getPaperWidget()->getLineSpacing());
  paragDia->setFlow(gui->getPaperWidget()->getFlow());
  paragDia->setLeftBorder(gui->getPaperWidget()->getLeftBorder());
  paragDia->setRightBorder(gui->getPaperWidget()->getRightBorder());
  paragDia->setTopBorder(gui->getPaperWidget()->getTopBorder());
  paragDia->setBottomBorder(gui->getPaperWidget()->getBottomBorder());
  paragDia->show();
}

/*===============================================================*/
void KWordView::formatPage()
{
  KoPageLayout pgLayout;
  KoColumns cl;
  m_pKWordDoc->getPageLayout(pgLayout,cl);

  KoHeadFoot hf;
  int flags = FORMAT_AND_BORDERS;
  if (m_pKWordDoc->getProcessingType() == KWordDocument::WP)
    flags = flags | COLUMNS;
  else
    flags = flags | DISABLE_BORDERS;
  
  if (KoPageLayoutDia::pageLayout(pgLayout,hf,cl,flags)) 
    {
      m_pKWordDoc->setPageLayout(pgLayout,cl);
      gui->getVertRuler()->setPageLayout(pgLayout);
      gui->getHorzRuler()->setPageLayout(pgLayout);
    }
}

/*===============================================================*/
void KWordView::formatNumbering()
{
}

/*===============================================================*/
void KWordView::formatStyle()
{
}

/*===============================================================*/
void KWordView::extraSpelling()
{
}

/*===============================================================*/
void KWordView::extraStylist()
{
  if (styleManager)
    {
      styleManager->close();
      delete styleManager;
      styleManager = 0;
    }
  styleManager = new KWStyleManager(0,m_pKWordDoc);
  styleManager->setCaption(i18n("KWord - Stylist"));
  styleManager->show();
}

/*===============================================================*/
void KWordView::extraOptions()
{
}

/*===============================================================*/
void KWordView::toolsEdit()
{
  gui->getPaperWidget()->mmEdit();
}

/*===============================================================*/
void KWordView::toolsEditFrame()
{
  gui->getPaperWidget()->mmEditFrame();
}

/*===============================================================*/
void KWordView::toolsCreateText()
{
  gui->getPaperWidget()->mmCreateText();
}

/*===============================================================*/
void KWordView::toolsCreatePix()
{
  gui->getPaperWidget()->mmCreatePix();
}

/*===============================================================*/
void KWordView::helpContents()
{
}

/*===============================================================*/
void KWordView::helpAbout()
{
  QMessageBox::information(this,"KWord",
			   i18n("KWord 0.0.1 alpha\n\n"
			   "(c) by Torben Weis <weis@kde.org> and \n"
			   "Reginald Stadlbauer <reggie@kde.org> 1998\n\n"
			   "KWord is under GNU GPL"));
}

/*===============================================================*/
void KWordView::helpAboutKOffice()
{
}

/*===============================================================*/
void KWordView::helpAboutKDE()
{
}

/*====================== text style selected  ===================*/
void KWordView::textStyleSelected(const char *size)
{
}

/*======================= text size selected  ===================*/
void KWordView::textSizeSelected(const char *size)
{
  tbFont.setPointSize(atoi(size));
  format.setPTFontSize(atoi(size));
  gui->getPaperWidget()->formatChanged(format);
}

/*======================= text font selected  ===================*/
void KWordView::textFontSelected(const char *font)
{
  tbFont.setFamily(font);
  format.setUserFont(new KWUserFont(m_pKWordDoc,font));
  gui->getPaperWidget()->formatChanged(format);
}

/*========================= text bold ===========================*/
void KWordView::textBold()
{
  tbFont.setBold(!tbFont.bold());
  format.setWeight(tbFont.bold() ? QFont::Bold : QFont::Normal);
  gui->getPaperWidget()->formatChanged(format);
}

/*========================== text italic ========================*/
void KWordView::textItalic()
{
  tbFont.setItalic(!tbFont.italic());
  format.setItalic(tbFont.italic() ? 1 : 0);
  gui->getPaperWidget()->formatChanged(format);
}

/*======================== text underline =======================*/
void KWordView::textUnderline()
{
  tbFont.setUnderline(!tbFont.underline());
  format.setUnderline(tbFont.underline() ? 1 : 0);
  gui->getPaperWidget()->formatChanged(format);
}

/*=========================== text color ========================*/
void KWordView::textColor()
{
  if (KColorDialog::getColor(tbColor))
    {
      OpenPartsUI::Pixmap pix;
      pix.data = CORBA::string_dup( colorToPixString( tbColor ) );

      m_vToolBarText->setButtonPixmap( ID_TEXT_COLOR , pix );
      format.setColor(tbColor);
      gui->getPaperWidget()->formatChanged(format);
    }
}

/*======================= text align left =======================*/
void KWordView::textAlignLeft()
{
  flow = KWParagLayout::LEFT;
  m_vToolBarText->setButton(ID_ALEFT,true);  
  m_vToolBarText->setButton(ID_ACENTER,false);  
  m_vToolBarText->setButton(ID_ARIGHT,false);  
  m_vToolBarText->setButton(ID_ABLOCK,false);  
  gui->getPaperWidget()->setFlow(KWParagLayout::LEFT);
}

/*======================= text align center =====================*/
void KWordView::textAlignCenter()
{
  flow = KWParagLayout::CENTER;
  m_vToolBarText->setButton(ID_ALEFT,false);  
  m_vToolBarText->setButton(ID_ACENTER,true);  
  m_vToolBarText->setButton(ID_ARIGHT,false);  
  m_vToolBarText->setButton(ID_ABLOCK,false);  
  gui->getPaperWidget()->setFlow(KWParagLayout::CENTER);
}

/*======================= text align right ======================*/
void KWordView::textAlignRight()
{
  flow = KWParagLayout::RIGHT;
  m_vToolBarText->setButton(ID_ALEFT,false);  
  m_vToolBarText->setButton(ID_ACENTER,false);  
  m_vToolBarText->setButton(ID_ARIGHT,true);  
  m_vToolBarText->setButton(ID_ABLOCK,false);  
  gui->getPaperWidget()->setFlow(KWParagLayout::RIGHT);
}

/*======================= text align block ======================*/
void KWordView::textAlignBlock()
{
  flow = KWParagLayout::BLOCK;
  m_vToolBarText->setButton(ID_ARIGHT,false);  
  m_vToolBarText->setButton(ID_ACENTER,false);  
  m_vToolBarText->setButton(ID_ALEFT,false);  
  m_vToolBarText->setButton(ID_ABLOCK,true);  
  gui->getPaperWidget()->setFlow(KWParagLayout::BLOCK);
}

/*====================== enumerated list ========================*/
void KWordView::textEnumList()
{
}

/*====================== unsorted list ==========================*/
void KWordView::textUnsortList()
{
}

/*===============================================================*/
void KWordView::textSuperScript()
{
  m_vToolBarText->setButton(ID_SUBSCRIPT,false);  
  if (m_vToolBarText->isButtonOn(ID_SUPERSCRIPT))
    vertAlign = KWFormat::VA_SUPER;
  else
    vertAlign = KWFormat::VA_NORMAL;
  format.setVertAlign(vertAlign);
  gui->getPaperWidget()->formatChanged(format);
}

/*===============================================================*/
void KWordView::textSubScript()
{
  m_vToolBarText->setButton(ID_SUPERSCRIPT,false);  
  if (m_vToolBarText->isButtonOn(ID_SUBSCRIPT))
    vertAlign = KWFormat::VA_SUB;
  else
    vertAlign = KWFormat::VA_NORMAL;
  format.setVertAlign(vertAlign);
  gui->getPaperWidget()->formatChanged(format);
}

/*===============================================================*/
void KWordView::textBorderLeft()
{
  if (m_vToolBarText->isButtonOn(ID_BRD_LEFT))
    left = tmpBrd;
  else
    left.ptWidth = 0;

  gui->getPaperWidget()->setParagLeftBorder(left);
}

/*===============================================================*/
void KWordView::textBorderRight()
{
  if (m_vToolBarText->isButtonOn(ID_BRD_RIGHT))
    right = tmpBrd;
  else
    right.ptWidth = 0;

  gui->getPaperWidget()->setParagRightBorder(right);
}

/*===============================================================*/
void KWordView::textBorderTop()
{
  if (m_vToolBarText->isButtonOn(ID_BRD_TOP))
    top = tmpBrd;
  else
    top.ptWidth = 0;

  gui->getPaperWidget()->setParagTopBorder(top);
}

/*===============================================================*/
void KWordView::textBorderBottom()
{
  if (m_vToolBarText->isButtonOn(ID_BRD_BOTTOM))
    bottom = tmpBrd;
  else
    bottom.ptWidth = 0;

  gui->getPaperWidget()->setParagBottomBorder(bottom);
}

/*================================================================*/
void KWordView::textBorderColor()
{
  if (KColorDialog::getColor(tmpBrd.color))
  {
    OpenPartsUI::Pixmap pix;
    pix.data = CORBA::string_dup( colorToPixString( tmpBrd.color ) );
    m_vToolBarText->setButtonPixmap( ID_BORDER_COLOR , pix );
  }
  
//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderLeft))
//     {
//       left = tmpBrd;
//       gui->getPaperWidget()->setParagLeftBorder(left);
//     }
//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderRight))
//     {
//       right = tmpBrd;
//       gui->getPaperWidget()->setParagRightBorder(right);
//     }
//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderTop))
//     {
//       top = tmpBrd;
//       gui->getPaperWidget()->setParagTopBorder(top);
//     }
//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderBottom))
//     {
//       bottom = tmpBrd;
//       gui->getPaperWidget()->setParagBottomBorder(bottom);
//     }
}

/*================================================================*/
void KWordView::textBorderWidth(const char *width)
{
  tmpBrd.ptWidth = atoi(width);

//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderLeft))
//     {
//       left = tmpBrd;
//       gui->getPaperWidget()->setParagLeftBorder(left);
//     }
//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderRight))
//     {
//       right = tmpBrd;
//       gui->getPaperWidget()->setParagRightBorder(right);
//     }
//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderTop))
//     {
//       top = tmpBrd;
//       gui->getPaperWidget()->setParagTopBorder(top);
//     }
//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderBottom))
//     {
//       bottom = tmpBrd;
//       gui->getPaperWidget()->setParagBottomBorder(bottom);
//     }
}

/*================================================================*/
void KWordView::textBorderStyle(const char *style)
{
  QString stl(style);

  if (stl == i18n("solid line"))
    tmpBrd.style = KWParagLayout::SOLID;
  else if (stl == i18n("dash line (----)"))
    tmpBrd.style = KWParagLayout::DASH;
  else if (stl == i18n("dot line (****)"))
    tmpBrd.style = KWParagLayout::DOT;
  else if (stl == i18n("dash dot line (-*-*)"))
    tmpBrd.style = KWParagLayout::DASH_DOT;
  else if (stl == i18n("dash dot dot line (-**-)"))
    tmpBrd.style = KWParagLayout::DASH_DOT_DOT;

//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderLeft))
//     {
//       left = tmpBrd;
//       gui->getPaperWidget()->setParagLeftBorder(left);
//     }
//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderRight))
//     {
//       right = tmpBrd;
//       gui->getPaperWidget()->setParagRightBorder(right);
//     }
//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderTop))
//     {
//       top = tmpBrd;
//       gui->getPaperWidget()->setParagTopBorder(top);
//     }
//   if (m_vToolBarText->isButtonOn(m_idButtonText_BorderBottom))
//     {
//       bottom = tmpBrd;
//       gui->getPaperWidget()->setParagBottomBorder(bottom);
//     }
}

/*================================================================*/
void KWordView::resizeEvent(QResizeEvent *e)
{
  QWidget::resizeEvent(e);
  if (gui) gui->resize(width(),height());
}

/*================================================================*/
void KWordView::keyPressEvent(QKeyEvent *e)
{
  if (gui) gui->keyEvent(e);
}

/*================================================================*/
void KWordView::mousePressEvent(QMouseEvent *e)
{
}

/*================================================================*/
void KWordView::mouseMoveEvent(QMouseEvent *e)
{
}

/*================================================================*/
void KWordView::mouseReleaseEvent(QMouseEvent *e)
{
}

/*================================================================*/
bool KWordView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{ 
  if ( CORBA::is_nil( _menubar ) )
  {
    m_vMenuEdit = 0L;
    m_vMenuView = 0L;
    m_vMenuInsert = 0L;
    m_vMenuFormat = 0L;
    m_vMenuExtra = 0L;
    m_vMenuHelp = 0L;
    return true;
  }
  
  // edit menu
  _menubar->insertMenu( i18n("&Edit"), m_vMenuEdit, -1, -1 );

  QString tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/undo.xpm";
  OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap( tmp );
  m_idMenuEdit_Undo = m_vMenuEdit->insertItem6( pix, i18n("No Undo possible"), this, "editUndo", 0, -1, -1 );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, false );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/redo.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuEdit_Redo = m_vMenuEdit->insertItem6( pix, i18n("No Redo possible"), this, "editRedo", 0, -1, -1 );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, false );
  m_vMenuEdit->insertSeparator( -1 );

  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editcut.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuEdit_Cut = m_vMenuEdit->insertItem6( pix, i18n("&Cut"), this, "editCut", 0, -1, -1 );

  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editcopy.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuEdit_Copy = m_vMenuEdit->insertItem6( pix, i18n("&Copy"), this, "editCopy", 0, -1, -1 );

  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editpaste.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuEdit_Paste = m_vMenuEdit->insertItem6( pix, i18n("&Paste"), this, "editPaste", 0, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );
  m_idMenuEdit_Find = m_vMenuEdit->insertItem4( i18n("&Find..."), this, "editFind", 0, -1, -1 );
  m_idMenuEdit_FindReplace = m_vMenuEdit->insertItem4( i18n("&Replace..."), this, "editFindReplace", 0, -1, -1 );
  
  // View
  _menubar->insertMenu( i18n( "&View" ), m_vMenuView, -1, -1 );

  m_idMenuView_NewView = m_vMenuView->insertItem4( i18n("&New View"), this, "newView", 0, -1, -1 );

  // insert menu
  _menubar->insertMenu( i18n( "&Insert" ), m_vMenuInsert, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/picture.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuInsert_Picture = m_vMenuInsert->insertItem6( pix, i18n("&Picture..."), this, "insertPicture", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/clipart.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuInsert_Clipart = m_vMenuInsert->insertItem6( pix, i18n("&Clipart..."), this, "insertClipart", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/table.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuInsert_Table = m_vMenuInsert->insertItem6( pix, i18n("&Table..."), this, "insertTable", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/parts.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuInsert_Table = m_vMenuInsert->insertItem6( pix, i18n("&Objects..."), this, "insertPart", 0, -1, -1 );

  // tools menu
  _menubar->insertMenu( i18n( "&Tools" ), m_vMenuTools, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/edittool.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuTools_Edit = m_vMenuTools->insertItem6(pix, i18n("&Edit Text"), this, "toolsEdit", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/editframetool.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuTools_EditFrame = m_vMenuTools->insertItem6(pix, i18n("&Edit Frames"), this, "toolsEditFrame", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/textframetool.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuTools_CreateText = m_vMenuTools->insertItem6(pix, i18n("&Create Text Frame"), this, "toolsCreateText", 0, -1, -1 );

  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/picframetool.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idMenuTools_CreatePix = m_vMenuTools->insertItem6(pix, i18n("&Create Picture Frame"), this, "toolsCreatePix", 0, -1, -1 );

  m_vMenuTools->setCheckable(true);
  m_vMenuTools->setItemChecked(m_idMenuTools_Edit,true);

  // format menu
  _menubar->insertMenu( i18n( "&Format" ), m_vMenuFormat, -1, -1 );

  m_idMenuFormat_Font = m_vMenuFormat->insertItem4( i18n("&Font..."), this, "formatFont", 0, -1, -1 );
  m_idMenuFormat_Color = m_vMenuFormat->insertItem4( i18n("&Color..."), this, "formatColor", 0, -1, -1 );
  m_idMenuFormat_Paragraph = m_vMenuFormat->insertItem4( i18n("Paragraph..."), this, "formatParagraph", 0, -1, -1 );
  m_idMenuFormat_Page = m_vMenuFormat->insertItem4( i18n("Page..."), this, "formatPage", 0, -1, -1 );

  m_vMenuFormat->insertSeparator( -1 );

  //m_idMenuFormat_Numbering = m_vMenuFormat->insertItem4( i18n("Numbering..."), this, "formatNumbering", 0, -1, -1 );
  m_idMenuFormat_Style = m_vMenuFormat->insertItem4( i18n("&Style..."), this, "formatStyle", 0, -1, -1 );

  // extra menu
  _menubar->insertMenu( i18n( "&Extra" ), m_vMenuExtra, -1, -1 );

  m_idMenuExtra_Spelling = m_vMenuExtra->insertItem4( i18n("&Spelling..."), this, "extraSpelling", 0, -1, -1 );
  m_idMenuExtra_Stylist = m_vMenuExtra->insertItem4( i18n("&Stylist..."), this, "extraStylist", 0, -1, -1 );

  m_vMenuExtra->insertSeparator( -1 );

  m_idMenuExtra_Options = m_vMenuExtra->insertItem4( i18n("&Options..."), this, "extraOptions", 0, -1, -1 );

  // help menu
  m_vMenuHelp = _menubar->helpMenu();
  if ( CORBA::is_nil( m_vMenuHelp ) )
  {
    _menubar->insertSeparator( -1 );
    _menubar->setHelpMenu( _menubar->insertMenu( i18n( "&Help" ), m_vMenuHelp, -1, -1 ) );
  }
  else
    m_vMenuHelp->insertSeparator( -1 );
  
  m_idMenuHelp_Contents = m_vMenuHelp->insertItem4( i18n("&Contents"), this, "helpContents", 0, -1, -1 );
  /* m_rMenuBar->insertSeparator(m_idMenuHelp);
  m_idMenuHelp_About = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&About KWord...")),m_idMenuHelp,
					      this,CORBA::string_dup("helpAbout"));
  m_idMenuHelp_AboutKOffice = m_rMenuBar->insertItem(CORBA::string_dup(i18n("About K&Office...")),m_idMenuHelp,
						     this,CORBA::string_dup("helpAboutKOffice"));
  m_idMenuHelp_AboutKDE = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&About KDE...")),m_idMenuHelp,
						 this,CORBA::string_dup("helpAboutKDE")); */

  return true;
}

/*======================= setup edit toolbar ===================*/
bool KWordView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  if ( CORBA::is_nil( _factory ) )
  {
    m_vToolBarEdit = 0L;
    m_vToolBarText = 0L;
    m_vToolBarInsert = 0L;
    return true;
  }

  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  m_vToolBarEdit->setFullWidth( false );

  // undo
  QString tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/undo.xpm";
  OpenPartsUI::Pixmap_var pix = OPUIUtils::loadPixmap( tmp );
  m_idButtonEdit_Undo = m_vToolBarEdit->insertButton2( pix, ID_UNDO, SIGNAL( clicked() ), this, "editUndo", true, i18n("Undo"), -1 );
  m_vToolBarEdit->setItemEnabled(ID_UNDO,false);

  // redo
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/redo.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonEdit_Redo = m_vToolBarEdit->insertButton2( pix, ID_REDO, SIGNAL( clicked() ), this, "editRedo", true, i18n("Redo"), -1);
  m_vToolBarEdit->setItemEnabled(ID_REDO,false);
  
  m_vToolBarEdit->insertSeparator( -1 );

  // cut
  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editcut.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonEdit_Cut = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editCut", true, i18n("Cut"), -1);

  // copy
  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editcopy.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonEdit_Copy = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editCopy", true, i18n("Copy"), -1);

  // paste
  tmp = kapp->kde_toolbardir().copy();
  tmp += "/editpaste.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonEdit_Paste = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editPaste", true, i18n("Paste"), -1);

  m_vToolBarEdit->enable( OpenPartsUI::Show );

  // TOOLBAR Insert
  m_vToolBarInsert = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarInsert->setFullWidth( false );
 
  // picture
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/picture.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonInsert_Picture = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertPicture", true, i18n("Insert Picture"), -1);
      
  // clipart
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/clipart.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonInsert_Clipart = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertClipart", true, i18n("Insert Clipart"), -1);

  // line
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/table.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonInsert_Table = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertTable", true, i18n("Insert Table"), -1);

  // parts
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/parts.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonInsert_Part = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertPart", true, i18n("Insert Object"), -1);

  m_vToolBarInsert->enable( OpenPartsUI::Show );

  // TOOLBAR Tools
  m_vToolBarTools = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarTools->setFullWidth( false );
 
  // edit
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/edittool.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonTools_Edit = m_vToolBarTools->insertButton2( pix, ID_TOOL_EDIT, SIGNAL( clicked() ), this, "toolsEdit", 
							 true, i18n("Edit Text Tool"), -1);
  m_vToolBarTools->setToggle(ID_TOOL_EDIT,true);
  m_vToolBarTools->setButton(ID_TOOL_EDIT,true);

  // edit frame
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/editframetool.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonTools_EditFrame = m_vToolBarTools->insertButton2( pix, ID_TOOL_EDIT_FRAME, SIGNAL( clicked() ), this, "toolsEditFrame", 
							      true, i18n("Edit Frames Tool"), -1);
  m_vToolBarTools->setToggle(ID_TOOL_EDIT_FRAME,true);

  // create text frame
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/textframetool.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonTools_CreateText = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_TEXT, SIGNAL( clicked() ), this, "toolsCreateText", 
							       true, i18n("Create Text Frame"), -1);
  m_vToolBarTools->setToggle(ID_TOOL_CREATE_TEXT,true);

  // create pix frame
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/picframetool.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonTools_CreatePix = m_vToolBarTools->insertButton2( pix, ID_TOOL_CREATE_PIX, SIGNAL( clicked() ), this, "toolsCreatePix", 
							       true, i18n("Create Picture Frame"), -1);
  m_vToolBarTools->setToggle(ID_TOOL_CREATE_PIX,true);

  m_vToolBarTools->enable( OpenPartsUI::Show );

  // TOOLBAR Text
  m_vToolBarText = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarText->setFullWidth( false );

  // style combobox
  OpenPartsUI::StrList stylelist;
  stylelist.length(m_pKWordDoc->paragLayoutList.count());
  for (unsigned int i = 0;i < m_pKWordDoc->paragLayoutList.count();i++)
    stylelist[i] = CORBA::string_dup(m_pKWordDoc->paragLayoutList.at(i)->getName());
  m_idComboText_Style = m_vToolBarText->insertCombo( stylelist, ID_STYLE_LIST, false, SIGNAL( activated( const char* ) ),
						     this, "textStyleSelected", true, i18n("Style"),
						     200, -1, OpenPartsUI::AtBottom );

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
							this, "textSizeSelected", true,
							i18n( "Font Size"  ), 50, -1, OpenPartsUI::AtBottom );
  m_vToolBarText->setCurrentComboItem(ID_FONT_SIZE,8);
  tbFont.setPointSize(12);

  // fonts combobox
  getFonts();
  OpenPartsUI::StrList fonts;
  fonts.length( fontList.count() );
  for(unsigned int i = 0;i < fontList.count(); i++ )
    fonts[i] = CORBA::string_dup( fontList.at(i) );
  m_idComboText_FontList = m_vToolBarText->insertCombo( fonts, ID_FONT_LIST, true, SIGNAL( activated( const char* ) ), this,
							"textFontSelected", true, i18n("Font List"),
							200, -1, OpenPartsUI::AtBottom );
  tbFont.setFamily(fontList.at(0));
  m_vToolBarText->setCurrentComboItem(ID_FONT_LIST,0);

  m_vToolBarText->insertSeparator( -1 );

  // bold
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/bold.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_Bold = m_vToolBarText->insertButton2( pix, ID_BOLD, SIGNAL( clicked() ), this, "textBold", true, i18n("Bold"), -1);
  m_vToolBarText->setToggle(ID_BOLD,true);
  m_vToolBarText->setButton(ID_BOLD,false);
  tbFont.setBold(false);

  // italic
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/italic.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_Italic = m_vToolBarText->insertButton2( pix, ID_ITALIC, SIGNAL( clicked() ), this, "textItalic", true, i18n("Italic"), -1);
  m_vToolBarText->setToggle(ID_ITALIC,true);
  m_vToolBarText->setButton(ID_ITALIC,false);
  tbFont.setItalic(false);
  
  // underline
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/underl.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_Underline = m_vToolBarText->insertButton2( pix, ID_UNDERLINE, SIGNAL( clicked() ), this, "textUnderline", true, i18n("Underline"), -1);
  m_vToolBarText->setToggle(ID_UNDERLINE,true);
  m_vToolBarText->setButton(ID_UNDERLINE,false);
  tbFont.setUnderline(false);
  
  // color
  tbColor = black;
  OpenPartsUI::Pixmap colpix;
  colpix.data = CORBA::string_dup( colorToPixString( tbColor ) );
  m_idButtonText_Color = m_vToolBarText->insertButton2( colpix, ID_TEXT_COLOR, SIGNAL( clicked() ), this, "textColor", 
							true, i18n("Text Color"), -1 );

  m_vToolBarText->insertSeparator( -1 );

  // align left
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/alignLeft.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_ALeft = m_vToolBarText->insertButton2( pix, ID_ALEFT, SIGNAL( clicked() ), this, "textAlignLeft", 
							true, i18n("Align Left"), -1);
  m_vToolBarText->setToggle(ID_ALEFT,true);
  m_vToolBarText->setButton(ID_ALEFT,true);

  // align center
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/alignCenter.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_ACenter = m_vToolBarText->insertButton2( pix, ID_ACENTER, SIGNAL( clicked() ), this, "textAlignCenter", 
							  true, i18n("Align Center"), -1);
  m_vToolBarText->setToggle(ID_ACENTER,true);
  m_vToolBarText->setButton(ID_ACENTER,false);
  
  // align right
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/alignRight.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_ARight = m_vToolBarText->insertButton2( pix, ID_ARIGHT, SIGNAL( clicked() ), this, "textAlignRight", 
							 true, i18n("Align Right"), -1);
  m_vToolBarText->setToggle(ID_ARIGHT,true);
  m_vToolBarText->setButton(ID_ARIGHT,false);
  
  // align block
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/alignBlock.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_ABlock = m_vToolBarText->insertButton2( pix, ID_ABLOCK, SIGNAL( clicked() ), this, "textAlignBlock", 
							 true, i18n("Align Block"), -1);
  m_vToolBarText->setToggle(ID_ABLOCK,true);
  m_vToolBarText->setButton(ID_ABLOCK,false);
  
  m_vToolBarText->insertSeparator( -1 );

  // enum list
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/enumList.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_EnumList = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textEnumList", true, i18n("Enumerated List"), -1);

  // unsorted list
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/unsortedList.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_EnumList = m_vToolBarText->insertButton2( pix, 1, SIGNAL( clicked() ), this, "textUnsortList", true, i18n("Unsorted List"), -1);

  m_vToolBarText->insertSeparator( -1 );

  // superscript
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/super.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_SuperScript = m_vToolBarText->insertButton2( pix, ID_SUPERSCRIPT, SIGNAL( clicked() ), this, "textSuperScript", 
							      true, i18n("Superscript"), -1);
  m_vToolBarText->setToggle(ID_SUPERSCRIPT,true);
  m_vToolBarText->setButton(ID_SUPERSCRIPT,false);
  
  // subscript
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/sub.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_SubScript = m_vToolBarText->insertButton2( pix, ID_SUBSCRIPT, SIGNAL( clicked() ), this, "textSubScript", 
							    true, i18n("Subscript"), -1);
  m_vToolBarText->setToggle(ID_SUBSCRIPT,true);
  m_vToolBarText->setButton(ID_SUBSCRIPT,false);
  
  m_vToolBarText->insertSeparator( -1 );

  // border left
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/borderleft.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_BorderLeft = m_vToolBarText->insertButton2( pix, ID_BRD_LEFT, SIGNAL( clicked() ), this, "textBorderLeft", 
							     true, i18n("Border Left"), -1);
  m_vToolBarText->setToggle(ID_BRD_LEFT,true);
  m_vToolBarText->setButton(ID_BRD_LEFT,false);
  
  // border right
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/borderright.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_BorderRight = m_vToolBarText->insertButton2( pix, ID_BRD_RIGHT, SIGNAL( clicked() ), this, "textBorderRight", 
							      true, i18n("Border Right"), -1);
  m_vToolBarText->setToggle(ID_BRD_RIGHT,true);
  m_vToolBarText->setButton(ID_BRD_RIGHT,false);
  
  // border top
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/bordertop.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_BorderTop = m_vToolBarText->insertButton2( pix, ID_BRD_TOP, SIGNAL( clicked() ), this, "textBorderTop", 
							    true, i18n("Border Top"), -1);
  m_vToolBarText->setToggle(ID_BRD_TOP,true);
  m_vToolBarText->setButton(ID_BRD_TOP,false);
  
  // border bottom
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/borderbottom.xpm";
  pix = OPUIUtils::loadPixmap(tmp);
  m_idButtonText_BorderBottom = m_vToolBarText->insertButton2( pix, ID_BRD_BOTTOM, SIGNAL( clicked() ), this, "textBorderBottom", 
							       true, i18n("Border Bottom"), -1);
  m_vToolBarText->setToggle(ID_BRD_BOTTOM,true);
  m_vToolBarText->setButton(ID_BRD_BOTTOM,false);
  
  // border color
  tmpBrd.color = black;
  colpix.data = CORBA::string_dup( colorToPixString( tmpBrd.color ) );
  m_idButtonText_BorderColor = m_vToolBarText->insertButton2( colpix, ID_BORDER_COLOR, SIGNAL( clicked() ), this, "textBorderColor", 
							      true, i18n("Border Color"), -1);

  // border width combobox
  OpenPartsUI::StrList widthlist;
  widthlist.length( 10 );
  for( unsigned int i = 1; i <= 10; i++ )
  {
    char buffer[10];
    sprintf(buffer,"%i",i);
    widthlist[i-1] = CORBA::string_dup( buffer );
  }
  tmpBrd.ptWidth = 1;
  m_idComboText_BorderWidth = m_vToolBarText->insertCombo( widthlist, ID_BRD_WIDTH, true, SIGNAL( activated( const char* ) ),
							   this, "textBorderWidth", true, i18n("Border Width"),
							   60, -1, OpenPartsUI::AtBottom );

  // border style combobox
  stylelist.length( 5 );
  stylelist[0] = CORBA::string_dup( i18n("solid line") );
  stylelist[1] = CORBA::string_dup( i18n("dash line (----)") );
  stylelist[2] = CORBA::string_dup( i18n("dot line (****)") );
  stylelist[3] = CORBA::string_dup( i18n("dash dot line (-*-*)") );
  stylelist[4] = CORBA::string_dup( i18n("dash dot dot line (-**-)") );
  m_idComboText_BorderStyle = m_vToolBarText->insertCombo( stylelist,ID_BRD_STYLE, false, SIGNAL( activated( const char* ) ),
							   this, "textBorderStyle", true, i18n("Border Style"),
							   150, -1, OpenPartsUI::AtBottom );
  tmpBrd.style = KWParagLayout::SOLID;

  m_vToolBarText->enable( OpenPartsUI::Show );

  return true;
}      

/*============== create a pixmapstring from a color ============*/
char* KWordView::colorToPixString(QColor c)
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
void KWordView::getFonts()
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

/*================================================================*/
void KWordView::setParagBorderValues()
{
  m_vToolBarText->setCurrentComboItem(ID_BRD_WIDTH,tmpBrd.ptWidth - 1);
  m_vToolBarText->setCurrentComboItem(ID_BRD_STYLE,static_cast<int>(tmpBrd.style));

  OpenPartsUI::Pixmap colpix;
  colpix.data = CORBA::string_dup( colorToPixString( tmpBrd.color ) );
  m_vToolBarText->setButtonPixmap(ID_BORDER_COLOR, colpix );
}

/*================================================================*/
void KWordView::slotInsertObject(KWordChild *_child)
{ 
  OpenParts::View_var v = _child->createView( m_vKoMainWindow );
  if (CORBA::is_nil( v ) )
  {
    printf("void KWordView::slotInsertObject( const KRect& _rect, OPParts::Document_ptr _doc )\n");
    printf("return value is 0L\n");
    exit(1);
  }

  KOffice::View_var kv = KOffice::View::_narrow( v );
  kv->setMode( KOffice::View::ChildMode );
  assert( !CORBA::is_nil( kv ) );

  KWordFrame *p = new KWordFrame( this, _child );
  p->attachView( kv );
  p->setGeometry( _child->geometry() );
  
  QObject::connect(p,SIGNAL(sig_geometryEnd( KoFrame* ) ),
		   this,SLOT(slotGeometryEnd( KoFrame* ) ) );
  QObject::connect(p,SIGNAL(sig_moveEnd( KoFrame* ) ),
		   this,SLOT(slotMoveEnd( KoFrame* ) ) );  

  m_lstFrames.append( p );
  p->show();
} 

/*================================================================*/
void KWordView::slotUpdateChildGeometry(KWordChild *_child)
{
  // Find frame for child
  KWordFrame *f = 0L;
  QListIterator<KWordFrame> it( m_lstFrames );
  for (;it.current() && !f;++it)
    if (it.current()->child() == _child) f = it.current();
  
  assert(f != 0L);
  
  // Are we already up to date ?
  if (_child->geometry() == f->partGeometry())
    return;
  
  // TODO scaling
  f->setPartGeometry(_child->geometry());
}

/*================================================================*/
void KWordView::slotGeometryEnd( KoFrame* _frame)
{
  KWordFrame *f = (KWordFrame*)_frame;
  // TODO scaling
  m_pKWordDoc->changeChildGeometry(f->child(),_frame->partGeometry());
}

/*================================================================*/
void KWordView::slotMoveEnd( KoFrame* _frame)
{
  KWordFrame *f = (KWordFrame*)_frame;
  // TODO scaling
  m_pKWordDoc->changeChildGeometry(f->child(),_frame->partGeometry());
}

/*================================================================*/
void KWordView::paragDiaOk()
{
  gui->getPaperWidget()->setLeftIndent(paragDia->getLeftIndent());
  gui->getPaperWidget()->setFirstLineIndent(paragDia->getFirstLineIndent());
  gui->getPaperWidget()->setSpaceBeforeParag(paragDia->getSpaceBeforeParag());
  gui->getPaperWidget()->setSpaceAfterParag(paragDia->getSpaceAfterParag());
  gui->getPaperWidget()->setLineSpacing(paragDia->getLineSpacing());
  gui->getHorzRuler()->setLeftIndent(static_cast<int>(paragDia->getLeftIndent()));
  gui->getHorzRuler()->setFirstIndent(static_cast<int>(paragDia->getFirstLineIndent()));
  gui->getPaperWidget()->setFlow(paragDia->getFlow());
  gui->getPaperWidget()->setParagLeftBorder(paragDia->getLeftBorder());
  gui->getPaperWidget()->setParagRightBorder(paragDia->getRightBorder());
  gui->getPaperWidget()->setParagTopBorder(paragDia->getTopBorder());
  gui->getPaperWidget()->setParagBottomBorder(paragDia->getBottomBorder());
  setFlow(paragDia->getFlow());
}

/*================================================================*/
void KWordView::newPageLayout(KoPageLayout _layout)
{
  KoPageLayout pgLayout;
  KoColumns cl;
  m_pKWordDoc->getPageLayout(pgLayout,cl);

  m_pKWordDoc->setPageLayout(_layout,cl);
  gui->getHorzRuler()->setPageLayout(_layout);
  gui->getVertRuler()->setPageLayout(_layout);

  if (m_pKWordDoc->getProcessingType() == KWordDocument::DTP)
    gui->getPaperWidget()->frameSizeChanged(_layout);
}

/*================================================================*/
void KWordView::setMode( KOffice::View::Mode _mode)
{
  KoViewIf::setMode(_mode);
  
  if (mode() == KOffice::View::ChildMode && !m_bFocus)
    m_bShowGUI = false;
  else
    m_bShowGUI = true;

  if (gui) 
    {
      gui->showGUI(m_bShowGUI);
      gui->getPaperWidget()->recalcText();
    }
}

/*================================================================*/
void KWordView::setFocus(CORBA::Boolean _mode)
{
  KoViewIf::setFocus(_mode);

  bool old = m_bShowGUI;
  
  if (mode() == KOffice::View::ChildMode && !m_bFocus)
    m_bShowGUI = false;
  else
    m_bShowGUI = true;

  if (gui) gui->showGUI(m_bShowGUI);

  if (old != m_bShowGUI)
    resizeEvent(0L);
}

/******************************************************************/
/* Class: KWordGUI                                                */
/******************************************************************/

/*================================================================*/
KWordGUI::KWordGUI( QWidget *parent, bool __show, KWordDocument *_doc, KWordView *_view )
  : QWidget(parent,"")
{
  doc = _doc;
  view = _view;

  r_horz = r_vert = 0;

  paperWidget = new KWPage(this,doc,this);

  s_vert = new QScrollBar(QScrollBar::Vertical,this);
  s_horz = new QScrollBar(QScrollBar::Horizontal,this);
  QObject::connect(s_vert,SIGNAL(valueChanged(int)),this,SLOT(scrollV(int)));
  QObject::connect(s_horz,SIGNAL(valueChanged(int)),this,SLOT(scrollH(int)));
  s_vert->setValue(s_vert->maxValue());
  s_horz->setValue(s_horz->maxValue());
  s_vert->setValue(s_vert->minValue());
  s_horz->setValue(s_horz->minValue());

  s_horz->hide();
  s_vert->hide();

  KoPageLayout layout;
  KoColumns cols;
  doc->getPageLayout(layout,cols);

  tabChooser = new KoTabChooser(this,KoTabChooser::TAB_ALL);

  r_horz = new KoRuler(this,paperWidget,KoRuler::HORIZONTAL,layout,KoRuler::F_INDENTS | KoRuler::F_TABS,tabChooser);
  r_vert = new KoRuler(this,paperWidget,KoRuler::VERTICAL,layout,0);
  connect(r_horz,SIGNAL(newPageLayout(KoPageLayout)),view,SLOT(newPageLayout(KoPageLayout)));
  connect(r_horz,SIGNAL(newLeftIndent(int)),paperWidget,SLOT(newLeftIndent(int)));
  connect(r_horz,SIGNAL(newFirstIndent(int)),paperWidget,SLOT(newFirstIndent(int)));
  connect(r_horz,SIGNAL(openPageLayoutDia()),view,SLOT(openPageLayoutDia()));
  connect(r_vert,SIGNAL(newPageLayout(KoPageLayout)),view,SLOT(newPageLayout(KoPageLayout)));
  connect(r_vert,SIGNAL(openPageLayoutDia()),view,SLOT(openPageLayoutDia()));

  r_horz->setLeftIndent(static_cast<int>(MM_TO_POINT(paperWidget->getLeftIndent())));
  r_horz->setFirstIndent(static_cast<int>(MM_TO_POINT(paperWidget->getFirstLineIndent())));

//   if (doc->getProcessingType() == KWordDocument_impl::DTP)
//     {
//       connect(r_horz,SIGNAL(newPageLayout(KoPageLayout)),paperWidget,SLOT(frameSizeChanged(KoPageLayout)));
//       connect(r_vert,SIGNAL(newPageLayout(KoPageLayout)),paperWidget,SLOT(frameSizeChanged(KoPageLayout)));
//     }

  r_horz->hide();
  r_vert->hide();

  xOffset = 0;
  yOffset = 0;

  setFocusProxy(paperWidget);
  paperWidget->show();
  paperWidget->setFocusPolicy(QWidget::StrongFocus);
  scrollH(0);
  scrollV(0);
  paperWidget->setXOffset(xOffset);
  paperWidget->setYOffset(yOffset);

  reorganize();

  if (doc->getProcessingType() == KWordDocument::DTP)
    paperWidget->setRuler2Frame(0,0);

  // HACK
  if (doc->viewCount() == 1)
    {
      QKeyEvent e(Event_KeyPress,Key_Delete,0,0);
      paperWidget->keyPressEvent(&e);
      scrollH(0);
      scrollV(0);
      paperWidget->setXOffset(xOffset);
      paperWidget->setYOffset(yOffset);
    }
}

/*================================================================*/
void KWordGUI::showGUI(bool __show)
{
  _show = __show;
  reorganize();
}

/*================================================================*/
void KWordGUI::setRanges()
{
  if (s_vert && s_horz && doc && paperWidget)
    {
      int wid = doc->getPTPaperWidth();
      int hei = doc->getPTPaperHeight();
      int range;

      s_vert->setSteps(10,hei);
      range = (hei * doc->getPages() - paperWidget->height());
      s_vert->setRange(0,range);

      s_horz->setSteps(10,wid);
      range = wid - paperWidget->width();
      s_horz->setRange(0,range);
    }
}

/*================================================================*/
void KWordGUI::scrollH(int _value)
{
  int xo = xOffset;
     
  xOffset = _value;
  paperWidget->scroll(xo - _value,0);
  if (r_horz)
    r_horz->setOffset(xOffset,0);
}

/*================================================================*/
void KWordGUI::scrollV(int _value)
{
  int yo = yOffset;
  
  yOffset = _value;
  paperWidget->scroll(0,yo - _value);
  if (r_vert)
    r_vert->setOffset(0,-paperWidget->getVertRulerPos());
}

/*================================================================*/
void KWordGUI::resizeEvent(QResizeEvent *e)
{
  QWidget::resizeEvent(e);
  reorganize();
}

/*================================================================*/
void KWordGUI::keyPressEvent(QKeyEvent* e)
{
  kapp->notify(paperWidget,e);
}

/*================================================================*/
void KWordGUI::reorganize()
{
  if (_show)
    {
      s_vert->show(); 
      s_horz->show();
      r_vert->show();
      r_horz->show();
      tabChooser->show();
      
      tabChooser->setGeometry(0,0,20,20);

      r_horz->setGeometry(20,0,width() - 36,20);
      r_vert->setGeometry(0,20,20,height() - 36);
      s_horz->setGeometry(0,height() - 16,width() - 16,16);
      s_vert->setGeometry(width() - 16,0,16,height() - 16);
      paperWidget->setGeometry(20,20,width() - 36,height() - 36);

      setRanges();
    }
  else
    {
      s_vert->hide(); 
      s_horz->hide();
      r_vert->hide();
      r_horz->hide();
      tabChooser->hide();

      paperWidget->setGeometry(0,0,width(),height());
    }
}

