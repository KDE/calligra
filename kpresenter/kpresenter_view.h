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
/* Module: KPresenter View (header)                               */
/******************************************************************/

#ifndef __kpresenter_view_h__
#define __kpresenter_view_h__

class KPresenterView_impl;
class KPresenterShell_impl;
class KPresenterDocument_impl;
class KPresenterChild;
class BackDia;
class Page;

#include <stdlib.h>

#include <view_impl.h>
#include <document_impl.h>
#include <part_frame_impl.h>
#include <menu_impl.h>
#include <toolbar_impl.h>
#include <op_app.h>
#include <utils.h>
#include <part_frame_impl.h>
#include <koPartSelectDia.h>

#include <qwidget.h>
#include <qrect.h>
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
#include <qpoint.h>

#include <kcolordlg.h>
#include <koAboutDia.h>
#include <koPageLayoutDia.h>
#include <kfontdialog.h>

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

#include <X11/Xlib.h>
#include <signal.h>

/******************************************************************/
/* class KPresenterFrame                                          */
/******************************************************************/
class KPresenterFrame : public PartFrame_impl
{
  Q_OBJECT

public:

  // constructor
  KPresenterFrame(KPresenterView_impl*,KPresenterChild*);
  
  // get child
  KPresenterChild* child() {return m_pKPresenterChild;}

  // get view
  KPresenterView_impl* view() {return m_pKPresenterView;}
  
protected:

  // child
  KPresenterChild *m_pKPresenterChild;

  // view
  KPresenterView_impl *m_pKPresenterView;

};

