/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
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

#include <qwidget.h>
#include <qevent.h>
#include <qdialog.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qlist.h>
#include <qcolor.h>

#include <knewpanner.h>
#include <ktreelist.h>
#include <kapp.h>
#include <ktablistbox.h>

#include "kpobject.h"
#include "kpbackground.h"

class KPresenterDocument_impl;

const QString ObjName[] = {"Picture","Line","Rectangle","Ellipse","Text","Autoform",
			   "Clipart","Undefined"};
const QString BackTypeName[] = {"Color","Picture","Clipart"};
const QString BackViewName[] = {"Zoom","Center","Tiled"};
const QString BackColorTypeName[] = {"Plain","Horizontal Gradient","Vertical Gradient","Diagonal Gradient 1","Diagonal Gradient 2",
				     "Circle Gradient","Rectangle Gradient"};
const QString PageEffectName[] = {"No effect","Close horizontal","Close vertical","Close from all directions",
				  "Open horizontal","Open vertical","Open from all directions","Interlocking horizontal 1",
				  "Interlocking horizontal 2","Interlocking vertical 1","Interlocking vertical 2"};
const QString ShadowDirectionName[] = {"","Left/Up","Up","Right/Up","Right","Right/Bottom","Bottom","Left/Bottom","Left"};
const QString EffectName[] = {"No Effect","Come from right","Come from left","Come from top","Come from bottom","Come from right/top",
			      "Come from right/bottom","Come from left/top","Come from left/bottom","Wipe from left","Wipe from right",
			      "Wipe from top","Wipe from bottom"};
const QString Effect2Name[] = {"No Effect","Paragraph after paragraph"};

/******************************************************************/
/* Class: PresStructViewer                                        */
/******************************************************************/

class PresStructViewer : public QDialog
{
  Q_OBJECT

public:
  PresStructViewer(QWidget *parent,const char *name,KPresenterDocument_impl *_doc);

signals:
  void presStructViewClosed();

protected slots:
  void itemSelected(int);

protected:
  struct ItemInfo
  {
    int num;
    KTreeListItem *item;
  };

  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);
  void setupTreeView();
  void fillWithPageInfo(KPBackGround *_page,int _num);
  void fillWithObjInfo(KPObject *_obj,int _num);

  KNewPanner *panner;
  KTreeList *treelist;
  KTabListBox *list;
  KPresenterDocument_impl *doc;
  QList<ItemInfo> pageList,objList;

};

#endif
