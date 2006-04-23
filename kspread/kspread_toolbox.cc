/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#include "kspread_toolbox.h"
#include "KoDocumentChild.h"

#include <kwin.h>

#include <qlayout.h>
#include <qspinbox.h>
#include <qobject.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QChildEvent>

using namespace KSpread;

ToolBox::ToolBox( QWidget* parent, const char* name )
    : QFrame( parent, name, Qt::WType_TopLevel | Qt::WStyle_Tool )
{
    KWin::setType( winId(), NET::Tool );

    setFrameShape( Panel );
    setFrameShadow( Raised );

    m_layout = new QVBoxLayout( this );
    m_layout->setMargin(2);
    m_layout->setSpacing(2);
    // m_layout->addSpacing( 12 );
}

void ToolBox::setEnabled( bool enable )
{
    if ( enable )
    {
        if ( !children().isEmpty() )
        {
            QList<QObject*> children( QObject::children() );
	    foreach ( QObject* child, children )
	    {
		if ( child->isWidgetType() )
                    static_cast<QWidget*>(child)->setEnabled( true );
	    }
	}
    }
    else
    {
	if ( focusWidget() == this )
	    focusNextPrevChild( true );
        if ( !children().isEmpty() )
        {
            QList<QObject*> children( QObject::children() );
	    foreach ( QObject* child, children )
	    {
		if ( child->isWidgetType() )
	        {
                    static_cast<QWidget*>(child)->setEnabled( FALSE );
		    // child->clearWState( WState_ForceDisabled );
		}
	    }
	}
    }
}

/*
void ToolBox::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    painter.fillRect( 0, 0, width(), 12, darkBlue );

    painter.end();

    QFrame::paintEvent( ev );
}
*/

void ToolBox::childEvent( QChildEvent* ev )
{
    if ( ev->inserted() && ev->child()->isWidgetType() )
	m_layout->addWidget( (QWidget*)ev->child() );
    resize( sizeHint() );
}

void ToolBox::mousePressEvent( QMouseEvent* ev )
{
    m_startPos = geometry().topLeft();
    m_mousePos = ev->globalPos();
}

void ToolBox::mouseMoveEvent( QMouseEvent* ev )
{
    setGeometry( m_startPos.x() - m_mousePos.x() + ev->globalPos().x(),
		 m_startPos.y() - m_mousePos.y() + ev->globalPos().y(),
		 width(), height() );
}

// ---------------------------------------------

KoTransformToolBox::KoTransformToolBox( KoDocumentChild* ch, QWidget* parent, const char* name )
    : ToolBox( parent, name )
{
    m_child = 0;

    m_rotation = new QSpinBox( 0, 360, 5, this );
    m_rotation->setSuffix( " deg" );
    m_scale = new QSpinBox( 10, 400, 10, this );
    m_scale->setSuffix( "%" );
    m_shearX = new QSpinBox( -100, 100, 1, this );
    m_shearX->setSuffix( " px" );
    m_shearY = new QSpinBox( -100, 100, 1, this );
    m_shearY->setSuffix( " px" );

    setDocumentChild( ch );

    connect( m_rotation, SIGNAL( valueChanged( int ) ),
	     this, SLOT( slotRotationChanged( int ) ) );
    connect( m_scale, SIGNAL( valueChanged( int ) ),
	     this, SLOT( slotScalingChanged( int ) ) );
    connect( m_shearX, SIGNAL( valueChanged( int ) ),
	     this, SLOT( slotXShearingChanged( int ) ) );
    connect( m_shearY, SIGNAL( valueChanged( int ) ),
	     this, SLOT( slotYShearingChanged( int ) ) );
}

void KoTransformToolBox::setDocumentChild( KoDocumentChild* ch )
{
    if ( m_child == ch )
	return;

    m_child = ch;

    if ( m_child )
    {
	setRotation( m_child->rotation() );
	setScaling( m_child->xScaling() );
	setXShearing( m_child->xShearing() );
	setYShearing( m_child->yShearing() );
    }
}

double KoTransformToolBox::rotation() const
{
    return m_rotation->text().toDouble();
}

double KoTransformToolBox::scaling() const
{
    return m_scale->text().toDouble() / 100.0;
}

double KoTransformToolBox::xShearing() const
{
    return m_shearX->text().toDouble() / 10.0;
}

double KoTransformToolBox::yShearing() const
{
    return m_shearY->text().toDouble() / 10.0;
}

void KoTransformToolBox::slotRotationChanged( int v )
{
    if ( m_child )
	m_child->setRotation( double( v ) );

    emit rotationChanged( double( v ) );
}

void KoTransformToolBox::slotScalingChanged( int v )
{
    if ( m_child )
	m_child->setScaling( double( v ) / 100.0, double( v ) / 100.0 );

    emit scalingChanged( double( v ) / 100.0 );
}

void KoTransformToolBox::slotXShearingChanged( int v )
{
    if ( m_child )
	m_child->setShearing( double( v ) / 10.0, m_child->yShearing() );

    emit xShearingChanged( double( v ) / 10.0 );
}

void KoTransformToolBox::slotYShearingChanged( int v )
{
    if ( m_child )
	m_child->setShearing( m_child->xShearing(), double( v ) / 10.0 );

    emit yShearingChanged( double( v ) / 10.0 );
}

void KoTransformToolBox::setRotation( double v )
{
    m_rotation->setValue( int( v ) );
}

void KoTransformToolBox::setScaling( double v )
{
    m_scale->setValue( int( v * 100.0 ) );
}

void KoTransformToolBox::setXShearing( double v )
{
    m_shearX->setValue( int( v * 10.0 ) );
}

void KoTransformToolBox::setYShearing( double v )
{
    m_shearY->setValue( int( v * 10.0 ) );
}

#include "kspread_toolbox.moc"
