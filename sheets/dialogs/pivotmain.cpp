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

    int index = 0;
    for (int i = range.left(); i <= r; ++i) {
        cell = Cell(sheet, i, row);
        text = cell.displayText();
       // d->mainWidget.Labels->insertItem(index++, text);
	if(text.length() >0)
	{
        item = new QListWidgetItem(text);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Unchecked);
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
