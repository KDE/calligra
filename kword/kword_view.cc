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

#include <op_app.h>
#include <utils.h>
#include <part_frame_impl.h>

#include <qpainter.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qkeycode.h>

#include <koPartSelectDia.h>

#include <kapp.h>

#define DEBUG

// Qt bug
char *getenv(const char *name);    

/******************************************************************/
/* Class: KWordFrame                                              */
/******************************************************************/
KWordFrame::KWordFrame(KWordView_impl* _view,KWordChild* _child) 
  : PartFrame_impl(_view)
{
  m_pKWordView = _view;
  m_pKWordChild = _child;
}

/******************************************************************/
/* Class: KWordView_impl                                          */
/******************************************************************/

/*================================================================*/
KWordView_impl::KWordView_impl(QWidget *_parent = 0L,const char *_name = 0L) 
  : QWidget(_parent,_name), View_impl(), KWord::KWordView_skel(), format()
{
  setWidget(this);

  Control_impl::setFocusPolicy(OPControls::Control::ClickFocus);
 
  m_pKWordDoc = 0L;
  m_bUnderConstruction = true;
  m_bShowGUI = true;

  m_lstFrames.setAutoDelete(true);  
  gui = 0;
  flow = KWParagLayout::LEFT;
  paragDia = 0;
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
}

/*================================================================*/
KWordView_impl::~KWordView_impl()
{
  sdeb("KWordView_impl::~KWordView_impl()\n");
  cleanUp();
  edeb("...KWordView_impl::~KWordView_impl()\n");
}

/*================================================================*/
void KWordView_impl::cleanUp()
{
  sdeb("void KWordView_impl::cleanUp()\n" );
  if (m_bIsClean) return;
  
  m_pKWordDoc->removeView(this);
  
  m_lstFrames.clear();

  mdeb("rMenuBar %i %i\n",m_rMenuBar->refCount(),m_rMenuBar->_refcnt());
  
  m_rMenuBar = 0L;
  m_vMenuBarFactory = 0L;

  m_rToolBarFile = 0L;
  m_vToolBarFactory = 0L;

  View_impl::cleanUp();

  edeb("... void KWordView_impl::cleanUp()\n" );
}
  
/*================================================================*/
void KWordView_impl::setDocument(KWordDocument_impl *_doc)
{
  if (m_pKWordDoc) m_pKWordDoc->removeView(this);

  View_impl::setDocument(_doc);
  
  m_pKWordDoc = _doc;

  m_pKWordDoc->addView(this);

  QObject::connect(m_pKWordDoc,SIGNAL(sig_insertObject(KWordChild*)),
		   this,SLOT(slotInsertObject(KWordChild*)));
  QObject::connect(m_pKWordDoc,SIGNAL(sig_updateChildGeometry(KWordChild*)),
		   this,SLOT(slotUpdateChildGeometry(KWordChild*)));
  if (gui) gui->setDocument(m_pKWordDoc);

  format.setDefaults(m_pKWordDoc);
  if (gui) gui->getPaperWidget()->formatChanged(format);

}
  
/*=========================== file print =======================*/
CORBA::Boolean KWordView_impl::printDlg()
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
void KWordView_impl::construct()
{
  if (m_pKWordDoc == 0L && !m_bUnderConstruction) return;
  
  assert(m_pKWordDoc != 0L);
  
  m_bUnderConstruction = false;
  m_lstFrames.clear();

  QListIterator<KWordChild> it = m_pKWordDoc->childIterator();
  for(;it.current();++it)
    slotInsertObject(it.current());
}

/*================================================================*/
void KWordView_impl::setFormat(KWFormat &_format,bool _check = true,bool _update_page = true)
{
  if (_check && format == _format) return;

  format = _format;

  if (_format.getUserFont()->getFontName())
    {
      fontList.find(_format.getUserFont()->getFontName());
      m_rToolBarText->setCurrentComboItem(m_idComboText_FontList,fontList.at());
    }

  if (_format.getPTFontSize() != -1)
    m_rToolBarText->setCurrentComboItem(m_idComboText_FontSize,_format.getPTFontSize() - 4);
  
  if (_format.getWeight() != -1)
    {
      m_rToolBarText->setButton(m_idButtonText_Bold,_format.getWeight() == QFont::Bold);
      tbFont.setBold(_format.getWeight() == QFont::Bold);
    }
  if (_format.getItalic() != -1)
    {
      m_rToolBarText->setButton(m_idButtonText_Italic,_format.getItalic() == 1);
      tbFont.setItalic(_format.getItalic() == 1);
    }
  if (_format.getUnderline() != -1)
    {
      m_rToolBarText->setButton(m_idButtonText_Underline,_format.getUnderline() == 1);
      tbFont.setUnderline(_format.getUnderline() == 1);
    }

  if (_format.getColor().isValid())
    {
      m_rToolBarText->setButtonPixmap(m_idButtonText_Color,CORBA::string_dup(colorToPixString(_format.getColor())));
      tbColor = QColor(_format.getColor());
    }

  m_rToolBarText->setButton(m_idButtonText_SuperScript,false);  
  m_rToolBarText->setButton(m_idButtonText_SubScript,false);  

  if (_format.getVertAlign() == KWFormat::VA_NORMAL)
    vertAlign = KWFormat::VA_NORMAL;
  else if (_format.getVertAlign() == KWFormat::VA_SUB)
    {
      vertAlign = KWFormat::VA_SUB;
      m_rToolBarText->setButton(m_idButtonText_SubScript,true);  
   }
  else if (_format.getVertAlign() == KWFormat::VA_SUPER)
    {
      vertAlign = KWFormat::VA_SUPER;
      m_rToolBarText->setButton(m_idButtonText_SuperScript,true);  
   }

  format = _format;
  
  if (_update_page)
    gui->getPaperWidget()->formatChanged(format);
}

