/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998	  */
/* Version: 0.0.1						  */
/* Author: Reginald Stadlbauer, Torben Weis			  */
/* E-Mail: reggie@kde.org, weis@kde.org				  */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs			  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* written for KDE (http://www.kde.org)				  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* License: GNU GPL						  */
/******************************************************************/
/* Module: View (header)					  */
/******************************************************************/

#ifndef kword_view_h
#define kword_view_h

#include <koView.h>
#include <koFrame.h>

#include <qwidget.h>
#include <qlist.h>
#include <qcolor.h>
#include <qfont.h>
#include <qbrush.h>
#include <qstringlist.h>

#include "kword.h"
#include "format.h"
#include "paraglayout.h"
#include "searchdia.h"

#include <koPageLayoutDia.h>

class KWPartFrameSet;
class KWordView;
class KWordDocument;
class KWordChild;
class KWordGUI;
class KWPaintWindow;
class KWordShell;
class QResizeEvent;
class QMouseEvent;
class QKeyEvent;
class QDragEnterEvent;
class QDragMoveEvent;
class QDragLeaveEvent;
class QDropEvent;
class QCloseEvent;
class QFocusEvent;
class KWParag;
class KWParagDia;
class KWStyleManager;
class KWTableDia;
class KWDocStruct;
class KSpell;
class QScrollView;
class QSplitter;

/******************************************************************/
/* Class: KWordFrame						  */
/******************************************************************/

class KWordFrame : public KoFrame
{
    Q_OBJECT

public:
    KWordFrame( KWordView*, KWordChild* );

    KWordChild* child()
    { return m_pKWordChild; }
    KWordView* wordView()
    { return m_pKWordView; }

protected:
    KWordChild *m_pKWordChild;
    KWordView *m_pKWordView;

};

/******************************************************************/
/* Class: KWordView						  */
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

    void setShell( KWordShell *_shell ) { shell = _shell; }

    // IDL
    virtual void editUndo();
    virtual void editRedo();
    virtual void editCut();
    virtual void editCopy();
    virtual void editPaste();
    virtual void editSelectAll();
    virtual void editFind();
    virtual void editDeleteFrame();
    virtual void editReconnectFrame();
    virtual void editCustomVars();
    virtual void editSerialLetterDataBase();

    virtual void newView();
    virtual void viewFormattingChars();
    virtual void viewFrameBorders();
    virtual void viewTableGrid();
    virtual void viewHeader();
    virtual void viewFooter();
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
    virtual void insertVariableCustom();
    virtual void insertVariableSerialLetter();
    virtual void insertFootNoteEndNote();
    virtual void insertContents();

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
    virtual void tableDelete();

    virtual void helpContents();
    virtual void helpAbout();
    virtual void helpAboutKOffice();
    virtual void helpAboutKDE();

    virtual void textStyleSelected( const QString &style );
    virtual void textSizeSelected( const QString &size );
    virtual void textFontSelected( const QString &font );
    virtual void textBold();
    virtual void textItalic();
    virtual void textUnderline();
    virtual void textColor();
    virtual void textAlignLeft();
    virtual void textAlignCenter();
    virtual void textAlignRight();
    virtual void textAlignBlock();
    virtual void textLineSpacing( const QString &spc );
    virtual void textEnumList();
    virtual void textUnsortList();
    virtual void textSuperScript();
    virtual void textSubScript();
    virtual void textBorderLeft();
    virtual void textBorderRight();
    virtual void textBorderTop();
    virtual void textBorderBottom();
    virtual void textBorderColor();
    virtual void textBorderWidth( const QString &width );
    virtual void textBorderStyle( const QString &style );
    virtual void frameBorderLeft();
    virtual void frameBorderRight();
    virtual void frameBorderTop();
    virtual void frameBorderBottom();
    virtual void frameBorderColor();
    virtual void frameBorderWidth( const QString &width );
    virtual void frameBorderStyle( const QString &style );
    virtual void frameBackColor();

    virtual void formulaPower();
    virtual void formulaSubscript();
    virtual void formulaParentheses();
    virtual void formulaAbsValue();
    virtual void formulaBrackets();
    virtual void formulaFraction();
    virtual void formulaRoot();
    virtual void formulaIntegral();
    virtual void formulaMatrix();
    virtual void formulaLeftSuper();
    virtual void formulaLeftSub();

    virtual void setMode( KOffice::View::Mode _mode );
    virtual void setFocus( bool mode );

    // C++
    virtual bool printDlg();

    virtual void setFormat( const KWFormat &_format, bool _check = true,
			    bool _update_page = true, bool _redraw = true );
    virtual void setFlow( KWParagLayout::Flow _flow );
    virtual void setLineSpacing( int _spc );
    virtual void setParagBorders( KWParagLayout::Border _left, KWParagLayout::Border _right,
				  KWParagLayout::Border _top, KWParagLayout::Border _bottom );

    KWordGUI *getGUI() { return gui; }
    void uncheckAllTools();
    void setTool( MouseMode _mouseMode );
    void updateStyle( QString _styleName, bool _updateFormat = true );
    void updateStyleList();

    bool getViewFormattingChars() { return _viewFormattingChars; }
    bool getViewFrameBorders() { return _viewFrameBorders; }
    bool getViewTableGrid() { return _viewTableGrid; }

    void setSearchEntry( KWSearchDia::KWSearchEntry *e ) { searchEntry = e; }
    void setReplaceEntry( KWSearchDia::KWSearchEntry *e ) { replaceEntry = e; }

    void changeUndo( QString, bool );
    void changeRedo( QString, bool );

    KOffice::MainWindow_var getMainWindow() { return m_vKoMainWindow; }
    OpenParts::Id getID() { return OPPartIf::m_id; }

    void showFormulaToolbar( bool show );

public slots:
    void slotInsertObject( KWordChild *_child, KWPartFrameSet *_kwpf );
    void slotUpdateChildGeometry( KWordChild *_child );
    void slotGeometryEnd( KoFrame* );
    void slotMoveEnd( KoFrame* );
    void paragDiaOk();
    void styleManagerOk();
    void openPageLayoutDia()  { formatPage(); }
    void newPageLayout( KoPageLayout _layout );
    void spellCheckerReady();
    void spellCheckerMisspelling( char*, QStringList, unsigned );
    void spellCheckerCorrected( char*, char*, unsigned );
    void spellCheckerDone( char* );
    void searchDiaClosed();

    void clipboardDataChanged();

protected:
    // C++
    virtual void init();
    // IDL
    virtual bool event( const QCString & _event, const CORBA::Any& _value );
    // C++
    bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
    bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );

    virtual void cleanUp();

    void resizeEvent( QResizeEvent *e );
    void keyPressEvent( QKeyEvent *e );
    void keyReleaseEvent( QKeyEvent *e );
    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );
    void focusInEvent( QFocusEvent *e );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dragLeaveEvent( QDragLeaveEvent *e );
    virtual void dropEvent( QDropEvent *e );

    void getFonts();
    void setParagBorderValues();

    KWordDocument *m_pKWordDoc;

    bool m_bUnderConstruction;

    // edit menu
    OpenPartsUI::Menu_var m_vMenuEdit;
    long int m_idMenuEdit_Undo;
    long int m_idMenuEdit_Redo;
    long int m_idMenuEdit_Cut;
    long int m_idMenuEdit_Copy;
    long int m_idMenuEdit_Paste;
    long int m_idMenuEdit_SelectAll;
    long int m_idMenuEdit_Find;
    long int m_idMenuEdit_DeleteFrame;
    long int m_idMenuEdit_ReconnectFrame;
    long int m_idMenuEdit_CustomVars;
    long int m_idMenuEdit_SerialLetterDataBase;

    // view menu
    OpenPartsUI::Menu_var m_vMenuView;
    long int m_idMenuView_NewView;
    long int m_idMenuView_FormattingChars;
    long int m_idMenuView_FrameBorders;
    long int m_idMenuView_TableGrid;
    long int m_idMenuView_Header;
    long int m_idMenuView_Footer;
    long int m_idMenuView_FootNotes;
    long int m_idMenuView_EndNotes;

    // insert menu
    OpenPartsUI::Menu_var m_vMenuInsert;
    long int m_idMenuInsert_Picture;
    long int m_idMenuInsert_Clipart;
    long int m_idMenuInsert_SpecialChar;
    long int m_idMenuInsert_FrameBreak;
    long int m_idMenuInsert_Variable;
    OpenPartsUI::Menu_var m_vMenuInsert_Variable;
    long int m_idMenuInsert_VariableDateFix;
    long int m_idMenuInsert_VariableDateVar;
    long int m_idMenuInsert_VariableTimeFix;
    long int m_idMenuInsert_VariableTimeVar;
    long int m_idMenuInsert_VariablePageNum;
    long int m_idMenuInsert_VariableCustom;
    long int m_idMenuInsert_VariableSerialLetter;
    long int m_idMenuInsert_FootNoteEndNote;
    long int m_idMenuInsert_Contents;

    // format menu
    OpenPartsUI::Menu_var m_vMenuFormat;
    long int m_idMenuFormat_Font;
    long int m_idMenuFormat_Color;
    long int m_idMenuFormat_Paragraph;
    long int m_idMenuFormat_Page;
    long int m_idMenuFormat_Numbering;
    long int m_idMenuFormat_Style;
    long int m_idMenuFormat_FrameSet;

    // extra menu
    OpenPartsUI::Menu_var m_vMenuExtra;
    long int m_idMenuExtra_Spelling;
    long int m_idMenuExtra_AutoFormat;
    long int m_idMenuExtra_Stylist;
    long int m_idMenuExtra_Options;

    // tools menu
    OpenPartsUI::Menu_var m_vMenuTools;
    long int m_idMenuTools_Edit;
    long int m_idMenuTools_EditFrame;
    long int m_idMenuTools_CreateText;
    long int m_idMenuTools_CreatePix;
    long int m_idMenuTools_Clipart;
    long int m_idMenuTools_Table;
    long int m_idMenuTools_KSpreadTable;
    long int m_idMenuTools_Formula;
    long int m_idMenuTools_Part;

    // table menu
    OpenPartsUI::Menu_var m_vMenuTable;
    long int m_idMenuTable_InsertRow;
    long int m_idMenuTable_InsertCol;
    long int m_idMenuTable_DeleteRow;
    long int m_idMenuTable_DeleteCol;
    long int m_idMenuTable_JoinCells;
    long int m_idMenuTable_SplitCells;
    long int m_idMenuTable_UngroupTable;
    long int m_idMenuTable_Delete;

    // help menu
    OpenPartsUI::Menu_var m_vMenuHelp;
    long int m_idMenuHelp_Contents;

    // edit toolbar
    OpenPartsUI::ToolBar_var m_vToolBarEdit;
    long int m_idButtonEdit_Undo;
    long int m_idButtonEdit_Redo;
    long int m_idButtonEdit_Cut;
    long int m_idButtonEdit_Copy;
    long int m_idButtonEdit_Paste;
    long int m_idButtonEdit_Spelling;
    long int m_idButtonEdit_Find;

    // insert toolbar
    OpenPartsUI::ToolBar_var m_vToolBarInsert;
    long int m_idButtonInsert_Picture;
    long int m_idButtonInsert_Clipart;
    long int m_idButtonInsert_SpecialChar;

    // text toolbar
    OpenPartsUI::ToolBar_var m_vToolBarText;
    long int m_idComboText_Style;
    long int m_idComboText_FontSize;
    long int m_idComboText_FontList;
    long int m_idComboText_LineSpacing;
    long int m_idButtonText_Bold;
    long int m_idButtonText_Italic;
    long int m_idButtonText_Underline;
    long int m_idButtonText_Color;
    long int m_idButtonText_ARight;
    long int m_idButtonText_ACenter;
    long int m_idButtonText_ALeft;
    long int m_idButtonText_ABlock;
    long int m_idButtonText_EnumList;
    long int m_idButtonText_UnsortList;
    long int m_idButtonText_SuperScript;
    long int m_idButtonText_SubScript;
    long int m_idButtonText_BorderLeft;
    long int m_idButtonText_BorderRight;
    long int m_idButtonText_BorderTop;
    long int m_idButtonText_BorderBottom;
    long int m_idButtonText_BorderColor;
    long int m_idComboText_BorderWidth;
    long int m_idComboText_BorderStyle;

    // tools toolbar
    OpenPartsUI::ToolBar_var m_vToolBarTools;
    long int m_idButtonTools_Edit;
    long int m_idButtonTools_EditFrame;
    long int m_idButtonTools_CreateText;
    long int m_idButtonTools_CreatePix;
    long int m_idButtonTools_Clipart;
    long int m_idButtonTools_Table;
    long int m_idButtonTools_KSpreadTable;
    long int m_idButtonTools_Formula;
    long int m_idButtonTools_Part;

    // frame toolbar
    OpenPartsUI::ToolBar_var m_vToolBarFrame;
    long int m_idButtonFrame_BorderLeft;
    long int m_idButtonFrame_BorderRight;
    long int m_idButtonFrame_BorderTop;
    long int m_idButtonFrame_BorderBottom;
    long int m_idButtonFrame_BorderColor;
    long int m_idComboFrame_BorderWidth;
    long int m_idComboFrame_BorderStyle;
    long int m_idButtonFrame_BackColor;

    // table toolbar
    OpenPartsUI::ToolBar_var m_vToolBarTable;
    long int m_idButtonTable_InsertRow;
    long int m_idButtonTable_DeleteRow;
    long int m_idButtonTable_InsertCol;
    long int m_idButtonTable_DeleteCol;

    // formula toolbar
    OpenPartsUI::ToolBar_var m_vToolBarFormula;
    long int m_idButtonFormula_Power;
    long int m_idButtonFormula_Subscript;
    long int m_idButtonFormula_Parentheses;
    long int m_idButtonFormula_AbsValue;
    long int m_idButtonFormula_Brackets;
    long int m_idButtonFormula_Fraction;
    long int m_idButtonFormula_Root;
    long int m_idButtonFormula_Integral;
    long int m_idButtonFormula_Matrix;
    long int m_idButtonFormula_LeftSuper;
    long int m_idButtonFormula_LeftSub;

    // text toolbar values
    QFont tbFont;
    QColor tbColor;
    QStringList fontList;
    QStringList styleList;

    KWordGUI *gui;
    bool m_bShowGUI;
    bool _viewFormattingChars, _viewFrameBorders, _viewTableGrid;

    KWFormat format;
    KWParagLayout::Flow flow;
    KWFormat::VertAlign vertAlign;
    KWParagLayout::Border left, right, top, bottom, tmpBrd, frmBrd;
    KWSearchDia::KWSearchEntry *searchEntry, *replaceEntry;
    QBrush backColor;
    int spc;

    KWParagDia *paragDia;
    KWStyleManager *styleManager;
    KSpell *kspell;
    KWSearchDia *searchDia;
    KWTableDia *tableDia;

    KWParag *currParag;
    int currFrameSetNum;
    OpenPartsUI::BarPosition oldFramePos, oldTextPos;

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
/* Class: KWordGUI						  */
/******************************************************************/

class KWordGUI : public QWidget
{
    Q_OBJECT

public:
    KWordGUI( QWidget *parent, bool __show, KWordDocument *_doc, KWordView *_view );

    KWordDocument *getDocument()
    { return doc; }

    void showGUI( bool __show );

    void setDocument( KWordDocument *_doc );

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

    void keyEvent( QKeyEvent *e )
    { keyPressEvent( e ); }

    void scrollTo( int _x, int _y );

protected slots:
    void unitChanged( QString );
    void reorganize();

protected:
    void resizeEvent( QResizeEvent *e );
    void keyPressEvent( QKeyEvent *e );
    void keyReleaseEvent( QKeyEvent *e );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dragLeaveEvent( QDragLeaveEvent *e );
    virtual void dropEvent( QDropEvent *e );

    bool _show;
    KoRuler *r_vert, *r_horz;
    KWPage *paperWidget;
    KWordDocument *doc;
    KWordView *view;
    KoTabChooser *tabChooser;
    KWDocStruct *docStruct;
    QSplitter *panner;
    QWidget *left;
    bool _showStruct;

};

#endif
