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

#include <knewpanner.h>
#include <ktreelist.h>
#include <kapp.h>

#include "kpobject.h"

class KPresenterDocument_impl;

const QString ObjName[] = {"Picture","Line","Rectangle","Ellipse","Text","Autoform",
			   "Clipart","Undefined"};

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

protected:
  void resizeEvent(QResizeEvent *e);
  void closeEvent(QCloseEvent *e);
  void setupTreeView();

  KNewPanner *panner;
  KTreeList *treelist;
  QWidget *infoWidget;
  KPresenterDocument_impl *doc;
  QList<KTreeListItem> pageList,objList;

};

#endif
