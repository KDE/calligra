/***************************************************************************
                          maindlg.h  -  description
                             -------------------
    begin                : Sat Apr 6 2002
    copyright            : (C) 2002 by Chris Machemer
    email                : machey@ceinetworks.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.        *
 *                                                                         *
 ***************************************************************************/

#ifndef MAINDLG_H
#define MAINDLG_H

#include <qdom.h>
#include <tabdialog1.h>
#include <clstbldesigner.h>
#include <qbuilddlg.h>
#include "kdatabase_struct.h"

/**
  *@author root
  */

class MainDlg : public MyDialog1  {
public: 
	MainDlg(QWidget *parentWidget=0, const char * widgetName=0);
	~MainDlg(void);
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

    clsTblDesigner* myTblDesigner;
    QBuildDlg* myQueryBuilder;

    QDomDocument* myKDBFile;
};

#endif
