#include "toolbox.h"
#include "part.h"

#include <qlayout.h>
#include <qpainter.h>
#include <qspinbox.h>
#include <qobjectlist.h>

ToolBox::ToolBox( QWidget* parent, const char* name )
    : QFrame( parent, name, WStyle_Tool )
{
    setFrameShape( Panel );
    setFrameShadow( Raised );

    m_layout = new QVBoxLayout( this, 2, 2 );
    m_layout->addSpacing( 12 );
}

void ToolBox::setEnabled( bool enable )
{
    if ( enable )
    {
	if ( children() )
        {
	    QObjectListIt it( *children() );
	    QWidget *w;
	    while( (w=(QWidget *)it.current()) != 0 )
	    {
		++it;
		if ( w->isWidgetType() )
		    w->setEnabled( TRUE );
	    }
	}
    }
    else
    {
	if ( focusWidget() == this )
	    focusNextPrevChild( TRUE );
	if ( children() )
        {
	    QObjectListIt it( *children() );
	    QWidget *w;
	    while( (w=(QWidget *)it.current()) != 0 )
	    {
		++it;
		if ( w->isWidgetType() )
	        {
		    w->setEnabled( FALSE );
		    // w->clearWState( WState_ForceDisabled );
		}
	    }
	}
    }
}

void ToolBox::setCaption( const QString& )
{
    // #### todo
}

void ToolBox::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    painter.fillRect( 0, 0, width(), 12, darkBlue );

    painter.end();

    QFrame::paintEvent( ev );
}

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

TransformToolBox::TransformToolBox( PartChild* ch, QWidget* parent, const char* name )
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

    setPartChild( ch );

    connect( m_rotation, SIGNAL( valueChanged( int ) ),
	     this, SLOT( slotRotationChanged( int ) ) );
    connect( m_scale, SIGNAL( valueChanged( int ) ),
	     this, SLOT( slotScalingChanged( int ) ) );
    connect( m_shearX, SIGNAL( valueChanged( int ) ),
	     this, SLOT( slotXShearingChanged( int ) ) );
    connect( m_shearY, SIGNAL( valueChanged( int ) ),
	     this, SLOT( slotYShearingChanged( int ) ) );
}

void TransformToolBox::setPartChild( PartChild* ch )
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

double TransformToolBox::rotation() const
{
    return m_rotation->text().toDouble();
}

double TransformToolBox::scaling() const
{
    return m_scale->text().toDouble() / 100.0;
}

double TransformToolBox::xShearing() const
{
    return m_shearX->text().toDouble() / 10.0;
}

double TransformToolBox::yShearing() const
{
    return m_shearY->text().toDouble() / 10.0;
}

void TransformToolBox::slotRotationChanged( int v )
{
    if ( m_child )
	m_child->setRotation( double( v ) );

    emit rotationChanged( double( v ) );
}

void TransformToolBox::slotScalingChanged( int v )
{
    if ( m_child )
	m_child->setScaling( double( v ) / 100.0, double( v ) / 100.0 );

    emit scalingChanged( double( v ) / 100.0 );
}

void TransformToolBox::slotXShearingChanged( int v )
{
    if ( m_child )
	m_child->setShearing( double( v ) / 10.0, m_child->yShearing() );

    emit xShearingChanged( double( v ) / 10.0 );
}

void TransformToolBox::slotYShearingChanged( int v )
{
    if ( m_child )
	m_child->setShearing( m_child->xShearing(), double( v ) / 10.0 );

    emit yShearingChanged( double( v ) / 10.0 );
}

void TransformToolBox::setRotation( double v )
{
    m_rotation->setValue( int( v ) );
}

void TransformToolBox::setScaling( double v )
{
    m_scale->setValue( int( v * 100.0 ) );
}

void TransformToolBox::setXShearing( double v )
{
    m_shearX->setValue( int( v * 10.0 ) );
}

void TransformToolBox::setYShearing( double v )
{
    m_shearY->setValue( int( v * 10.0 ) );
}

#include "toolbox.moc"
