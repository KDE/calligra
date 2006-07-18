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
#include <knuminput.h>

#include "exportsizedia.h"


ExportSizeDia::ExportSizeDia( int width, int height,
			    QWidget *parent )
    : KDialog( parent)
{
    setCaption( i18n("Export Filter Parameters" ) );
    setButtons( KDialog::Ok|KDialog::Cancel );
    kapp->restoreOverrideCursor();

    setupGUI();

    m_realWidth  = width;
    m_realHeight = height;
    m_widthEdit ->setValue( m_realWidth );
    m_heightEdit->setValue( m_realHeight  );
    m_percWidthEdit->setValue( 100 );
    m_percHeightEdit->setValue( 100 );

    connectAll();
    connect( m_proportional, SIGNAL( clicked() ),
             this,         SLOT( proportionalClicked() ) );
}


ExportSizeDia::~ExportSizeDia()
{
}


void ExportSizeDia::setupGUI()
{
    //resize( size() );
    QWidget *page = new QWidget( this );
    setMainWidget(page);

#if 0
    Q3BoxLayout* mainLayout = new Q3VBoxLayout( page,
					      KDialog::marginHint(),
					      KDialog::spacingHint() );
#else
    Q3GridLayout *mainLayout = new Q3GridLayout( page, 5, 2,
					       KDialog::marginHint(),
					       KDialog::spacingHint() );
#endif
    m_proportional = new QCheckBox( page, "proportional" );
    m_proportional->setText( i18n( "Keep ratio" ) );
    m_proportional->setChecked( true );
    mainLayout->addWidget( m_proportional, 0, 0 );

    QLabel* width = new QLabel( page );
    width->setObjectName( "width" );
    width->setText( i18n( "Width:" ) );
    m_widthEdit = new KIntNumInput( page );
	m_widthEdit->setObjectName("widthEdit" );
    QLabel* height = new QLabel( page );
    height->setObjectName( "height" );
    height->setText( i18n( "Height:" ) );
    m_heightEdit = new KIntNumInput( page );
	m_heightEdit->setObjectName("heightEdit" );

    mainLayout->addWidget( width,      1, 0 );
    mainLayout->addWidget( m_widthEdit,  1, 1 );
    mainLayout->addWidget( height,     2, 0 );
    mainLayout->addWidget( m_heightEdit, 2, 1 );

    QLabel* percentWidth = new QLabel( page, "PercentWidth" );
    percentWidth->setText( i18n( "Width (%):" ) );
    m_percWidthEdit = new KDoubleNumInput( page);
	m_percWidthEdit->setObjectName("percWidthEdit" );
    QLabel* percentHeight = new QLabel( page, "PercentHeight" );
    percentHeight->setText( i18n( "Height (%):" ) );
    m_percHeightEdit = new KDoubleNumInput( page);
	m_percHeightEdit->setObjectName( "percHeightEdit" );

    mainLayout->addWidget( percentWidth,   3, 0 );
    mainLayout->addWidget( m_percHeightEdit, 3, 1 );
    mainLayout->addWidget( percentHeight,  4, 0 );
    mainLayout->addWidget( m_percWidthEdit,  4, 1 );

    /* Display the main layout */
    //mainLayout->addStretch( 5 );
    mainLayout->activate();
}


// ----------------------------------------------------------------
//                          public methods

int ExportSizeDia::width() const
{
    return m_widthEdit->value();
}


int ExportSizeDia::height() const
{
    return m_heightEdit->value();
}


// ----------------------------------------------------------------
//                            slots


void ExportSizeDia::widthChanged( int width )
{
    disconnectAll();
    width = qMin( width, m_realWidth * 10 );
    width = qMax( width, m_realWidth / 10 );
    double percent = (100.0 * static_cast<double>( width )
		      / static_cast<double>( m_realWidth ));
    m_percWidthEdit->setValue(  percent  );
    if ( m_proportional->isChecked() ) {
        m_percHeightEdit->setValue( percent );
        int height = static_cast<int>( m_realHeight * percent / 100.0 );
        m_heightEdit->setValue(  height );
    }
    connectAll();
}


void ExportSizeDia::heightChanged( int height )
{
    disconnectAll();
    height = qMin( height, m_realHeight * 10 );
    height = qMax( height, m_realHeight / 10 );
    double percent = (100.0 * static_cast<double>( height )
		      / static_cast<double>( m_realHeight ));
    m_percHeightEdit->setValue( percent  );
    if ( m_proportional->isChecked() ) {
        m_percWidthEdit->setValue(  percent  );
        int width = static_cast<int>( m_realWidth * percent / 100.0 );
        m_widthEdit->setValue( width );
    }
    connectAll();
}


void ExportSizeDia::percentWidthChanged( double percent )
{
    disconnectAll();
    percent = qMin( percent, 1000.0 );
    percent = qMax( percent, 10.0 );
    int width = static_cast<int>( m_realWidth * percent / 100. );
    m_widthEdit->setValue(  width  );
    if ( m_proportional->isChecked() ) {
        int height = static_cast<int>( m_realHeight * percent / 100. );
        m_heightEdit->setValue(  height  );
        m_percHeightEdit->setValue(  percent );
    }
    connectAll();
}


void ExportSizeDia::percentHeightChanged( double percent )
{
    disconnectAll();
    percent = qMin( percent, 1000.0 );
    percent = qMax( percent, 10.0 );
    if ( m_proportional->isChecked() ) {
        int width = static_cast<int>( m_realWidth * percent / 100. );
        m_widthEdit->setValue(  width  );
        m_percWidthEdit->setValue(  percent  );
    }
    int height = static_cast<int>( m_realHeight * percent / 100. );
    m_heightEdit->setValue(  height  );
    connectAll();
}


void ExportSizeDia::proportionalClicked()
{
    if ( m_proportional->isChecked() ) {
        disconnectAll();
        int width = m_widthEdit->value( );
        width = qMin( width, m_realWidth * 10 );
        width = qMax( width, m_realWidth / 10 );
        double percent = (100.0 * static_cast<double>( width )
			  / static_cast<double>( m_realWidth ));
        m_percHeightEdit->setValue(  percent  );
        int height = static_cast<int>( m_realHeight * percent / 100. );
        m_heightEdit->setValue(  height  );
        connectAll();
    }
}


// ----------------------------------------------------------------
//                          private methods


void ExportSizeDia::connectAll()
{
    connect( m_widthEdit,      SIGNAL( valueChanged(int) ),
             this,             SLOT( widthChanged( int ) ) );
    connect( m_heightEdit,     SIGNAL( valueChanged(int) ),
             this,             SLOT( heightChanged( int ) ) );
    connect( m_percWidthEdit,  SIGNAL( valueChanged(double) ),
             this,             SLOT( percentWidthChanged( double ) ) );
    connect( m_percHeightEdit, SIGNAL( valueChanged(double) ),
             this,             SLOT( percentHeightChanged(double ) ) );
}


void ExportSizeDia::disconnectAll()
{
    disconnect( m_widthEdit,      SIGNAL( valueChanged(int) ),
		this,             SLOT( widthChanged( int ) ) );
    disconnect( m_heightEdit,     SIGNAL( valueChanged(int) ),
		this,             SLOT( heightChanged( int ) ) );
    disconnect( m_percWidthEdit,  SIGNAL( valueChanged(double) ),
		this,             SLOT( percentWidthChanged( double ) ) );
    disconnect( m_percHeightEdit, SIGNAL( valueChanged(double) ),
		this,             SLOT( percentHeightChanged(double ) ) );
}


#if 0
void ExportSizeDia::slotOk()
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
#endif

#include "exportsizedia.moc"

