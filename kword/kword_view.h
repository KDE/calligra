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
/* Module: View (header)                                          */
/******************************************************************/

#ifndef kword_view_h
#define kword_view_h

class KWordView;
class KWordDocument;
class KWordChild;
class KWordGUI;
class KWPaintWindow;

#include <koView.h>
#include <koDocument.h>
#include <koFrame.h>
#include <opMenuIf.h>
#include <opToolBarIf.h>

#include <qpixmap.h>
#include <qwidget.h>
#include <krect.h>
#include <qlist.h>
#include <qevent.h>
#include <qscrollbar.h>
#include <qcolor.h>
#include <qfont.h>
#include <qmessagebox.h>
#include <qclipboard.h>

#include <koRuler.h>
#include <kcolordlg.h>
#include <koTabChooser.h>

#include "kword.h"
#include "kword_page.h"
#include "format.h"
#include "paraglayout.h"
#include "paragdia.h"
#include "stylist.h"

#include <koPageLayoutDia.h>

/******************************************************************/
/* Class: KWordFrame                                              */
/******************************************************************/

class KWordFrame : public KoFrame
{
  Q_OBJECT

public:
  KWordFrame(KWordView*,KWordChild*);
  
  KWordChild* child() 
    { return m_pKWordChild; }
  KWordView* wordView() 
    { return m_pKWordView; }
  
protected:
  KWordChild *m_pKWordChild;
  KWordView *m_pKWordView;

};

/******************************************************************/
/* Class: KWordView                                               */
/******************************************************************/

