/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page Structure Viewer (header)                         */
/******************************************************************/

#ifndef presstructview_h
#define presstructview_h

#include <stdlib.h>

#include <qwidget.h>
#include <qtableview.h>
#include <qevent.h>
#include <qdialog.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qlist.h>
#include <qcolor.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qpen.h>
#include <qbrush.h>

#include <knewpanner.h>
#include <ktreelist.h>
#include <kapp.h>
#include <ktablistbox.h>
#include <ktoolbar.h>

#include "kpobject.h"
#include "kpbackground.h"

class KPresenterDocument_impl;
class KPresenterView_impl;

const QString ObjName[] = {"Picture","Line","Rectangle","Ellipse","Text","Autoform",
			   "Clipart","Undefined"};
const QString BackTypeName[] = {"Color","Picture","Clipart"};
const QString BackViewName[] = {"Zoom","Center","Tiled"};
const QString BackColorTypeName[] = {"Plain","Horizontal Gradient","Vertical Gradient","Diagonal Gradient 1","Diagonal Gradient 2",
				     "Circle Gradient","Rectangle Gradient"};
const QString PageEffectName[] = {"No effect","Close horizontal","Close vertical","Close from all directions",
				  "Open horizontal","Open vertical","Open from all directions","Interlocking horizontal 1",
				  "Interlocking horizontal 2","Interlocking vertical 1","Interlocking vertical 2",
				  "Sourrond 1"};
const QString ShadowDirectionName[] = {"","Left/Up","Up","Right/Up","Right","Right/Bottom","Bottom","Left/Bottom","Left"};
const QString EffectName[] = {"No Effect","Come from right","Come from left","Come from top","Come from bottom","Come from right/top",
			      "Come from right/bottom","Come from left/top","Come from left/bottom","Wipe from left","Wipe from right",
			      "Wipe from top","Wipe from bottom"};
const QString Effect2Name[] = {"No Effect","Paragraph after paragraph"};
const QString PenStyleName[] = {"no pen","solid line","dash line (----)","dot line (****)","dash dot line (-*-*)",
				"dash dot dot line (-**-)"};
const QString BrushStyleName[] = {"no brush","100% fill pattern","94% fill pattern","88% fill pattern","63% fill pattern",
				  "50% fill pattern","37% fill pattern","12% fill pattern","6% fill pattern","horizontal lines",
				  "vertical lines","crossing lines","diagonal lines (/)","diagonal lines (\\)","diagonal crossing lines"};
const QString LineTypeName[] = {"Horizontal","Vertical","Left/Top - Right/Bottom","Left/Bottom - Right/Top"};
const QString LineEndName[] = {"Normal","Arrow","Square","Circle"};

/******************************************************************/
/* Class: PVTimeTable                                             */
/******************************************************************/

class PVTimeTable : public QTableView
{
  Q_OBJECT

public:
  PVTimeTable(QWidget *parent,KPresenterDocument_impl *_doc);

  void setPageNum(int _num);

protected:
  void paintCell(QPainter *painter,int row,int col);

  int page;
  KPresenterDocument_impl *doc;

};

/******************************************************************/
/* Class: PresStructViewer                                        */
/******************************************************************/

class PresStructViewer : public QDialog
{
  Q_OBJECT

public:
  PresStructViewer(QWidget *parent,const char *name,KPresenterDocument_impl *_doc,KPresenterView_impl *_view);

signals:
  void presStructViewClosed();

protected slots:
  void itemSelected(int);

  void slotStyle();
  void slotRotate();
  void slotShadow();
  void slotAlign();
  void slotEffect();
  void slotLower();
  void slotRaise();
  void slotDelete();
  void slotEdit();
  void slotBackground();
  void slotConfigPages();
  void slotChangeFilename();

protected:
  struct ItemInfo
  {
    int num;
    KTreeListItem *item;
  };

  static const int B_STYLE  = 1;
  static const int B_ROTATE = 2;
  static const int B_SHADOW = 3;
  static const int B_ALIGN  = 4;
  static const int B_EFFECT = 5;
  static const int B_LOWER  = 6;
  static const int B_RAISE  = 7;
  static const int B_DELETE = 8;
  static const int B_EDIT   = 9;
  static const int B_BACK   = 10;
  static const int B_CPAGES = 11;
  static const int B_CFILEN = 12;
  
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);
  void setupTreeView();
  void setupToolBar();
  void fillWithPageInfo(KPBackGround *_page,int _num);
  void fillWithObjInfo(KPObject *_obj,int _num);
  QString getColor(QColor _color);
  void disableAllFunctions();

  KNewPanner *panner,*h_panner;
  KTreeList *treelist;
  KTabListBox *list;
  KPresenterDocument_impl *doc;
  KPresenterView_impl *view;
  QList<ItemInfo> pageList,objList;
  KPObject *lastSelected;
  KToolBar *toolbar;
  PVTimeTable *timeTable;

};

#endif
