/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998			  */
/* Version: 0.1.0						  */
/* Author: Reginald Stadlbauer					  */
/* E-Mail: reggie@kde.org					  */
/* needs c++ library Qt (http://www.troll.no)			  */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)	  */
/* needs OpenParts and Kom (weis@kde.org)			  */
/* written for KDE (http://www.kde.org)				  */
/* KPresenter is under GNU GPL					  */
/******************************************************************/
/* Module: KPresenter View (header)				  */
/******************************************************************/

#ifndef kpresenter_view_h
#define kpresenter_view_h

#include <koView.h>
#include <koFrame.h>

#include <qwidget.h>
#include <qlist.h>
#include <qpen.h>
#include <qbrush.h>
#include <qstringlist.h>
#include <qcolor.h>
#include <qfont.h>

#include <qrect.h>
#include <qpoint.h>

#include <koMainWindow.h>

#include "kpresenter.h"
#include "kpresenter_doc.h"
#include "kpresenter_main.h"
#include "kpresenter_shell.h"
#include "global.h"
#include "ktextobject.h"

class KPresenterView;
class KPresenterShell;
class KPresenterDoc;
class KPresenterChild;
class BackDia;
class Page;
class KPPartObject;
class KoRuler;
class KPresenterShell;
class QScrollBar;
class AFChoose;
class StyleDia;
class OptionDia;
class PgConfDia;
class EffectDia;
class RotateDia;
class KSearchDialog;
class KSearchReplaceDialog;
class ShadowDia;
class KPPresStructView;
class DelPageDia;
class InsPageDia;
class ConfPieDia;
class ConfRectDia;
class SpacingDia;

/******************************************************************/
/* class KPresenterFrame					  */
/******************************************************************/
class KPresenterFrame : public KoFrame
{
    Q_OBJECT

public:

    // constructor
    KPresenterFrame( KPresenterView*, KPresenterChild* );

    // get child
    KPresenterChild* child() {return m_pKPresenterChild; }

    // get view
    KPresenterView* presenterView() {return m_pKPresenterView; }
protected:
    // child
    KPresenterChild *m_pKPresenterChild;

    // view
    KPresenterView *m_pKPresenterView;

};

