/******************************************************************/ 
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Table Dialog                                           */
/******************************************************************/

#include "kword_doc.h"

#include "tabledia.h"
#include "tabledia.moc"

/******************************************************************/
/* Class: KWTablePreview                                          */
/******************************************************************/

/*================================================================*/
void KWTablePreview::paintEvent(QPaintEvent *e)
{
  int wid = (width() - 10) / cols;
  int hei = (height() - 10) / rows;

  QPainter p;
  p.begin(this);

  p.setPen(QPen(lightGray));

  for (int i = 0;i < rows;i++)
    {
      for (int j = 0;j < cols;j++)
	p.drawRect(j * wid + 5,i * hei + 5,wid,hei);
    }
	
  p.end();
}

/******************************************************************/
/* Class: KWTableDia                                              */
/******************************************************************/

/*================================================================*/
KWTableDia::KWTableDia(QWidget* parent,const char* name,KWPage *_page,int rows,int cols)
  : QTabDialog(parent,name,true)
{
  page = _page;

  setupTab1(rows,cols);
    
  setCancelButton(i18n("Cancel"));
  setOkButton(i18n("OK"));
}

/*================================================================*/
void KWTableDia::setupTab1(int rows,int cols)
{
  tab1 = new QWidget(this);

  grid1 = new QGridLayout(tab1,5,2,15,7);

  lRows = new QLabel(i18n("Number of Rows:"),tab1);
  lRows->resize(lRows->sizeHint());
  grid1->addWidget(lRows,0,0);

  nRows = new QSpinBox(1,128,1,tab1);
  nRows->resize(nRows->sizeHint());
  nRows->setValue(rows);
  grid1->addWidget(nRows,1,0);

  lCols = new QLabel(i18n("Number of Columns:"),tab1);
  lCols->resize(lCols->sizeHint());
  grid1->addWidget(lCols,2,0);

  nCols = new QSpinBox(1,128,1,tab1);
  nCols->resize(nCols->sizeHint());
  nCols->setValue(cols);
  grid1->addWidget(nCols,3,0);

  preview = new KWTablePreview(tab1,rows,cols);
  preview->setBackgroundColor(white);
  grid1->addMultiCellWidget(preview,0,4,1,1);

  grid1->addRowSpacing(0,lRows->height());
  grid1->addRowSpacing(1,nRows->height());
  grid1->addRowSpacing(2,lCols->height());
  grid1->addRowSpacing(3,nCols->height());
  grid1->addRowSpacing(4,150 - (lRows->height() + nRows->height() + lCols->height() + nCols->height()));
  grid1->setRowStretch(0,0);
  grid1->setRowStretch(1,0);
  grid1->setRowStretch(2,0);
  grid1->setRowStretch(3,0);
  grid1->setRowStretch(4,1);

  grid1->addColSpacing(0,lRows->width());
  grid1->addColSpacing(0,nRows->width());
  grid1->addColSpacing(0,lCols->width());
  grid1->addColSpacing(0,nCols->width());
  grid1->addColSpacing(1,150);
  grid1->setColStretch(0,0);
  grid1->setColStretch(1,1);

  grid1->activate();

  addTab(tab1,i18n("Geometry"));

  connect(this,SIGNAL(applyButtonPressed()),this,SLOT(insertTable()));
  connect(nRows,SIGNAL(valueChanged(int)),this,SLOT(rowsChanged(int)));
  connect(nCols,SIGNAL(valueChanged(int)),this,SLOT(colsChanged(int)));
}

/*================================================================*/
void KWTableDia::insertTable()
{
  page->setTableConfig(nRows->value(),nCols->value());
  page->mmTable();
}

/*================================================================*/
void KWTableDia::rowsChanged(int _rows)
{
  preview->setRows(_rows);
}

/*================================================================*/
void KWTableDia::colsChanged(int _cols)
{
  preview->setCols(_cols);
}
