/***************************************************************************
                          maindlg.cpp  -  description
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

#include <qtabwidget.h>
#include <qlistview.h>
#include "maindlg.h"
#include "kdebug.h"

MainDlg::MainDlg(QWidget *parentWidget, const char * widgetName):MyDialog1(parentWidget,widgetName){
	//connect(this->tabWidget->page(0)->KListView1,SIGNAL(execute(KListViewItem *)),this,SLOT(slotItemClicked()));
}

MainDlg::~MainDlg(){
}

void MainDlg::slotTblItemClicked(QListViewItem *itemClicked){
     kdDebug() << "KDatabase:MainDlg tbl Item Clicked - " << itemClicked->text(0) << endl;

     myTblDesigner=new clsTblDesigner();
     myTblDesigner->show();
}

void MainDlg::slotViewItemClicked(QListViewItem *itemClicked){
     kdDebug() << "KDatabase:MainDlg view Item Clicked" << endl;
}

void MainDlg::slotFormItemClicked(QListViewItem *itemClicked){
     kdDebug() << "KDatabase:MainDlg form Item Clicked" << endl;
}

void MainDlg::slotAllItemClicked(QListViewItem *itemClicked){
     kdDebug() << "KDatabase:MainDlg all Item Clicked" << endl;
}
