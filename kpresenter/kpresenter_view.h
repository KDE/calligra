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
/* Module: KPresenter View (header)                               */
/******************************************************************/

#ifndef kpresenter_view_h
#define kpresenter_view_h

class KPresenterView;
class KPresenterShell;
class KPresenterDoc;
class KPresenterChild;
class BackDia;
class Page;

#include <stdlib.h>

#include <koView.h>
#include <koDocument.h>
#include <koFrame.h>
#include <opToolBarIf.h>
#include <opApplication.h>
#include <koPartSelectDia.h>

#include <qwidget.h>
#include <krect.h>
#include <qlist.h>
#include <qpainter.h>
#include <qstring.h>
#include <qmsgbox.h>
#include <qkeycode.h>
#include <qscrbar.h>
#include <qpen.h>
#include <qbrush.h>
#include <qstrlist.h>
#include <qevent.h>
#include <qcolor.h>
#include <qfont.h>
#include <qpopmenu.h>
#include <qcursor.h>
#include <qfileinf.h>
#include <kpoint.h>

#include <kcolordlg.h>
#include <koAboutDia.h>
#include <koPageLayoutDia.h>
#include <kfontdialog.h>
#include <koRuler.h>

#include "kpresenter.h"
#include "kpresenter_doc.h"
#include "kpresenter_main.h"
#include "kpresenter_shell.h"
#include "global.h"
#include "backdia.h"
#include "autoformEdit/afchoose.h"
#include "styledia.h"
#include "ktextobject.h"
#include "kcharselectdia.h"
#include "kenumlistdia.h"
#include "optiondia.h"
#include "pgconfdia.h"
#include "effectdia.h"
#include "rotatedia.h"
#include "ksearchdialogs.h"
#include "shadowdia.h"
#include "presstructview.h"
#include "delpagedia.h"
#include "inspagedia.h"
#include "setbackcmd.h"
#include "pgconfcmd.h"
#include "confpiedia.h"
#include "pglayoutcmd.h"
#include "shadowcmd.h"
#include "rotatecmd.h"

#include <X11/Xlib.h>
#include <signal.h>

/******************************************************************/
/* class KPresenterFrame                                          */
/******************************************************************/
class KPresenterFrame : public KoFrame
{
  Q_OBJECT

public:

  // constructor
  KPresenterFrame(KPresenterView*,KPresenterChild*);
  
  // get child
  KPresenterChild* child() {return m_pKPresenterChild;}

  // get view
  KPresenterView* presenterView() {return m_pKPresenterView;}
  
protected:

  // child
  KPresenterChild *m_pKPresenterChild;

  // view
  KPresenterView *m_pKPresenterView;

};