class KWordView : public QWidget,
		  virtual public KoViewIf,
		  virtual public KWord::KWordView_skel
{
  Q_OBJECT

public:
  // C++
  KWordView( QWidget *_parent, const char *_name, KWordDocument *_doc );
  virtual ~KWordView();

  // IDL  
  virtual void editUndo();
  virtual void editRedo();
  virtual void editCut();
  virtual void editCopy();
  virtual void editPaste();
  virtual void editSelectAll();
  virtual void editFind();
  virtual void editFindReplace();

  virtual void newView();

  virtual void insertPicture();
  virtual void insertClipart();
  virtual void insertSpecialChar();

  virtual void formatFont();
  virtual void formatColor();
  virtual void formatParagraph();
  virtual void formatPage();
  virtual void formatNumbering();
  virtual void formatStyle();

  virtual void extraSpelling();
  virtual void extraStylist();
  virtual void extraOptions();

  virtual void toolsEdit();
  virtual void toolsEditFrame();
  virtual void toolsCreateText();
  virtual void toolsCreatePix();
  virtual void toolsClipart();
  virtual void toolsTable();
  virtual void toolsFormula();
  virtual void toolsPart();

  virtual void helpContents();
  virtual void helpAbout();
  virtual void helpAboutKOffice();
  virtual void helpAboutKDE();

  virtual void textStyleSelected(const char *style);
  virtual void textSizeSelected(const char *size);
  virtual void textFontSelected(const char *font);
  virtual void textBold();
  virtual void textItalic();
  virtual void textUnderline();
  virtual void textColor();
  virtual void textAlignLeft();
  virtual void textAlignCenter();
  virtual void textAlignRight();
  virtual void textAlignBlock();
  virtual void textEnumList();
  virtual void textUnsortList();
  virtual void textSuperScript();
  virtual void textSubScript();
  virtual void textBorderLeft();
  virtual void textBorderRight();
  virtual void textBorderTop();
  virtual void textBorderBottom();
  virtual void textBorderColor();
  virtual void textBorderWidth(const char *width);
  virtual void textBorderStyle(const char *style);

  virtual void setMode( KOffice::View::Mode _mode);
  virtual void setFocus(CORBA::Boolean mode);

  // C++
  virtual CORBA::Boolean printDlg();

  virtual void setFormat(KWFormat &_format,bool _check = true,bool _update_page = true,bool _redraw = true);
  virtual void setFlow(KWParagLayout::Flow _flow);
  virtual void setParagBorders(KWParagLayout::Border _left,KWParagLayout::Border _right,
			       KWParagLayout::Border _top,KWParagLayout::Border _bottom);

  KWordGUI *getGUI() { return gui; }
  void uncheckAllTools();
  void setTool(MouseMode _mouseMode);
  void updateStyle(QString _styleName,bool _updateFormat = true);
  void updateStyleList();

public slots:
  void slotInsertObject(KWordChild *_child);
  void slotUpdateChildGeometry(KWordChild *_child);
  void slotGeometryEnd( KoFrame*);
  void slotMoveEnd( KoFrame*);
  void paragDiaOk();
  void styleManagerOk();
  void openPageLayoutDia()
    { formatPage(); }
  void newPageLayout(KoPageLayout _layout);
  
protected:
  // C++
  virtual void init();
  // IDL
  virtual bool event( const char* _event, const CORBA::Any& _value );
  // C++
  bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
  bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );

  virtual void cleanUp();
  
  void resizeEvent(QResizeEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);

  char* colorToPixString(QColor);
  void getFonts();
  void setParagBorderValues();

  KWordDocument *m_pKWordDoc;

  bool m_bUnderConstruction;
  
  // edit menu
  OpenPartsUI::Menu_var m_vMenuEdit;
  CORBA::Long m_idMenuEdit_Undo;
  CORBA::Long m_idMenuEdit_Redo;
  CORBA::Long m_idMenuEdit_Cut;
  CORBA::Long m_idMenuEdit_Copy;
  CORBA::Long m_idMenuEdit_Paste;
  CORBA::Long m_idMenuEdit_SelectAll;
  CORBA::Long m_idMenuEdit_Find;
  CORBA::Long m_idMenuEdit_FindReplace;

  // view menu
  OpenPartsUI::Menu_var m_vMenuView;
  CORBA::Long m_idMenuView_NewView;
 
  // insert menu
  OpenPartsUI::Menu_var m_vMenuInsert;
  CORBA::Long m_idMenuInsert_Picture;
  CORBA::Long m_idMenuInsert_Clipart;
  CORBA::Long m_idMenuInsert_SpecialChar;
 
  // format menu
  OpenPartsUI::Menu_var m_vMenuFormat;
  CORBA::Long m_idMenuFormat_Font;
  CORBA::Long m_idMenuFormat_Color;
  CORBA::Long m_idMenuFormat_Paragraph;
  CORBA::Long m_idMenuFormat_Page;
  CORBA::Long m_idMenuFormat_Numbering;
  CORBA::Long m_idMenuFormat_Style;

  // extra menu
  OpenPartsUI::Menu_var m_vMenuExtra;
  CORBA::Long m_idMenuExtra_Spelling;
  CORBA::Long m_idMenuExtra_Stylist;
  CORBA::Long m_idMenuExtra_Options;

  // tools menu
  OpenPartsUI::Menu_var m_vMenuTools;
  CORBA::Long m_idMenuTools_Edit;
  CORBA::Long m_idMenuTools_EditFrame;
  CORBA::Long m_idMenuTools_CreateText;
  CORBA::Long m_idMenuTools_CreatePix;
  CORBA::Long m_idMenuTools_Clipart;
  CORBA::Long m_idMenuTools_Table;
  CORBA::Long m_idMenuTools_Formula;
  CORBA::Long m_idMenuTools_Part;

  // help menu
  OpenPartsUI::Menu_var m_vMenuHelp;
  CORBA::Long m_idMenuHelp_Contents;

  // edit toolbar
  OpenPartsUI::ToolBar_var m_vToolBarEdit;
  CORBA::Long m_idButtonEdit_Undo;
  CORBA::Long m_idButtonEdit_Redo;
  CORBA::Long m_idButtonEdit_Cut;
  CORBA::Long m_idButtonEdit_Copy;
  CORBA::Long m_idButtonEdit_Paste;

  // insert toolbar
  OpenPartsUI::ToolBar_var m_vToolBarInsert;
  CORBA::Long m_idButtonInsert_Picture;
  CORBA::Long m_idButtonInsert_Clipart;
  CORBA::Long m_idButtonInsert_SpecialChar;

  // text toolbar
  OpenPartsUI::ToolBar_var m_vToolBarText;
  CORBA::Long m_idComboText_Style;
  CORBA::Long m_idComboText_FontSize;
  CORBA::Long m_idComboText_FontList;
  CORBA::Long m_idButtonText_Bold;
  CORBA::Long m_idButtonText_Italic;
  CORBA::Long m_idButtonText_Underline;
  CORBA::Long m_idButtonText_Color;
  CORBA::Long m_idButtonText_ARight;
  CORBA::Long m_idButtonText_ACenter;
  CORBA::Long m_idButtonText_ALeft;
  CORBA::Long m_idButtonText_ABlock;
  CORBA::Long m_idButtonText_EnumList;
  CORBA::Long m_idButtonText_UnsortList;
  CORBA::Long m_idButtonText_SuperScript;
  CORBA::Long m_idButtonText_SubScript;
  CORBA::Long m_idButtonText_BorderLeft;
  CORBA::Long m_idButtonText_BorderRight;
  CORBA::Long m_idButtonText_BorderTop;
  CORBA::Long m_idButtonText_BorderBottom;
  CORBA::Long m_idButtonText_BorderColor;
  CORBA::Long m_idComboText_BorderWidth;
  CORBA::Long m_idComboText_BorderStyle;

  // tools toolbar
  OpenPartsUI::ToolBar_var m_vToolBarTools;
  CORBA::Long m_idButtonTools_Edit;
  CORBA::Long m_idButtonTools_EditFrame;
  CORBA::Long m_idButtonTools_CreateText;
  CORBA::Long m_idButtonTools_CreatePix;
  CORBA::Long m_idButtonTools_Clipart;
  CORBA::Long m_idButtonTools_Table;
  CORBA::Long m_idButtonTools_Formula;
  CORBA::Long m_idButtonTools_Part;

  QList<KWordFrame> m_lstFrames;

  // text toolbar values
  QFont tbFont;
  QColor tbColor;
  QStrList fontList,styleList;

  KWordGUI *gui;
  bool m_bShowGUI;

  KWFormat format;
  KWParagLayout::Flow flow;
  KWFormat::VertAlign vertAlign;
  KWParagLayout::Border left,right,top,bottom,tmpBrd;

  KWParagDia *paragDia;
  KWStyleManager *styleManager;

  static const int ID_TOOL_EDIT = 2;
  static const int ID_TOOL_EDIT_FRAME = 3;
  static const int ID_TOOL_CREATE_TEXT = 4;
  static const int ID_TOOL_CREATE_PIX = 5;
  static const int ID_TEXT_COLOR = 6;
  static const int ID_BORDER_COLOR = 7;
  static const int ID_FONT_SIZE = 8;
  static const int ID_FONT_LIST = 9;
  static const int ID_STYLE_LIST = 10;
  static const int ID_BOLD = 11;
  static const int ID_ITALIC = 12;
  static const int ID_UNDERLINE = 13;
  static const int ID_ALEFT = 14;
  static const int ID_ACENTER = 15;
  static const int ID_ARIGHT = 16;
  static const int ID_ABLOCK = 17;
  static const int ID_BRD_LEFT = 18;
  static const int ID_BRD_RIGHT = 19;
  static const int ID_BRD_TOP = 20;
  static const int ID_BRD_BOTTOM = 21;
  static const int ID_SUBSCRIPT = 22;
  static const int ID_SUPERSCRIPT = 23;
  static const int ID_BRD_STYLE = 24;
  static const int ID_BRD_WIDTH = 25;
  static const int ID_REDO = 26;
  static const int ID_UNDO = 27;
  static const int ID_ENUM_LIST = 28;
  static const int ID_USORT_LIST = 29;
  static const int ID_TOOL_CREATE_TABLE = 30;
  static const int ID_TOOL_CREATE_FORMULA = 31;
  static const int ID_TOOL_CREATE_PART = 32;
  static const int ID_TOOL_CREATE_CLIPART = 33;

};

