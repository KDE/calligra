/***************************************************************************
                          maindlg2.h  -  description
                             -------------------
    begin                : Sat Apr 27 2002
    copyright            : (C) 2002 by Chris Machemer
    email                : machey@ceinetworks.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.         *
 *                                                                         *
 ***************************************************************************/

#ifndef MAINDLG2_H
#define MAINDLG2_H

#include <kdialog.h>
#include "kdbtabledesigner.h"
#include <qbuilddlg.h>
#include <qpaintdevice.h>
#include "kdatabase_struct.h"
#include <klistview.h>
#include <qlistview.h>
#include "ktabctl.h"

/**
  *@author root
  */

class KDBDocBrowser : public KDialog  {
       Q_OBJECT

public:
	KDBDocBrowser(QWidget *parentWidget=0, const char * widgetName=0, bool isModal=false);
	~KDBDocBrowser(void);
   bool initStruct(QDomDocument *kdbFile);
public slots:
   virtual void slotTblItemClicked(QListViewItem *itemClicked);
   virtual void slotViewItemClicked(QListViewItem *itemClicked);
   virtual void slotFormItemClicked(QListViewItem *itemClicked);
   virtual void slotAllItemClicked(QListViewItem *itemClicked);
private:
   bool refreshStructView();
   bool refreshTableView(QDomNode tableSection);
   bool refreshViewView(QDomNode viewSection);
   bool refreshFormView(QDomNode formSection);

    KDBTableDesigner *m_tblDesigner;
    QBuildDlg *myQueryBuilder;
    KDBStruct *m_struct;

    KTabCtl *tabWidget;
    QDomDocument* myKDBFile;
    QListView* KListView1;
    QListView* KListView2;
    QListView* KListView3;
    QListView* KListView4;

protected:
    void closeEvent(QCloseEvent *ev);

signals:
    void hideing();


};

#endif
