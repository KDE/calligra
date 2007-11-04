/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>

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

// KDE
#include <KDebug>

// KChart
#include "ChartProxyModel.h"
#include "ChartTableView.h"

using namespace KChart;

class ChartTableView::Private
{
public:
    Private();
    ChartProxyModel *sourceModel;
};

ChartTableView::Private::Private()
{
    sourceModel = 0;
}

ChartTableView::ChartTableView( QWidget *parent /* = 0 */ )
   : QTableView( parent ),
     d( new Private )
{
}

ChartTableView::~ChartTableView()
{
    delete d;
}

void ChartTableView::setModel( ChartProxyModel *model )
{
    d->sourceModel = model;
    QTableView::setModel( ( QAbstractItemModel* )model->sourceModel() );
}

ChartProxyModel *ChartTableView::model()
{
    return d->sourceModel;
}

void ChartTableView::commitData( QWidget *editor )
{
    QTableView::commitData( editor );
    d->sourceModel->dataChanged();
}

