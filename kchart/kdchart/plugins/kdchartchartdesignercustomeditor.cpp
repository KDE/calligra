/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#include <QStandardItemModel>
#include <KDChartChart>
#include <KDChartCartesianAxis>
#include <KDChartAbstractDiagram>
#include <KDChartLineDiagram>
#include <KDChartBarDiagram>
#include <KDChartPieDiagram>
#include <KDChartLegend>
#include <KDChartHeaderFooter>

#include "kdchartchartdesignercustomeditor.h"

#include <KDChartLegendPropertiesWidget.h>
#include <KDChartAxisPropertiesWidget.h>
#include <KDChartHeaderFooterPropertiesWidget.h>
#include <KDChartDiagramPropertiesWidget.h>

using namespace KDChart;

KDChartChartDesignerCustomEditor::KDChartChartDesignerCustomEditor( KDChart::Chart * chart )
    :mChart( chart )
{
    setupUi( this );
    mTypeCombo->setCurrentIndex( typeFromDiagram( ) );
    mGlobalLeadingTopSB->setValue( mChart->globalLeadingTop() );
    mGlobalLeadingLeftSB->setValue( mChart->globalLeadingLeft() );
    mGlobalLeadingRightSB->setValue( mChart->globalLeadingRight() );
    mGlobalLeadingBottomSB->setValue( mChart->globalLeadingBottom() );
    connect( mCloseButton, SIGNAL( clicked() ),
             this, SLOT( accept() ));
    connect( mTypeCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotTypeChanged( int ) ) );
    connect( mSubTypeCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotTypeChanged( int ) ) );
    connect( mGlobalLeadingTopSB, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotLeadingTopChanged( int ) ) );
    connect( mGlobalLeadingLeftSB, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotLeadingLeftChanged( int ) ) );
    connect( mGlobalLeadingRightSB, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotLeadingRightChanged( int ) ) );
    connect( mGlobalLeadingBottomSB, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotLeadingBottomChanged( int ) ) );

    mModel = new QStandardItemModel;
    mModel->insertRows( 0, 2, QModelIndex() );
    mModel->insertColumns(  0,  3,  QModelIndex() );
    for (int row = 0; row < 3; ++row) {
        for (int column = 0; column < 3; ++column) {
            QModelIndex index = mModel->index(row, column, QModelIndex());
            mModel->setData(index, QVariant(row+1 * column) );
        }
    }

    setupCombos();
    setupLegendsTab();
    setupAxesTab();
    setupHeaderFooterTab();
}

static QStringList barSubtypeItems()
{
    QStringList items;
    items << "Normal" << "Stacked" << "Percent";// << "Rows";
    return items;
}

/*not used for now */
/*
static QStringList lineSubtypeItems()
{
    QStringList items;
    items << "Normal" << "Stacked" << "Percent";
    return items;
}

static QStringList pieSubtypeItems()
{
    QStringList items;
    items << "Normal";
    return items;
}
*/

KDChartChartDesignerCustomEditor::ChartType KDChartChartDesignerCustomEditor::typeFromDiagram()
{
    BarDiagram *bar = qobject_cast<BarDiagram*>( mChart->coordinatePlane()->diagram() );
    if (bar) return Bar;
    LineDiagram *line = qobject_cast<LineDiagram*>( mChart->coordinatePlane()->diagram() );
    if (line) return Line;
    PieDiagram *pie = qobject_cast<PieDiagram*>( mChart->coordinatePlane()->diagram() );
    if ( pie ) return Pie;

    return Line;

}

KDChartChartDesignerCustomEditor::SubType KDChartChartDesignerCustomEditor::subTypeFromDiagram()
{
    BarDiagram *bar = qobject_cast<BarDiagram*>( mChart->coordinatePlane()->diagram() );
    if (bar) {
        if ( bar->type() == BarDiagram::Normal )
            return Normal;
        else if ( bar->type() == BarDiagram::Stacked )
            return Stacked;
        else
            return Percent;
    }

    LineDiagram *line = qobject_cast<LineDiagram*>( mChart->coordinatePlane()->diagram() );
    if (line) {
        if ( line->type() == LineDiagram::Normal )
            return Normal;
        else if ( line->type() == LineDiagram::Stacked )
            return Stacked;
        else
            return Percent;
    }

    PieDiagram *pie = qobject_cast<PieDiagram*>( mChart->coordinatePlane()->diagram() );
    if ( pie ) return Normal;

    return Normal;
}

void KDChartChartDesignerCustomEditor::setupCombos( )
{

    mTypeCombo->setCurrentIndex( typeFromDiagram() );
    mSubTypeCombo->addItems( barSubtypeItems() );
    mSubTypeCombo->setCurrentIndex( subTypeFromDiagram() );
}


