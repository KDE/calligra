/***************************************************************************
                          maindlg2.cpp  -  description
                             -------------------
    begin                : Sat Apr 27 2002
    copyright            : (C) 2002 by root
    email                : machey@ceinetworks.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License.         *
 *                                                                         *
 ***************************************************************************/

#include "maindlg2.h"
#include "kdebug.h"
#include "kdatabase_struct.h"
#include <klistview.h>
#include "ktabctl.h"

MainDlg2::MainDlg2(QWidget *parentWidget, const char * widgetName, bool isModal):KDialog(parentWidget,widgetName,isModal){
   tabWidget = new KTabCtl(this, "tabWidget");
   tabWidget->resize(525,320);
   KListView1 = new QListView(tabWidget, "KListView1");
   KListView2 = new QListView(tabWidget, "KListView2");
   KListView3 = new QListView(tabWidget, "KListView3");
   KListView4 = new QListView(tabWidget, "KListView4");
   KListView1->addColumn("Table");
   KListView1->addColumn("Description");
   KListView2->addColumn("View");
   KListView2->addColumn("Description");
   KListView3->addColumn("Form");
   KListView3->addColumn("Description");

   QListViewItem tblAdd = new QListViewItem(KListView1,"Add Table", "Click here to add a new table");
   QListViewItem vwAdd = new QListViewItem(KListView2,"Add View", "Click here to add a new view");
   QListViewItem frmAdd = new QListViewItem(KListView3,"Add Form", "Click here to add a new form");
   //tblAdd.setText(1, "Click here to add a new table");
   tblAdd.repaint();
   tabWidget->addTab(KListView1,"Tables");
   tabWidget->addTab(KListView2, "Views");
   tabWidget->addTab(KListView3, "Forms");
   tabWidget->addTab(KListView4, "All");

	connect(KListView1,SIGNAL(clicked(QListViewItem *)),this,SLOT(slotTblItemClicked(QListViewItem *)));
	connect(KListView2,SIGNAL(clicked(QListViewItem *)),this,SLOT(slotViewItemClicked(QListViewItem *)));
	connect(KListView3,SIGNAL(clicked(QListViewItem *)),this,SLOT(slotFormItemClicked(QListViewItem *)));


   resize(525,360);
   }

MainDlg2::~MainDlg2(){
}

void MainDlg2::slotTblItemClicked(QListViewItem *itemClicked){

     if(!(itemClicked == NULL)) {
        kdDebug() << "KDatabase:MainDlg tbl Item Clicked - " << itemClicked->text(0) << endl;
        myTblDesigner=new clsTblDesigner2();
        myTblDesigner->populateTblDesigner(itemClicked->text(0));
        myTblDesigner->show();
        }
}

void MainDlg2::slotViewItemClicked(QListViewItem *itemClicked){

     if(!(itemClicked == NULL)) {
        kdDebug() << "KDatabase:MainDlg view Item Clicked - " << itemClicked->text(0) << endl;
        myQueryBuilder=new QBuildDlg();
        myQueryBuilder->show();
        }
}

void MainDlg2::slotFormItemClicked(QListViewItem *itemClicked){
     kdDebug() << "KDatabase:MainDlg form Item Clicked" << endl;
}

void MainDlg2::slotAllItemClicked(QListViewItem *itemClicked){
     kdDebug() << "KDatabase:MainDlg all Item Clicked" << endl;
}


bool MainDlg2::initStruct(QDomDocument *kdbFile){

    myKDBFile = kdbFile;
    myStruct = new KDBStruct(kdbFile);
    refreshStructView();
    return(true);
}		

bool MainDlg2::refreshStructView(){

    kdDebug() << "KDatabase:MainDlg refreshStructView" << endl;
    QDomElement mainElement=myKDBFile->documentElement();
    QDomNode structureSection=mainElement.namedItem("STRUCTURE");
    refreshTableView(structureSection.namedItem("TABLES"));
    refreshViewView(structureSection.namedItem("VIEWS"));
    refreshFormView(structureSection.namedItem("FORMS"));

return(true);
}

bool MainDlg2::refreshTableView(QDomNode tableSection){

    QPtrList<QString> myList;
    QString myItemName;

    kdDebug() << "KDatabase:MainDlg refreshTableView" << endl;
    myList = myStruct->getTables();
    QPtrListIterator<QString> myIterator(myList);
    if(!(myIterator.isEmpty())) {
       while(!(myIterator==NULL)) {
         QString tmpTable = myIterator.current()->latin1();
         ++myIterator;
         QListViewItem myItem=new QListViewItem(KListView1,tmpTable,myIterator.current()->latin1());
         ++myIterator;
         }
      }
    return(true);
}

bool MainDlg2::refreshViewView(QDomNode viewSection){

    kdDebug() << "KDatabase:MainDlg refreshViewView" << endl;
    return(true);
}

bool MainDlg2::refreshFormView(QDomNode formSection){

    kdDebug() << "KDatabase:MainDlg refreshFormView" << endl;
    return(true);
}

#include "maindlg2.moc"
