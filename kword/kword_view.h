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
class KWordShell;

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
#include <qbrush.h>
#include <qdropsite.h>

#include <koRuler.h>
#include <kcolordlg.h>
#include <koTabChooser.h>
#include <knewpanner.h>

#include "kword.h"
#include "kword_page.h"
#include "format.h"
#include "paraglayout.h"
#include "paragdia.h"
#include "parag.h"
#include "frame.h"
#include "stylist.h"
#include "searchdia.h"
#include "tabledia.h"
#include "insdia.h"
#include "deldia.h"
#include "docstruct.h"
#include "variable.h"
#include "footnotedia.h"
#include "autoformatdia.h"

#include <koPageLayoutDia.h>
#include <koPartSelectDia.h>

#include <kspell.h>

class KWPartFrameSet;

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

  void setPartObject(KWPartFrameSet *o) { obj = o; }
  KWPartFrameSet *getPartObject() { return obj; }

protected:
  KWordChild *m_pKWordChild;
  KWordView *m_pKWordView;
  KWPartFrameSet *obj;

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
  KWordView(QWidget *_parent,const char *_name,KWordDocument *_doc);
  virtual ~KWordView();

  void setShell(KWordShell *_shell) { shell = _shell; }

  // IDL
  virtual void editUndo();
  virtual void editRedo();
  virtual void editCut();
  virtual void editCopy();
  virtual void editPaste();
  virtual void editSelectAll();
  virtual void editFind();

  virtual void newView();
  virtual void viewFormattingChars();
  virtual void viewFrameBorders();
  virtual void viewTableGrid();
  virtual void viewHeader();
  virtual void viewFooter();
  virtual void viewDocStruct();
  virtual void viewFootNotes();
  virtual void viewEndNotes();

  virtual void insertPicture();
  virtual void insertClipart();
  virtual void insertSpecialChar();
  virtual void insertFrameBreak();
  virtual void insertVariableDateFix();
  virtual void insertVariableDateVar();
  virtual void insertVariableTimeFix();
  virtual void insertVariableTimeVar();
  virtual void insertVariablePageNum();
  virtual void insertVariableOther();
  virtual void insertFootNoteEndNote();

  virtual void formatFont();
  virtual void formatColor();
  virtual void formatParagraph();
  virtual void formatPage();
  virtual void formatNumbering();
  virtual void formatStyle();
  virtual void formatFrameSet();

  virtual void extraSpelling();
  virtual void extraAutoFormat();
  virtual void extraStylist();
  virtual void extraOptions();

  virtual void toolsEdit();
  virtual void toolsEditFrame();
  virtual void toolsCreateText();
  virtual void toolsCreatePix();
  virtual void toolsClipart();
  virtual void toolsTable();
  virtual void toolsKSpreadTable();
  virtual void toolsFormula();
  virtual void toolsPart();

  virtual void tableInsertRow();
  virtual void tableInsertCol();
  virtual void tableDeleteRow();
  virtual void tableDeleteCol();
  virtual void tableJoinCells();
  virtual void tableSplitCells();
  virtual void tableUngroupTable();

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
  virtual void textLineSpacing(const char *spc);
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
  virtual void frameBorderLeft();
  virtual void frameBorderRight();
  virtual void frameBorderTop();
  virtual void frameBorderBottom();
  virtual void frameBorderColor();
  virtual void frameBorderWidth(const char *width);
  virtual void frameBorderStyle(const char *style);
  virtual void frameBackColor();

  virtual void setMode(KOffice::View::Mode _mode);
  virtual void setFocus(CORBA::Boolean mode);

  // C++
  virtual CORBA::Boolean printDlg();

  virtual void setFormat(KWFormat &_format,bool _check = true,bool _update_page = true,bool _redraw = true);
  virtual void setFlow(KWParagLayout::Flow _flow);
  virtual void setLineSpacing(int _spc);
  virtual void setParagBorders(KWParagLayout::Border _left,KWParagLayout::Border _right,
			       KWParagLayout::Border _top,KWParagLayout::Border _bottom);

  KWordGUI *getGUI() { return gui; }
  void uncheckAllTools();
  void setTool(MouseMode _mouseMode);
  void updateStyle(QString _styleName,bool _updateFormat = true);
  void updateStyleList();

  bool getViewFormattingChars() { return _viewFormattingChars; }
  bool getViewFrameBorders() { return _viewFrameBorders; }
  bool getViewTableGrid() { return _viewTableGrid; }

  void setFramesToParts();
  void hideAllFrames();

  void setSearchEntry(KWSearchDia::KWSearchEntry *e) { searchEntry = e; }
  void setReplaceEntry(KWSearchDia::KWSearchEntry *e) { replaceEntry = e; }

  void changeUndo(QString,bool);
  void changeRedo(QString,bool);

  void sendFocusEvent();