void KDChartChartDesignerCustomEditor::switchToCartesianPlaneAndMoveStuffToDiagram(
                    AbstractCartesianDiagram* diagram )
{
    if ( qobject_cast<PolarCoordinatePlane*>( mChart->coordinatePlane() ) ) {
        CartesianCoordinatePlane* plane = new CartesianCoordinatePlane( mChart );
        mChart->replaceCoordinatePlane( plane );
        mAxesList->clear();
    } else {
        AbstractCartesianDiagram *d =
                qobject_cast<AbstractCartesianDiagram*>( mChart->coordinatePlane()->diagram() );
        Q_FOREACH( CartesianAxis* axis, d->axes() ) {
            d->takeAxis( axis );
            diagram->addAxis( axis );
        }
        moveLegendsToDiagram( diagram );
    }
}

void KDChartChartDesignerCustomEditor::moveLegendsToDiagram(  AbstractDiagram* diagram )
{
    Q_FOREACH( Legend* legend, mChart->legends() ) {
        legend->setDiagram( diagram );
    }
}

void KDChartChartDesignerCustomEditor::slotTypeChanged( int /*index*/ )
{
    int index = mTypeCombo->currentIndex();
    int subindex = mSubTypeCombo->currentIndex();
    if (  index == 0 ) {

        KDChart::BarDiagram* diagram = new KDChart::BarDiagram;
        diagram->setModel(mModel);
        if (  subindex == 0 )
            diagram->setType( BarDiagram::Normal );
        else if (  subindex == 1 )
            diagram->setType( BarDiagram::Stacked );
        else
            diagram->setType( BarDiagram::Percent );
        switchToCartesianPlaneAndMoveStuffToDiagram( diagram );
        mChart->coordinatePlane()->replaceDiagram(diagram);
    } else if ( index == 1 ) {
        KDChart::LineDiagram* diagram = new KDChart::LineDiagram;
        diagram->setModel(mModel);
        if (  subindex == 0 )
            diagram->setType( LineDiagram::Normal );
        else if (  subindex == 1 )
            diagram->setType( LineDiagram::Stacked );
        else
            diagram->setType( LineDiagram::Percent );
        switchToCartesianPlaneAndMoveStuffToDiagram( diagram );
        mChart->coordinatePlane()->replaceDiagram(diagram);
    } else if (  index == 2 ) {
        if ( qobject_cast<CartesianCoordinatePlane*>( mChart->coordinatePlane() ) ) {
            PolarCoordinatePlane* plane = new PolarCoordinatePlane( mChart );
            mChart->replaceCoordinatePlane( plane );
            mAxesList->clear();
        }
        KDChart::PieDiagram* diagram = new KDChart::PieDiagram;
        diagram->setModel(mModel);
        mSubTypeCombo->setCurrentIndex( 0 );
        moveLegendsToDiagram( diagram );
        mChart->coordinatePlane()->replaceDiagram(diagram);
    }
}


void KDChartChartDesignerCustomEditor::slotLeadingTopChanged( int v )
{
    mChart->setGlobalLeadingTop( v );
}

void KDChartChartDesignerCustomEditor::slotLeadingLeftChanged( int v )
{
    mChart->setGlobalLeadingLeft( v );
}

void KDChartChartDesignerCustomEditor::slotLeadingRightChanged( int v )
{
    mChart->setGlobalLeadingRight( v );
}

void KDChartChartDesignerCustomEditor::slotLeadingBottomChanged( int v )
{
    mChart->setGlobalLeadingBottom( v );
}


void KDChartChartDesignerCustomEditor::setupLegendsTab()
{
    QVBoxLayout* vbox = new QVBoxLayout( mLegendDetailsGroup );
    mLegendEditor = new LegendPropertiesWidget( mLegendDetailsGroup );

    for (  int i = 0; i < mChart->legends().count(); ++i )
        mLegendsList->addItem( QString("Legend %1").arg( i+1 ) );

    vbox->addWidget( mLegendEditor );
    connect( mAddLegendBtn, SIGNAL( clicked() ),
             this, SLOT( slotAddLegend() ) );
    connect( mRemoveLegendBtn, SIGNAL( clicked() ),
             this, SLOT( slotRemoveLegend() ) );
    connect( mLegendsList, SIGNAL( currentRowChanged( int ) ),
             this, SLOT( slotCurrentLegendChanged( int ) ) );

}

void KDChartChartDesignerCustomEditor::setupAxesTab()
{

    QVBoxLayout* vbox = new QVBoxLayout( mAxisDetailsGroup );
    mAxisEditor = new AxisPropertiesWidget( mAxisDetailsGroup );
    ChartType type = typeFromDiagram();

    switch ( type ) {
    case Bar:
    case Line:
    {
        int count = dynamic_cast<const AbstractCartesianDiagram*>
                (mChart->coordinatePlane()->diagram() )->axes().count();
        for (  int i = 0; i < count; ++i )
            mAxesList->addItem( QString("Axis %1").arg( i+1 ) );
        break;
    }
    case Pie:
    default:
        qDebug() << "Axis for this diagram type are not supported for now";
        break;
    }

    vbox->addWidget( mAxisEditor );
    connect( mAddAxisBtn, SIGNAL( clicked() ),
             this, SLOT( slotAddAxis() ) );
    connect( mRemoveAxisBtn, SIGNAL( clicked() ),
             this, SLOT( slotRemoveAxis() ) );
    connect( mAxesList, SIGNAL( currentRowChanged( int ) ),
             this, SLOT( slotCurrentAxisChanged( int ) ) );

}


