#include "frame.h"
#include "view.h"

#include <qpainter.h>

Frame::Frame( QWidget* parent, const char* name )
    : QWidget( parent, name )
{
    m_state = Inactive;
    m_mode = -1;
    m_view = 0;

    setBackgroundColor( white );
    setMouseTracking( TRUE );
}

Frame::~Frame()
{
    if ( m_view )
	delete m_view;
}

void Frame::setView( View* view )
{
    if ( view == m_view )
	return;

    if ( m_view )
	m_view->removeEventFilter( this );

    m_view = view;
    if ( m_view )
    {
	m_view->installEventFilter( this );
	setMaximumSize( QMAX( m_view->maximumWidth() + 2 * border(), m_view->maximumWidth() ),
			QMAX( m_view->maximumHeight() + 2 * border(), m_view->maximumHeight() ) );
	setMinimumSize( m_view->minimumWidth() + leftBorder() + rightBorder(),
			m_view->minimumHeight() + topBorder() + bottomBorder() );
    }

    resizeEvent( 0 );
}

View* Frame::view()
{
    return m_view;
}

void Frame::setState( State state )
{
    if ( m_state == state )
	return;

    qDebug("State %i -> %i or %x", m_state, state, (int)this );

    State old = m_state;
    m_state = state;

    if ( m_view )
    {
	setMaximumSize( QMAX( m_view->maximumWidth() + 2 * border(), m_view->maximumWidth() ),
			QMAX( m_view->maximumHeight() + 2 * border(), m_view->maximumHeight() ) );
	setMinimumSize( m_view->minimumWidth() + leftBorder() + rightBorder(),
			m_view->minimumHeight() + topBorder() + bottomBorder() );
    }

    if ( m_state == Inactive )
    {
	m_state = old;
	int l = leftBorder();
	int r = rightBorder();
	int t = topBorder();
	int b = bottomBorder();
	m_state = Inactive;
	setGeometry( x() + l, y() + t, width() - l - r, height() - t - b );
    }
    else if ( ( m_state == Active || m_state == Selected ) && old == Inactive )	
	setGeometry( x() - leftBorder(), y() - topBorder(),
		     width() + leftBorder() + rightBorder(),
		     height() + topBorder() + bottomBorder()  );
    else if ( m_state == Active  && old == Selected )
    {
	qDebug("Diff is %i %i", leftBorder(), border() );
	setGeometry( x() - leftBorder() + border(), y() - topBorder() + border(),
		     width() + leftBorder() + rightBorder() - 2 * border(),
		     height() + topBorder() + bottomBorder() - 2 * border() );
    }
    update();
}

void Frame::paintEvent( QPaintEvent* )
{
    QPainter painter;
    painter.begin( this );

    painter.setPen( black );
    painter.fillRect( 0, 0, width(), height(), BDiagPattern );

    if ( m_state == Selected )
    {
	painter.fillRect( 0, 0, 5, 5, black );
	painter.fillRect( 0, height() - 5, 5, 5, black );
	painter.fillRect( width() - 5, height() - 5, 5, 5, black );
	painter.fillRect( width() - 5, 0, 5, 5, black );
	painter.fillRect( width() / 2 - 3, 0, 5, 5, black );
	painter.fillRect( width() / 2 - 3, height() - 5, 5, 5, black );
	painter.fillRect( 0, height() / 2 - 3, 5, 5, black );
	painter.fillRect( width() - 5, height() / 2 - 3, 5, 5, black );
    }

    painter.end();
}

