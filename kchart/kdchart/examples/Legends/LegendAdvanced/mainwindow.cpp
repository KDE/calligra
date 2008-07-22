/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/

#include "mainwindow.h"

#include <KDChartChart>
#include <KDChartLegend>
#include <KDChartPosition>
#include <KDChartLineDiagram>
#include <KDChartTextAttributes>
#include <QComboBox>
#include <QLineEdit>

class LegendItem : public QTreeWidgetItem
{
public:
    LegendItem( KDChart::Legend* legend, QTreeWidget* parent ) :
        QTreeWidgetItem( parent ), m_legend( legend ) {}

    KDChart::Legend* legend() const { return m_legend; }

private:
    KDChart::Legend* m_legend;
};


MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent )
{
    setupUi( this );

    QHBoxLayout* chartLayout = new QHBoxLayout( chartFrame );
    m_chart = new KDChart::Chart();
    chartLayout->addWidget( m_chart );

    m_model.loadFromCSV( ":/data" );

    // Set up the diagram
    m_lines = new KDChart::LineDiagram();
    m_lines->setModel( &m_model );
    m_chart->coordinatePlane()->replaceDiagram( m_lines );

    QPen pen = QPen( m_lines->pen(0).color(), 2 );
    pen.setStyle( Qt::DashLine );
    m_lines->setPen( 0, pen );
    pen = QPen( m_lines->pen(2).color(), 2 );
    pen.setStyle( Qt::DashDotDotLine );
    m_lines->setPen( 2, pen );

    // Add at least one legend for starters
    KDChart::Legend* legend = new KDChart::Legend( m_lines, m_chart );
    legend->setPosition( KDChart::Position::North );
    legend->setAlignment( Qt::AlignCenter );
    legend->setShowLines( false );
    legend->setTitleText( tr( "Legend" ) );
    legend->setOrientation( Qt::Vertical );
    m_chart->addLegend( legend );
    legend->show();

    LegendItem* newItem = new LegendItem( legend, legendsTV );
    newItem->setText( 0, tr( "North" ) );
    newItem->setText( 1, tr( "no" ) );
    newItem->setText( 2, tr( "Legend" ) );
    newItem->setText( 3, tr( "Vertical" ) );
    newItem->setText( 4, tr( "Center") );
    newItem->setText( 5, tr( "MarkersOnly" ) );

    alignmentMap[ Qt::AlignTop     | Qt::AlignLeft]    = tr("Top + Left");
    alignmentMap[ Qt::AlignTop     | Qt::AlignHCenter] = tr("Top + HCenter");
    alignmentMap[ Qt::AlignTop     | Qt::AlignRight]   = tr("Top + Right");
    alignmentMap[ Qt::AlignVCenter | Qt::AlignRight]   = tr("VCenter + Right");
    alignmentMap[ Qt::AlignBottom  | Qt::AlignRight]   = tr("Bottom + Right");
    alignmentMap[ Qt::AlignBottom  | Qt::AlignHCenter] = tr("Bottom + HCenter");
    alignmentMap[ Qt::AlignBottom  | Qt::AlignLeft]    = tr("Bottom + Left");
    alignmentMap[ Qt::AlignVCenter | Qt::AlignLeft]    = tr("VCenter + Left");
    alignmentMap[ Qt::AlignCenter]                     = tr("Center");

    m_chart->update();
}


void MainWindow::initAddLegendDialog( DerivedAddLegendDialog& conf,
                                      Qt::Alignment alignment ) const
{
    conf.titleTextED->setFocus();

    const QStringList labels = KDChart::Position::printableNames();
    const QList<QByteArray> names = KDChart::Position::names();


    for ( int i = 0, end = qMin( labels.size(), names.size() ) ; i != end ; ++i )
        conf.positionCO->addItem( labels[i], names[i] );

    QMap<Qt::Alignment, QString>::const_iterator it = alignmentMap.constBegin();
    while (it != alignmentMap.constEnd()) {
        conf.alignmentCO->addItem( it.value() );
        ++it;
    }
    const int idx = conf.alignmentCO->findText( alignmentMap[ alignment ] );
    if( idx > -1 )
        conf.alignmentCO->setCurrentIndex( idx );
}