/*****************************************************************/
/* class KPresenterView						 */
/*****************************************************************/
class KPresenterView : public QWidget,
		       virtual public KoViewIf,
		       virtual public KPresenter::KPresenterView_skel
{
    Q_OBJECT

public:

    // ------ C++ ------
    // constructor - destructor
    KPresenterView( QWidget *_parent, const char *_name, KPresenterDoc* _doc );
    ~KPresenterView();

    void setShell( KPresenterShell *_shell ) { shell = _shell; }

    void init();

    // clean
    virtual void cleanUp();

    // ------ IDL ------
    virtual bool printDlg();
    // edit menu
    virtual void editUndo();
    virtual void editRedo();
    virtual void editCut();
    virtual void editCopy();
    virtual void editPaste();
    virtual void editDelete();
    virtual void editSelectAll();
    virtual void editCopyPage();
    virtual void editDelPage();
    virtual void editFind();
    virtual void editFindReplace();
    virtual void editHeaderFooter();

    // view menu
    virtual void newView();

    // insert menu
    virtual void insertPage();
    virtual void insertPicture();
    virtual void insertClipart();

    // tools menu
    virtual void toolsMouse();
    virtual void toolsLine();
    virtual void toolsRectangle();
    virtual void toolsCircleOrEllipse();
    virtual void toolsPie();
    virtual void toolsText();
    virtual void toolsAutoform();
    virtual void toolsDiagramm();
    virtual void toolsTable();
    virtual void toolsFormula();
    virtual void toolsObject();

    // extra menu
    virtual void extraPenBrush();
    virtual void extraConfigPie();
    virtual void extraConfigRect();
    virtual void extraRaise();
    virtual void extraLower();
    virtual void extraRotate();
    virtual void extraShadow();
    virtual void extraAlignObj();
    virtual void extraBackground();
    virtual void extraLayout();
    virtual void extraOptions();
    virtual void extraLineBegin();
    virtual void extraLineEnd();
    virtual void extraWebPres();

    virtual void extraAlignObjLeftidl();
    virtual void extraAlignObjCenterHidl();
    virtual void extraAlignObjRightidl();
    virtual void extraAlignObjTopidl();
    virtual void extraAlignObjCenterVidl();
    virtual void extraAlignObjBottomidl();

    // screen menu
    virtual void screenConfigPages();
    virtual void screenPresStructView();
    virtual void screenAssignEffect();
    virtual void screenStart();
    virtual void screenStop();
    virtual void screenPause();
    virtual void screenFirst();
    virtual void screenPrev();
    virtual void screenNext();
    virtual void screenLast();
    virtual void screenSkip();
    virtual void screenFullScreen();
    virtual void screenPen();
    virtual void presPen1idl();
    virtual void presPen2idl();
    virtual void presPen3idl();
    virtual void presPen4idl();
    virtual void presPen5idl();
    virtual void presPen6idl();
    virtual void presPen7idl();
    virtual void presPen8idl();
    virtual void presPen9idl();
    virtual void presPen10idl();
    virtual void presPenColoridl();

    // help menu
    virtual void helpContents();

    // text toolbar
    virtual void sizeSelected( const QString & );
    virtual void fontSelected( const QString & );
    virtual void textBold();
    virtual void textItalic();
    virtual void textUnderline();
    virtual void textColor();
    virtual void textAlignLeft();
    virtual void textAlignCenter();
    virtual void textAlignRight();
    virtual void mtextAlignLeft();
    virtual void mtextAlignCenter();
    virtual void mtextAlignRight();
    virtual void mtextFont();
    virtual void textEnumList();
    virtual void textUnsortList();
    virtual void textNormalText();
    virtual void textDepthPlus();
    virtual void textDepthMinus();
    virtual void textSpacing();
    virtual void textContentsToHeight();
    virtual void textObjectToContents();

    // color bar
    virtual void setPenColor( long int id );
    virtual void setFillColor( long int id );

    // ------ C++ ------

    // create GUI - construct
    virtual void createGUI();
    virtual void construct();

    // get - set offsets
    int getDiffX() {return xOffset; }
    int getDiffY() {return yOffset; }
    void setDiffX( int _x ) {xOffset = _x; }
    void setDiffY( int _y ) {yOffset = _y; }

    // get current pagenum
    unsigned int getCurrPgNum();

    // return pointer to document
    class KPresenterDoc *kPresenterDoc() {return m_pKPresenterDoc; }

    // repaint page
    void repaint( bool );
    void repaint( unsigned int, unsigned int, unsigned int, unsigned int, bool );
    void repaint( QRect, bool );

    // properties
    void changePicture( unsigned int, const QString & );
    void changeClipart( unsigned int, QString );

    Page* getPage() {return page; }

    void changeUndo( QString, bool );
    void changeRedo( QString, bool );

    void presColorChanged();

    void setRulerMouseShow( bool _show );
    void setRulerMousePos( int mx, int my );

    // set scrollbar ranges
    void setRanges();

    KoRuler *getHRuler() { return h_ruler; }
    KoRuler *getVRuler() { return v_ruler; }
    QScrollBar *getHScrollBar() { return horz; }
    QScrollBar *getVScrollBar() { return vert; }

    void skipToPage( int _num );
    void makeRectVisible( QRect _rect );

    void restartPresStructView();

    PieType getPieType() { return pieType; }
    int getPieAngle() { return pieAngle; }
    int getPieLength() { return pieLength; }
    QPen getPen() { return pen; }
    QBrush getBrush() { return brush; }
    LineEnd getLineBegin() {return lineBegin; }
    LineEnd getLineEnd() {return lineEnd; }
    QColor getGColor1() {return gColor1; }
    QColor getGColor2() {return gColor2; }
    BCType getGType() {return gType; }
    FillType getFillType() {return fillType; }
    bool getGUnbalanced() { return gUnbalanced; }
    int getGXFactor() { return gXFactor; }
    int getGYFactor() { return gYFactor; }

    void setTool( ToolEditMode toolEditMode );

    int getRndX() { return rndX; }
    int getRndY() { return rndY; }

    QFont &currFont() { return tbFont; }
    QColor &currColor() { return tbColor; }

    void enableWebPres() {
	m_vToolBarExtra->setItemEnabled( ID_TOOL_WEBPRES, true );
	m_vMenuExtra->setItemEnabled( m_idMenuExtra_WepPres, true );
    }

    // get fonts
    static void getFonts( QStringList &lst );

    KOffice::MainWindow_var getMainWindow() { return m_vKoMainWindow; }
    OpenParts::Id getID() { return OPPartIf::m_id; }

public slots:

    // Document signals
    void slotKPresenterModified();
    void slotInsertObject( KPresenterChild *_child, KPPartObject *_kppo );
    void slotUpdateChildGeometry( KPresenterChild *_child );

    // KPresenterFrame signals
    void slotGeometryEnd( KoFrame* );
    void slotMoveEnd( KoFrame* );

protected slots:

    // dialog slots
    void backOk( bool );
    void afChooseOk( const QString & );
    void styleOk();
    void optionOk();
    void pgConfOk();
    void effectOk();
    void rotateOk();
    void shadowOk();
    void psvClosed();
    void delPageOk( int, DelPageMode );
    void insPageOk( int, InsPageMode, InsertPos );
    void confPieOk();
    void confRectOk();
    void spacingOk( int, int, int, int );

    // scrolling
    void scrollH( int );
    void scrollV( int );

    // textobject
    void fontChanged( QFont* );
    void colorChanged( QColor* );
    void alignChanged( TxtParagraph::HorzAlign );

    void extraLineBeginNormal();
    void extraLineBeginArrow();
    void extraLineBeginRect();
    void extraLineBeginCircle();
    void extraLineEndNormal();
    void extraLineEndArrow();
    void extraLineEndRect();
    void extraLineEndCircle();

    // screen presentation
    void presPen1();
    void presPen2();
    void presPen3();
    void presPen4();
    void presPen5();
    void presPen6();
    void presPen7();
    void presPen8();
    void presPen9();
    void presPen10();
    void presPenColor();

    // search/replace
    void search( QString, bool, bool );
    void replace( QString, QString, bool, bool );
    void replaceAll( QString, QString, bool );

    void stopPres() {continuePres = false; }

    // align objs
    void extraAlignObjLeft() { extraAlignObjLeftidl(); }
    void extraAlignObjCenterH() { extraAlignObjCenterHidl(); }
    void extraAlignObjRight() { extraAlignObjRightidl(); }
    void extraAlignObjTop() { extraAlignObjTopidl(); }
    void extraAlignObjCenterV() { extraAlignObjCenterVidl(); }
    void extraAlignObjBottom() { extraAlignObjBottomidl(); }

				// layout
    void newPageLayout( KoPageLayout _layout );
    void openPageLayoutDia() { extraLayout(); }
    void unitChanged( QString );

protected:

// ********* functions ***********

// C++
    virtual bool event( const QCString &_event, const CORBA::Any& _value );
// C++
    virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
    virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );

// resize event
    void resizeEvent( QResizeEvent* );
    virtual void dragEnterEvent( QDragEnterEvent *e );
    virtual void dragMoveEvent( QDragMoveEvent *e );
    virtual void dragLeaveEvent( QDragLeaveEvent *e );
    virtual void dropEvent( QDropEvent *e );

// GUI
    void setupPopupMenus();
    void setupScrollbars();
    void setupRulers();

    void keyPressEvent( QKeyEvent* );

    void doAutomaticScreenPres();

// ********** variables **********

// document
    KPresenterDoc *m_pKPresenterDoc;

// flags
    bool m_bKPresenterModified;
    bool m_bUnderConstruction;
    bool searchFirst, continuePres, exitPres;

// edit menu
    OpenPartsUI::Menu_var m_vMenuEdit;
    long int m_idMenuEdit_Undo;
    long int m_idMenuEdit_Redo;
    long int m_idMenuEdit_Cut;
    long int m_idMenuEdit_Copy;
    long int m_idMenuEdit_Paste;
    long int m_idMenuEdit_Delete;
    long int m_idMenuEdit_SelectAll;
    long int m_idMenuEdit_CopyPage;
    long int m_idMenuEdit_DelPage;
    long int m_idMenuEdit_Find;
    long int m_idMenuEdit_FindReplace;
    long int m_idMenuEdit_HeaderFooter;

// view menu
    OpenPartsUI::Menu_var m_vMenuView;
    long int m_idMenuView_NewView;

// insert menu
    OpenPartsUI::Menu_var m_vMenuInsert;
    long int m_idMenuInsert_Page;
    long int m_idMenuInsert_Picture;
    long int m_idMenuInsert_Clipart;

// tools menu
    OpenPartsUI::Menu_var m_vMenuTools;
    long int m_idMenuTools_Mouse;
    long int m_idMenuTools_Line;
    long int m_idMenuTools_Rectangle;
    long int m_idMenuTools_Circle;
    long int m_idMenuTools_Pie;
    long int m_idMenuTools_Text;
    long int m_idMenuTools_Autoform;
    long int m_idMenuTools_Diagramm;
    long int m_idMenuTools_Table;
    long int m_idMenuTools_Formula;
    long int m_idMenuTools_Part;