void Frame::mousePressEvent( QMouseEvent* ev )
{
    QRect r1( 0, 0, 5, 5 );
    QRect r2( 0, height() - 5, 5, 5 );
    QRect r3( width() - 5, height() - 5, 5, 5 );
    QRect r4( width() - 5, 0, 5, 5 );
    QRect r5( width() / 2 - 3, 0, 5, 5 );
    QRect r6( width() / 2 - 3, height() - 5, 5, 5 );
    QRect r7( 0, height() / 2 - 3, 5, 5 );
    QRect r8( width()- 5, height() / 2 - 3, 5, 5 );

    if ( r1.contains( ev->pos() ) )
	m_mode = 1;
    else if ( r2.contains( ev->pos() ) )
	m_mode = 2;
    else if ( r3.contains( ev->pos() ) )
	m_mode = 3;
    else if ( r4.contains( ev->pos() ) )
	m_mode = 4;
    else if ( r5.contains( ev->pos() ) )
	m_mode = 5;
    else if ( r6.contains( ev->pos() ) )
	m_mode = 6;
    else if ( r7.contains( ev->pos() ) )
	m_mode = 7;
    else if ( r8.contains( ev->pos() ) )
	m_mode = 8;
    else
	m_mode = 0;

    if ( m_state == Active )
	m_mode = 0;

    qDebug("---- MODE=%i", m_mode );

    m_mousePressPos = mapToParent( ev->pos() );
    m_framePos = mapToParent( QPoint( 0, 0 ) );
    m_width = width();
    m_height = height();
}

void Frame::mouseMoveEvent( QMouseEvent* ev )
{
    if ( m_mode == 0 )
    {
	QPoint p = parentWidget()->mapFromGlobal( ev->globalPos() );
	move( QPoint( p.x() + m_framePos.x() - m_mousePressPos.x(),
		      p.y() + m_framePos.y() - m_mousePressPos.y() ) );
    }
    else if ( m_mode == 1 )
    {
	QPoint p = parentWidget()->mapFromGlobal( ev->globalPos() );
	int w = QMIN( QMAX( m_width + m_mousePressPos.x() - p.x(), minimumWidth() ), maximumWidth() );
	int h = QMIN( QMAX( m_height + m_mousePressPos.y() - p.y(), minimumHeight() ), maximumHeight() );
	setGeometry( m_framePos.x() - w + m_width,
		     m_framePos.y() - h + m_height, w, h );	
    }
    else if ( m_mode == 2 )
    {
	QPoint p = parentWidget()->mapFromGlobal( ev->globalPos() );
	int w = QMIN( QMAX( m_width + m_mousePressPos.x() - p.x(), minimumWidth() ), maximumWidth() );
	int h = QMIN( QMAX( m_height - m_mousePressPos.y() + p.y(), minimumHeight() ), maximumHeight() );
	setGeometry( m_framePos.x() - w + m_width,
		     m_framePos.y(), w, h );
    }
    else if ( m_mode == 3 )
    {
	QPoint p = parentWidget()->mapFromGlobal( ev->globalPos() );
	int w = QMIN( QMAX( m_width - m_mousePressPos.x() + p.x(), minimumWidth() ), maximumWidth() );
	int h = QMIN( QMAX( m_height - m_mousePressPos.y() + p.y(), minimumHeight() ), maximumHeight() );
	resize( w, h );
    }
    else if ( m_mode == 4 )
    {
	QPoint p = parentWidget()->mapFromGlobal( ev->globalPos() );
	int w = QMIN( QMAX( m_width - m_mousePressPos.x() + p.x(), minimumWidth() ), maximumWidth() );
	int h = QMIN( QMAX( m_height + m_mousePressPos.y() - p.y(), minimumHeight() ), maximumHeight() );
	setGeometry( m_framePos.x(),
		     m_framePos.y() - h + m_height, w, h );
    }
    else if ( m_mode == 5 )
    {
	QPoint p = parentWidget()->mapFromGlobal( ev->globalPos() );
	int h = QMIN( QMAX( m_height + m_mousePressPos.y() - p.y(), minimumHeight() ), maximumHeight() );
	setGeometry( m_framePos.x(),
		     m_framePos.y() - h + m_height, m_width, h );
    }
    else if ( m_mode == 6 )
    {
	QPoint p = parentWidget()->mapFromGlobal( ev->globalPos() );
	int h = QMIN( QMAX( m_height - m_mousePressPos.y() + p.y(), minimumHeight() ), maximumHeight() );
	resize( m_width, h );
    }
    else if ( m_mode == 7 )
    {
	QPoint p = parentWidget()->mapFromGlobal( ev->globalPos() );
	int w = QMIN( QMAX( m_width + m_mousePressPos.x() - p.x(), minimumWidth() ), maximumWidth() );
	setGeometry( m_framePos.x() - w + m_width,
		     m_framePos.y(), w, m_height );
    }
    else if ( m_mode == 8 )
    {
	QPoint p = parentWidget()->mapFromGlobal( ev->globalPos() );
	int w = QMIN( QMAX( m_width - m_mousePressPos.x() + p.x(), minimumWidth() ), maximumWidth() );
	resize( w, m_height );
    }
    else if ( m_state == Selected )
    {
	QRect r1( 0, 0, 5, 5 );
	QRect r2( 0, height() - 5, 5, 5 );
	QRect r3( width() - 5, height() - 5, 5, 5 );
	QRect r4( width() - 5, 0, 5, 5 );
	QRect r5( width() / 2 - 3, 0, 5, 5 );
	QRect r6( width() / 2 - 3, height() - 5, 5, 5 );
	QRect r7( 0, height() / 2 - 3, 5, 5 );
	QRect r8( width()- 5, height() / 2 - 3, 5, 5 );
	
	if ( r1.contains( ev->pos() ) || r3.contains( ev->pos() ) )
	    setCursor( sizeFDiagCursor );
	else if ( r2.contains( ev->pos() ) || r4.contains( ev->pos() ) )
	    setCursor( sizeBDiagCursor );	
	else if ( r5.contains( ev->pos() ) || r6.contains( ev->pos() ) )
	     setCursor( sizeVerCursor );
	else if ( r7.contains( ev->pos() ) || r8.contains( ev->pos() ) )
	    setCursor( sizeHorCursor );
	else
	    setCursor( pointingHandCursor );
    }
    else
	setCursor( pointingHandCursor );
}

