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
 *   the Free Software Foundation; version 2 of the License.        *
 *                                                                         *
 ***************************************************************************/

#include <qtabwidget.h>
#include <qdom.h>
#include <qlistview.h>
#include "maindlg.h"
#include "kdebug.h"
#include "kdatabase_struct.h"

MainDlg::MainDlg(QWidget *parentWidget, const char * widgetName):MyDialog1(parentWidget,widgetName){
	//connect(this->tabWidget->page(0)->KListView1,SIGNAL(execute(KListViewItem *)),this,SLOT(slotItemClicked()));
}

MainDlg::~MainDlg(){
}

void MainDlg::slotTblItemClicked(QListViewItem *itemClicked){
     kdDebug() << "KDatabase:MainDlg tbl Item Clicked - " << itemClicked->text(0) << endl;

     myTblDesigner=new clsTblDesigner();
     myTblDesigner->populateTblDesigner(itemClicked->text(0));
     myTblDesigner->show();
}

void MainDlg::slotViewItemClicked(QListViewItem *itemClicked){
     kdDebug() << "KDatabase:MainDlg view Item Clicked" << endl;

     myQueryBuilder=new QBuildDlg();
     myQueryBuilder->show();
}

void MainDlg::slotFormItemClicked(QListViewItem *itemClicked){
     kdDebug() << "KDatabase:MainDlg form Item Clicked" << endl;
}

void MainDlg::slotAllItemClicked(QListViewItem *itemClicked){
     kdDebug() << "KDatabase:MainDlg all Item Clicked" << endl;
}


bool MainDlg::initStruct(QDomDocument *kdbFile){

    myKDBFile = kdbFile;
    myStruct = new KDBStruct(kdbFile);
    refreshStructView();
    return(true);
}		

bool MainDlg::refreshStructView(){

    kdDebug() << "KDatabase:MainDlg refreshStructView" << endl;
    QDomElement mainElement=myKDBFile->documentElement();
    QDomNode structureSection=mainElement.namedItem("STRUCTURE");
    refreshTableView(structureSection.namedItem("TABLES"));
    refreshViewView(structureSection.namedItem("VIEWS"));
    refreshFormView(structureSection.namedItem("FORMS"));

return(true);
}

bool MainDlg::refreshTableView(QDomNode tableSection){

    kdDebug() << "KDatabase:MainDlg refreshTableView" << endl;
    myStruct->getTables();
    return(true);
}

bool MainDlg::refreshViewView(QDomNode viewSection){

    kdDebug() << "KDatabase:MainDlg refreshViewView" << endl;
    return(true);
}

bool MainDlg::refreshFormView(QDomNode formSection){

    kdDebug() << "KDatabase:MainDlg refreshFormView" << endl;
    return(true);
}
