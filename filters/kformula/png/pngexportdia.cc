/* This file is part of the KDE project
   Copyright (C) 2002 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qcheckbox.h>
#include <qimage.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpaintdevice.h>
#include <qrect.h>
#include <qvbuttongroup.h>
#include <qwidget.h>

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <kformulacontainer.h>
#include <kformuladocument.h>
#include <kformulamimesource.h>

#include "pngexportdia.h"


PNGExportDia::PNGExportDia( QDomDocument dom, QString outFile, QWidget *parent, const char *name )
    : KDialogBase( parent, name, true, i18n("PNG Export Filter Parameters" ), Ok|Cancel ),
      _fileOut( outFile )
{
    kapp->restoreOverrideCursor();

    doc = new KFormula::Document( kapp->config() );
    formula = new KFormula::Container( doc );
    if ( !formula->load( dom ) ) {
        kdError() << "Failed." << endl;
    }

    setupGUI();

    QRect rect = formula->boundingRect();
    widthEdit->setText( QString::number( realWidth = rect.width() ) );
    heightEdit->setText( QString::number( realHeight = rect.height() ) );
    percWidthEdit->setText( "100" );
    percHeightEdit->setText( "100" );

    connectAll();
    connect( proportional, SIGNAL( clicked() ),
             this, SLOT( proportionalClicked() ) );
}

PNGExportDia::~PNGExportDia()
{
    delete formula;
    delete doc;
}

void PNGExportDia::connectAll()
{
    connect( widthEdit, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( widthChanged( const QString& ) ) );
    connect( heightEdit, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( heightChanged( const QString& ) ) );
    connect( percWidthEdit, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( percentWidthChanged( const QString& ) ) );
    connect( percHeightEdit, SIGNAL( textChanged( const QString& ) ),
             this, SLOT( percentHeightChanged( const QString& ) ) );
}

void PNGExportDia::disconnectAll()
{
    disconnect( widthEdit, SIGNAL( textChanged( const QString& ) ),
                this, SLOT( widthChanged( const QString& ) ) );
    disconnect( heightEdit, SIGNAL( textChanged( const QString& ) ),
                this, SLOT( heightChanged( const QString& ) ) );
    disconnect( percWidthEdit, SIGNAL( textChanged( const QString& ) ),
                this, SLOT( percentWidthChanged( const QString& ) ) );
    disconnect( percHeightEdit, SIGNAL( textChanged( const QString& ) ),
                this, SLOT( percentHeightChanged( const QString& ) ) );
}

void PNGExportDia::widthChanged( const QString& text )
{
    disconnectAll();
    bool success;
    int width = text.toInt( &success );
    if ( success ) {
        width = QMIN( width, realWidth*10 );
        width = QMAX( width, realWidth/10 );
        double percent = 100.*static_cast<double>( width )/static_cast<double>( realWidth );
        percWidthEdit->setText( QString::number( percent ) );
        if ( proportional->isChecked() ) {
            percHeightEdit->setText( QString::number( percent ) );
            int height = static_cast<int>( realHeight*percent/100. );
            heightEdit->setText( QString::number( height ) );
        }
    }
    connectAll();
}

void PNGExportDia::heightChanged( const QString& text )
{
    disconnectAll();
    bool success;
    int height = text.toInt( &success );
    if ( success ) {
        height = QMIN( height, realHeight*10 );
        height = QMAX( height, realHeight/10 );
        double percent = 100.*static_cast<double>( height )/static_cast<double>( realHeight );
        percHeightEdit->setText( QString::number( percent ) );
        if ( proportional->isChecked() ) {
            percWidthEdit->setText( QString::number( percent ) );
            int width = static_cast<int>( realWidth*percent/100. );
            widthEdit->setText( QString::number( width ) );
        }
    }
    connectAll();
}

void PNGExportDia::percentWidthChanged( const QString& text )
{
    disconnectAll();
    bool success;
    double percent = text.toDouble( &success );
    if ( success ) {
        percent = QMIN( percent, 1000 );
        percent = QMAX( percent, 10 );
        int width = static_cast<int>( realWidth*percent/100. );
        widthEdit->setText( QString::number( width ) );
        if ( proportional->isChecked() ) {
            int height = static_cast<int>( realHeight*percent/100. );
            heightEdit->setText( QString::number( height ) );
            percHeightEdit->setText( QString::number( percent ) );
        }
    }
    connectAll();
}

void PNGExportDia::percentHeightChanged( const QString& text )
{
    disconnectAll();
    bool success;
    double percent = text.toDouble( &success );
    if ( success ) {
        percent = QMIN( percent, 1000 );
        percent = QMAX( percent, 10 );
        if ( proportional->isChecked() ) {
            int width = static_cast<int>( realWidth*percent/100. );
            widthEdit->setText( QString::number( width ) );
            percWidthEdit->setText( QString::number( percent ) );
        }
        int height = static_cast<int>( realHeight*percent/100. );
        heightEdit->setText( QString::number( height ) );
    }
    connectAll();
}

void PNGExportDia::proportionalClicked()
{
    if ( proportional->isChecked() ) {
        disconnectAll();
        bool success;
        int width = widthEdit->text().toInt( &success );
        if ( success ) {
            width = QMIN( width, realWidth*10 );
            width = QMAX( width, realWidth/10 );
            double percent = 100.*static_cast<double>( width )/static_cast<double>( realWidth );
            percHeightEdit->setText( QString::number( percent ) );
            int height = static_cast<int>( realHeight*percent/100. );
            heightEdit->setText( QString::number( height ) );
        }
        connectAll();
    }
}

void PNGExportDia::setupGUI()
{
    resize( size() );
    QWidget *page = new QWidget( this );
    setMainWidget(page);

    QBoxLayout* mainLayout = new QVBoxLayout( page, 0, spacingHint() );

    proportional = new QCheckBox( page, "proportional" );
    proportional->setText( i18n( "Keep ratio" ) );
    proportional->setChecked( true );
    mainLayout->addWidget( proportional );

    QLabel* height = new QLabel( page, "Height" );
    height->setText( i18n( "Height" ) );
    widthEdit = new QLineEdit( page, "widthEdit" );
    QLabel* width = new QLabel( page, "Width" );
    width->setText( i18n( "Width" ) );
    heightEdit = new QLineEdit( page, "heightEdit" );

    QGridLayout* layout1 = new QGridLayout;
    layout1->addWidget( height, 1, 0 );
    layout1->addWidget( widthEdit, 0, 1 );
    layout1->addWidget( width, 0, 0 );
    layout1->addWidget( heightEdit, 1, 1 );

    mainLayout->addLayout( layout1 );

    QLabel* percentHeight = new QLabel( page, "PercentHeight" );
    percentHeight->setText( i18n( "Height (%)" ) );
    QLabel* percentWidth = new QLabel( page, "PercentWidth" );
    percentWidth->setText( i18n( "Width (%)" ) );
    percWidthEdit = new QLineEdit( page, "percWidthEdit" );
    percHeightEdit = new QLineEdit( page, "percHeightEdit" );

    QGridLayout* layout2 = new QGridLayout;
    layout2->addWidget( percWidthEdit, 0, 1 );
    layout2->addWidget( percHeightEdit, 1, 1 );
    layout2->addWidget( percentHeight, 1, 0 );
    layout2->addWidget( percentWidth, 0, 0 );

    mainLayout->addLayout( layout2 );

    /* Display the main layout */
    mainLayout->addStretch( 5 );
    mainLayout->activate();
}


void PNGExportDia::slotOk()
{
    hide();
    //doc->setZoomAndResolution( 100, 600, 600 );
    //doc->setZoomAndResolution( 1000, QPaintDevice::x11AppDpiX(), QPaintDevice::x11AppDpiY() );
    //doc->newZoomAndResolution( false, false );
    bool w_success;
    bool h_success;
    int width = widthEdit->text().toInt( &w_success );
    int height = heightEdit->text().toInt( &h_success );
    if ( !w_success || !h_success ) {
        width = realWidth;
        height = realHeight;
    }
    else {
        width = QMIN( width, realWidth*10 );
        width = QMAX( width, realWidth/10 );
        height = QMIN( height, realHeight*10 );
        height = QMAX( height, realHeight/10 );
    }
    QImage image = formula->drawImage( width, height );
    if ( !image.save( _fileOut, "PNG" ) ) {
        KMessageBox::error( 0, i18n( "Failed to write file." ), i18n( "PNG Export Error" ) );
    }
    reject();
}

#include "pngexportdia.moc"