/*****************************************************************/
/* class KPresenterView_impl                                     */
/*****************************************************************/
class KPresenterView_impl : public QWidget,
			    virtual public View_impl,
			    virtual public KPresenter::KPresenterView_skel
{
  Q_OBJECT

public:

  // ------ C++ ------
  // constructor - destructor
  KPresenterView_impl(QWidget *_parent = 0L,const char *_name = 0L);
  ~KPresenterView_impl();

  // clean
  virtual void cleanUp();
  
  // ------ IDL ------
  // edit menu
  virtual void editCut();
  virtual void editCopy();
  virtual void editPaste();
  virtual void editDelete();
  virtual void editSelectAll();
  
  // view menu
  virtual void newView();

  // insert menu
  virtual void insertPage();
  virtual void insertPicture();
  virtual void insertClipart();
  virtual void insertLine();
  virtual void insertRectangle();
  virtual void insertCircleOrEllipse();
  virtual void insertText();
  virtual void insertAutoform();

  void insertLineHidl();
  void insertLineVidl();
  void insertLineD1idl();
  void insertLineD2idl();
  void insertNormRectidl();
  void insertRoundRectidl();
  void insertObject();

  // extra menu
  virtual void extraPenBrush();
  virtual void extraRaise();
  virtual void extraLower();
  virtual void extraRotate();
  virtual void extraBackground();
  virtual void extraLayout();
  virtual void extraOptions();

  // screen menu
  virtual void screenConfigPages();
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

  // help menu
  virtual void helpContents();
  virtual void helpAbout();
  virtual void helpAboutKOffice();
  virtual void helpAboutKDE();

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
  
  virtual void setMode(OPParts::Part::Mode _mode);
  virtual void setFocus(CORBA::Boolean mode);

  // ------ C++ ------
  // set document
  virtual void setDocument(KPresenterDocument_impl *_doc);

  // create GUI - construct
  virtual void createGUI();
  virtual void construct();

  // get - set offsets
  int getDiffX() {return xOffset;}
  int getDiffY() {return yOffset;}
  void setDiffX(int _x) {xOffset = _x;}
  void setDiffY(int _y) {yOffset = _y;}

  // get current pagenum
  unsigned int getCurrPgNum() {return((int)(vert->value()+this->height()/2) / vert->pageStep() + 1);}

  // return pointer to document
  class KPresenterDocument_impl *KPresenterDoc() {return m_pKPresenterDoc;}

  // repaint page
  void repaint(bool);
  void repaint(unsigned int,unsigned int,unsigned int,unsigned int,bool);

  // properties
  void changePicture(unsigned int,const char*);
  void changeClipart(unsigned int,const char*);

  QPen getPen() {return pen;} 
  void presentParts(float,QPainter*,QRect,int,int);

public slots:

  // Document signals
  void slotKPresenterModified();
  void slotInsertObject(KPresenterChild *_child);
  void slotUpdateChildGeometry(KPresenterChild *_child);
  
  // KPresenterFrame signals
  void slotGeometryEnd(PartFrame_impl*);
  void slotMoveEnd(PartFrame_impl*);
  
protected slots:

  // dialog slots
  void backOk(bool);
  void afChooseOk(const char*);
  void styleOk();
  void optionOk();
  void pgConfOk();
  void effectOk();

  // scrolling
  void scrollH(int);
  void scrollV(int);

  // textobject
  void fontChanged(QFont*);
  void colorChanged(QColor*);
  void alignChanged(TxtParagraph::HorzAlign);

  // graphic objects
  void insertLineH();
  void insertLineV();
  void insertLineD1();
  void insertLineD2();
  void insertNormRect();
  void insertRoundRect();

protected:

  // ********* functions ***********

  // resize event
  void resizeEvent(QResizeEvent*);

  // GUI
  void setupMenu();
  void setupPopupMenus();
  void setupEditToolbar();
  void setupInsertToolbar();
  void setupTextToolbar();
  void setupExtraToolbar();
  void setupScreenToolbar();
  void setupScrollbars();

  // set scrollbar ranges
  void setRanges();
  
  // create a pixmapstring from a color
  char* colorToPixString(QColor);
  
  // get fonts
  void getFonts();

  void startRectSelection(const char*);
  void cancelRectSelection();
  void paintRectSelection();
  void mousePressEvent(QMouseEvent*);
  void mouseReleaseEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void keyPressEvent(QKeyEvent*);
  
  // screen presentation
  void hideParts();
  void showParts();

  // ********** variables **********

  // document
  KPresenterDocument_impl *m_pKPresenterDoc;

  // flags
  bool m_bKPresenterModified;
  bool m_bUnderConstruction;
  
  // menubar
  OPParts::MenuBarFactory_var m_vMenuBarFactory;
  MenuBar_ref m_rMenuBar;

  // edit menu
  CORBA::Long m_idMenuEdit;
  CORBA::Long m_idMenuEdit_Cut;
  CORBA::Long m_idMenuEdit_Copy;
  CORBA::Long m_idMenuEdit_Paste;
  CORBA::Long m_idMenuEdit_Delete;
  CORBA::Long m_idMenuEdit_SelectAll;

  // view menu
  CORBA::Long m_idMenuView;
  CORBA::Long m_idMenuView_NewView;
 
  // insert menu
  CORBA::Long m_idMenuInsert;
  CORBA::Long m_idMenuInsert_Page;
  CORBA::Long m_idMenuInsert_Picture;
  CORBA::Long m_idMenuInsert_Clipart;
  CORBA::Long m_idMenuInsert_Line;
  CORBA::Long m_idMenuInsert_LineHorz;
  CORBA::Long m_idMenuInsert_LineVert;
  CORBA::Long m_idMenuInsert_LineD1;
  CORBA::Long m_idMenuInsert_LineD2;
  CORBA::Long m_idMenuInsert_Rectangle;
  CORBA::Long m_idMenuInsert_RectangleNormal;
  CORBA::Long m_idMenuInsert_RectangleRound;
  CORBA::Long m_idMenuInsert_Circle;
  CORBA::Long m_idMenuInsert_Text;
  CORBA::Long m_idMenuInsert_Autoform;
  CORBA::Long m_idMenuInsert_Part;
 
  // extra menu
  CORBA::Long m_idMenuExtra;
  CORBA::Long m_idMenuExtra_TFont;
  CORBA::Long m_idMenuExtra_TColor;
  CORBA::Long m_idMenuExtra_TAlign;
  CORBA::Long m_idMenuExtra_TAlign_Left;
  CORBA::Long m_idMenuExtra_TAlign_Center;
  CORBA::Long m_idMenuExtra_TAlign_Right;
  CORBA::Long m_idMenuExtra_TType;
  CORBA::Long m_idMenuExtra_TType_EnumList;
  CORBA::Long m_idMenuExtra_TType_UnsortList;
  CORBA::Long m_idMenuExtra_TType_NormalText;
  CORBA::Long m_idMenuExtra_PenBrush;
  CORBA::Long m_idMenuExtra_Raise;
  CORBA::Long m_idMenuExtra_Lower;
  CORBA::Long m_idMenuExtra_Rotate;
  CORBA::Long m_idMenuExtra_Background;
  CORBA::Long m_idMenuExtra_Layout;
  CORBA::Long m_idMenuExtra_Options;
  
  // screenpresentation menu
  CORBA::Long m_idMenuScreen;
  CORBA::Long m_idMenuScreen_ConfigPage;
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
  CORBA::Long m_idMenuScreen_Pen;

  // help menu
  CORBA::Long m_idMenuHelp;
  CORBA::Long m_idMenuHelp_Contents;
  CORBA::Long m_idMenuHelp_About;
  CORBA::Long m_idMenuHelp_AboutKOffice;
  CORBA::Long m_idMenuHelp_AboutKDE;

  // right button popup menus
  QPopupMenu *rb_line,*rb_rect;

  // toolbar
  OPParts::ToolBarFactory_var m_vToolBarFactory;

  // edit toolbar
  ToolBar_ref m_rToolBarEdit;
  CORBA::Long m_idButtonEdit_Cut;
  CORBA::Long m_idButtonEdit_Copy;
  CORBA::Long m_idButtonEdit_Paste;
  CORBA::Long m_idButtonEdit_Delete;

  // insert toolbar
  ToolBar_ref m_rToolBarInsert;
  CORBA::Long m_idButtonInsert_Picture;
  CORBA::Long m_idButtonInsert_Clipart;
  CORBA::Long m_idButtonInsert_Line;
  CORBA::Long m_idButtonInsert_Rectangle;
  CORBA::Long m_idButtonInsert_Circle;
  CORBA::Long m_idButtonInsert_Text;
  CORBA::Long m_idButtonInsert_Autoform;
  CORBA::Long m_idButtonInsert_Part;

  // text toolbar
  ToolBar_ref m_rToolBarText;
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
  ToolBar_ref m_rToolBarExtra;
  CORBA::Long m_idButtonExtra_Style;
  CORBA::Long m_idButtonExtra_Raise;
  CORBA::Long m_idButtonExtra_Lower;
  CORBA::Long m_idButtonExtra_Rotate;

  // screen toolbar
  ToolBar_ref m_rToolBarScreen;
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

  // default pen and brush
  QPen pen;
  QBrush brush;

  // the page
  Page *page;

  // text toolbar values
  QFont tbFont;
  TxtParagraph::HorzAlign tbAlign;
  QColor tbColor;
  QStrList fontList;

  bool m_bRectSelection;
  QRect m_rctRectSelection;
  QString m_strNewPart;
  bool m_bShowGUI;
  bool presStarted;

  KPresenterShell_impl* shell;
  QSize oldSize;

  int screensaver_pid;
};

#endif
