/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>

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

#include <QLabel>
#include <QPainter>
#include <QLayout>
#include <QPushButton>
#include <QFileInfo>
#include <QPaintEvent>
#include <QGridLayout>
#include <Q3PtrList>
#include <QPointF>
#include <QRadialGradient>
#include <QLinearGradient>
#include <QConicalGradient>

#include <knuminput.h>
#include <kcombobox.h>
#include <klocale.h>
#include <klistbox.h>
#include <kiconloader.h>

#include "vgradientwidget.h"
#include "vgradienttabwidget.h"
#include "karbon_resourceserver.h"
#include "vqpainter.h"
#include "vfill.h"

/// helper function to clone a gradient
QGradient * cloneGradient( const QGradient * gradient )
{
    if( ! gradient )
        return 0;

    QGradient * clone = 0;

    switch( gradient->type() )
    {
        case QGradient::LinearGradient:
        {
            const QLinearGradient * lg = static_cast<const QLinearGradient*>( gradient );
            clone = new QLinearGradient( lg->start(), lg->finalStop() );
            break;
        }
        case QGradient::RadialGradient:
        {
            const QRadialGradient * rg = static_cast<const QRadialGradient*>( gradient );
            clone = new QRadialGradient( rg->center(), rg->radius(), rg->focalPoint() );
            break;
        }
        case QGradient::ConicalGradient:
        {
            const QConicalGradient * cg = static_cast<const QConicalGradient*>( gradient );
            clone = new QConicalGradient( cg->center(), cg->angle() );
            break;
        }
        default:
            return 0;
    }

    clone->setSpread( gradient->spread() );
    clone->setStops( gradient->stops() );

    return clone;
}

VGradientListItem::VGradientListItem( const QGradient * gradient, const QString & filename )
    : Q3ListBoxItem( 0L ), m_gradient( gradient ), m_filename( filename )
{
    m_pixmap = QPixmap( 200, 16 );

    QLinearGradient paintGradient;
    paintGradient.setStops( gradient->stops() );
    paintGradient.setStart( QPointF( 0, 0 ) );
    paintGradient.setFinalStop( QPointF( m_pixmap.width() - 1, 0 ) );

    QPainter painter( &m_pixmap );
    painter.fillRect( QRect( 0, 0, m_pixmap.width(), m_pixmap.height() ), QBrush( paintGradient ) );

    m_delete = QFileInfo( filename ).isWritable();
}

VGradientListItem::VGradientListItem( const VGradientListItem& gradient )
    : Q3ListBoxItem( 0L )
{
    m_pixmap = gradient.m_pixmap;
    m_delete = gradient.m_delete;
    m_gradient = cloneGradient( gradient.gradient() );
    m_filename = gradient.m_filename;
}

VGradientListItem::~VGradientListItem()
{
    delete m_gradient;
}

int VGradientListItem::width( const Q3ListBox* lb ) const
{
    return lb->width() - 25;
}

void VGradientListItem::paint( QPainter* painter )
{
    painter->save();
    QRect r ( 0, 0, width( listBox() ), height( listBox() ) );
    painter->scale( ( (float)( width( listBox() ) ) ) / 200., 1. );
    painter->drawPixmap( 0, 0, m_pixmap );
    painter->restore();
    if ( isSelected() )
        painter->setPen( listBox()->palette().highlightedText().color() );
    else
        painter->setPen( listBox()->palette().base().color() );
    painter->drawRect( r );
}

VGradientPreview::VGradientPreview( QWidget* parent )
    : QWidget( parent ), m_gradient( 0 )
{

    QPalette p = palette();
    p.setBrush(QPalette::Window, QBrush(Qt::NoBrush));
    // TODO: check if this is equivalent with the line below
    // setBackgroundMode( Qt::NoBackground );
    setMinimumSize( 70, 70 );
}

VGradientPreview::~VGradientPreview()
{
    delete m_gradient;
}