void MainWindow::on_addLegendPB_clicked()
{
    DerivedAddLegendDialog conf;
    initAddLegendDialog( conf, Qt::AlignCenter );
    if( conf.exec() == QDialog::Accepted ) {
        KDChart::Legend* legend = new KDChart::Legend( m_lines, m_chart );
        m_chart->addLegend( legend );
        legend->setPosition(
            KDChart::Position::fromName( conf.positionCO->itemData( conf.positionCO->currentIndex() ).toByteArray() ) );
        // get the alignment
        Qt::Alignment alignment = Qt::AlignCenter;
        const QString selectedAlignment( conf.alignmentCO->currentText() );
        QMap<Qt::Alignment, QString>::const_iterator i = alignmentMap.constBegin();
        while (i != alignmentMap.constEnd()) {
            if (  i.value() == selectedAlignment ){
                alignment = i.key();
                break;
            }
            ++i;
        }
        legend->setAlignment( alignment );
        legend->setShowLines( conf.showLinesCB->isChecked() );
        legend->setTitleText( conf.titleTextED->text() );
        legend->setOrientation( ( conf.orientationCO->currentIndex() == 0 ) ? Qt::Vertical : Qt::Horizontal );

        switch( conf.styleCO->currentIndex() )
        {
        case 0:
            legend->setLegendStyle( KDChart::Legend::MarkersOnly );
            break;
        case 1:
            legend->setLegendStyle( KDChart::Legend::LinesOnly );
            break;
        case 2:
            legend->setLegendStyle(KDChart::Legend::MarkersAndLines );
            break;
        default:
            legend->setLegendStyle( KDChart::Legend::MarkersOnly );
            break;
        }

        LegendItem* newItem = new LegendItem( legend, legendsTV );
        newItem->setText( 0, conf.positionCO->currentText() );
        newItem->setText( 1, conf.showLinesCB->isChecked() ? tr("yes") : tr("no") );
        newItem->setText( 2, conf.titleTextED->text() );
        newItem->setText( 3, conf.orientationCO->currentText() );
        newItem->setText( 4, selectedAlignment );
        newItem->setText( 5, conf.styleCO->currentText() );
        m_chart->update();
    }
}


void MainWindow::on_editLegendPB_clicked()
{
    if ( legendsTV->selectedItems().size() == 0 ) return;
    LegendItem* item = static_cast<LegendItem*>( legendsTV->selectedItems().first() );
    KDChart::Legend* legend = item->legend();
    DerivedAddLegendDialog conf;
    initAddLegendDialog( conf, legend->alignment() );
    conf.showLinesCB->setChecked( legend->showLines() );
    conf.titleTextED->setText( legend->titleText() );

    // In this example we are using legend position names, that match
    // exactly the names in KDChart::Legend::LegendPosition,
    // so we can use a shortcut here, to set the respective name in
    // the dialog's list, and we need no error checking for findText():
    conf.positionCO->setCurrentIndex(
            conf.positionCO->findText( legend->position().printableName() ) );
    conf.orientationCO->setCurrentIndex( (legend->orientation()==Qt::Vertical)?0:1 );
    conf.styleCO->setCurrentIndex( legend->legendStyle() );

    if( conf.exec() == QDialog::Accepted ) {
        //legend->setPosition( (KDChart::Legend::LegendPosition)conf.positionCO->currentIndex() );
        legend->setPosition(
            KDChart::Position::fromName( conf.positionCO->itemData( conf.positionCO->currentIndex() ).toByteArray() ) );
        // get the alignment
        Qt::Alignment alignment = Qt::AlignCenter;
        const QString selectedAlignment( conf.alignmentCO->currentText() );
        QMap<Qt::Alignment, QString>::const_iterator i = alignmentMap.constBegin();
        while (i != alignmentMap.constEnd()) {
            if (  i.value() == selectedAlignment ){
                alignment = i.key();
                break;
            }
            ++i;
        }
        legend->setAlignment( alignment );
        legend->setShowLines( conf.showLinesCB->isChecked() );
        legend->setTitleText( conf.titleTextED->text() );
        legend->setOrientation( ( conf.orientationCO->currentIndex() == 0 ) ? Qt::Vertical : Qt::Horizontal );

        switch( conf.styleCO->currentIndex() )
        {
        case 0:
            legend->setLegendStyle( KDChart::Legend::MarkersOnly );
            break;
        case 1:
            legend->setLegendStyle( KDChart::Legend::LinesOnly );
            break;
        case 2:
            legend->setLegendStyle(KDChart::Legend::MarkersAndLines );
            break;
        default:
            legend->setLegendStyle( KDChart::Legend::MarkersOnly );
            break;
        }

        item->setText( 0, conf.positionCO->currentText() );
        item->setText( 1, conf.showLinesCB->isChecked() ? tr("yes") : tr("no") );
        item->setText( 2, conf.titleTextED->text() );
        item->setText( 3, conf.orientationCO->currentText() );
        item->setText( 4, selectedAlignment );
        item->setText( 5, conf.styleCO->currentText() );
        m_chart->update();
    }
}



void MainWindow::on_removeLegendPB_clicked()
{
    if ( legendsTV->selectedItems().size() == 0 ) return;
    QList<QTreeWidgetItem*> items = legendsTV->selectedItems();
    for( QList<QTreeWidgetItem*>::const_iterator it = items.begin();
         it != items.end(); ++it )
    {
        KDChart::Legend* legend = static_cast<LegendItem*>( (*it) )->legend();
#if 0
        // Note: Despite it being owned by the Chart, you *can* just delete
        //       the legend: KD Chart will notice that and adjust its layout ...
        delete legend;
#else
        // ... but the correct way is to first take it, so the Chart is no longer owning it:
        m_chart->takeLegend( legend );
        // ... and then delete it:
        delete legend;
#endif
        delete (*it);
    }
    m_chart->update();
}

void MainWindow::on_legendsTV_itemSelectionChanged()
{
    removeLegendPB->setEnabled( legendsTV->selectedItems().count() > 0 );
    removeLegendPB->setEnabled( legendsTV->selectedItems().count() == 1 );
}