/*****************************************************************/
/* class KPresenterView                                          */
/*****************************************************************/
class KPresenterView : public QWidget,
		       virtual public KoViewIf,
		       virtual public KPresenter::KPresenterView_skel
{
  Q_OBJECT

public:

  // ------ C++ ------
  // constructor - destructor
  KPresenterView(QWidget *_parent, const char *_name, KPresenterDoc* _doc );
  ~KPresenterView();

  void init();
  
  // clean
  virtual void cleanUp();
  
  // ------ IDL ------
  virtual CORBA::Boolean printDlg();
  // edit menu
  virtual void editUndo();
  virtual void editRedo();
  virtual void editCut();
  virtual void editCopy();
  virtual void editPaste();
  virtual void editDelete();
  virtual void editSelectAll();
  virtual void editDelPage();
  virtual void editFind();
  virtual void editFindReplace();
  
  // view menu
  virtual void newView();

  // insert menu
  virtual void insertPage();
  virtual void insertPicture();
  virtual void insertClipart();
  virtual void insertAutoform();

  // tools menu
  virtual void toolsMouse();
  virtual void toolsLine();
  virtual void toolsRectangle();
  virtual void toolsCircleOrEllipse();
  virtual void toolsPie();
  virtual void toolsText();
  virtual void toolsObject();

  virtual void toolsLineHidl();
  virtual void toolsLineVidl();
  virtual void toolsLineD1idl();
  virtual void toolsLineD2idl();
  virtual void toolsNormRectidl();
  virtual void toolsRoundRectidl();

  // extra menu
  virtual void extraPenBrush();
  virtual void extraConfigPie();
  virtual void extraRaise();
  virtual void extraLower();
  virtual void extraRotate();
  virtual void extraShadow();
  virtual void extraAlignObj();
  virtual void extraBackground();
  virtual void extraLayout();
  virtual void extraOptions();

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
  virtual void sizeSelected(const char*);
  virtual void fontSelected(const char*);
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
  
  // ------ C++ ------

  // create GUI - construct
  virtual void createGUI();
  virtual void construct();

  // get - set offsets
  int getDiffX() {return xOffset;}
  int getDiffY() {return yOffset;}
  void setDiffX(int _x) {xOffset = _x;}
  void setDiffY(int _y) {yOffset = _y;}

  // get current pagenum
  unsigned int getCurrPgNum() {return(static_cast<int>(vert->value()+this->height()/2) / vert->pageStep() + 1);}

  // return pointer to document
  class KPresenterDoc *kPresenterDoc() {return m_pKPresenterDoc;}

  // repaint page
  void repaint(bool);
  void repaint(unsigned int,unsigned int,unsigned int,unsigned int,bool);
  void repaint(KRect,bool);

  // properties
  void changePicture(unsigned int,const char*);
  void changeClipart(unsigned int,QString);

  void presentParts(float,QPainter*,KRect,int,int);
  void hideParts();
  void showParts();

  Page* getPage() {return page;}

  void changeUndo(QString,bool);
  void changeRedo(QString,bool);

  void presColorChanged();

  void setRulerMouseShow(bool _show)
    { v_ruler->showMousePos(_show); h_ruler->showMousePos(_show); } 
  void setRulerMousePos(int mx,int my)
    { v_ruler->setMousePos(mx,my); h_ruler->setMousePos(mx,my); } 

  // set scrollbar ranges
  void setRanges();

  KoRuler *getHRuler() { return h_ruler; }
  KoRuler *getVRuler() { return v_ruler; }

  void skipToPage(int _num);
  void makeRectVisible(KRect _rect);

  void restartPresStructView();

  PieType getPieType() { return pieType; }
  int getPieAngle() { return pieAngle; }
  int getPieLength() { return pieLength; }
  QPen getPen() {return pen;} 
  QBrush getBrush() {return brush;} 
  LineEnd getLineBegin() {return lineBegin;} 
  LineEnd getLineEnd() {return lineEnd;} 
  QColor getGColor1() {return gColor1;}
  QColor getGColor2() {return gColor2;}
  BCType getGType() {return gType;}
  FillType getFillType() {return fillType;}

  void setTool(ToolEditMode toolEditMode);

public slots:

  // Document signals
  void slotKPresenterModified();
  void slotInsertObject(KPresenterChild *_child);
  void slotUpdateChildGeometry(KPresenterChild *_child);
  
  // KPresenterFrame signals
  void slotGeometryEnd( KoFrame* );
  void slotMoveEnd( KoFrame* );

protected slots:

  // dialog slots
  void backOk(bool);
  void afChooseOk(const char*);
  void styleOk();
  void optionOk();
  void pgConfOk();
  void effectOk();
  void rotateOk();
  void shadowOk();
  void psvClosed();
  void delPageOk(int,DelPageMode);
  void insPageOk(int,InsPageMode,InsertPos);
  void confPieOk();

  // scrolling
  void scrollH(int);
  void scrollV(int);

  // textobject
  void fontChanged(QFont*);
  void colorChanged(QColor*);
  void alignChanged(TxtParagraph::HorzAlign);

  // graphic objects
  void toolsLineH();
  void toolsLineV();
  void toolsLineD1();
  void toolsLineD2();
  void toolsNormRect();
  void toolsRoundRect();

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
  void search(QString,bool,bool);
  void replace(QString,QString,bool,bool);
  void replaceAll(QString,QString,bool);

  void stopPres() {continuePres = false;}

  // align objs
  void extraAlignObjLeft()
    { extraAlignObjLeftidl(); }
  void extraAlignObjCenterH()
    { extraAlignObjCenterHidl(); }
  void extraAlignObjRight()
    { extraAlignObjRightidl(); }
  void extraAlignObjTop()
    { extraAlignObjTopidl(); }
  void extraAlignObjCenterV()
    { extraAlignObjCenterVidl(); }
  void extraAlignObjBottom()
    { extraAlignObjBottomidl(); }

  // layout
  void newPageLayout(KoPageLayout _layout);
  void openPageLayoutDia()
    { extraLayout(); }


protected:

  // ********* functions ***********

  // C++
  virtual bool event( const char* _event, const CORBA::Any& _value );
  // C++
  virtual bool mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar );
  virtual bool mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory );

  // resize event
  void resizeEvent(QResizeEvent*);

  // GUI
  void setupPopupMenus();
  void setupScrollbars();
  void setupAccelerators();
  void setupRulers();

  // create a pixmapstring from a color
  char* colorToPixString(QColor);
  
  // get fonts
  void getFonts();

  void keyPressEvent(QKeyEvent*);

  void doAutomaticScreenPres();
  
  // ********** variables **********

  // document
  KPresenterDoc *m_pKPresenterDoc;

  // flags
  bool m_bKPresenterModified;
  bool m_bUnderConstruction;
  bool searchFirst,continuePres,exitPres;
  
  // edit menu
  OpenPartsUI::Menu_var m_vMenuEdit;
  CORBA::Long m_idMenuEdit_Undo;
  CORBA::Long m_idMenuEdit_Redo;
  CORBA::Long m_idMenuEdit_Cut;
  CORBA::Long m_idMenuEdit_Copy;
  CORBA::Long m_idMenuEdit_Paste;
  CORBA::Long m_idMenuEdit_Delete;
  CORBA::Long m_idMenuEdit_SelectAll;
  CORBA::Long m_idMenuEdit_DelPage;
  CORBA::Long m_idMenuEdit_Find;
  CORBA::Long m_idMenuEdit_FindReplace;

  // view menu
  OpenPartsUI::Menu_var m_vMenuView;
  CORBA::Long m_idMenuView_NewView;
 
  // insert menu
  OpenPartsUI::Menu_var m_vMenuInsert;
  CORBA::Long m_idMenuInsert_Page;
  CORBA::Long m_idMenuInsert_Picture;
  CORBA::Long m_idMenuInsert_Clipart;
  CORBA::Long m_idMenuInsert_Autoform;

  // tools menu
  OpenPartsUI::Menu_var m_vMenuTools;
  CORBA::Long m_idMenuTools_Mouse;
  OpenPartsUI::Menu_var m_vMenuTools_Line;
  CORBA::Long m_idMenuTools_LineHorz;
  CORBA::Long m_idMenuTools_LineVert;
  CORBA::Long m_idMenuTools_LineD1;
  CORBA::Long m_idMenuTools_LineD2;
  OpenPartsUI::Menu_var m_vMenuTools_Rectangle;
  CORBA::Long m_idMenuTools_RectangleNormal;
  CORBA::Long m_idMenuTools_RectangleRound;
  CORBA::Long m_idMenuTools_Circle;
  CORBA::Long m_idMenuTools_Pie;
  CORBA::Long m_idMenuTools_Text;
  CORBA::Long m_idMenuTools_Part;
 
  // extra menu
  OpenPartsUI::Menu_var m_vMenuExtra;
  CORBA::Long m_idMenuExtra_TFont;
  CORBA::Long m_idMenuExtra_TColor;
  OpenPartsUI::Menu_var m_vMenuExtra_TAlign;
  CORBA::Long m_idMenuExtra_TAlign_Left;
  CORBA::Long m_idMenuExtra_TAlign_Center;
  CORBA::Long m_idMenuExtra_TAlign_Right;
  OpenPartsUI::Menu_var m_vMenuExtra_TType;
  CORBA::Long m_idMenuExtra_TType_EnumList;
  CORBA::Long m_idMenuExtra_TType_UnsortList;
  CORBA::Long m_idMenuExtra_TType_NormalText;
  CORBA::Long m_idMenuExtra_PenBrush;
  CORBA::Long m_idMenuExtra_Pie;
  CORBA::Long m_idMenuExtra_Raise;
  CORBA::Long m_idMenuExtra_Lower;
  CORBA::Long m_idMenuExtra_Rotate;
  CORBA::Long m_idMenuExtra_Shadow;
  OpenPartsUI::Menu_var m_vMenuExtra_AlignObj;
  CORBA::Long m_idMenuExtra_AlignObj_Left;
  CORBA::Long m_idMenuExtra_AlignObj_CenterH;
  CORBA::Long m_idMenuExtra_AlignObj_Right;
  CORBA::Long m_idMenuExtra_AlignObj_Top;
  CORBA::Long m_idMenuExtra_AlignObj_CenterV;
  CORBA::Long m_idMenuExtra_AlignObj_Bottom;
  CORBA::Long m_idMenuExtra_Background;
  CORBA::Long m_idMenuExtra_Layout;
  CORBA::Long m_idMenuExtra_Options;
  
  // screenpresentation menu
  OpenPartsUI::Menu_var m_vMenuScreen;
  CORBA::Long m_idMenuScreen_ConfigPage;
  CORBA::Long m_idMenuScreen_PresStructView;
  CORBA::Long m_idMenuScreen_AssignEffect;
  CORBA::Long m_idMenuScreen_Start;
  CORBA::Long m_idMenuScreen_Stop;
  CORBA::Long m_idMenuScreen_Pause;
  CORBA::Long m_idMenuScreen_First;
  CORBA::Long m_idMenuScreen_Prev;
  CORBA::Long m_idMenuScreen_Next;
  CORBA::Long m_idMenuScreen_Last;
  CORBA::Long m_idMenuScreen_Skip;
  CORBA::Long m_idMenuScreen_FullScreen;
  OpenPartsUI::Menu_var m_vMenuScreen_Pen;
  OpenPartsUI::Menu_var m_vMenuScreen_PenWidth;
  CORBA::Long m_idMenuScreen_PenColor;
  CORBA::Long m_idMenuScreen_PenW1;
  CORBA::Long m_idMenuScreen_PenW2;
  CORBA::Long m_idMenuScreen_PenW3;
  CORBA::Long m_idMenuScreen_PenW4;
  CORBA::Long m_idMenuScreen_PenW5;
  CORBA::Long m_idMenuScreen_PenW6;
  CORBA::Long m_idMenuScreen_PenW7;
  CORBA::Long m_idMenuScreen_PenW8;
  CORBA::Long m_idMenuScreen_PenW9;
  CORBA::Long m_idMenuScreen_PenW10;

  // help menu
  OpenPartsUI::Menu_var m_vMenuHelp;
  CORBA::Long m_idMenuHelp_Contents;

  // right button popup menus
  QPopupMenu *rb_line,*rb_rect,*rb_pen,*rb_pen_width,*rb_oalign;

  int W1,W2,W3,W4,W5,W6,W7,W8,W9,W10,P_COL;

  // edit toolbar
  OpenPartsUI::ToolBar_var m_vToolBarEdit;
  CORBA::Long m_idButtonEdit_Undo;
  CORBA::Long m_idButtonEdit_Redo;
  CORBA::Long m_idButtonEdit_Cut;
  CORBA::Long m_idButtonEdit_Copy;
  CORBA::Long m_idButtonEdit_Paste;
  CORBA::Long m_idButtonEdit_Delete;

  // insert toolbar
  OpenPartsUI::ToolBar_var m_vToolBarInsert;
  CORBA::Long m_idButtonInsert_Page;
  CORBA::Long m_idButtonInsert_Picture;
  CORBA::Long m_idButtonInsert_Clipart;
  CORBA::Long m_idButtonInsert_Autoform;

  // tools toolbar
  OpenPartsUI::ToolBar_var m_vToolBarTools;
  CORBA::Long m_idButtonTools_Mouse;
  CORBA::Long m_idButtonTools_Line;
  CORBA::Long m_idButtonTools_Rectangle;
  CORBA::Long m_idButtonTools_Circle;
  CORBA::Long m_idButtonTools_Pie;
  CORBA::Long m_idButtonTools_Text;
  CORBA::Long m_idButtonTools_Part;

  // text toolbar
  OpenPartsUI::ToolBar_var m_vToolBarText;
  CORBA::Long m_idComboText_FontSize;
  CORBA::Long m_idComboText_FontList;
  CORBA::Long m_idButtonText_Bold;
  CORBA::Long m_idButtonText_Italic;
  CORBA::Long m_idButtonText_Underline;
  CORBA::Long m_idButtonText_Color;
  CORBA::Long m_idButtonText_ARight;
  CORBA::Long m_idButtonText_ACenter;
  CORBA::Long m_idButtonText_ALeft;
  CORBA::Long m_idButtonText_EnumList;
  CORBA::Long m_idButtonText_UnsortList;
  CORBA::Long m_idButtonText_NormalText;

  // extra toolbar
  OpenPartsUI::ToolBar_var m_vToolBarExtra;
  CORBA::Long m_idButtonExtra_Style;
  CORBA::Long m_idButtonExtra_Pie;
  CORBA::Long m_idButtonExtra_Raise;
  CORBA::Long m_idButtonExtra_Lower;
  CORBA::Long m_idButtonExtra_Rotate;
  CORBA::Long m_idButtonExtra_Shadow;
  CORBA::Long m_idButtonExtra_Align;

  // screen toolbar
  OpenPartsUI::ToolBar_var m_vToolBarScreen;
  CORBA::Long m_idButtonScreen_Stop;
  CORBA::Long m_idButtonScreen_Pause;
  CORBA::Long m_idButtonScreen_Start;
  CORBA::Long m_idButtonScreen_First;
  CORBA::Long m_idButtonScreen_Prev;
  CORBA::Long m_idButtonScreen_Next;
  CORBA::Long m_idButtonScreen_Last;
  CORBA::Long m_idButtonScreen_Effect;
  CORBA::Long m_idButtonScreen_Full;
  CORBA::Long m_idButtonScreen_Pen;

  // scrollbars
  QScrollBar *vert,*horz;
  int xOffset,yOffset;
  int _xOffset,_yOffset;

  // frames
  QList<KPresenterFrame> m_lstFrames;

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
  PresStructViewer *presStructView;
  DelPageDia *delPageDia;
  InsPageDia *insPageDia;
  ConfPieDia *confPieDia;

  // default pen and brush
  QPen pen;
  QBrush brush;
  LineEnd lineBegin;
  LineEnd lineEnd;
  QColor gColor1,gColor2;
  BCType gType;
  FillType fillType;
  PieType pieType;
  int pieLength,pieAngle;

  // the page
  Page *page;
  KoRuler *h_ruler,*v_ruler;

  // text toolbar values
  QFont tbFont;
  TxtParagraph::HorzAlign tbAlign;
  QColor tbColor;
  QStrList fontList;

  bool m_bRectSelection;
  KRect m_rctRectSelection;
  QString m_strNewPart;
  bool m_bShowGUI;
  bool presStarted;

  KSize oldSize;

  int screensaver_pid;

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

};

#endif
