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

#include <KDChartWidget>
#include <KDChartCartesianAxis>
#include <KDChartAbstractCartesianDiagram>
#include <KDChartLineDiagram>
#include <KDChartBarDiagram>
#include <KDChartLegend>

#include "kdchartwidgetdesignercustomeditor.h"

#include <KDChartLegendPropertiesWidget.h>
#include <KDChartAxisPropertiesWidget.h>
#include <KDChartHeaderFooterPropertiesWidget.h>

using namespace KDChart;

KDChartWidgetDesignerCustomEditor::KDChartWidgetDesignerCustomEditor( KDChart::Widget * chart )
    :mChart( chart )
{
    setupUi( this );
    mTypeCombo->setCurrentIndex( mChart->type() - 1 );
    slotTypeChanged( mChart->type() - 1 );
    mGlobalLeadingTopSB->setValue( mChart->globalLeadingTop() );
    mGlobalLeadingLeftSB->setValue( mChart->globalLeadingLeft() );
    mGlobalLeadingRightSB->setValue( mChart->globalLeadingRight() );
    mGlobalLeadingBottomSB->setValue( mChart->globalLeadingBottom() );
    connect( mCloseButton, SIGNAL( clicked() ),
             this, SLOT( accept() ));
    connect( mTypeCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotTypeChanged( int ) ) );
    connect( mSubTypeCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotSubTypeChanged( int ) ) );
    connect( mGlobalLeadingTopSB, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotLeadingTopChanged( int ) ) );
    connect( mGlobalLeadingLeftSB, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotLeadingLeftChanged( int ) ) );
    connect( mGlobalLeadingRightSB, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotLeadingRightChanged( int ) ) );
    connect( mGlobalLeadingBottomSB, SIGNAL( valueChanged( int ) ),
             this, SLOT( slotLeadingBottomChanged( int ) ) );

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

static QStringList lineSubtypeItems()
{
    QStringList items;
    items << "Normal" << "Stacked" << "Percent";
    return items;
}

static bool isCartesian( KDChart::Widget::ChartType type )
{
    return (type == KDChart::Widget::Bar || type == KDChart::Widget::Line);
}

static bool isPolar( KDChart::Widget::ChartType type )
{
    return (type == KDChart::Widget::Pie
            || type == KDChart::Widget::Ring
            || type == KDChart::Widget::Polar );
}

void KDChartWidgetDesignerCustomEditor::slotTypeChanged( int index )
{
    KDChart::Widget::ChartType type = static_cast<KDChart::Widget::ChartType>(index+1);
    if ( isCartesian(mChart->type()) && isPolar( type ) )
        mAxesList->clear();
    mChart->setType( type );
    mSubTypeCombo->clear();
    switch ( type ) {
        case KDChart::Widget::Bar:
            mSubTypeCombo->addItems( barSubtypeItems() );
            break;
        case KDChart::Widget::Line:
            mSubTypeCombo->addItems( lineSubtypeItems() );
            break;
        case KDChart::Widget::Pie:
            break;
        case KDChart::Widget::Ring:
            break;
        case KDChart::Widget::Polar:
            break;
      case KDChart::Widget::NoType:
        default:
            break;
    }
    mSubTypeCombo->setCurrentIndex( mChart->subType() );
}

void KDChartWidgetDesignerCustomEditor::slotSubTypeChanged( int index )
{
    KDChart::Widget::SubType type = static_cast<KDChart::Widget::SubType>(index);
    mChart->setSubType( type );
}

void KDChartWidgetDesignerCustomEditor::slotLeadingTopChanged( int v )
{
    mChart->setGlobalLeadingTop( v );
}

void KDChartWidgetDesignerCustomEditor::slotLeadingLeftChanged( int v )
{
    mChart->setGlobalLeadingLeft( v );
}

void KDChartWidgetDesignerCustomEditor::slotLeadingRightChanged( int v )
{
    mChart->setGlobalLeadingRight( v );
}