void VGradientPreview::setGradient( const QGradient *gradient )
{
    delete m_gradient;
    m_gradient = cloneGradient( gradient );

    switch( m_gradient->type() )
    {
        case QGradient::LinearGradient:
        {
            QLinearGradient * g = static_cast<QLinearGradient*>( m_gradient );
            g->setStart( QPointF( 0.0, 0.0 ) );
            g->setFinalStop( QPointF( width(), 0.0 ) );
            break;
        }
        case QGradient::RadialGradient:
        {
            QRadialGradient * g = static_cast<QRadialGradient*>( m_gradient );
            g->setCenter( QPointF( 0.5 * width(), 0.5 * height() ) );
            g->setFocalPoint( QPointF( 0.5 * width(), 0.5 * height() ) );
            g->setRadius( 0.3 * width() );
            break;
        }
        case QGradient::ConicalGradient:
        {
            QConicalGradient * g = static_cast<QConicalGradient*>( m_gradient );
            g->setCenter( QPointF( 0.5 * width(), 0.5 * height() ) );
            g->setAngle( 0.0 );
            break;
        }
        default:
            delete m_gradient;
            m_gradient = 0;
    }

    update();
}

void VGradientPreview::paintEvent( QPaintEvent* )
{
    QPainter painter( this );

    // TODO draw a checker board as background?
    //painter.setBrush( QBrush( SmallIcon( "karbon" ) ) );
    painter.setBrush( palette().base() );
    painter.drawRect( QRectF( 0, 0, width(), height() ) );

    if( ! m_gradient )
        return;

    painter.setBrush( QBrush( *m_gradient ) );
    painter.drawRect( QRectF( 0, 0, width(), height() ) );

    painter.setPen( palette().light().color() );
    // light frame around widget
    QRect frame( 1, 1, width()-2, height()-2 );
    painter.drawRect( frame );

    painter.setPen( palette().dark().color() );
    painter.drawLine( QPointF( 0, height() - 1 ), QPointF( 0, 0 ) );
    painter.drawLine( QPointF( 0, 0 ), QPointF( width() - 1, 0 ) );
    painter.drawLine( QPointF( width() - 2, 2 ), QPointF( width() - 2, height() - 2 ) );
    painter.drawLine( QPointF( width() - 2, height() - 2 ), QPointF( 2, height() - 2 ) );

}

VGradientTabWidget::VGradientTabWidget( QWidget* parent )
    : QTabWidget( parent ), m_gradient( 0 ), m_resourceServer( 0 ), m_gradOpacity( 1.0 )
{
    // create a default gradient
    m_gradient = new QLinearGradient( QPointF(0,0), QPointF(100,100) );
    m_gradient->setColorAt( 0.0, Qt::white );
    m_gradient->setColorAt( 1.0, Qt::green );

    setupUI();
    setupConnections();
}

VGradientTabWidget::~VGradientTabWidget()
{
    delete m_gradient;
}

