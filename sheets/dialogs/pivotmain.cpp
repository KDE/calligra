/* This file is part of the KDE project
   Copyright (C) 2012-2013 Jigar Raisinghani <jigarraisinghani@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local

#include "pivotmain.h"
#include "ui_pivotmain.h"
#include "ui_pivotoptions.h"
#include "pivotoptions.h"
#include "pivotfilters.h"
#include "ui/Selection.h"
#include "Sheet.h"
#include<QMessageBox>
#include "Value.h"
#include "ValueCalc.h"
#include "ValueConverter.h"
#include "Map.h"
#include "DocBase.h"
#include<QTimer>
#include<QObject>
using namespace Calligra::Sheets;

class PivotMain::Private
{
public:
    Selection *selection;
    Ui::PivotMain mainWidget;
};
PivotMain::PivotMain(QWidget* parent, Selection* selection) :
    KDialog(parent),
    d(new Private)
{
    QWidget* widget = new QWidget(this);
    d->mainWidget.setupUi(widget);
    setMainWidget(widget);
    d->selection=selection;
    setCaption(i18n("Pivot Main"));
    setButtons(Ok|Cancel|User1|User2);
    setButtonGuiItem(User1, KGuiItem(i18n("Options")));
    setButtonGuiItem(User2, KGuiItem(i18n("Add Filter")));
    enableButton(User1,true);
    enableButton(User2,true);
    enableButton(Ok,true);
    d->mainWidget.TotalRows->setChecked(true);
    d->mainWidget.TotalColumns->setChecked(true);


    d->mainWidget.Labels->setSelectionMode(QAbstractItemView::ExtendedSelection);
    d->mainWidget.Labels->setDragEnabled(true);
    d->mainWidget.Labels->setDragDropMode(QAbstractItemView::InternalMove);
    d->mainWidget.Labels->viewport()->setAcceptDrops(true);
    d->mainWidget.Labels->setDropIndicatorShown(true);

    d->mainWidget.Rows->setSelectionMode(QAbstractItemView::SingleSelection);
    d->mainWidget.Rows->setDragEnabled(true);
    d->mainWidget.Rows->setDragDropMode(QAbstractItemView::DropOnly);
    d->mainWidget.Rows->viewport()->setAcceptDrops(true);
    d->mainWidget.Rows->setDropIndicatorShown(true);

    d->mainWidget.Columns->setSelectionMode(QAbstractItemView::SingleSelection);
    d->mainWidget.Columns->setDragEnabled(true);
    d->mainWidget.Columns->setDragDropMode(QAbstractItemView::DropOnly);
    d->mainWidget.Columns->viewport()->setAcceptDrops(true);
    d->mainWidget.Columns->setDropIndicatorShown(true);

    d->mainWidget.Values->setSelectionMode(QAbstractItemView::SingleSelection);
    d->mainWidget.Values->setDragEnabled(true);
    d->mainWidget.Values->setDragDropMode(QAbstractItemView::DropOnly);
    d->mainWidget.Values->viewport()->setAcceptDrops(true);
    d->mainWidget.Values->setDropIndicatorShown(true);

    d->mainWidget.PageFields->setSelectionMode(QAbstractItemView::SingleSelection);
    d->mainWidget.PageFields->setDragEnabled(true);
    d->mainWidget.PageFields->setDragDropMode(QAbstractItemView::DropOnly);
    d->mainWidget.PageFields->viewport()->setAcceptDrops(true);
    d->mainWidget.PageFields->setDropIndicatorShown(true);
    
    connect(this,SIGNAL(user2Clicked()),this,SLOT(on_AddFilter_clicked()));
    connect(this, SIGNAL(user1Clicked()), this, SLOT(on_Options_clicked()));
    extractColumnNames();
    connect(this, SIGNAL(okClicked()), this, SLOT(on_Ok_clicked()));
}

PivotMain::~PivotMain()
{
    delete d;
}
void PivotMain::extractColumnNames()
{
    Sheet *const sheet = d->selection->lastSheet();
    const QRect range = d->selection->lastRange();

    int r = range.right();
    int row = range.top();

    Cell cell;
    QListWidgetItem * item;
    QString text;
    
    for (int i = range.left(); i <= r; ++i) {
        cell = Cell(sheet, i, row);
        text = cell.displayText();
       
	if(text.length() >0)
	{
        item = new QListWidgetItem(text);
	item->setFlags(item->flags());
        d->mainWidget.Labels->addItem(item);
	}  
    }  
}


void PivotMain::on_Options_clicked()
{
    PivotOptions *pOptions=new PivotOptions(this,d->selection);
    pOptions->setModal(true);
    pOptions->exec();
}

void PivotMain::on_AddFilter_clicked()
{

        PivotFilters *pFilters=new PivotFilters(this,d->selection);
        pFilters->setModal(true);
        pFilters->exec();

}
void PivotMain::Summarize()
{
    Sheet *const sheet = d->selection->lastSheet();
    const QRect range = d->selection->lastRange();
    
    Map* myMap = sheet->map();
    Sheet* mySheet=myMap->createSheet("PivotTest");
    
    int r = range.right();
    int row=range.top();
    int bottom=range.bottom();
    Cell cell;
    
    ValueConverter *c;
    
    Value res(0);
    ValueCalc *calc= new ValueCalc(c);
    
    QVector<Value> vect;    
    for (int i = range.left(); i <= r; ++i) {
	cell= Cell(sheet,i,row);
	vect.append(Value(cell.value()));
	
      }


  
  //For Creating QLists for Rows,Columns,Values and PageField
  int counter;
  QListWidgetItem *item1;
  QList<QListWidgetItem *> rowList,columnList,valueList,pageList;
  
  counter= d->mainWidget.Rows->count();
  for(int i=0;i<counter;i++)
  {
	
        item1=d->mainWidget.Rows->item(i);
        rowList.append(item1);
    
  }
  counter= d->mainWidget.Columns->count();
  for(int i=0;i<counter;i++)
  {
	
        item1=d->mainWidget.Columns->item(i);
        columnList.append(item1); 
  }
  counter= d->mainWidget.PageFields->count();
  for(int i=0;i<counter;i++)
  {
        item1=d->mainWidget.PageFields->item(i);
        pageList.append(item1);
  }
  counter= d->mainWidget.Values->count();
  for(int i=0;i<counter;i++)
  {
        item1=d->mainWidget.Values->item(i);
        valueList.append(item1); 
  }
  
  //Summarization using vectors
  int rowpos=-1,colpos=-1,valpos=-1;
  QVector<Value> rowVector,rowVectorArr[rowList.size()],columnVectorArr[columnList.size()],columnVector,valueVector;
  QVector<int> rowposVect,colposVect,valposVect;
  
  for(int i=0;i<rowList.size();i++)
  {
      rowpos=vect.indexOf(Value(rowList.at(i)->text()));
      for(int j=row+1;j<=bottom;j++)
      {
	cell =Cell(sheet,rowpos+1,j);
	if(rowVector.contains(Value(cell.value()))==0)
	{
	  rowVector.append(Value(cell.value()));
	  rowVectorArr[i].append(Value(cell.value()));
      
	}  
      }
      rowposVect.append(rowpos);
  }
    
  for(int i=0;i<columnList.size();i++)
  {
      colpos=vect.indexOf(Value(columnList.at(i)->text()));
      for(int j=row+1;j<=bottom;j++)
      {
	cell =Cell(sheet,colpos+1,j);
	if(columnVector.contains(Value(cell.value()))==0)
	{
 	columnVector.append(Value(cell.value()));
	columnVectorArr[i].append(Value(cell.value()));
	}
      }
      colposVect.append(colpos);
  }
  
  int count=1,count2=0,prevcount=1;
  QVector<Value> rowVect[rowposVect.count()];
  for(int i=0;i<rowposVect.count();i++)
  {
    for(int j=i+1;j<rowposVect.count();j++)
    {
      count*=rowVectorArr[j].count();
    }
    for(int k=0;k<(rowVectorArr[i].count())*prevcount;k++)
    {
      Cell(mySheet,((k)*count)+1+colposVect.count(),i+1).setValue(rowVectorArr[i].at(k%rowVectorArr[i].count()));
      
      for(int l=0;l<count;l++)
	rowVect[i].append(rowVectorArr[i].at(k%rowVectorArr[i].count()));
      
      count2++;
    }
    prevcount=count2;
    count=1;
    count2=0;
  }

  count=1,count2=0,prevcount=1;
  QVector<Value> colVect[colposVect.count()];
  for(int i=0;i<colposVect.count();i++)
  {
    for(int j=i+1;j<colposVect.count();j++)
    {
      count*=columnVectorArr[j].count();
    }
    for(int k=0;k<(columnVectorArr[i].count())*prevcount;k++)
    {
       
      Cell(mySheet,i+1,((k)*count)+1+rowposVect.count()).setValue(columnVectorArr[i].at(k%columnVectorArr[i].count()));
      
      for(int l=0;l<count;l++)
	colVect[i].append(columnVectorArr[i].at(k%columnVectorArr[i].count()));
      
      count2++;
    }
    prevcount=count2;
    count=1;
    count2=0;
  }  
  
  for(int i=0;i<valueList.size();i++)
  {
     valpos=vect.indexOf(Value(valueList.at(i)->text()));
     valposVect.append(valpos);    
  }
  
  

  Cell(mySheet,1,1).setValue(Value("PIVOT TABLES"));
  for(int l=0;l<rowVect[0].count();l++)
  {
    for(int m=0;m<colVect[0].count();m++)
      {

	      QVector<Value> aggregate;
	      for(int k=row+1;k<=bottom;k++)
	      {int flag=0;
		for(int i=0;i<rowposVect.count();i++)
		{
		  for(int j=0;j<colposVect.count();j++)
		    {
 
		      if(!(Cell(sheet,rowposVect.at(i)+1,k).value()==rowVect[i].at(l) && Cell(sheet,colposVect.at(j)+1,k).value()==colVect[j].at(m)))
			flag=1;
		      
		    }
		  }
		if(flag==0)
		aggregate.append(Cell(sheet,valpos+1,k).value());
	      }

		calc->arrayWalk(aggregate,res,calc->awFunc("sum"),Value(0));
 		
		Cell(mySheet,l+colposVect.count()+1,m+rowposVect.count()+1).setValue(res);

		aggregate.clear();
		res=Value(0);
	    
      }
    }
  
  
  //Clearing Vectors
  rowVector.clear();
  columnVector.clear();
  valueVector.clear();
  rowposVect.clear();
  colposVect.clear();
  valposVect.clear();
  
  //Adding built sheet to myMap for viewing
  myMap->addSheet(mySheet);
  
}

void PivotMain::on_Ok_clicked()
{
  
  Summarize();
  QMessageBox msgBox;
  msgBox.setText("Pivot Tables Built");
  msgBox.exec();
}