void KDChartWidgetDesignerCustomEditor::slotLeadingBottomChanged( int v )
{
    mChart->setGlobalLeadingBottom( v );
}

void KDChartWidgetDesignerCustomEditor::setupLegendsTab()
{
    QVBoxLayout* vbox = new QVBoxLayout( mLegendDetailsGroup );
    mLegendEditor = new LegendPropertiesWidget( mLegendDetailsGroup );

    for (  int i = 0; i < mChart->allLegends().count(); ++i )
        mLegendsList->addItem( QString("Legend %1").arg( i+1 ) );

    vbox->addWidget( mLegendEditor );
    connect( mAddLegendBtn, SIGNAL( clicked() ),
             this, SLOT( slotAddLegend() ) );
    connect( mRemoveLegendBtn, SIGNAL( clicked() ),
             this, SLOT( slotRemoveLegend() ) );
    connect( mLegendsList, SIGNAL( currentRowChanged( int ) ),
             this, SLOT( slotCurrentLegendChanged( int ) ) );

}

void KDChartWidgetDesignerCustomEditor::setupAxesTab()
{
    QVBoxLayout* vbox = new QVBoxLayout( mAxisDetailsGroup );
    mAxisEditor = new AxisPropertiesWidget( mAxisDetailsGroup );

    KDChart::Widget::ChartType type = mChart->type();
    switch ( type ) {
    case KDChart::Widget::Bar:
        for (  int i = 0; i < mChart->barDiagram()->axes().count(); ++i )
            mAxesList->addItem( QString("Axis %1").arg( i+1 ) );
        break;
    case KDChart::Widget::Line:
        for (  int i = 0; i < mChart->lineDiagram()->axes().count(); ++i )
            mAxesList->addItem( QString("Axis %1").arg( i+1 ) );
        break;
    case KDChart::Widget::Pie:
    case KDChart::Widget::Ring:
    case KDChart::Widget::Polar:
    case KDChart::Widget::NoType:
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

void KDChartWidgetDesignerCustomEditor::setupHeaderFooterTab()
{
    QVBoxLayout* vbox = new QVBoxLayout( mHeaderFooterDetailsGroup );
    mHeaderFooterEditor = new HeaderFooterPropertiesWidget( mHeaderFooterDetailsGroup );

    for (  int i = 0; i < mChart->allHeadersFooters().count(); ++i )
        mHeaderFootersList->addItem( QString("HeaderFooter %1").arg( i+1 ) );

    vbox->addWidget( mHeaderFooterEditor );
    connect( mAddHeaderFooterBtn, SIGNAL( clicked() ),
             this, SLOT( slotAddHeaderFooter() ) );
    connect( mRemoveHeaderFooterBtn, SIGNAL( clicked() ),
             this, SLOT( slotRemoveHeaderFooter() ) );
    connect( mHeaderFootersList, SIGNAL( currentRowChanged( int ) ),
             this, SLOT( slotCurrentHeaderFooterChanged( int ) ) );
}

void KDChartWidgetDesignerCustomEditor::slotAddLegend()
{

    mChart->addLegend( Position::East );
    mLegendsList->addItem( QString("Legend %1").arg(mChart->allLegends().count() ) );

}

void KDChartWidgetDesignerCustomEditor::slotRemoveLegend()
{
    int idx = mLegendsList->currentRow();
    if ( idx == -1 || idx >= mChart->allLegends().count() ) return;
    Legend* l = mChart->allLegends()[idx];
    mChart->takeLegend( l );
    delete l;
    delete mLegendsList->takeItem( idx );
}

void KDChartWidgetDesignerCustomEditor::slotCurrentLegendChanged( int idx )
{
    Legend* l = 0;
    if ( idx != -1 && idx < mChart->allLegends().count() )
        l = mChart->allLegends()[idx];
    mLegendEditor->setLegend( l );
}

void KDChartWidgetDesignerCustomEditor::slotAddAxis()
{
    CartesianAxis * axis = new CartesianAxis( );
    axis->setPosition( CartesianAxis::Left );
    KDChart::Widget::ChartType type = mChart->type();
    switch ( type ) {
    case KDChart::Widget::Bar:
        mChart->barDiagram()->addAxis( axis );
        mAxesList->addItem( QString("Axis %1").arg(mChart->barDiagram()->axes().count() ) );
        break;
    case KDChart::Widget::Line:
        mChart->lineDiagram()->addAxis( axis );
        mAxesList->addItem( QString("Axis %1").arg(mChart->lineDiagram()->axes().count() ) );
        break;
    case KDChart::Widget::Pie:
    case KDChart::Widget::Ring:
    case KDChart::Widget::Polar:
    case KDChart::Widget::NoType:
    default:
        qDebug() << "Axis for this diagram type are not supported at the moment";
        break;
    }

}

void KDChartWidgetDesignerCustomEditor::slotRemoveAxis()
{
    int idx = mAxesList->currentRow();
    if ( idx == -1  ) return;
    KDChart::Widget::ChartType type = mChart->type();
    switch ( type ) {
    case KDChart::Widget::Bar:
    {
        if ( idx >= mChart->barDiagram()->axes().count() ) break;
        CartesianAxis* l = mChart->barDiagram()->axes()[idx];
        mChart->barDiagram()->takeAxis( l );
        delete l;
        delete mAxesList->takeItem(  idx );
        break;
    }
    case KDChart::Widget::Line:
    {
        if ( idx >= mChart->lineDiagram()->axes().count() ) break;
        CartesianAxis* l = mChart->lineDiagram()->axes()[idx];
        mChart->lineDiagram()->takeAxis( l );
        delete l;
        delete mAxesList->takeItem(  idx );
        break;
    }
    case KDChart::Widget::Pie:
    case KDChart::Widget::Ring:
    case KDChart::Widget::Polar:
    case KDChart::Widget::NoType:
    default:
        break;
    }

}

void KDChartWidgetDesignerCustomEditor::slotCurrentAxisChanged( int idx )
{

    if ( idx == -1 ) return;
    KDChart::Widget::ChartType type = mChart->type();
    switch ( type ) {
    case KDChart::Widget::Bar:
    {
        if ( idx >= mChart->barDiagram()->axes().count() ) break;
        CartesianAxis* l = mChart->barDiagram()->axes()[idx];
        mAxisEditor->setAxis( l );
        break;
    }
    case KDChart::Widget::Line:
    {
        if ( idx >= mChart->lineDiagram()->axes().count() ) break;
        CartesianAxis* l = mChart->lineDiagram()->axes()[idx];
        mAxisEditor->setAxis( l );
        break;
    }
    case KDChart::Widget::Pie:
    case KDChart::Widget::Ring:
    case KDChart::Widget::Polar:
    case KDChart::Widget::NoType:
        default:
            qDebug() << "Axis for this diagram type are not supported at the moment";
            break;
    }

}

void KDChartWidgetDesignerCustomEditor::slotAddHeaderFooter()
{
    HeaderFooter * hf = new HeaderFooter();
    hf->setType( HeaderFooter::Header );
    hf->setText(  "Header" );
    mChart->addHeaderFooter( hf );
    mHeaderFootersList->addItem( QString("HeaderFooter %1").arg(mChart->allHeadersFooters().count() ) );

}

void KDChartWidgetDesignerCustomEditor::slotRemoveHeaderFooter()
{
    int idx = mHeaderFootersList->currentRow();
    if ( idx == -1 || idx >= mChart->allHeadersFooters().count() ) return;
    HeaderFooter* l = mChart->allHeadersFooters()[idx];
    mChart->takeHeaderFooter( l );
    delete l;
    delete mHeaderFootersList->takeItem( idx );
}

void KDChartWidgetDesignerCustomEditor::slotCurrentHeaderFooterChanged( int idx )
{
    if ( idx == -1 || idx >= mChart->allHeadersFooters().count() ) return;
    HeaderFooter* l = mChart->allHeadersFooters()[idx];
    mHeaderFooterEditor->setHeaderFooter( l );
}