public slots:
  void slotInsertObject(KWordChild *_child,KWPartFrameSet *_kwpf);
  void slotUpdateChildGeometry(KWordChild *_child);
  void slotGeometryEnd(KoFrame*);
  void slotMoveEnd(KoFrame*);
  void paragDiaOk();
  void styleManagerOk();
  void openPageLayoutDia()
    { formatPage(); }
  void newPageLayout(KoPageLayout _layout);
  void spellCheckerReady();
  void spellCheckerMisspelling(char*,QStrList*,unsigned);
  void spellCheckerCorrected(char*,char*,unsigned);
  void spellCheckerDone(char*);
  void searchDiaClosed();

  void clipboardDataChanged();
  void selectionOnOff();

protected:
  // C++
  virtual void init();
  // IDL
  virtual bool event(const char* _event,const CORBA::Any& _value);
  // C++
  bool mappingCreateMenubar(OpenPartsUI::MenuBar_ptr _menubar);
  bool mappingCreateToolbar(OpenPartsUI::ToolBarFactory_ptr _factory);

  virtual void cleanUp();

  void resizeEvent(QResizeEvent *e);
  void keyPressEvent(QKeyEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void focusInEvent(QFocusEvent *e);
  virtual void dragEnterEvent(QDragEnterEvent *e);
  virtual void dragMoveEvent(QDragMoveEvent *e);
  virtual void dragLeaveEvent(QDragLeaveEvent *e);
  virtual void dropEvent(QDropEvent *e);

  enum PType {TXT_COLOR,FRAME_COLOR,BACK_COLOR};
  QString colorToPixString(QColor,PType _type);
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

  // view menu
  OpenPartsUI::Menu_var m_vMenuView;
  CORBA::Long m_idMenuView_NewView;
  CORBA::Long m_idMenuView_FormattingChars;
  CORBA::Long m_idMenuView_FrameBorders;
  CORBA::Long m_idMenuView_TableGrid;
  CORBA::Long m_idMenuView_Header;
  CORBA::Long m_idMenuView_Footer;
  CORBA::Long m_idMenuView_DocStruct;
  CORBA::Long m_idMenuView_FootNotes;
  CORBA::Long m_idMenuView_EndNotes;

  // insert menu
  OpenPartsUI::Menu_var m_vMenuInsert;
  CORBA::Long m_idMenuInsert_Picture;
  CORBA::Long m_idMenuInsert_Clipart;
  CORBA::Long m_idMenuInsert_SpecialChar;
  CORBA::Long m_idMenuInsert_FrameBreak;
  CORBA::Long m_idMenuInsert_Variable;
  OpenPartsUI::Menu_var m_vMenuInsert_Variable;
  CORBA::Long m_idMenuInsert_VariableDateFix;
  CORBA::Long m_idMenuInsert_VariableDateVar;
  CORBA::Long m_idMenuInsert_VariableTimeFix;
  CORBA::Long m_idMenuInsert_VariableTimeVar;
  CORBA::Long m_idMenuInsert_VariablePageNum;
  CORBA::Long m_idMenuInsert_VariableOther;
  CORBA::Long m_idMenuInsert_FootNoteEndNote;

  // format menu
  OpenPartsUI::Menu_var m_vMenuFormat;
  CORBA::Long m_idMenuFormat_Font;
  CORBA::Long m_idMenuFormat_Color;
  CORBA::Long m_idMenuFormat_Paragraph;
  CORBA::Long m_idMenuFormat_Page;
  CORBA::Long m_idMenuFormat_Numbering;
  CORBA::Long m_idMenuFormat_Style;
  CORBA::Long m_idMenuFormat_FrameSet;

  // extra menu
  OpenPartsUI::Menu_var m_vMenuExtra;
  CORBA::Long m_idMenuExtra_Spelling;
  CORBA::Long m_idMenuExtra_AutoFormat;
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
  CORBA::Long m_idMenuTools_KSpreadTable;
  CORBA::Long m_idMenuTools_Formula;
  CORBA::Long m_idMenuTools_Part;

  // table menu
  OpenPartsUI::Menu_var m_vMenuTable;
  CORBA::Long m_idMenuTable_InsertRow;
  CORBA::Long m_idMenuTable_InsertCol;
  CORBA::Long m_idMenuTable_DeleteRow;
  CORBA::Long m_idMenuTable_DeleteCol;
  CORBA::Long m_idMenuTable_JoinCells;
  CORBA::Long m_idMenuTable_SplitCells;
  CORBA::Long m_idMenuTable_UngroupTable;

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
  CORBA::Long m_idButtonEdit_Spelling;
  CORBA::Long m_idButtonEdit_Find;

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
  CORBA::Long m_idComboText_LineSpacing;
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
  CORBA::Long m_idButtonTools_KSpreadTable;
  CORBA::Long m_idButtonTools_Formula;
  CORBA::Long m_idButtonTools_Part;

  // frame toolbar
  OpenPartsUI::ToolBar_var m_vToolBarFrame;
  CORBA::Long m_idButtonFrame_BorderLeft;
  CORBA::Long m_idButtonFrame_BorderRight;
  CORBA::Long m_idButtonFrame_BorderTop;
  CORBA::Long m_idButtonFrame_BorderBottom;
  CORBA::Long m_idButtonFrame_BorderColor;
  CORBA::Long m_idComboFrame_BorderWidth;
  CORBA::Long m_idComboFrame_BorderStyle;
  CORBA::Long m_idButtonFrame_BackColor;

  // table toolbar
  OpenPartsUI::ToolBar_var m_vToolBarTable;
  CORBA::Long m_idButtonTable_InsertRow;
  CORBA::Long m_idButtonTable_DeleteRow;
  CORBA::Long m_idButtonTable_InsertCol;
  CORBA::Long m_idButtonTable_DeleteCol;

  QList<KWordFrame> m_lstFrames;

  // text toolbar values
  QFont tbFont;
  QColor tbColor;
  QStrList fontList,styleList;

  KWordGUI *gui;
  bool m_bShowGUI;
  bool _viewFormattingChars,_viewFrameBorders,_viewTableGrid;

  KWFormat format;
  KWParagLayout::Flow flow;
  KWFormat::VertAlign vertAlign;
  KWParagLayout::Border left,right,top,bottom,tmpBrd,frmBrd;
  KWSearchDia::KWSearchEntry *searchEntry,*replaceEntry;
  QBrush backColor;
  int spc;

  KWParagDia *paragDia;
  KWStyleManager *styleManager;
  KSpell *kspell;
  KWSearchDia *searchDia;
  KWTableDia *tableDia;

  KWParag *currParag;
  int currFrameSetNum;
  OpenPartsUI::BarPosition oldFramePos,oldTextPos;

  KWordShell *shell;

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
  static const int ID_TOOL_CREATE_KSPREAD_TABLE = 34;
  static const int ID_FBRD_STYLE = 35;
  static const int ID_FBRD_WIDTH = 36;
  static const int ID_FBORDER_COLOR = 37;
  static const int ID_FBRD_LEFT = 38;
  static const int ID_FBRD_RIGHT = 39;
  static const int ID_FBRD_TOP = 40;
  static const int ID_FBRD_BOTTOM = 41;
  static const int ID_FBACK_COLOR = 42;
  static const int ID_LINE_SPC = 43;
  static const int ID_EDIT_CUT = 44;
  static const int ID_EDIT_COPY = 45;
  static const int ID_EDIT_PASTE = 46;
  static const int ID_TABLE_INSROW = 47;
  static const int ID_TABLE_DELROW = 48;
  static const int ID_TABLE_INSCOL = 49;
  static const int ID_TABLE_DELCOL = 50;

};