/*================================================================*/
void KWordView_impl::setFlow(KWParagLayout::Flow _flow)
{
  if (_flow != flow)
    {
      flow = _flow;
      m_rToolBarText->setButton(m_idButtonText_ALeft,false);  
      m_rToolBarText->setButton(m_idButtonText_ACenter,false);  
      m_rToolBarText->setButton(m_idButtonText_ARight,false);  
      m_rToolBarText->setButton(m_idButtonText_ABlock,false);  
      
      switch (flow)
	{
	case KWParagLayout::LEFT:
	  m_rToolBarText->setButton(m_idButtonText_ALeft,true);  
	  break;
	case KWParagLayout::CENTER:
	  m_rToolBarText->setButton(m_idButtonText_ACenter,true);  
	  break;
	case KWParagLayout::RIGHT:
	  m_rToolBarText->setButton(m_idButtonText_ARight,true);  
	  break;
	case KWParagLayout::BLOCK:
	  m_rToolBarText->setButton(m_idButtonText_ABlock,true);  
	  break;
	}
    }
}

/*================================================================*/
void KWordView_impl::setParagBorders(KWParagLayout::Border _left,KWParagLayout::Border _right,
				     KWParagLayout::Border _top,KWParagLayout::Border _bottom)
{
  if (left != _left || right != _right || top != _top || bottom != _bottom)
    {
      m_rToolBarText->setButton(m_idButtonText_BorderLeft,false);  
      m_rToolBarText->setButton(m_idButtonText_BorderRight,false);  
      m_rToolBarText->setButton(m_idButtonText_BorderTop,false);  
      m_rToolBarText->setButton(m_idButtonText_BorderBottom,false);  
      
      left = _left;
      right = _right;
      top = _top;
      bottom = _bottom;

      if (left.ptWidth > 0)
	{
	  m_rToolBarText->setButton(m_idButtonText_BorderLeft,true);  
	  tmpBrd = left;
	  setParagBorderValues();
	}
      if (right.ptWidth > 0)
	{
	  m_rToolBarText->setButton(m_idButtonText_BorderRight,true);  
	  tmpBrd = right;
	  setParagBorderValues();
	}
      if (top.ptWidth > 0)
	{
	  m_rToolBarText->setButton(m_idButtonText_BorderTop,true);  
	  tmpBrd = top;
	  setParagBorderValues();
	}
      if (bottom.ptWidth > 0)
	{
	  m_rToolBarText->setButton(m_idButtonText_BorderBottom,true);  
	  tmpBrd = bottom;
	  setParagBorderValues();
	}
    }
}

/*================================================================*/
void KWordView_impl::createGUI()
{
  sdeb("void KWordView_impl::createGUI() %i | %i\n",refCount(),_refcnt());

  m_vMenuBarFactory = m_vPartShell->menuBarFactory();
  if (!CORBA::is_nil(m_vMenuBarFactory)) setupMenu();
  

  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if (!CORBA::is_nil(m_vToolBarFactory))
    {
      setupEditToolbar();
      setupInsertToolbar();
      setupTextToolbar();
    }

  edeb("...void KWordView_impl::createGUI() %i | %i\n",refCount(),_refcnt());

  gui = new KWordGUI(this,m_bShowGUI,m_pKWordDoc,this);
  gui->setGeometry(0,0,width(),height());
  gui->show();

  gui->getPaperWidget()->formatChanged(format);
  widget()->setFocusProxy(gui);

  setFormat(format,false);
}

/*===============================================================*/
void KWordView_impl::editUndo()
{
}

/*===============================================================*/
void KWordView_impl::editRedo()
{
}

/*===============================================================*/
void KWordView_impl::editCut()
{
  gui->getPaperWidget()->editCut();
}

/*===============================================================*/
void KWordView_impl::editCopy()
{
  gui->getPaperWidget()->editCopy();
}

/*===============================================================*/
void KWordView_impl::editPaste()
{
  QClipboard *cb = QApplication::clipboard();
  if (cb->text())
    gui->getPaperWidget()->editPaste(cb->text());
}

/*===============================================================*/
void KWordView_impl::editSelectAll()
{
}

/*===============================================================*/
void KWordView_impl::editFind()
{
}

/*===============================================================*/
void KWordView_impl::editFindReplace()
{
}

/*================================================================*/
void KWordView_impl::newView()
{
  assert((m_pKWordDoc != 0L));

  KWordShell_impl* shell = new KWordShell_impl;
  shell->enableMenuBar();
  shell->PartShell_impl::enableStatusBar();
  shell->enableToolBars();
  shell->show();
  shell->setDocument(m_pKWordDoc);
  
  CORBA::release(shell);
}

