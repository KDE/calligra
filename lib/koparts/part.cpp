#include "part.h"

#include <qfile.h>
#include <qpoint.h>
#include <qpointarray.h>
#include <qpainter.h>

Part::Part( QObject* parent, const char* name )
    : QObject( parent, name )
{
}

Part::~Part()
{
}

Part* Part::parentPart()
{
    if ( parent() && parent()->inherits( "Part" ) )
	return (Part*)parent();
    
    return 0;
}

QString Part::readConfigFile( const QString& filename ) const
{
    QFile file( filename );
    if ( !file.open( IO_ReadOnly ) )
	return QString::null;

    uint size = file.size();
    char* buffer = new char[ size + 1 ];
    file.readBlock( buffer, size );
    buffer[ size ] = 0;
    file.close();

    QString text = QString::fromUtf8( buffer, size );
    delete[] buffer;

    return text;
}

QString Part::config()
{
    if (  m_config.isEmpty() )
	m_config = configFile();

    return m_config;
}

View* Part::firstView()
{
    return m_views.first();
}

View* Part::nextView()
{
    return m_views.next();
}

void Part::slotViewDestroyed()
{
    m_views.removeRef( (View *)sender() );
}

void Part::addView( View* view )
{
    m_views.append( view );
    connect( view, SIGNAL( destroyed() ), this, SLOT( slotViewDestroyed() ) );
}

Part* Part::hitTest( const QPoint& p )
{
    return hitTest( p, QWMatrix() );
}

Part* Part::hitTest( const QPoint&, const QWMatrix& )
{
    return this;
}

QAction* Part::action( const char* name )
{
    return m_collection.action( name );
}

QActionCollection* Part::actionCollection()
{
    return &m_collection;
}

// -------------------------------------------------

PartChild::PartChild( Part* parent, Part* part, const QRect& geometry )
    : QObject( parent )
{
    m_part = part;
    m_geometry = geometry;
    m_scaleX = 1.0;
    m_scaleY = 1.0;
    m_shearX = 0.0;
    m_shearY = 0.0;
    m_rotation = 0.0;
    m_lock = FALSE;
    m_transparent = FALSE;

    updateMatrix();
}

PartChild::PartChild( Part* parent )
    : QObject( parent )
{
    m_scaleX = 1.0;
    m_scaleY = 1.0;
    m_shearX = 0.0;
    m_shearY = 0.0;
    m_rotation = 0.0;
    m_lock = FALSE;
    m_transparent = FALSE;
}

void PartChild::setPart( Part* part, const QRect& geometry )
{
    m_part = part;
    m_geometry = geometry;

    updateMatrix();
}

PartChild::~PartChild()
{
    if ( m_part )
	delete m_part;
}

void PartChild::setGeometry( const QRect& rect )
{
    if ( !m_lock )
	m_old = framePointArray();

    m_geometry = rect;
    updateMatrix();

    if ( !m_lock )
	emit changed( this );
}

QRect PartChild::geometry() const
{
    return m_geometry;
}

Part* PartChild::part()
{
    return m_part;
}

QRegion PartChild::region() const
{
    return QRegion( pointArray() );
}

QRegion PartChild::region( const QWMatrix& matrix ) const
{
    return QRegion( pointArray( matrix ) );
}

QPointArray PartChild::pointArray( const QWMatrix& matrix ) const
{
    return pointArray( QRect( 0, 0, m_geometry.width(), m_geometry.height() ), matrix );
}

QPointArray PartChild::pointArray() const
{
    return pointArray( QRect( 0, 0, m_geometry.width(), m_geometry.height() ), QWMatrix() );
}

bool PartChild::contains( const QPoint& p ) const
{
    return region().contains( p );
}

QRect PartChild::boundingRect() const
{
    return pointArray().boundingRect();
}

QRect PartChild::boundingRect( const QWMatrix& matrix ) const
{
    return pointArray( matrix ).boundingRect();
}

bool PartChild::isRectangle() const
{
    return !( m_shearX != 0.0 || m_shearY != 0.0 || m_rotation != 0.0 );
}

void PartChild::setClipRegion( QPainter& painter, bool combine )
{
    painter.setClipping( TRUE );
    if ( combine && !painter.clipRegion().isEmpty() )
	painter.setClipRegion( region( painter.worldMatrix() ).intersect( painter.clipRegion() ) );
    else
	painter.setClipRegion( region( painter.worldMatrix() ) );
}

void PartChild::setScaling( double x, double y )
{
    if ( !m_lock )
	m_old = framePointArray();

    m_scaleX = x;
    m_scaleY = y;
    // updateMatrix();

    if ( !m_lock )
	emit changed( this );
}

double PartChild::xScaling() const
{
    return m_scaleX;
}

double PartChild::yScaling() const
{
    return m_scaleY;
}

void PartChild::setShearing( double x, double y )
{
    if ( !m_lock )
	m_old = framePointArray();

    m_shearX = x;
    m_shearY = y;
    updateMatrix();

    if ( !m_lock )
	emit changed( this );
}

double PartChild::xShearing() const
{
    return m_shearX;
}

double PartChild::yShearing() const
{
    return m_shearY;
}

void PartChild::setRotation( double rot )
{
    if ( !m_lock )
	m_old = framePointArray();

    m_rotation = rot;
    updateMatrix();

    if ( !m_lock )
	emit changed( this );
}

double PartChild::rotation() const
{
    return m_rotation;
}

void PartChild::setRotationPoint( const QPoint& pos )
{
    if ( !m_lock )
	m_old = framePointArray();

    m_rotationPoint = pos;
    updateMatrix();

    if ( !m_lock )
	emit changed( this );
}

