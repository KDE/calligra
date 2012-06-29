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
#include "pivotoptions.h"
#include "pivotfilters.h"

PivotMain::PivotMain(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PivotMain)
{
    ui->setupUi(this);
    ui->TotalRows->setChecked(true);
    ui->TotalColumns->setChecked(true);


    ui->Labels->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->Labels->setDragEnabled(true);
    ui->Labels->setDragDropMode(QAbstractItemView::InternalMove);
    ui->Labels->viewport()->setAcceptDrops(true);
    ui->Labels->setDropIndicatorShown(true);

    ui->Rows->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->Rows->setDragEnabled(true);
    ui->Rows->setDragDropMode(QAbstractItemView::DropOnly);
    ui->Rows->viewport()->setAcceptDrops(true);
    ui->Rows->setDropIndicatorShown(true);

    ui->Columns->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->Columns->setDragEnabled(true);
    ui->Columns->setDragDropMode(QAbstractItemView::DropOnly);
    ui->Columns->viewport()->setAcceptDrops(true);
    ui->Columns->setDropIndicatorShown(true);

    ui->Values->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->Values->setDragEnabled(true);
    ui->Values->setDragDropMode(QAbstractItemView::DropOnly);
    ui->Values->viewport()->setAcceptDrops(true);
    ui->Values->setDropIndicatorShown(true);

    ui->PageFields->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->PageFields->setDragEnabled(true);
    ui->PageFields->setDragDropMode(QAbstractItemView::DropOnly);
    ui->PageFields->viewport()->setAcceptDrops(true);
    ui->PageFields->setDropIndicatorShown(true);



}

PivotMain::~PivotMain()
{
    delete ui;
}

void PivotMain::on_Options_clicked()
{
    PivotOptions pOptions;
    pOptions.setModal(true);
    pOptions.exec();
}

void PivotMain::on_AddFilter_clicked()
{
    if(ui->AddFilter->isChecked())
    {
        PivotFilters pFilters;
        pFilters.setModal(true);
        pFilters.exec();
    }
}