void VGradientTabWidget::setupUI()
{
    m_editTab = new QWidget();
    QGridLayout* editLayout = new QGridLayout( m_editTab );

    editLayout->addWidget( new QLabel( i18n( "Type:" ), m_editTab ), 0, 0 );
    editLayout->addWidget( new QLabel( i18n( "Repeat:" ), m_editTab ), 1, 0 );
    editLayout->addWidget( new QLabel( i18n( "Target:" ), m_editTab ), 2, 0 );

    m_gradientType = new KComboBox( false, m_editTab );
    m_gradientType->insertItem( 0, i18n( "Linear" ) );
    m_gradientType->insertItem( 1, i18n( "Radial" ) );
    m_gradientType->insertItem( 2, i18n( "Conical" ) );
    editLayout->addWidget( m_gradientType, 0, 1 );

    m_gradientRepeat = new KComboBox( false, m_editTab );
    m_gradientRepeat->insertItem( 0, i18n( "None" ) );
    m_gradientRepeat->insertItem( 1, i18n( "Reflect" ) );
    m_gradientRepeat->insertItem( 2, i18n( "Repeat" ) );
    editLayout->addWidget( m_gradientRepeat, 1, 1 );

    m_gradientTarget = new KComboBox( false, m_editTab );
    m_gradientTarget->insertItem( 0, i18n( "Stroke" ) );
    m_gradientTarget->insertItem( 1, i18n( "Fill" ) );
    m_gradientTarget->setCurrentIndex( FillGradient );
    editLayout->addWidget( m_gradientTarget, 2, 1 );

    m_gradientWidget = new VGradientWidget( m_editTab );
    m_gradientWidget->setStops( m_gradient->stops() );
    editLayout->addWidget( m_gradientWidget, 4, 0, 1, 2 );

    editLayout->addWidget( new QLabel( i18n( "Overall opacity:" ), m_editTab ), 5, 0 );
    m_opacity = new KIntNumInput( 100, m_editTab );
    m_opacity->setRange( 0, 100, 1, true );
    m_opacity->setValue( 100 );
    editLayout->addWidget( m_opacity, 5, 1, 1, 1 );

    m_addToPredefs = new QPushButton( i18n( "&Add to Predefined Gradients" ), m_editTab );
    editLayout->addWidget( m_addToPredefs, 6, 0, 1, 2 );

    editLayout->setSpacing( 3 );
    editLayout->setMargin( 6 );
    editLayout->setRowMinimumHeight( 0, 12 );

    QWidget* predefTab  = new QWidget();
    QGridLayout* predefLayout = new QGridLayout( predefTab );
    predefLayout->setSpacing( 3 );
    predefLayout->setMargin( 6 );
    predefLayout->setRowMinimumHeight( 0, 12 );
    m_predefGradientsView = new KListBox( predefTab );
    predefLayout->addWidget( m_predefGradientsView, 0, 0, 1, 2 );

    m_predefDelete = new QPushButton( i18n( "&Delete" ), predefTab );
    predefLayout->addWidget( m_predefDelete, 2, 0 );

    m_predefImport = new QPushButton( i18n( "&Import" ), predefTab );
    predefLayout->addWidget( m_predefImport, 2, 1 );
    m_predefImport->setEnabled( false );

    addTab( m_editTab, i18n( "Edit Gradient" ) );
    addTab( predefTab, i18n( "Predefined Gradients" ) );
}

void VGradientTabWidget::setupConnections()
{
    connect( m_gradientType, SIGNAL( activated( int ) ), this, SLOT( combosChange( int ) ) );
    connect( m_gradientRepeat, SIGNAL( activated( int ) ), this, SLOT( combosChange( int ) ) );
    connect( m_gradientWidget, SIGNAL( changed() ), this, SLOT( stopsChanged() ) );
    connect( m_addToPredefs, SIGNAL( clicked() ), this, SLOT( addGradientToPredefs() ) );
    connect( m_predefGradientsView, SIGNAL( doubleClicked( Q3ListBoxItem *, const QPoint & ) ), this, SLOT( changeToPredef( Q3ListBoxItem* ) ) );
    connect( m_predefDelete, SIGNAL( clicked() ), this, SLOT( deletePredef() ) );
    connect( m_opacity, SIGNAL( valueChanged( int ) ), this, SLOT( opacityChanged( int ) ) );
}

void VGradientTabWidget::updateUI()
{
    m_gradientType->setCurrentIndex( m_gradient->type() );
    m_gradientRepeat->setCurrentIndex( m_gradient->spread() );
    m_opacity->setValue( 100 );
    m_gradientWidget->setStops( m_gradient->stops() );
}