/*===============================================================*/
void KWordView_impl::insertPicture()
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
void KWordView_impl::insertTable()
{
}

/*===============================================================*/
void KWordView_impl::insertClipart()
{
}

/*===============================================================*/
void KWordView_impl::insertPart()
{
}

/*===============================================================*/
void KWordView_impl::formatFont()
{
}

/*===============================================================*/
void KWordView_impl::formatColor()
{
}

/*===============================================================*/
void KWordView_impl::formatParagraph()
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
  paragDia->show();
}

/*===============================================================*/
void KWordView_impl::formatPage()
{
  KoPageLayout pgLayout;
  KoColumns cl;
  m_pKWordDoc->getPageLayout(pgLayout,cl);

  KoHeadFoot hf;
  int flags = FORMAT_AND_BORDERS;
  if (m_pKWordDoc->getProcessingType() == KWordDocument_impl::WP)
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
void KWordView_impl::formatNumbering()
{
}

/*===============================================================*/
void KWordView_impl::formatStyle()
{
}

/*===============================================================*/
void KWordView_impl::extraSpelling()
{
}

/*===============================================================*/
void KWordView_impl::extraStylist()
{
}

/*===============================================================*/
void KWordView_impl::extraOptions()
{
}

/*===============================================================*/
void KWordView_impl::helpContents()
{
}

/*===============================================================*/
void KWordView_impl::helpAbout()
{
  QMessageBox::information(this,"KWord",
			   i18n("KWord 0.0.1 alpha\n\n"
			   "(c) by Torben Weis <weis@kde.org> and \n"
			   "Reginald Stadlbauer <reggie@kde.org> 1998\n\n"
			   "KWord is under GNU GPL"));
}

/*===============================================================*/
void KWordView_impl::helpAboutKOffice()
{
}

/*===============================================================*/
void KWordView_impl::helpAboutKDE()
{
}

/*====================== text style selected  ===================*/
void KWordView_impl::textStyleSelected(const char *size)
{
}

/*======================= text size selected  ===================*/
void KWordView_impl::textSizeSelected(const char *size)
{
  tbFont.setPointSize(atoi(size));
  format.setPTFontSize(atoi(size));
  gui->getPaperWidget()->formatChanged(format);
}

/*======================= text font selected  ===================*/
void KWordView_impl::textFontSelected(const char *font)
{
  tbFont.setFamily(font);
  format.setUserFont(new KWUserFont(m_pKWordDoc,font));
  gui->getPaperWidget()->formatChanged(format);
}

/*========================= text bold ===========================*/
void KWordView_impl::textBold()
{
  tbFont.setBold(!tbFont.bold());
  format.setWeight(tbFont.bold() ? QFont::Bold : QFont::Normal);
  gui->getPaperWidget()->formatChanged(format);
}

/*========================== text italic ========================*/
void KWordView_impl::textItalic()
{
  tbFont.setItalic(!tbFont.italic());
  format.setItalic(tbFont.italic() ? 1 : 0);
  gui->getPaperWidget()->formatChanged(format);
}

/*======================== text underline =======================*/
void KWordView_impl::textUnderline()
{
  tbFont.setUnderline(!tbFont.underline());
  format.setUnderline(tbFont.underline() ? 1 : 0);
  gui->getPaperWidget()->formatChanged(format);
}

/*=========================== text color ========================*/
void KWordView_impl::textColor()
{
  if (KColorDialog::getColor(tbColor))
    {
      m_rToolBarText->setButtonPixmap(m_idButtonText_Color,CORBA::string_dup(colorToPixString(tbColor)));
      format.setColor(tbColor);
      gui->getPaperWidget()->formatChanged(format);
    }
}

/*======================= text align left =======================*/
void KWordView_impl::textAlignLeft()
{
  flow = KWParagLayout::LEFT;
  m_rToolBarText->setButton(m_idButtonText_ALeft,false);  
  m_rToolBarText->setButton(m_idButtonText_ACenter,false);  
  m_rToolBarText->setButton(m_idButtonText_ARight,false);  
  m_rToolBarText->setButton(m_idButtonText_ABlock,false);  
  gui->getPaperWidget()->setFlow(KWParagLayout::LEFT);
}

/*======================= text align center =====================*/
void KWordView_impl::textAlignCenter()
{
  flow = KWParagLayout::CENTER;
  m_rToolBarText->setButton(m_idButtonText_ACenter,false);  
  m_rToolBarText->setButton(m_idButtonText_ALeft,false);  
  m_rToolBarText->setButton(m_idButtonText_ARight,false);  
  m_rToolBarText->setButton(m_idButtonText_ABlock,false);  
  gui->getPaperWidget()->setFlow(KWParagLayout::CENTER);
}

/*======================= text align right ======================*/
void KWordView_impl::textAlignRight()
{
  flow = KWParagLayout::RIGHT;
  m_rToolBarText->setButton(m_idButtonText_ARight,false);  
  m_rToolBarText->setButton(m_idButtonText_ACenter,false);  
  m_rToolBarText->setButton(m_idButtonText_ALeft,false);  
  m_rToolBarText->setButton(m_idButtonText_ABlock,false);  
  gui->getPaperWidget()->setFlow(KWParagLayout::RIGHT);
}

/*======================= text align block ======================*/
void KWordView_impl::textAlignBlock()
{
  flow = KWParagLayout::BLOCK;
  m_rToolBarText->setButton(m_idButtonText_ABlock,false);  
  m_rToolBarText->setButton(m_idButtonText_ACenter,false);  
  m_rToolBarText->setButton(m_idButtonText_ARight,false);  
  m_rToolBarText->setButton(m_idButtonText_ALeft,false);  
  gui->getPaperWidget()->setFlow(KWParagLayout::BLOCK);
}

/*====================== enumerated list ========================*/
void KWordView_impl::textEnumList()
{
}

/*====================== unsorted list ==========================*/
void KWordView_impl::textUnsortList()
{
}

/*===============================================================*/
void KWordView_impl::textSuperScript()
{
  m_rToolBarText->setButton(m_idButtonText_SubScript,false);  
  if (!m_rToolBarText->isButtonOn(m_idButtonText_SuperScript))
    vertAlign = KWFormat::VA_SUPER;
  else
    vertAlign = KWFormat::VA_NORMAL;
  format.setVertAlign(vertAlign);
  gui->getPaperWidget()->formatChanged(format);
}

/*===============================================================*/
void KWordView_impl::textSubScript()
{
  m_rToolBarText->setButton(m_idButtonText_SuperScript,false);  
  if (!m_rToolBarText->isButtonOn(m_idButtonText_SubScript))
    vertAlign = KWFormat::VA_SUB;
  else
    vertAlign = KWFormat::VA_NORMAL;
  format.setVertAlign(vertAlign);
  gui->getPaperWidget()->formatChanged(format);
}

/*===============================================================*/
void KWordView_impl::textBorderLeft()
{
  if (!m_rToolBarText->isButtonOn(m_idButtonText_BorderLeft))
    left = tmpBrd;
  else
    left.ptWidth = 0;

  gui->getPaperWidget()->setParagLeftBorder(left);
}

/*===============================================================*/
void KWordView_impl::textBorderRight()
{
  if (!m_rToolBarText->isButtonOn(m_idButtonText_BorderRight))
    right = tmpBrd;
  else
    right.ptWidth = 0;

  gui->getPaperWidget()->setParagRightBorder(right);
}

/*===============================================================*/
void KWordView_impl::textBorderTop()
{
  if (!m_rToolBarText->isButtonOn(m_idButtonText_BorderTop))
    top = tmpBrd;
  else
    top.ptWidth = 0;

  gui->getPaperWidget()->setParagTopBorder(top);
}

/*===============================================================*/
void KWordView_impl::textBorderBottom()
{
  if (!m_rToolBarText->isButtonOn(m_idButtonText_BorderBottom))
    bottom = tmpBrd;
  else
    bottom.ptWidth = 0;

  gui->getPaperWidget()->setParagBottomBorder(bottom);
}

/*================================================================*/
void KWordView_impl::textBorderColor()
{
  if (KColorDialog::getColor(tmpBrd.color))
    m_rToolBarText->setButtonPixmap(m_idButtonText_BorderColor,CORBA::string_dup(colorToPixString(tmpBrd.color)));

//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderLeft))
//     {
//       left = tmpBrd;
//       gui->getPaperWidget()->setParagLeftBorder(left);
//     }
//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderRight))
//     {
//       right = tmpBrd;
//       gui->getPaperWidget()->setParagRightBorder(right);
//     }
//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderTop))
//     {
//       top = tmpBrd;
//       gui->getPaperWidget()->setParagTopBorder(top);
//     }
//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderBottom))
//     {
//       bottom = tmpBrd;
//       gui->getPaperWidget()->setParagBottomBorder(bottom);
//     }
}

/*================================================================*/
void KWordView_impl::textBorderWidth(const char *width)
{
  tmpBrd.ptWidth = atoi(width);

//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderLeft))
//     {
//       left = tmpBrd;
//       gui->getPaperWidget()->setParagLeftBorder(left);
//     }
//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderRight))
//     {
//       right = tmpBrd;
//       gui->getPaperWidget()->setParagRightBorder(right);
//     }
//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderTop))
//     {
//       top = tmpBrd;
//       gui->getPaperWidget()->setParagTopBorder(top);
//     }
//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderBottom))
//     {
//       bottom = tmpBrd;
//       gui->getPaperWidget()->setParagBottomBorder(bottom);
//     }
}

/*================================================================*/
void KWordView_impl::textBorderStyle(const char *style)
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

//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderLeft))
//     {
//       left = tmpBrd;
//       gui->getPaperWidget()->setParagLeftBorder(left);
//     }
//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderRight))
//     {
//       right = tmpBrd;
//       gui->getPaperWidget()->setParagRightBorder(right);
//     }
//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderTop))
//     {
//       top = tmpBrd;
//       gui->getPaperWidget()->setParagTopBorder(top);
//     }
//   if (m_rToolBarText->isButtonOn(m_idButtonText_BorderBottom))
//     {
//       bottom = tmpBrd;
//       gui->getPaperWidget()->setParagBottomBorder(bottom);
//     }
}