/******************************************************************/
/* Class: KWordGUI                                                */
/******************************************************************/

class KExtPanner : public KNewPanner
{
  Q_OBJECT

public:
  KExtPanner(QWidget *parent = 0,const char *name = 0,Orientation orient = Vertical,Units units = Percent,int pos = 50)
    : KNewPanner(parent,name,orient,units,pos) {}

protected:
  void resizeEvent(QResizeEvent *e)
    { KNewPanner::resizeEvent(e); emit pannerResized(); }

signals:
  void pannerResized();

};

class KWordGUI : public QWidget
{
  Q_OBJECT

public:
  KWordGUI(QWidget *parent,bool __show,KWordDocument *_doc,KWordView *_view);

  KWordDocument *getDocument()
    { return doc; }

  void showGUI(bool __show);

  void setDocument(KWordDocument *_doc)
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
  KWDocStruct *getDocStruct()
    { return docStruct; }

  void showDocStruct(bool __show);

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
  void unitChanged(QString);
  void reorganize();

protected:
  void resizeEvent(QResizeEvent *e);
  void keyPressEvent(QKeyEvent *e);
  virtual void dragEnterEvent(QDragEnterEvent *e);
  virtual void dragMoveEvent(QDragMoveEvent *e);
  virtual void dragLeaveEvent(QDragLeaveEvent *e);
  virtual void dropEvent(QDropEvent *e);

  int xOffset,yOffset;
  bool _show;
  QScrollBar *s_vert,*s_horz;
  KoRuler *r_vert,*r_horz;
  KWPage *paperWidget;
  KWordDocument *doc;
  KWordView *view;
  KoTabChooser *tabChooser;
  KWDocStruct *docStruct;
  KExtPanner *panner;
  QWidget *left;
  bool _showStruct;

};

#endif