void VGradientTabWidget::updatePredefGradients()
{
    m_predefGradientsView->clear();
    if( ! m_resourceServer )
        return;

    Q3PtrList<VGradientListItem>* gradientList = m_resourceServer->gradients();
    if( gradientList->count() > 0 )
        for( VGradientListItem* g = gradientList->first(); g != NULL; g = gradientList->next() )
            m_predefGradientsView->insertItem( new VGradientListItem( *g ) );
}

double VGradientTabWidget::opacity() const
{
    return m_opacity->value() / 100.0;
}

void VGradientTabWidget::setOpacity( double opacity )
{
    if( opacity < 0.0 || opacity > 1.0 )
        return;

    m_gradOpacity = opacity;
    m_opacity->setValue( int(opacity*100.0) );
}

const QGradient * VGradientTabWidget::gradient()
{
    return m_gradient;
}

void VGradientTabWidget::setGradient( const QGradient* gradient )
{
    delete m_gradient;
    m_gradient = cloneGradient( gradient );

    updateUI();
}

VGradientTabWidget::VGradientTarget VGradientTabWidget::target()
{
    return (VGradientTarget)m_gradientTarget->currentIndex();
}

void VGradientTabWidget::setTarget( VGradientTarget target )
{
    m_gradientTarget->setCurrentIndex( target );
}

void VGradientTabWidget::setResourceServer( KarbonResourceServer* server )
{
    m_resourceServer = server;
    updatePredefGradients();
}

void VGradientTabWidget::combosChange( int )
{
    QGradient * newGradient = 0;

    switch( m_gradientType->currentIndex() )
    {
        case QGradient::LinearGradient:
            newGradient = new QLinearGradient();
        break;
        case QGradient::RadialGradient:
            newGradient = new QRadialGradient();
        break;
        case QGradient::ConicalGradient:
            newGradient = new QConicalGradient();
        break;
        default:
            return;
    }
    newGradient->setSpread( (QGradient::Spread)m_gradientRepeat->currentIndex() );
    newGradient->setStops( m_gradient->stops() );
    delete m_gradient;
    m_gradient = newGradient;

    emit changed();
}

void VGradientTabWidget::opacityChanged( int value )
{
    m_gradOpacity = value / 100.0;

    QGradientStops stops = m_gradient->stops();
    uint stopCount = stops.count();
    for( uint i = 0; i < stopCount; ++i )
        stops[i].second.setAlphaF( m_gradOpacity );
    m_gradient->setStops( stops );

    m_gradientWidget->setStops( stops );

    emit changed();
}

void VGradientTabWidget::addGradientToPredefs()
{
    VGradientListItem* item = m_resourceServer->addGradient( cloneGradient( m_gradient ) );
    m_predefGradientsView->insertItem( item );
}

void VGradientTabWidget::predefSelected( Q3ListBoxItem* item )
{
    if( item )
    {
        VGradientListItem* gradientItem = (VGradientListItem*)item;
        m_predefDelete->setEnabled( gradientItem->canDelete() );
    }
}

void VGradientTabWidget::changeToPredef( Q3ListBoxItem* item )
{
    if( item )
    {
        VGradientListItem* gradientItem = (VGradientListItem*)item;
        delete m_gradient;
        m_gradient = cloneGradient( gradientItem->gradient() );
        m_gradientType->setCurrentIndex( m_gradient->type() );
        m_gradientRepeat->setCurrentIndex( m_gradient->spread() );
        m_opacity->setValue( 100 );
        m_gradientWidget->setStops( m_gradient->stops() );
        setCurrentWidget( m_editTab );
        emit changed();
    }
}

void VGradientTabWidget::deletePredef()
{
    int i = m_predefGradientsView->currentItem();
    if( !m_predefGradientsView->item( i ) )
        return;
    m_resourceServer->removeGradient( (VGradientListItem*)m_predefGradientsView->item( i ) );
    m_predefGradientsView->removeItem( i );
}

void VGradientTabWidget::stopsChanged()
{
    m_gradient->setStops( m_gradientWidget->stops() );
    emit changed();
}

#include "vgradienttabwidget.moc"