/*================================================================*/
void KWordView_impl::resizeEvent(QResizeEvent *e)
{
  QWidget::resizeEvent(e);
  gui->resize(width(),height());
}

/*================================================================*/
void KWordView_impl::keyPressEvent(QKeyEvent *e)
{
  if (gui) gui->keyEvent(e);
}

/*================================================================*/
void KWordView_impl::mousePressEvent(QMouseEvent *e)
{
}

/*================================================================*/
void KWordView_impl::mouseMoveEvent(QMouseEvent *e)
{
}

/*================================================================*/
void KWordView_impl::mouseReleaseEvent(QMouseEvent *e)
{
}

/*================================================================*/
void KWordView_impl::setupMenu()
{ 
  // Menubar
  m_rMenuBar = m_vMenuBarFactory->createMenuBar(this);
  
  // edit menu
  m_idMenuEdit = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&Edit")));
  QString tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/undo.xpm";
  QString pix = loadPixmap(tmp);
  m_idMenuEdit_Undo = m_rMenuBar->insertItemP(CORBA::string_dup(pix),CORBA::string_dup(i18n("No Undo possible")),m_idMenuEdit,
					      this,CORBA::string_dup("editUndo"));
  m_rMenuBar->setItemEnabled(m_idMenuEdit_Undo,false);
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/redo.xpm";
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

  m_rMenuBar->insertSeparator(m_idMenuEdit);
  m_idMenuEdit_Find = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Find...")),m_idMenuEdit,
					     this,CORBA::string_dup("editFind"));
  m_idMenuEdit_FindReplace = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Replace...")),m_idMenuEdit,
						    this,CORBA::string_dup("editFindReplace"));
  
  
  // View
  m_idMenuView = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&View")));
  m_idMenuView_NewView = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&New View")), m_idMenuView,
						this,CORBA::string_dup("newView"));

  // insert menu
  m_idMenuInsert = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&Insert")));

  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/picture.xpm";
  pix = loadPixmap(tmp);
  m_idMenuInsert_Picture = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						   CORBA::string_dup(i18n("&Picture...")),m_idMenuInsert,
						   this,CORBA::string_dup("insertPicture"));
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/clipart.xpm";
  pix = loadPixmap(tmp);
  m_idMenuInsert_Clipart = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						   CORBA::string_dup(i18n("&Clipart...")),m_idMenuInsert,
						   this,CORBA::string_dup("insertClipart"));
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/table.xpm";
  pix = loadPixmap(tmp);
  m_idMenuInsert_Table = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						 CORBA::string_dup(i18n("&Table...")),m_idMenuInsert,
						 this,CORBA::string_dup("insertTable"));
  tmp = kapp->kde_datadir().copy();
  tmp += "/kword/toolbar/parts.xpm";
  pix = loadPixmap(tmp);
  m_idMenuInsert_Table = m_rMenuBar->insertItemP(CORBA::string_dup(pix),
						 CORBA::string_dup(i18n("&Objects...")),m_idMenuInsert,
						 this,CORBA::string_dup("insertPart"));

  // format menu
  m_idMenuFormat = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&Format")));
  m_idMenuFormat_Font = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Font...")),m_idMenuFormat,
					      this,CORBA::string_dup("formatFont"));
  m_idMenuFormat_Color = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Color...")),m_idMenuFormat,
					       this,CORBA::string_dup("formatColor"));
  m_idMenuFormat_Paragraph = m_rMenuBar->insertItem(CORBA::string_dup(i18n("Paragraph...")),m_idMenuFormat,
						    this,CORBA::string_dup("formatParagraph"));
  m_idMenuFormat_Page = m_rMenuBar->insertItem(CORBA::string_dup(i18n("Page...")),m_idMenuFormat,
					       this,CORBA::string_dup("formatPage"));
  m_rMenuBar->insertSeparator(m_idMenuFormat);
  m_idMenuFormat_Numbering = m_rMenuBar->insertItem(CORBA::string_dup(i18n("Numbering...")),m_idMenuFormat,
						    this,CORBA::string_dup("formatNumbering"));
  m_idMenuFormat_Style = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Style...")),m_idMenuFormat,
						this,CORBA::string_dup("formatStyle"));

  // extra menu
  m_idMenuExtra = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&Extra")));
  m_idMenuExtra_Spelling = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Spelling...")),m_idMenuExtra,
						   this,CORBA::string_dup("extraSpelling"));
  m_idMenuExtra_Stylist = m_rMenuBar->insertItem(CORBA::string_dup(i18n("S&tylist...")),m_idMenuExtra,
						 this,CORBA::string_dup("extraStylist"));
  m_rMenuBar->insertSeparator(m_idMenuExtra);
  m_idMenuExtra_Options = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Options...")),m_idMenuExtra,
						 this,CORBA::string_dup("extraOptions"));

  // help menu
  m_idMenuHelp = m_rMenuBar->insertMenu(CORBA::string_dup(i18n("&Help")));
  m_idMenuHelp_Contents = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&Contents")),m_idMenuHelp,
						 this,CORBA::string_dup("helpContents"));
  m_rMenuBar->insertSeparator(m_idMenuHelp);
  m_idMenuHelp_About = m_rMenuBar->insertItem(CORBA::string_dup(i18n("&About KWord...")),m_idMenuHelp,
					      this,CORBA::string_dup("helpAbout"));
  m_idMenuHelp_AboutKOffice = m_rMenuBar->insertItem(CORBA::string_dup(i18n("About K&Office...")),m_idMenuHelp,
						     this,CORBA::string_dup("helpAboutKOffice"));
  m_idMenuHelp_AboutKDE = m_rMenuBar->insertItem(CORBA::string_dup(i18n("About &KDE...")),m_idMenuHelp,
						 this,CORBA::string_dup("helpAboutKDE"));
}

