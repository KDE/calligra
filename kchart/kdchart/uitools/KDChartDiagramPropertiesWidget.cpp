
/****************************************************************************
 ** Copyright (C) 2001-2006 Klarälvdalens Datakonsult AB.  All rights reserved.
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
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#include <QWidget>
#include <QDebug>
#include <QLabel>

#include <KDChartPosition.h>
#include <KDChartLineDiagram.h>
#include <KDChartBarDiagram.h>
#include <KDChartPieDiagram.h>

#include <KDChartDiagramPropertiesWidget.h>
#include <KDChartDiagramPropertiesWidget_p.h>


#define d d_func()

using namespace KDChart;

DiagramPropertiesWidget::Private::Private()
    :linediagram(0), bardiagram( 0 ), piediagram( 0 ), instantApply(true)
{

}

DiagramPropertiesWidget::Private::~Private()
{
}

DiagramPropertiesWidget::DiagramPropertiesWidget( QWidget *parent )
    :QWidget( parent ), _d( new Private )
{
    setupUi( this );

    connect( mTypeCombo, SIGNAL( activated( int ) ),
             this, SLOT( slotTypeChanged( int ) ) );

    setEnabled( true );

}

DiagramPropertiesWidget::~DiagramPropertiesWidget()
{

}

void DiagramPropertiesWidget::setLineDiagram( LineDiagram * ld )
{
     d->linediagram = ld;
    if ( ld ) {
        readFromLineDiagram( ld );
        setEnabled( true );
    } else {
        setEnabled( true);
    }
}

void DiagramPropertiesWidget::setBarDiagram( BarDiagram * bd )
{
     d->bardiagram = bd;
    if ( bd ) {
        readFromBarDiagram( bd );
        setEnabled( true );
    }
    else {
      setEnabled( true);
    }
}

void DiagramPropertiesWidget::setPieDiagram( PieDiagram * pd )
{
     d->piediagram = pd;
    if ( pd ) {
        readFromPieDiagram( pd );
        setEnabled( true );
    } else {
        setEnabled( true);
    }
}

void DiagramPropertiesWidget::setInstantApply( bool value )
{
    d->instantApply = value;
}


void DiagramPropertiesWidget::readFromLineDiagram( const LineDiagram * ld  )
{
    if ( !ld ) return;
    mTypeCombo->setCurrentIndex( 1 ) ;
    //if (  ld->type() == LineDiagram::Header )
    //  headerRB->setChecked( true );
    //else
    //  footerRB->setChecked( true );
}

void DiagramPropertiesWidget::readFromBarDiagram( const BarDiagram * bd  )
{
    if ( !bd ) return;
    mTypeCombo->setCurrentIndex( 0 ) ;
    //if (  ld->type() == LineDiagram::Header )
    //  headerRB->setChecked( true );
    //else
    //  footerRB->setChecked( true );
}


void DiagramPropertiesWidget::readFromPieDiagram( const PieDiagram * pd  )
{
    if ( !pd ) return;
    mTypeCombo->setCurrentIndex( 2 ) ;
    //if (  ld->type() == LineDiagram::Header )
    //  headerRB->setChecked( true );
    //else
    //  footerRB->setChecked( true );
}

void DiagramPropertiesWidget::writeToLineDiagram( LineDiagram * ld )
{
    if ( !ld ) return;
    ld->setType( LineDiagram::Normal );
}
void DiagramPropertiesWidget::writeToBarDiagram( BarDiagram * bd )
{
    if ( !bd ) return;
    bd->setType( BarDiagram::Normal );
}

void DiagramPropertiesWidget::writeToPieDiagram( PieDiagram * pd )
{
    if ( !pd ) return;
    //ld->setType( LineDiagram::Normal );
}


void DiagramPropertiesWidget::slotTypeChanged( int type )
{
    qDebug() << "type" << type;
/*
    if ( d->headerfooter && d->instantApply ) {
        if (  headerRB->isChecked() )
            d->headerfooter->setType( Diagram::Header );
        else
            d->headerfooter->setType( Diagram::Footer );
    } else {
        emit changed();
    }
*/
    emit changed();
}