/******************************************************************/
/* Class: KWordGUI                                                */
/******************************************************************/

class KWordGUI : public QWidget
{
  Q_OBJECT

public:
  KWordGUI( QWidget *parent, bool __show, KWordDocument *_doc, KWordView *_view );
  
  KWordDocument *getDocument()
    { return doc; }

  void showGUI(bool __show);

  void setDocument( KWordDocument *_doc )
    { doc = _doc; paperWidget->setDocument(doc); }

  QScrollBar *getVertScrollBar()
    { return s_vert; }
  QScrollBar *getHorzScrollBar()
    { return s_horz; }
  KWordView *getView()
    { return view; }
  KWPage *getPaperWidget()
    { return paperWidget; }
  KoRuler *getVertRuler()
    { return r_vert; }
  KoRuler *getHorzRuler()
    { return r_horz; }
  KoTabChooser *getTabChooser()
    { return tabChooser; }
  
  void setOffset(int _x,int _y)
    { xOffset = _x; yOffset = _y; }

  void keyEvent(QKeyEvent *e)
    { keyPressEvent(e); }

  void setRanges();

  void scrollTo(int _x,int _y)
    { if (_x != xOffset) scrollH(_x); if (_y != yOffset) scrollV(_y); }

protected slots:
  void scrollH(int);
  void scrollV(int);

protected:
  void resizeEvent(QResizeEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void reorganize();

  int xOffset,yOffset;
  bool _show;
  QScrollBar *s_vert,*s_horz;
  KoRuler *r_vert,*r_horz; 
  KWPage *paperWidget;
  KWordDocument *doc;
  KWordView *view;
  KoTabChooser *tabChooser;
};

#endif