/*======================= setup edit toolbar ===================*/
void KWordView_impl::setupEditToolbar()
{
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if (!CORBA::is_nil(m_vToolBarFactory))
    {
      // toolbar
      m_rToolBarEdit = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(i18n("Edit")));
      m_rToolBarEdit->setFullWidth(false);

      // undo
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/undo.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonEdit_Undo = m_rToolBarEdit->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Undo")),
							 this,CORBA::string_dup("editUndo"));
      m_rToolBarEdit->setItemEnabled(m_idButtonEdit_Undo,false);

      // redo
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/redo.xpm";
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
    }
}

/*======================= setup insert toolbar =================*/
void KWordView_impl::setupInsertToolbar()
{
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if (!CORBA::is_nil(m_vToolBarFactory))
    {
      // toolbar
      m_rToolBarInsert = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(i18n("Insert")));
      m_rToolBarInsert->setFullWidth(false);
 
      // picture
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/picture.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonInsert_Picture = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
								CORBA::string_dup(i18n("Insert Picture")),
								this,CORBA::string_dup("insertPicture"));
      
      // clipart
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/clipart.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Clipart = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
								CORBA::string_dup(i18n("Insert Clipart")),
								this,CORBA::string_dup("insertClipart"));

      // line
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/table.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Table = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
							      CORBA::string_dup(i18n("Insert Table")),
							      this,CORBA::string_dup("insertTable"));
      // parts
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/parts.xpm";
      pix = loadPixmap(tmp);
      m_idButtonInsert_Part = m_rToolBarInsert->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Insert Object")),
							     this,CORBA::string_dup("insertPart"));
   }
}

