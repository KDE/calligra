/* This file is part of the KDE project

   Copyright 2007 Inge Wallin   <inge@lysator.liu.se>

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

// Qt
#include <QDialog>

// Local
#include "ChartDataConfigWidget.h"

// KDE
#include <KDebug>

// KOffice
#include "KoCanvasResourceProvider.h"

// KChart
#include "ui_ChartDataConfigWidget.h"
#include "ui_ChartTableEditor.h"
#include "ChartShape.h"


using namespace KChart;

class ChartDataConfigWidget::Private
{
public:
    ~Private();

    ChartShape                *chart;
    Ui::ChartDataConfigWidget  ui;
    Ui::ChartTableEditor       tableEditor;
    QDialog                   *tableEditorDialog;
};

ChartDataConfigWidget::Private::~Private()
{
    delete tableEditorDialog;
}

ChartDataConfigWidget::ChartDataConfigWidget()
    : KoShapeConfigWidgetBase()
    , d( new Private )
{
    d->chart = 0;
    d->ui.setupUi( this );
    d->tableEditorDialog = new QDialog( this );
    d->tableEditor.setupUi( d->tableEditorDialog );
    d->tableEditorDialog->hide();

    d->ui.m_firstRowAsLabel->setChecked( true );
    d->ui.m_firstColumnAsLabel->setChecked( true );
    d->ui.useExternalDataSource->setChecked( false );
    d->ui.areaLabel->hide();
    d->ui.area->hide();

    createActions();

    // We need only connect one of the data direction buttons, since
    // they are mutually exclusive.
    connect( d->ui.m_dataInRows, SIGNAL( toggled( bool ) ),
             this,               SLOT( setDataInRows( bool ) ) );
    connect( d->ui.m_firstRowAsLabel, SIGNAL( toggled( bool ) ),
             this,                    SIGNAL( firstRowIsLabelChanged( bool ) ) );
    connect( d->ui.m_firstColumnAsLabel, SIGNAL( toggled( bool ) ),
             this,                       SIGNAL( firstColumnIsLabelChanged( bool ) ) );
    connect( d->ui.useExternalDataSource, SIGNAL( toggled( bool ) ),
             this,                        SLOT( setUseExternalDatasource( bool ) ) );
    connect( d->ui.editData, SIGNAL( clicked( bool ) ),
             this, SLOT( slotShowTableEditor( bool ) ) );
}

ChartDataConfigWidget::~ChartDataConfigWidget()
{
    delete d;
}

void ChartDataConfigWidget::createActions()
{
    QAction *cutRowsAction       = new QAction( KIcon( "edit-cut" ), i18n( "Cut Rows" ),    d->tableEditor.table );
    QAction *cutColumnsAction    = new QAction( KIcon( "edit-cut" ), i18n( "Cut Columns" ), d->tableEditor.table );
    QAction *cutCellsAction    = new QAction( KIcon( "edit-cut" ), i18n( "Cut Cells" ), d->tableEditor.table );
    QAction *copyRowsAction      = new QAction( KIcon( "edit-copy" ), i18n( "Copy Rows" ), d->tableEditor.table );
    QAction *copyColumnsAction   = new QAction( KIcon( "edit-copy" ), i18n( "Copy Columns" ), d->tableEditor.table );
    QAction *copyCellsAction   = new QAction( KIcon( "edit-copy" ), i18n( "Copy Cells" ), d->tableEditor.table );
    QAction *deleteRowsAction    = new QAction( KIcon( "edit-delete" ), i18n( "Delete Rows" ), d->tableEditor.table );
    QAction *deleteColumnsAction = new QAction( KIcon( "edit-delete" ), i18n( "Delete Columns" ), d->tableEditor.table );
    QAction *deleteCellsAction = new QAction( KIcon( "edit-delete" ), i18n( "Delete Cells" ), d->tableEditor.table );
    QAction *insertRowsAction    = new QAction( KIcon( "edit-paste" ), i18n( "Insert Rows" ), d->tableEditor.table );
    QAction *insertColumnsAction = new QAction( KIcon( "edit-paste" ), i18n( "Insert Columns" ), d->tableEditor.table );
    QAction *insertCellsAction = new QAction( KIcon( "edit-paste" ), i18n( "Insert Cells" ), d->tableEditor.table );

    QAction *separator1 = new QAction( d->tableEditor.table );
    QAction *separator2 = new QAction( d->tableEditor.table );
    separator1->setSeparator( true );
    separator2->setSeparator( true );

    d->tableEditor.table->addAction( copyRowsAction );
    d->tableEditor.table->addAction( cutRowsAction );
    d->tableEditor.table->addAction( deleteRowsAction );
    d->tableEditor.table->addAction( insertRowsAction );
    d->tableEditor.table->addAction( separator1 );
    d->tableEditor.table->addAction( copyColumnsAction );
    d->tableEditor.table->addAction( cutColumnsAction );
    d->tableEditor.table->addAction( deleteColumnsAction );
    d->tableEditor.table->addAction( insertColumnsAction );
    d->tableEditor.table->addAction( separator2 );
    d->tableEditor.table->addAction( copyCellsAction );
    d->tableEditor.table->addAction( cutCellsAction );
    d->tableEditor.table->addAction( deleteCellsAction );
    d->tableEditor.table->addAction( insertCellsAction );

    d->tableEditor.table->setContextMenuPolicy( Qt::ActionsContextMenu );
}

void ChartDataConfigWidget::open( KoShape* chart )
{
    d->chart = dynamic_cast<ChartShape*>( chart );
    d->tableEditor.table->setModel( d->chart->model() );
}

void ChartDataConfigWidget::save()
{
    // FIXME: Add commands here.
}

KAction* ChartDataConfigWidget::createAction()
{
    return 0;
}

void ChartDataConfigWidget::slotShowTableEditor( bool show )
{
    if ( show )
        d->tableEditorDialog->hide();
    else
        d->tableEditorDialog->show();
}

void ChartDataConfigWidget::setDataInRows( bool checked )
{
    if ( checked )
        emit dataDirectionChanged( Qt::Horizontal );
    else
        emit dataDirectionChanged( Qt::Vertical );
}

void ChartDataConfigWidget::setUseExternalDatasource( bool checked )
{
    emit useExternalDatasourceChanged( checked );
    if ( checked ) {
        d->ui.editData->hide();
        d->ui.areaLabel->show();
        d->ui.area->show();
    } else {
        d->ui.editData->show();
        d->ui.areaLabel->hide();
        d->ui.area->hide();
    }
    d->tableEditorDialog->hide();
}

#include "ChartDataConfigWidget.moc"