void KDChartChartDesignerCustomEditor::setupHeaderFooterTab()
{
    QVBoxLayout* vbox = new QVBoxLayout( mHeaderFooterDetailsGroup );
    mHeaderFooterEditor = new HeaderFooterPropertiesWidget( mHeaderFooterDetailsGroup );

    for (  int i = 0; i < mChart->headerFooters().count(); ++i )
        mHeaderFootersList->addItem( QString("HeaderFooter %1").arg( i+1 ) );

    vbox->addWidget( mHeaderFooterEditor );
    connect( mAddHeaderFooterBtn, SIGNAL( clicked() ),
             this, SLOT( slotAddHeaderFooter() ) );
    connect( mRemoveHeaderFooterBtn, SIGNAL( clicked() ),
             this, SLOT( slotRemoveHeaderFooter() ) );
    connect( mHeaderFootersList, SIGNAL( currentRowChanged( int ) ),
             this, SLOT( slotCurrentHeaderFooterChanged( int ) ) );
}



void KDChartChartDesignerCustomEditor::slotAddLegend()
{
    Legend *l = new Legend( mChart->coordinatePlane()->diagram(), mChart );
    l->setPosition(  Position::East );
    mChart->addLegend( l );
    mLegendsList->addItem( QString("Legend %1").arg(mChart->legends().count() ) );

}

void KDChartChartDesignerCustomEditor::slotRemoveLegend()
{
    int idx = mLegendsList->currentRow();
    if ( idx == -1 || idx >= mChart->legends().count() ) return;
    Legend* l = mChart->legends()[idx];
    mChart->takeLegend( l );
    delete l;
    delete mLegendsList->takeItem( idx );
}

void KDChartChartDesignerCustomEditor::slotCurrentLegendChanged( int idx )
{
    Legend* l = 0;
    if ( idx != -1 && idx < mChart->legends().count() )
        l = mChart->legends()[idx];
    mLegendEditor->setLegend( l );
}

void KDChartChartDesignerCustomEditor::slotAddAxis()
{
    switch ( typeFromDiagram() ) {
    case Bar:
    case Line:
    {
        AbstractCartesianDiagram *d =
                qobject_cast<AbstractCartesianDiagram*>( mChart->coordinatePlane()->diagram() );
        int count = d->axes().count();
        CartesianAxis * axis = new CartesianAxis( );
        axis->setPosition( CartesianAxis::Left );
        d->addAxis( axis );
        mAxesList->addItem( QString("Axis %1").arg( count ) );
        break;
    }
    case Pie:
    default:
        qDebug() << "Axis for this diagram type are not supported at the moment";
        break;
    }
}

void KDChartChartDesignerCustomEditor::slotRemoveAxis()
{
    int idx = mAxesList->currentRow();
    if ( idx == -1  ) return;

    switch ( typeFromDiagram() ) {
    case Bar:
    case Line:
    {
        AbstractCartesianDiagram *d =
                qobject_cast<AbstractCartesianDiagram*>( mChart->coordinatePlane()->diagram() );
        int count = d->axes().count();
        if ( idx >= count ) break;
        CartesianAxis* l = d->axes()[idx];
        d->takeAxis( l );
        delete l;
        delete mAxesList->takeItem(  idx );
        break;
    }
    case Pie:
    default:
        break;
    }
}

void KDChartChartDesignerCustomEditor::slotCurrentAxisChanged( int idx )
{
    if ( idx == -1 ) return;
    switch ( typeFromDiagram() ) {
    case Bar:
    case Line:
    {
        AbstractCartesianDiagram *d =
                qobject_cast<AbstractCartesianDiagram*>( mChart->coordinatePlane()->diagram() );
        int count = d->axes().count();
        if ( idx >= count ) break;
        CartesianAxis* l = d->axes()[idx];
        mAxisEditor->setAxis( l );
        break;
    }
    case Pie:
        default:
            qDebug() << "Axis for this diagram type are not supported at the moment";
            break;
    }
}


void KDChartChartDesignerCustomEditor::slotAddHeaderFooter()
{
    HeaderFooter * hf = new HeaderFooter();
    hf->setType( HeaderFooter::Header );
    hf->setText(  "Header" );
    mChart->addHeaderFooter( hf );
    mHeaderFootersList->addItem( QString("HeaderFooter %1").arg(mChart->headerFooters().count() ) );

}

void KDChartChartDesignerCustomEditor::slotRemoveHeaderFooter()
{
    int idx = mHeaderFootersList->currentRow();
    if ( idx == -1 || idx >= mChart->headerFooters().count() ) return;
    HeaderFooter* l = mChart->headerFooters()[idx];
    mChart->takeHeaderFooter( l );
    delete l;
    delete mHeaderFootersList->takeItem( idx );
}

void KDChartChartDesignerCustomEditor::slotCurrentHeaderFooterChanged( int idx )
{
    if ( idx == -1 || idx >= mChart->headerFooters().count() ) return;
    HeaderFooter* l = mChart->headerFooters()[idx];
    mHeaderFooterEditor->setHeaderFooter( l );
}