/*======================= setup text toolbar ===================*/
void KWordView_impl::setupTextToolbar()
{
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if (!CORBA::is_nil(m_vToolBarFactory))
    {
      // toolbar
      m_rToolBarText = m_vToolBarFactory->createToolBar(this,CORBA::string_dup(i18n("Text")));
      m_rToolBarText->setFullWidth(false);

      // style combobox
      m_idComboText_Style = m_rToolBarText->insertCombo(false,CORBA::string_dup(i18n("Style")),200,
							this,CORBA::string_dup("textStyleSelected"));

      // size combobox
      m_idComboText_FontSize = m_rToolBarText->insertCombo(true,CORBA::string_dup(i18n("Font Size")),60,
							   this,CORBA::string_dup("textSizeSelected"));
      for(unsigned int i = 4;i <= 100;i++)
	{
	  char buffer[10];
	  sprintf(buffer,"%i",i);
	  m_rToolBarText->insertComboItem(m_idComboText_FontSize,CORBA::string_dup(buffer),-1);
	}
      m_rToolBarText->setCurrentComboItem(m_idComboText_FontSize,8);
      tbFont.setPointSize(12);

      // fonts combobox
      getFonts();
      m_idComboText_FontList = m_rToolBarText->insertCombo(true,CORBA::string_dup(i18n("Font List")),200,
							   this,CORBA::string_dup("textFontSelected"));
      for(unsigned int i = 0;i <= fontList.count()-1;i++)
 	m_rToolBarText->insertComboItem(m_idComboText_FontList,CORBA::string_dup(fontList.at(i)),-1);
      m_rToolBarText->setCurrentComboItem(m_idComboText_FontList,1);
      m_rToolBarText->setCurrentComboItem(m_idComboText_FontList,0);
      m_rToolBarText->insertSeparator();
      tbFont.setFamily(fontList.at(0));

      // bold
      QString tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/bold.xpm";
      QString pix = loadPixmap(tmp);
      m_idButtonText_Bold = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Bold")),
							 this,CORBA::string_dup("textBold"));
      m_rToolBarText->setToggle(m_idButtonText_Bold,true);
      m_rToolBarText->setButton(m_idButtonText_Bold,false);
      tbFont.setBold(false);

      // italic
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/italic.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_Italic = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Italic")),
							   this,CORBA::string_dup("textItalic"));
      m_rToolBarText->setToggle(m_idButtonText_Italic,true);
      m_rToolBarText->setButton(m_idButtonText_Italic,false);
      tbFont.setItalic(false);

      // underline
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/underl.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_Underline = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Underline")),
							      this,CORBA::string_dup("textUnderline"));
      m_rToolBarText->setToggle(m_idButtonText_Underline,true);
      m_rToolBarText->setButton(m_idButtonText_Underline,false);
      tbFont.setUnderline(false);

      // color
      m_idButtonText_Color = m_rToolBarText->insertButton(CORBA::string_dup(""),CORBA::string_dup(i18n("Text Color")),
							  this,CORBA::string_dup("textColor"));
      m_rToolBarText->setButtonPixmap(m_idButtonText_Color,CORBA::string_dup(colorToPixString(black)));
      tbColor = black;
      m_rToolBarText->insertSeparator();

      // align left
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/alignLeft.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_ALeft = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Align Left")),
							  this,CORBA::string_dup("textAlignLeft"));
      m_rToolBarText->setToggle(m_idButtonText_ALeft,true);
      m_rToolBarText->setButton(m_idButtonText_ALeft,true);

      // align center
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/alignCenter.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_ACenter = m_rToolBarText->insertButton(CORBA::string_dup(pix),
							    CORBA::string_dup(i18n("Align Center")),
							    this,CORBA::string_dup("textAlignCenter"));
      m_rToolBarText->setToggle(m_idButtonText_ACenter,true);
      m_rToolBarText->setButton(m_idButtonText_ACenter,false);

      // align right
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/alignRight.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_ARight = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Align Right")),
							   this,CORBA::string_dup("textAlignRight"));
      m_rToolBarText->setToggle(m_idButtonText_ARight,true);
      m_rToolBarText->setButton(m_idButtonText_ARight,false);

      // align block
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/alignBlock.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_ABlock = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Align Block")),
							   this,CORBA::string_dup("textAlignBlock"));
      m_rToolBarText->setToggle(m_idButtonText_ABlock,true);
      m_rToolBarText->setButton(m_idButtonText_ABlock,false);

      m_rToolBarText->insertSeparator();

      // enum list
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/enumList.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_EnumList = m_rToolBarText->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Enumerated List")),
							     this,CORBA::string_dup("textEnumList"));
      // unsorted list
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/unsortedList.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_EnumList = m_rToolBarText->insertButton(CORBA::string_dup(pix),
							     CORBA::string_dup(i18n("Unsorted List")),
							     this,CORBA::string_dup("textUnsortList"));

      m_rToolBarText->insertSeparator();

      // superscript
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/super.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_SuperScript = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Superscript")),
								this,CORBA::string_dup("textSuperScript"));
      m_rToolBarText->setToggle(m_idButtonText_SuperScript,true);
      m_rToolBarText->setButton(m_idButtonText_SuperScript,false);

      // subscript
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/sub.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_SubScript = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Subscript")),
							      this,CORBA::string_dup("textSubScript"));
      m_rToolBarText->setToggle(m_idButtonText_SubScript,true);
      m_rToolBarText->setButton(m_idButtonText_SubScript,false);

      m_rToolBarText->insertSeparator();

      // border left
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/borderleft.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_BorderLeft = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Border Left")),
							       this,CORBA::string_dup("textBorderLeft"));
      m_rToolBarText->setToggle(m_idButtonText_BorderLeft,true);
      m_rToolBarText->setButton(m_idButtonText_BorderLeft,false);

      // border right
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/borderright.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_BorderRight = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Border Right")),
								this,CORBA::string_dup("textBorderRight"));
      m_rToolBarText->setToggle(m_idButtonText_BorderRight,true);
      m_rToolBarText->setButton(m_idButtonText_BorderRight,false);

      // border top
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/bordertop.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_BorderTop = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Border Top")),
							      this,CORBA::string_dup("textBorderTop"));
      m_rToolBarText->setToggle(m_idButtonText_BorderTop,true);
      m_rToolBarText->setButton(m_idButtonText_BorderTop,false);

      // border bottom
      tmp = kapp->kde_datadir().copy();
      tmp += "/kword/toolbar/borderbottom.xpm";
      pix = loadPixmap(tmp);
      m_idButtonText_BorderBottom = m_rToolBarText->insertButton(CORBA::string_dup(pix),CORBA::string_dup(i18n("Border Bottom")),
								 this,CORBA::string_dup("textBorderBottom"));
      m_rToolBarText->setToggle(m_idButtonText_BorderBottom,true);
      m_rToolBarText->setButton(m_idButtonText_BorderBottom,false);

      // border color
      m_idButtonText_BorderColor = m_rToolBarText->insertButton(CORBA::string_dup(""),CORBA::string_dup(i18n("Border Color")),
								this,CORBA::string_dup("textBorderColor"));
      tmpBrd.color = black;
      m_rToolBarText->setButtonPixmap(m_idButtonText_BorderColor,CORBA::string_dup(colorToPixString(black)));

      // border width combobox
      m_idComboText_BorderWidth = m_rToolBarText->insertCombo(true,CORBA::string_dup(i18n("Border Width")),60,
							      this,CORBA::string_dup("textBorderWidth"));
      for(unsigned int i = 1;i <= 10;i++)
	{
	  char buffer[10];
	  sprintf(buffer,"%i",i);
	  m_rToolBarText->insertComboItem(m_idComboText_BorderWidth,CORBA::string_dup(buffer),-1);
	}
      tmpBrd.ptWidth = 1;

      // border style combobox
      m_idComboText_BorderStyle = m_rToolBarText->insertCombo(false,CORBA::string_dup(i18n("Border Style")),150,
							      this,CORBA::string_dup("textBorderStyle"));
      m_rToolBarText->insertComboItem(m_idComboText_BorderStyle,CORBA::string_dup(i18n("solid line")),-1);
      m_rToolBarText->insertComboItem(m_idComboText_BorderStyle,CORBA::string_dup(i18n("dash line (----)")),-1);
      m_rToolBarText->insertComboItem(m_idComboText_BorderStyle,CORBA::string_dup(i18n("dot line (****)")),-1);
      m_rToolBarText->insertComboItem(m_idComboText_BorderStyle,CORBA::string_dup(i18n("dash dot line (-*-*)")),-1);
      m_rToolBarText->insertComboItem(m_idComboText_BorderStyle,CORBA::string_dup(i18n("dash dot dot line (-**-)")),-1);
      tmpBrd.style = KWParagLayout::SOLID;
    }
}      