// text menu
    OpenPartsUI::Menu_var m_vMenuText;
    long int m_idMenuText_TFont;
    long int m_idMenuText_TColor;
    OpenPartsUI::Menu_var m_vMenuText_TAlign;
    long int m_idMenuText_TAlign_Left;
    long int m_idMenuText_TAlign_Center;
    long int m_idMenuText_TAlign_Right;
    OpenPartsUI::Menu_var m_vMenuText_TType;
    long int m_idMenuText_TType_EnumList;
    long int m_idMenuText_TType_UnsortList;
    long int m_idMenuText_TType_NormalText;
    long int m_idMenuText_TDepthPlus;
    long int m_idMenuText_TDepthMinus;
    long int m_idMenuText_TSpacing;
    long int m_idMenuText_TExtentCont2Height;
    long int m_idMenuText_TExtentObj2Cont;

// extra menu
    OpenPartsUI::Menu_var m_vMenuExtra;
    long int m_idMenuExtra_PenBrush;
    long int m_idMenuExtra_Pie;
    long int m_idMenuExtra_Rect;
    long int m_idMenuExtra_Raise;
    long int m_idMenuExtra_Lower;
    long int m_idMenuExtra_Rotate;
    long int m_idMenuExtra_Shadow;
    OpenPartsUI::Menu_var m_vMenuExtra_AlignObj;
    long int m_idMenuExtra_AlignObj_Left;
    long int m_idMenuExtra_AlignObj_CenterH;
    long int m_idMenuExtra_AlignObj_Right;
    long int m_idMenuExtra_AlignObj_Top;
    long int m_idMenuExtra_AlignObj_CenterV;
    long int m_idMenuExtra_AlignObj_Bottom;
    long int m_idMenuExtra_Background;
    long int m_idMenuExtra_Layout;
    long int m_idMenuExtra_Options;
    long int m_idMenuExtra_WepPres;

    // screenpresentation menu
    OpenPartsUI::Menu_var m_vMenuScreen;
    long int m_idMenuScreen_ConfigPage;
    long int m_idMenuScreen_PresStructView;
    long int m_idMenuScreen_AssignEffect;
    long int m_idMenuScreen_Start;
    long int m_idMenuScreen_Stop;
    long int m_idMenuScreen_Pause;
    long int m_idMenuScreen_First;
    long int m_idMenuScreen_Prev;
    long int m_idMenuScreen_Next;
    long int m_idMenuScreen_Last;
    long int m_idMenuScreen_Skip;
    long int m_idMenuScreen_FullScreen;
    OpenPartsUI::Menu_var m_vMenuScreen_Pen;
    OpenPartsUI::Menu_var m_vMenuScreen_PenWidth;
    long int m_idMenuScreen_PenColor;
    long int m_idMenuScreen_PenW1;
    long int m_idMenuScreen_PenW2;
    long int m_idMenuScreen_PenW3;
    long int m_idMenuScreen_PenW4;
    long int m_idMenuScreen_PenW5;
    long int m_idMenuScreen_PenW6;
    long int m_idMenuScreen_PenW7;
    long int m_idMenuScreen_PenW8;
    long int m_idMenuScreen_PenW9;
    long int m_idMenuScreen_PenW10;

    // help menu
    OpenPartsUI::Menu_var m_vMenuHelp;
    long int m_idMenuHelp_Contents;

    // right button popup menus
    QPopupMenu *rb_pen, *rb_pen_width, *rb_oalign, *rb_lbegin, *rb_lend;

    int W1, W2, W3, W4, W5, W6, W7, W8, W9, W10, P_COL;

    // edit toolbar
    OpenPartsUI::ToolBar_var m_vToolBarEdit;
    long int m_idButtonEdit_Undo;
    long int m_idButtonEdit_Redo;
    long int m_idButtonEdit_Cut;
    long int m_idButtonEdit_Copy;
    long int m_idButtonEdit_Paste;
    long int m_idButtonEdit_Delete;

    // insert toolbar
    OpenPartsUI::ToolBar_var m_vToolBarInsert;
    long int m_idButtonInsert_Page;
    long int m_idButtonInsert_Picture;
    long int m_idButtonInsert_Clipart;

    // tools toolbar
    OpenPartsUI::ToolBar_var m_vToolBarTools;
    long int m_idButtonTools_Mouse;
    long int m_idButtonTools_Line;
    long int m_idButtonTools_Rectangle;
    long int m_idButtonTools_Circle;
    long int m_idButtonTools_Pie;
    long int m_idButtonTools_Text;
    long int m_idButtonTools_Autoform;
    long int m_idButtonTools_Diagramm;
    long int m_idButtonTools_Table;
    long int m_idButtonTools_Formula;
    long int m_idButtonTools_Part;

    // text toolbar
    OpenPartsUI::ToolBar_var m_vToolBarText;
    long int m_idComboText_FontSize;
    long int m_idComboText_FontList;
    long int m_idButtonText_Bold;
    long int m_idButtonText_Italic;
    long int m_idButtonText_Underline;
    long int m_idButtonText_Color;
    long int m_idButtonText_ARight;
    long int m_idButtonText_ACenter;
    long int m_idButtonText_ALeft;
    long int m_idButtonText_EnumList;
    long int m_idButtonText_UnsortList;
    long int m_idButtonText_NormalText;
    long int m_idButtonText_DepthPlus;
    long int m_idButtonText_DepthMinus;
    long int m_idButtonText_Spacing;

    // extra toolbar
    OpenPartsUI::ToolBar_var m_vToolBarExtra;
    long int m_idButtonExtra_Style;
    long int m_idButtonExtra_Pie;
    long int m_idButtonExtra_Rect;
    long int m_idButtonExtra_Raise;
    long int m_idButtonExtra_Lower;
    long int m_idButtonExtra_Rotate;
    long int m_idButtonExtra_Shadow;
    long int m_idButtonExtra_Align;
    long int m_idButtonExtra_LineBegin;
    long int m_idButtonExtra_LineEnd;
    long int m_idButtonExtra_WebPres;

    // screen toolbar
    OpenPartsUI::ToolBar_var m_vToolBarScreen;
    long int m_idButtonScreen_Stop;
    long int m_idButtonScreen_Pause;
    long int m_idButtonScreen_Start;
    long int m_idButtonScreen_First;
    long int m_idButtonScreen_Prev;
    long int m_idButtonScreen_Next;
    long int m_idButtonScreen_Last;
    long int m_idButtonScreen_Effect;
    long int m_idButtonScreen_Full;
    long int m_idButtonScreen_Pen;

    // color bar
    OpenPartsUI::ColorBar_var m_vColorBar;

    // scrollbars
    QScrollBar *vert, *horz;
    int xOffset, yOffset;
    int _xOffset, _yOffset;

    // dialogs
    BackDia *backDia;
    AFChoose *afChoose;
    StyleDia *styleDia;
    OptionDia *optionDia;
    PgConfDia *pgConfDia;
    EffectDia *effectDia;
    RotateDia *rotateDia;
    KSearchDialog *searchDia;
    KSearchReplaceDialog *replaceDia;
    ShadowDia *shadowDia;
    KPPresStructView *presStructView;
    DelPageDia *delPageDia;
    InsPageDia *insPageDia;
    ConfPieDia *confPieDia;
    ConfRectDia *confRectDia;
    SpacingDia *spacingDia;

    // default pen and brush
    QPen pen;
    QBrush brush;
    LineEnd lineBegin;
    LineEnd lineEnd;
    QColor gColor1, gColor2;
    BCType gType;
    FillType fillType;
    PieType pieType;
    bool gUnbalanced;
    int gXFactor, gYFactor;
    int pieLength, pieAngle;
    int rndX, rndY;

    // the page
    Page *page;
    KoRuler *h_ruler, *v_ruler;

    // text toolbar values
    QFont tbFont;
    TxtParagraph::HorzAlign tbAlign;
    QColor tbColor;
    QStringList fontList;

    bool m_bRectSelection;
    QRect m_rctRectSelection;
    QString m_strNewPart;
    bool m_bShowGUI;
    bool presStarted;
    bool allowWebPres;
    
    QSize oldSize;

    int screensaver_pid;

    KPresenterShell *shell;

    // ids
    static const int ID_TOOL_MOUSE = 2;
    static const int ID_TOOL_LINE = 3;
    static const int ID_TOOL_RECT = 4;
    static const int ID_TOOL_ELLIPSE = 5;
    static const int ID_TOOL_TEXT = 6;
    static const int ID_TOOL_PIE = 7;
    static const int ID_TOOL_OBJECT = 8;
    static const int ID_TEXT_COLOR = 9;
    static const int ID_ALEFT = 10;
    static const int ID_ACENTER = 11;
    static const int ID_ARIGHT = 12;
    static const int ID_BOLD = 13;
    static const int ID_ITALIC = 14;
    static const int ID_UNDERLINE = 15;
    static const int ID_FONT_LIST = 16;
    static const int ID_FONT_SIZE = 17;
    static const int ID_UNDO = 18;
    static const int ID_REDO = 19;
    static const int ID_TOOL_DIAGRAMM = 20;
    static const int ID_TOOL_TABLE = 21;
    static const int ID_TOOL_FORMULA = 22;
    static const int ID_TOOL_AUTOFORM = 23;
    static const int ID_TOOL_WEBPRES = 24;

};

#endif