QPoint PartChild::rotationPoint() const
{
    return m_rotationPoint;
}

void PartChild::transform( QPainter& painter )
{
    setClipRegion( painter, TRUE );
    QWMatrix m = painter.worldMatrix();
    m = m_matrix * m;
    m.scale( m_scaleX, m_scaleY );
    painter.setWorldMatrix( m );
}

QRect PartChild::contentRect() const
{
    return QRect( 0, 0, m_geometry.width() / m_scaleX, m_geometry.height() / m_scaleY );
}

Part* PartChild::hitTest( const QPoint& p )
{
    if ( !region().contains( p ) )
	return 0;

    QWMatrix m;
    m = m_matrix * m;
    m.scale( m_scaleX, m_scaleY );

    return part()->hitTest( p, m );
}

Part* PartChild::hitTest( const QPoint& p, const QWMatrix& matrix )
{
    if ( !region( matrix ).contains( p ) )
	return 0;

    QWMatrix m( matrix );
    m = m_matrix * m;
    m.scale( m_scaleX, m_scaleY );

    return part()->hitTest( p, m );
}

QPointArray PartChild::framePointArray() const
{
    return pointArray( QRect( -6, -6, m_geometry.width() + 12, m_geometry.height() + 12 ), QWMatrix() );
}

QPointArray PartChild::framePointArray( const QWMatrix& matrix ) const
{
    return pointArray( QRect( -6, -6, m_geometry.width() + 12, m_geometry.height() + 12 ), matrix );
}

QRegion PartChild::frameRegion( const QWMatrix& matrix, bool solid ) const
{
    QPointArray arr = pointArray( QRect( -6, -6, m_geometry.width() + 12, m_geometry.height() + 12 ), matrix );

    if ( solid )
	return QRegion( arr );

    return QRegion( arr ).subtract( region( matrix ) );
}

PartChild::Gadget PartChild::gadgetHitTest( const QPoint& p, const QWMatrix& matrix )
{
    if ( !frameRegion( matrix ).contains( p ) )
	return NoGadget;

    if ( QRegion( pointArray( QRect( -5, -5, 5, 5 ), matrix ) ).contains( p ) )
	return TopLeft;
    if ( QRegion( pointArray( QRect( m_geometry.width() / 2 - 3, -5, 5, 5 ), matrix ) ).contains( p ) )
	return TopMid;
    if ( QRegion( pointArray( QRect( m_geometry.width(), -5, 5, 5 ), matrix ) ).contains( p ) )
	return TopRight;
    if ( QRegion( pointArray( QRect( -5, m_geometry.height() / 2 - 3, 5, 5 ), matrix ) ).contains( p ) )
	return MidLeft;
    if ( QRegion( pointArray( QRect( -5, m_geometry.height(), 5, 5 ), matrix ) ).contains( p ) )
	return BottomLeft;
    if ( QRegion( pointArray( QRect( m_geometry.width() / 2 - 3,
				     m_geometry.height(), 5, 5 ), matrix ) ).contains( p ) )
	return BottomMid;
    if ( QRegion( pointArray( QRect( m_geometry.width(), m_geometry.height(), 5, 5 ), matrix ) ).contains( p ) )
	return BottomRight;
    if ( QRegion( pointArray( QRect( m_geometry.width(),
				     m_geometry.height() / 2 - 3, 5, 5 ), matrix ) ).contains( p ) )
	return MidRight;

    return Move;
}

QPointArray PartChild::pointArray( const QRect& r, const QWMatrix& matrix ) const
{
    QPoint topleft = m_matrix.map( QPoint( r.left(), r.top() ) );
    QPoint topright = m_matrix.map( QPoint( r.right(), r.top() ) );
    QPoint bottomleft = m_matrix.map( QPoint( r.left(), r.bottom() ) );
    QPoint bottomright = m_matrix.map( QPoint( r.right(), r.bottom() ) );

    QPointArray arr( 4 );
    arr.setPoint( 0, topleft );
    arr.setPoint( 1, topright );
    arr.setPoint( 2, bottomright );
    arr.setPoint( 3, bottomleft );

    for( int i = 0; i < 4; ++i )
	arr.setPoint( i, matrix.map( arr.point( i ) ) );

    return arr;
}

void PartChild::updateMatrix()
{
    QWMatrix r;
    r.rotate( - m_rotation );
    QPoint p = r.map( QPoint( m_rotationPoint.x(),
			      m_rotationPoint.y() ) );

    QWMatrix m;
    m.rotate( m_rotation );
    m.translate( -m_rotationPoint.x() + m_geometry.x(), -m_rotationPoint.y() + m_geometry.y() );
    m.translate( p.x(), p.y() );
    m.shear( m_shearX, m_shearY );

    m_matrix = m;
}

QWMatrix PartChild::matrix() const
{
    return m_matrix;
}

void PartChild::lock()
{
    if ( m_lock )
	return;

    m_old = framePointArray();
    m_lock = TRUE;
}

void PartChild::unlock()
{
    if ( !m_lock )
	return;

    m_lock = FALSE;
    emit changed( this );
}

QPointArray PartChild::oldPointArray( const QWMatrix& matrix )
{
    QPointArray arr = m_old;

    for( int i = 0; i < 4; ++i )
	arr.setPoint( i, matrix.map( arr.point( i ) ) );

    return arr;
}

void PartChild::setTransparent( bool b )
{
    m_transparent = b;
}

bool PartChild::isTransparent()
{
    return m_transparent;
}

#include "part.moc"
