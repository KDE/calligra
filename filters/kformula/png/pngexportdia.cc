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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <QCheckBox>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <qpaintdevice.h>
#include <QRect>
#include <QWidget>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3GridLayout>

#include <kapplication.h>
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <kformulacontainer.h>
#include <kformuladocument.h>
#include <kformulamimesource.h>

#include "pngexportdia.h"
#include <knuminput.h>
#include <kglobal.h>

PNGExportDia::PNGExportDia( const QDomDocument &dom, const QString &outFile, QWidget *parent )
    : KDialog( parent, i18n("PNG Export Filter Parameters" ), KDialog::Ok|KDialog::Cancel ),
      _fileOut( outFile )
{
    kapp->restoreOverrideCursor();
    wrapper = new KFormula::DocumentWrapper( KGlobal::config(), 0 );
    KFormula::Document* doc = new KFormula::Document;
    wrapper->document( doc );
    formula = doc->createFormula();
    if ( !doc->loadXML( dom ) ) {
        kError() << "Failed." << endl;
    }

    setupGUI();

    QRect rect = formula->boundingRect();
    realWidth = rect.width();
    realHeight = rect.height();
    widthEdit->setValue(  realWidth );
    heightEdit->setValue(  realHeight  );
    percWidthEdit->setValue( 100 );
    percHeightEdit->setValue( 100 );

    connectAll();
    connect( proportional, SIGNAL( clicked() ),
             this, SLOT( proportionalClicked() ) );
}

PNGExportDia::~PNGExportDia()
{
    delete wrapper;
}

void PNGExportDia::connectAll()
{
    connect( widthEdit, SIGNAL( valueChanged(int) ),
             this, SLOT( widthChanged( int ) ) );
    connect( heightEdit, SIGNAL( valueChanged(int) ),
             this, SLOT( heightChanged( int ) ) );
    connect( percWidthEdit, SIGNAL( valueChanged(double) ),
             this, SLOT( percentWidthChanged( double ) ) );
    connect( percHeightEdit, SIGNAL( valueChanged(double) ),
             this, SLOT( percentHeightChanged(double ) ) );
}

void PNGExportDia::disconnectAll()
{
    disconnect( widthEdit, SIGNAL( valueChanged(int) ),
             this, SLOT( widthChanged( int ) ) );
    disconnect( heightEdit, SIGNAL( valueChanged(int) ),
             this, SLOT( heightChanged( int ) ) );
    disconnect( percWidthEdit, SIGNAL( valueChanged(double) ),
             this, SLOT( percentWidthChanged( double ) ) );
    disconnect( percHeightEdit, SIGNAL( valueChanged(double) ),
             this, SLOT( percentHeightChanged(double ) ) );
}

void PNGExportDia::widthChanged( int width )
{
    disconnectAll();
    width = qMin( width, realWidth*10 );
    width = qMax( width, realWidth/10 );
    double percent = 100.*static_cast<double>( width )/static_cast<double>( realWidth );
    percWidthEdit->setValue(  percent  );
    if ( proportional->isChecked() ) {
        percHeightEdit->setValue( percent );
        int height = static_cast<int>( realHeight*percent/100. );
        heightEdit->setValue(  height );
    }
    connectAll();
}

void PNGExportDia::heightChanged( int height )
{
    disconnectAll();
    height = qMin( height, realHeight*10 );
    height = qMax( height, realHeight/10 );
    double percent = 100.*static_cast<double>( height )/static_cast<double>( realHeight );
    percHeightEdit->setValue(  percent  );
    if ( proportional->isChecked() ) {
        percWidthEdit->setValue(  percent  );
        int width = static_cast<int>( realWidth*percent/100. );
        widthEdit->setValue( width );
    }
    connectAll();
}

void PNGExportDia::percentWidthChanged( double percent )
{
    disconnectAll();
    percent = qMin( percent, 1000.0 );
    percent = qMax( percent, 10.0 );
    int width = static_cast<int>( realWidth*percent/100. );
    widthEdit->setValue(  width  );
    if ( proportional->isChecked() ) {
        int height = static_cast<int>( realHeight*percent/100. );
        heightEdit->setValue(  height  );
        percHeightEdit->setValue(  percent );
    }
    connectAll();
}

void PNGExportDia::percentHeightChanged( double percent )
{
    disconnectAll();
    percent = qMin( percent, 1000.0 );
    percent = qMax( percent, 10.0);
    if ( proportional->isChecked() ) {
        int width = static_cast<int>( realWidth*percent/100. );
        widthEdit->setValue(  width  );
        percWidthEdit->setValue(  percent  );
    }
    int height = static_cast<int>( realHeight*percent/100. );
    heightEdit->setValue(  height  );
    connectAll();
}

void PNGExportDia::proportionalClicked()
{
    if ( proportional->isChecked() ) {
        disconnectAll();
        int width = widthEdit->value( );
        width = qMin( width, realWidth*10 );
        width = qMax( width, realWidth/10 );
        double percent = 100.*static_cast<double>( width )/static_cast<double>( realWidth );
        percHeightEdit->setValue(  percent  );
        int height = static_cast<int>( realHeight*percent/100. );
        heightEdit->setValue(  height  );
        connectAll();
    }
}

void PNGExportDia::setupGUI()
{
    QWidget *page = new QWidget( this );
    setMainWidget(page);

    Q3BoxLayout* mainLayout = new Q3VBoxLayout( page, KDialog::marginHint(), KDialog::spacingHint() );

    proportional = new QCheckBox( page, "proportional" );
    proportional->setText( i18n( "Keep ratio" ) );
    proportional->setChecked( true );
    mainLayout->addWidget( proportional );

    QLabel* height = new QLabel( page, "Height" );
    height->setText( i18n( "Height" ) );
    widthEdit = new KIntNumInput( page );
    QLabel* width = new QLabel( page, "Width" );
    width->setText( i18n( "Width" ) );
    heightEdit = new KIntNumInput( page );

    Q3GridLayout* layout1 = new Q3GridLayout(this);
    layout1->addWidget( height, 1, 0 );
    layout1->addWidget( widthEdit, 0, 1 );
    layout1->addWidget( width, 0, 0 );
    layout1->addWidget( heightEdit, 1, 1 );

    mainLayout->addLayout( layout1 );

    QLabel* percentHeight = new QLabel( page, "PercentHeight" );
    percentHeight->setText( i18n( "Height (%)" ) );
    QLabel* percentWidth = new QLabel( page, "PercentWidth" );
    percentWidth->setText( i18n( "Width (%)" ) );
    percWidthEdit = new KDoubleNumInput( page );
    percHeightEdit = new KDoubleNumInput( page );

    Q3GridLayout* layout2 = new Q3GridLayout(page);
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
    int width = widthEdit->value();
    int height = heightEdit->value();
//     kDebug( KFormula::DEBUGID ) << k_funcinfo
//                                  << "(" << width << " " << height << ")"
//                                  << endl;
//     width = realWidth;
//     height = realHeight;
    QImage image = formula->drawImage( width, height );
    if ( !image.save( _fileOut, "PNG" ) ) {
        KMessageBox::error( 0, i18n( "Failed to write file." ), i18n( "PNG Export Error" ) );
    }
    reject();
}

#include "pngexportdia.moc"