/*============== create a pixmapstring from a color ============*/
char* KWordView_impl::colorToPixString(QColor c)
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
void KWordView_impl::getFonts()
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
void KWordView_impl::setParagBorderValues()
{
  m_rToolBarText->setCurrentComboItem(m_idComboText_BorderWidth,tmpBrd.ptWidth - 1);
  m_rToolBarText->setCurrentComboItem(m_idComboText_BorderStyle,static_cast<int>(tmpBrd.style));
  m_rToolBarText->setButtonPixmap(m_idButtonText_BorderColor,CORBA::string_dup(colorToPixString(tmpBrd.color)));
}

/*================================================================*/
void KWordView_impl::slotInsertObject(KWordChild *_child)
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
      printf("void KWordView_impl::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n");
      printf("Could not create view\n");
      exit(1);
    }
  
  if (CORBA::is_nil(v))
    {
      printf("void KWordView_impl::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n");
      printf("return value is 0L\n");
      exit(1);
    }

  v->setMode(OPParts::Part::ChildMode);
  v->setPartShell(partShell());

  KWordFrame *p = new KWordFrame(this,_child);
  p->attach(v);
  p->setGeometry(_child->geometry());
  p->show();
  m_lstFrames.append(p);
  
  QObject::connect(p,SIGNAL(sig_geometryEnd(PartFrame_impl*)),
		   this,SLOT(slotGeometryEnd(PartFrame_impl*)));
  QObject::connect(p,SIGNAL(sig_moveEnd(PartFrame_impl*)),
		   this,SLOT(slotMoveEnd(PartFrame_impl*)));  
} 

