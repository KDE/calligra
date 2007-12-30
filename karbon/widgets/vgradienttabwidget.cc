/* This file is part of the KDE project
   Copyright (C) 2001-2002 Beno�t Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2002-2003 Rob Buis <buis@kde.org>
   Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>

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

#include "vgradientwidget.h"
#include "vgradienttabwidget.h"
#include "KarbonGradientItem.h"

#include <KoAbstractGradient.h>
#include <KoStopGradient.h>
#include <KoSegmentGradient.h>
#include <KoResourceItemChooser.h>
#include <KoResourceServer.h>
#include <KoResourceServerProvider.h>

#include <knuminput.h>
#include <kcombobox.h>
#include <klocale.h>
#include <kiconloader.h>
#include <klistwidget.h>
#include <kfiledialog.h>
#include <kurl.h>
#include <kcomponentdata.h>
#include <kstandarddirs.h>

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

#include <math.h>

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

void transferGradientPosition( const QGradient * srcGradient, QGradient * dstGradient )
{
    // first check if gradients have the same type
    if( srcGradient->type() == dstGradient->type() )
    {
        switch( srcGradient->type() )
        {
            case QGradient::LinearGradient:
            {
                const QLinearGradient * src = static_cast<const QLinearGradient*>( srcGradient );
                QLinearGradient * dst = static_cast<QLinearGradient*>( dstGradient );
                dst->setStart( src->start() );
                dst->setFinalStop( src->finalStop() );
                break;
            }
            case QGradient::RadialGradient:
            {
                const QRadialGradient * src = static_cast<const QRadialGradient*>( srcGradient );
                QRadialGradient * dst = static_cast<QRadialGradient*>( dstGradient );
                dst->setCenter( src->center() );
                dst->setRadius( src->radius() );
                dst->setFocalPoint( src->focalPoint() );
                break;
            }
            case QGradient::ConicalGradient:
            {
                const QConicalGradient * src = static_cast<const QConicalGradient*>( srcGradient );
                QConicalGradient * dst = static_cast<QConicalGradient*>( dstGradient );
                dst->setCenter( src->center() );
                dst->setAngle( src->angle() );
                break;
            }
            default:
                return;
        }
        return;
    }

    // try to preserve gradient positions as best as possible
    QPointF start, stop;
    switch( srcGradient->type() )
    {
        case QGradient::LinearGradient:
        {
            const QLinearGradient * g = static_cast<const QLinearGradient*>( srcGradient );
            start = g->start();
            stop = g->finalStop();
            break;
        }
        case QGradient::RadialGradient:
        {
            const QRadialGradient * g = static_cast<const QRadialGradient*>( srcGradient );
            start = g->center();
            stop = QPointF( g->radius(), 0.0 );
            break;
        }
        case QGradient::ConicalGradient:
        {
            const QConicalGradient * g = static_cast<const QConicalGradient*>( srcGradient );
            start = g->center();
            double radAngle = g->angle()*M_PI/180.0;
            stop = QPointF( 50.0 * cos( radAngle), 50.*sin( radAngle ) );
            break;
        }
        default:
            start = QPointF( 0.0, 0.0 );
            stop = QPointF( 50.0, 50.0 );
    }

    switch( dstGradient->type() )
    {
        case QGradient::LinearGradient:
        {
            QLinearGradient * g = static_cast<QLinearGradient*>( dstGradient );
            g->setStart( start );
            g->setFinalStop( stop );
            break;
        }
        case QGradient::RadialGradient:
        {
            QRadialGradient * g = static_cast<QRadialGradient*>( dstGradient );
            QPointF diff = stop-start;
            double radius = sqrt( diff.x()*diff.x() + diff.y()*diff.y() );
            g->setCenter( start );
            g->setFocalPoint( start );
            g->setRadius( radius );
            break;
        }
        case QGradient::ConicalGradient:
        {
            QConicalGradient * g = static_cast<QConicalGradient*>( dstGradient );
            QPointF diff = stop-start;
            double angle = atan2( diff.y(), diff.x() );
            if( angle < 0.0 )
                angle += 2*M_PI;
            g->setCenter( start );
            g->setAngle( angle*180/M_PI );
            break;
        }
        default:
            return;
    }
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

    QPixmap checker(8, 8);
    QPainter p(&checker);
    p.fillRect(0, 0, 4, 4, Qt::lightGray);
    p.fillRect(4, 0, 4, 4, Qt::darkGray);
    p.fillRect(0, 4, 4, 4, Qt::darkGray);
    p.fillRect(4, 4, 4, 4, Qt::lightGray);
    p.end();

    QRect rect = QRect( 0, 0, width(), height() );

    // TODO draw a checker board as background?
    //painter.setBrush( QBrush( SmallIcon( "karbon" ) ) );
    painter.fillRect( rect, QBrush(checker));
    //painter.setBrush( palette().base() );
    //painter.drawRect( rect ) ) );

    if( ! m_gradient )
        return;

    painter.setBrush( QBrush( *m_gradient ) );
    painter.drawRect( rect );

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
    : QTabWidget( parent ), m_gradient( 0 ), m_resourceServer( 0 ), m_resourceAdapter(0), m_gradOpacity( 1.0 )
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
    delete m_resourceAdapter;
    delete m_gradient;
}

void VGradientTabWidget::resizeEvent( QResizeEvent * )
{
    //m_predefGradientsView->setIconSize( QSize( m_predefGradientsView->width(), 16 ) );
}

void VGradientTabWidget::setupUI()
{
    m_editTab = new QWidget();
    QGridLayout* editLayout = new QGridLayout( m_editTab );

    editLayout->addWidget( new QLabel( i18n( "Type:" ), m_editTab ), 0, 0 );
    editLayout->addWidget( new QLabel( i18n( "Repeat:" ), m_editTab ), 1, 0 );
    editLayout->addWidget( new QLabel( i18n( "Target:" ), m_editTab ), 2, 0 );

    m_gradientType = new KComboBox( false, m_editTab );
    m_gradientType->insertItem( 0, i18nc( "Linear gradient type", "Linear" ) );
    m_gradientType->insertItem( 1, i18nc( "Radial gradient type", "Radial" ) );
    m_gradientType->insertItem( 2, i18nc( "Conical gradient type", "Conical" ) );
    editLayout->addWidget( m_gradientType, 0, 1 );

    m_gradientRepeat = new KComboBox( false, m_editTab );
    m_gradientRepeat->insertItem( 0, i18nc( "No gradient spread", "None" ) );
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
    m_opacity->setRange( 0, 100 );
    m_opacity->setValue( 100 );
    editLayout->addWidget( m_opacity, 5, 1 );

    m_addToPredefs = new QPushButton( i18n( "&Add to Predefined Gradients" ), m_editTab );
    editLayout->addWidget( m_addToPredefs, 6, 0, 1, 2 );

    editLayout->setSpacing( 3 );
    editLayout->setMargin( 6 );
    editLayout->setRowMinimumHeight( 0, 12 );
    editLayout->setRowStretch( 7, 1 );

    QWidget* predefTab  = new QWidget();
    QGridLayout* predefLayout = new QGridLayout( predefTab );
    m_predefGradientsView = new KoResourceItemChooser( predefTab );
    m_predefGradientsView->setIconSize( QSize( 300, 20 ) );
    predefLayout->addWidget( m_predefGradientsView, 0, 0, 1, 2 );

    predefLayout->setSpacing( 3 );
    predefLayout->setMargin( 6 );
    predefLayout->setRowMinimumHeight( 0, 12 );

    addTab( m_editTab, i18n( "Edit Gradient" ) );
    addTab( predefTab, i18n( "Predefined Gradients" ) );
}

void VGradientTabWidget::setupConnections()
{
    connect( m_gradientType, SIGNAL( activated( int ) ), this, SLOT( combosChange( int ) ) );
    connect( m_gradientRepeat, SIGNAL( activated( int ) ), this, SLOT( combosChange( int ) ) );
    connect( m_gradientWidget, SIGNAL( changed() ), this, SLOT( stopsChanged() ) );
    connect( m_addToPredefs, SIGNAL( clicked() ), this, SLOT( addGradientToPredefs() ) );
    connect( m_predefGradientsView, SIGNAL( itemDoubleClicked( QTableWidgetItem * ) ), this, SLOT( changeToPredef( QTableWidgetItem* ) ) );
    connect( m_predefGradientsView, SIGNAL( deleteClicked() ), this, SLOT( deletePredef() ) );
    connect( m_predefGradientsView, SIGNAL( importClicked() ), this, SLOT( importGradient() ) );
    connect( m_opacity, SIGNAL( valueChanged( int ) ), this, SLOT( opacityChanged( int ) ) );
}

void VGradientTabWidget::blockChildSignals( bool block )
{
    m_gradientType->blockSignals( block );
    m_gradientRepeat->blockSignals( block );
    m_gradientWidget->blockSignals( block );
    m_addToPredefs->blockSignals( block );
    m_predefGradientsView->blockSignals( block );
    m_opacity->blockSignals( block );
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

    QList<KoAbstractGradient*> gradients = m_resourceServer->resources();
    if( gradients.count() > 0 )
    {
        foreach( KoAbstractGradient * gradient, gradients ) {
            m_predefGradientsView->addItem( new KarbonGradientItem( gradient ) );
        }
    }
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

void VGradientTabWidget::setResourceServer( KoResourceServer<KoAbstractGradient>* server )
{
    delete m_resourceAdapter;
    m_resourceAdapter = 0;
    m_resourceServer = server;

    if( m_resourceServer )
    {
        m_resourceAdapter = new KoResourceServerAdapter<KoAbstractGradient>( m_resourceServer );
        connect( m_resourceAdapter, SIGNAL(resourceAdded(KoResource*)), 
                 this, SLOT(addResource(KoResource*)));
        connect( m_resourceAdapter, SIGNAL(removingResource(KoResource*)), 
                 this, SLOT(removeResource(KoResource*)));
        m_resourceAdapter->connectToResourceServer();
    }
}

void VGradientTabWidget::combosChange( int )
{
    QGradient * newGradient = 0;

    QPointF start, stop;
    // try to preserve gradient positions
    switch( m_gradient->type() )
    {
        case QGradient::LinearGradient:
        {
            QLinearGradient * g = static_cast<QLinearGradient*>( m_gradient );
            start = g->start();
            stop = g->finalStop();
            break;
        }
        case QGradient::RadialGradient:
        {
            QRadialGradient * g = static_cast<QRadialGradient*>( m_gradient );
            start = g->center();
            stop = QPointF( g->radius(), 0.0 );
            break;
        }
        case QGradient::ConicalGradient:
        {
            QConicalGradient * g = static_cast<QConicalGradient*>( m_gradient );
            start = g->center();
            double radAngle = g->angle()*M_PI/180.0;
            stop = QPointF( 50.0 * cos( radAngle), 50.*sin( radAngle ) );
            break;
        }
        default:
            start = QPointF( 0.0, 0.0 );
            stop = QPointF( 50.0, 50.0 );
    }

    switch( m_gradientType->currentIndex() )
    {
        case QGradient::LinearGradient:
            newGradient = new QLinearGradient( start, stop );
            break;
        case QGradient::RadialGradient:
        {
            QPointF diff = stop-start;
            double radius = sqrt( diff.x()*diff.x() + diff.y()*diff.y() );
            newGradient = new QRadialGradient( start, radius, start );
            break;
        }
        case QGradient::ConicalGradient:
        {
            QPointF diff = stop-start;
            double angle = atan2( diff.y(), diff.x() );
            if( angle < 0.0 )
                angle += 2*M_PI;
            newGradient = new QConicalGradient( start, angle*180/M_PI );
            break;
        }
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
    QString savePath = m_resourceServer->saveLocation();

    int i = 1;
    QFileInfo fileInfo;

    do {
        fileInfo.setFile( savePath + QString("%1.svg").arg( i++, 4, 10, QChar('0') ) );
    }
    while( fileInfo.exists() );

    KoStopGradient * g = KoStopGradient::fromQGradient( m_gradient );
    if( ! g )
        return;
    g->setFilename( fileInfo.filePath() );
    g->setValid( true );

    if( !m_resourceServer->addResource( g ) )
        delete g;
}

void VGradientTabWidget::changeToPredef( QTableWidgetItem * item )
{
    if( ! item )
        return;

    KarbonGradientItem * gradientItem = dynamic_cast<KarbonGradientItem*>(item);
    if( ! gradientItem )
        return;

    QGradient * newGradient = gradientItem->gradient()->toQGradient();
    if( m_gradient )
    {
        m_gradient->setStops( newGradient->stops() );
        delete newGradient;
    }
    else
    {
        m_gradient = newGradient;
    }
    blockChildSignals( true );
    m_gradientType->setCurrentIndex( m_gradient->type() );
    m_gradientRepeat->setCurrentIndex( m_gradient->spread() );
    m_opacity->setValue( 100 );
    m_gradientWidget->setStops( m_gradient->stops() );
    blockChildSignals( false );
    setCurrentWidget( m_editTab );
    emit changed();
}

void VGradientTabWidget::importGradient()
{
    QString filter( "*.svg *.kgr *.ggr" );
    QString filename = KFileDialog::getOpenFileName( KUrl(), filter, 0, i18n( "Choose Gradient to Add" ) );

    if(m_resourceAdapter)
        m_resourceAdapter->importResource(filename);
}

void VGradientTabWidget::deletePredef()
{
     KarbonGradientItem * item = dynamic_cast<KarbonGradientItem*>( m_predefGradientsView->currentItem() );
     if( ! item )
         return;

     m_resourceServer->removeResource( item->gradient() );
}

void VGradientTabWidget::stopsChanged()
{
    m_gradient->setStops( m_gradientWidget->stops() );
    emit changed();
}

void VGradientTabWidget::addResource(KoResource* resource)
{
    KoAbstractGradient * gradient = dynamic_cast<KoAbstractGradient*>( resource );

    if( gradient && gradient->valid() ) {
        KarbonGradientItem* item = new KarbonGradientItem( gradient );
        m_itemMap[resource] = item;
        m_predefGradientsView->addItem(item);
    }
}

void VGradientTabWidget::removeResource(KoResource* resource)
{
    KoResourceItem *item = m_itemMap[resource];

    if(item) {
        m_itemMap.remove(item->resource());
        m_predefGradientsView->removeItem( item );
    }
}

#include "vgradienttabwidget.moc"