void Frame::mouseReleaseEvent( QMouseEvent* )
{
    m_mode = -1;
}

void Frame::resizeEvent( QResizeEvent* )
{
    if ( !m_view )
	return;

    if ( m_state == Active || m_state == Selected )
	m_view->setGeometry( 5, 5, width() - 10, height() - 10 );
    else
	m_view->setGeometry( 0, 0, width(), height() );
}

bool Frame::eventFilter( QObject* obj, QEvent* ev )
{
    if ( obj == m_view && ev->type() == QEvent::User + 291173 )
    {
	qDebug("Activate event");
	ViewActivateEvent* e = (ViewActivateEvent*)ev;
	if ( e->part() == m_view->part() )
        {
	    if ( e->activated() )
		setState( Active );
	    else
		setState( Inactive );
	}
    }
    else if ( obj == m_view && ev->type() == QEvent::User + 291174 )
    {
	qDebug("Selected event");
	ViewSelectEvent* e = (ViewSelectEvent*)ev;
	if ( e->part() == m_view->part() )
        {
	    if ( e->selected() )
		setState( Selected );
	    else
		setState( Inactive );
	}
    }

    return FALSE;
}

int Frame::leftBorder() const
{
    if ( m_state == Inactive )
	return 0;
    if ( m_state == Selected || !m_view )
	return border();

    return m_view->leftBorder() + border();
}

int Frame::rightBorder() const
{
    if ( m_state == Inactive )
	return 0;
    if ( m_state == Selected || !m_view )
	return border();

    return m_view->rightBorder() + border();
}

int Frame::topBorder() const
{
    if ( m_state == Inactive )
	return 0;
    if ( m_state == Selected || !m_view )
	return border();

    return m_view->topBorder() + border();
}

int Frame::bottomBorder() const
{
    if ( m_state == Inactive )
	return 0;
    if ( m_state == Selected || !m_view )
	return border();

    return m_view->bottomBorder() + border();
}

int Frame::border() const
{
    if ( m_state == Inactive )
	return 0;
    return 5;
}

#include "frame.moc"