/*================================================================*/
void KWordView_impl::slotUpdateChildGeometry(KWordChild *_child)
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
void KWordView_impl::slotGeometryEnd(PartFrame_impl* _frame)
{
  KWordFrame *f = (KWordFrame*)_frame;
  // TODO scaling
  m_pKWordDoc->changeChildGeometry(f->child(),_frame->partGeometry());
}

/*================================================================*/
void KWordView_impl::slotMoveEnd(PartFrame_impl* _frame)
{
  KWordFrame *f = (KWordFrame*)_frame;
  // TODO scaling
  m_pKWordDoc->changeChildGeometry(f->child(),_frame->partGeometry());
}

/*================================================================*/
void KWordView_impl::paragDiaOk()
{
  gui->getPaperWidget()->setLeftIndent(paragDia->getLeftIndent());
  gui->getPaperWidget()->setFirstLineIndent(paragDia->getFirstLineIndent());
  gui->getPaperWidget()->setSpaceBeforeParag(paragDia->getSpaceBeforeParag());
  gui->getPaperWidget()->setSpaceAfterParag(paragDia->getSpaceAfterParag());
  gui->getPaperWidget()->setLineSpacing(paragDia->getLineSpacing());
  gui->getHorzRuler()->setLeftIndent(static_cast<int>(paragDia->getLeftIndent()));
  gui->getHorzRuler()->setFirstIndent(static_cast<int>(paragDia->getFirstLineIndent()));
  gui->getPaperWidget()->setFlow(paragDia->getFlow());
  setFlow(paragDia->getFlow());
}

/*================================================================*/
void KWordView_impl::newPageLayout(KoPageLayout _layout)
{
  KoPageLayout pgLayout;
  KoColumns cl;
  m_pKWordDoc->getPageLayout(pgLayout,cl);

  m_pKWordDoc->setPageLayout(_layout,cl);
  gui->getHorzRuler()->setPageLayout(_layout);
  gui->getVertRuler()->setPageLayout(_layout);

  if (m_pKWordDoc->getProcessingType() == KWordDocument_impl::DTP)
    gui->getPaperWidget()->frameSizeChanged(_layout);
}

/*================================================================*/
void KWordView_impl::setMode(OPParts::Part::Mode _mode)
{
  Part_impl::setMode(_mode);
  
  if (mode() == OPParts::Part::ChildMode && !m_bFocus)
    m_bShowGUI = false;
  else
    m_bShowGUI = true;

  if (gui) gui->showGUI(m_bShowGUI);
}

/*================================================================*/
void KWordView_impl::setFocus(CORBA::Boolean _mode)
{
  Part_impl::setFocus(_mode);

  bool old = m_bShowGUI;
  
  if (mode() == OPParts::Part::ChildMode && !m_bFocus)
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
KWordGUI::KWordGUI(QWidget *parent,bool __show,KWordDocument_impl *_doc,KWordView_impl *_view)
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

  if (doc->getProcessingType() == KWordDocument_impl::DTP)
    paperWidget->setRuler2Frame(0,0);

  // HACK
  if (doc->getNumViews() == 1)
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

