/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2013 Jigar Raisinghani <jigarraisinghani@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local

#include "pivot.h"
#include "ui_pivot.h"
#include "ui_pivotmain.h"
#include "pivotmain.h"

#include <QMessageBox>


using namespace Calligra::Sheets;

class Pivot::Private
{
public:
    Selection *selection;
    Ui::Pivot mainWidget;
};


Pivot::Pivot(QWidget* parent,Selection* selection):
    KoDialog(parent),
    d(new Private)
{
    setCaption(i18n("Select Source"));
    
    QWidget* widget = new QWidget(this);
    d->mainWidget.setupUi(widget);
    setButtons(Ok|Cancel);   
    d->mainWidget.Current->setChecked(true);
    setMainWidget(widget);
    d->selection=selection;
    connect(this, &KoDialog::okClicked, this, &Pivot::slotUser2Clicked);

}

Pivot::~Pivot()
{
    delete d;
}

void Pivot::slotUser2Clicked()
{
	  if(d->mainWidget.Current->isChecked())
	  {
	      PivotMain *pMain= new PivotMain(this,d->selection);
	      pMain->setModal(true);
	      pMain->exec();
	  }
	  
	  if(d->mainWidget.External->isChecked())
	  {
	      QMessageBox msgBox;
	      msgBox.setText("Functionality Yet to be Added");
	      msgBox.exec();
	      
	      Pivot *p=new Pivot(this,d->selection);
	      p->setModal(true);
	      p->exec();
	  }
}
