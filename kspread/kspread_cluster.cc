#include <stdlib.h>

#include "kspread_cluster.h"
#include "kspread_cell.h"
#include "kspread_layout.h"

/****************************************************
 *
 * KSpreadCluster
 *
 ****************************************************/

KSpreadCluster::KSpreadCluster()
    : m_first( 0 ), m_autoDelete( FALSE )
{
    m_cluster = (KSpreadCell***)malloc( KSPREAD_CLUSTER_LEVEL1 * KSPREAD_CLUSTER_LEVEL1 * sizeof( KSpreadCell** ) );

    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
	for( int y = 0; y < KSPREAD_CLUSTER_LEVEL1; ++y )
	    m_cluster[ y * KSPREAD_CLUSTER_LEVEL1 + x ] = 0;
}

KSpreadCluster::~KSpreadCluster()
{
    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
	for( int y = 0; y < KSPREAD_CLUSTER_LEVEL1; ++y )
        {
	    KSpreadCell** cl = m_cluster[ y * KSPREAD_CLUSTER_LEVEL1 + x ];
	    if ( cl )
	    {
		free( cl );
		m_cluster[ y * KSPREAD_CLUSTER_LEVEL1 + x ] = 0;
	    }
	}
	
    if ( m_autoDelete )
    {
	KSpreadCell* cell = m_first;
	while( cell )
        {
	    KSpreadCell* n = cell->nextCell();
	    delete cell;
	    cell = n;
	}
    }


    free( m_cluster );
}

void KSpreadCluster::clear()
{
    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
	for( int y = 0; y < KSPREAD_CLUSTER_LEVEL1; ++y )
        {
	    KSpreadCell** cl = m_cluster[ y * KSPREAD_CLUSTER_LEVEL1 + x ];
	    if ( cl )
	    {
		free( cl );
		m_cluster[ y * KSPREAD_CLUSTER_LEVEL1 + x ] = 0;
	    }
	}

    if ( m_autoDelete )
    {
	KSpreadCell* cell = m_first;
	while( cell )
        {
	    KSpreadCell* n = cell->nextCell();
	    delete cell;
	    cell = n;
	}
    }

    m_first = 0;
}

KSpreadCell* KSpreadCluster::lookup( int x, int y )
{
    if ( x >= KSPREAD_CLUSTER_MAX || x < 0 || y >= KSPREAD_CLUSTER_MAX || y < 0 )
	return 0;

    int cx = x / KSPREAD_CLUSTER_LEVEL2;
    int cy = y / KSPREAD_CLUSTER_LEVEL2;
    int dx = x % KSPREAD_CLUSTER_LEVEL2;
    int dy = y % KSPREAD_CLUSTER_LEVEL2;

    KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + cx ];
    if ( !cl )
	return 0;

    return cl[ dy * KSPREAD_CLUSTER_LEVEL2 + dx ];
}

const KSpreadCell* KSpreadCluster::lookup( int x, int y ) const
{
    if ( x >= KSPREAD_CLUSTER_MAX || x < 0 || y >= KSPREAD_CLUSTER_MAX || y < 0 )
	return 0;

    int cx = x / KSPREAD_CLUSTER_LEVEL2;
    int cy = y / KSPREAD_CLUSTER_LEVEL2;
    int dx = x % KSPREAD_CLUSTER_LEVEL2;
    int dy = y % KSPREAD_CLUSTER_LEVEL2;

    KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + cx ];
    if ( !cl )
	return 0;

    return cl[ dy * KSPREAD_CLUSTER_LEVEL2 + dx ];
}

void KSpreadCluster::insert( KSpreadCell* cell, int x, int y )
{
    if ( x >= KSPREAD_CLUSTER_MAX || x < 0 || y >= KSPREAD_CLUSTER_MAX || y < 0 )
	return;

    int cx = x / KSPREAD_CLUSTER_LEVEL2;
    int cy = y / KSPREAD_CLUSTER_LEVEL2;
    int dx = x % KSPREAD_CLUSTER_LEVEL2;
    int dy = y % KSPREAD_CLUSTER_LEVEL2;

    KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + cx ];
    if ( !cl )
    {
	cl = (KSpreadCell**)malloc(  KSPREAD_CLUSTER_LEVEL2 * KSPREAD_CLUSTER_LEVEL2 * sizeof( KSpreadCell*  ) );
	m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + cx ] = cl;
	
	for( int a = 0; a < KSPREAD_CLUSTER_LEVEL2; ++a )
	    for( int b = 0; b < KSPREAD_CLUSTER_LEVEL2; ++b )
	    cl[ b * KSPREAD_CLUSTER_LEVEL2 + a ] = 0;
    }

    if ( cl[ dy * KSPREAD_CLUSTER_LEVEL2 + dx ] )
	remove( x, y );

    cl[ dy * KSPREAD_CLUSTER_LEVEL2 + dx ] = cell;

    if ( m_first )
    {
	cell->setNextCell( m_first );
	m_first->setPreviousCell( cell );
    }
    m_first = cell;
}
    
void KSpreadCluster::remove( int x, int y )
{
    if ( x >= KSPREAD_CLUSTER_MAX || x < 0 || y >= KSPREAD_CLUSTER_MAX || y < 0 )
	return;

    int cx = x / KSPREAD_CLUSTER_LEVEL2;
    int cy = y / KSPREAD_CLUSTER_LEVEL2;
    int dx = x % KSPREAD_CLUSTER_LEVEL2;
    int dy = y % KSPREAD_CLUSTER_LEVEL2;

    KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + cx ];
    if ( !cl )
	return;

    KSpreadCell* c = cl[ dy * KSPREAD_CLUSTER_LEVEL2 + dx ];
    if ( !c )
	return;

    cl[ dy * KSPREAD_CLUSTER_LEVEL2 + dx ] = 0;

    if ( m_autoDelete )
    {
	if ( m_first == c )
	    m_first = c->nextCell();
        if(c->isForceExtraCells())
        {
                    c->forceExtraCells(c->column(),c->row(),0,0);
        }
	delete c;
    }
    else
    {
	if ( m_first == c )
	    m_first = c->nextCell();
	if ( c->previousCell() )
	    c->previousCell()->setNextCell( c->nextCell() );
	if ( c->nextCell() )
	    c->nextCell()->setPreviousCell( c->previousCell() );
	c->setNextCell( 0 );
	c->setPreviousCell( 0 );
    }
}

bool KSpreadCluster::shiftRow( const QPoint& marker )
{
    bool dummy;
    return shiftRow( marker, dummy );
}

bool KSpreadCluster::shiftColumn( const QPoint& marker )
{
    bool dummy;
    return shiftColumn( marker, dummy );
}

void KSpreadCluster::unshiftColumn( const QPoint& marker )
{
    bool dummy;
    unshiftColumn( marker, dummy );
}

void KSpreadCluster::unshiftRow( const QPoint& marker )
{
    bool dummy;
    unshiftRow( marker, dummy );
}

void KSpreadCluster::setAutoDelete( bool b )
{
    m_autoDelete = b;
}

bool KSpreadCluster::autoDelete() const
{
    return m_autoDelete;
}

KSpreadCell* KSpreadCluster::firstCell()
{
    return m_first;
}

bool KSpreadCluster::shiftRow( const QPoint& marker, bool& work )
{
    work = FALSE;

    if ( marker.x() >= KSPREAD_CLUSTER_MAX || marker.x() < 0 ||
	 marker.y() >= KSPREAD_CLUSTER_MAX || marker.y() < 0 )
	return FALSE;

    int cx = marker.x() / KSPREAD_CLUSTER_LEVEL2;
    int cy = marker.y() / KSPREAD_CLUSTER_LEVEL2;
    int dx = marker.x() % KSPREAD_CLUSTER_LEVEL2;
    int dy = marker.y() % KSPREAD_CLUSTER_LEVEL2;

    // Is there a cell at the right most position ?
    // In this case the shift is impossible.
    KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + KSPREAD_CLUSTER_LEVEL1 - 1 ];
    if ( cl && cl[ dy * KSPREAD_CLUSTER_LEVEL2 + KSPREAD_CLUSTER_LEVEL2 - 1 ] )
	return FALSE;

    bool a = autoDelete();
    setAutoDelete( FALSE );

    // Move cells in this row one to the right.
    for( int i = KSPREAD_CLUSTER_LEVEL1 - 1; i >= cx ; --i )
    {
	KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + i ];
	if ( cl )
        {
	    work = TRUE;
	    int left = 0;
	    if ( i == cx )
		left = dx;
	    int right = KSPREAD_CLUSTER_LEVEL2 - 1;
	    if ( i == KSPREAD_CLUSTER_LEVEL1 - 1 )
		right = KSPREAD_CLUSTER_LEVEL2 - 2;
	    for( int k = right; k >= left; --k )
	    {
		KSpreadCell* c = cl[ dy * KSPREAD_CLUSTER_LEVEL2 + k ];
		if ( c )
	        {
		    remove( c->column(), c->row() );
		    c->move( c->column() + 1, c->row() );
		    insert( c, c->column(), c->row() );
		}
	    }
	}
    }

    setAutoDelete( a );

    return TRUE;
}

bool KSpreadCluster::shiftColumn( const QPoint& marker, bool& work )
{
    work = FALSE;
	
    if ( marker.x() >= KSPREAD_CLUSTER_MAX || marker.x() < 0 ||
	 marker.y() >= KSPREAD_CLUSTER_MAX || marker.y() < 0 )
	return FALSE;

    int cx = marker.x() / KSPREAD_CLUSTER_LEVEL2;
    int cy = marker.y() / KSPREAD_CLUSTER_LEVEL2;
    int dx = marker.x() % KSPREAD_CLUSTER_LEVEL2;
    int dy = marker.y() % KSPREAD_CLUSTER_LEVEL2;

    // Is there a cell at the right most position ?
    // In this case the shift is impossible.
    KSpreadCell** cl = m_cluster[ KSPREAD_CLUSTER_LEVEL1 * ( KSPREAD_CLUSTER_LEVEL1 - 1 ) + cx ];
    if ( cl && cl[ KSPREAD_CLUSTER_LEVEL2 * ( KSPREAD_CLUSTER_LEVEL2 - 1 ) + dx ] )
	return FALSE;

    bool a = autoDelete();
    setAutoDelete( FALSE );

    // Move cells in this column one down.
    for( int i = KSPREAD_CLUSTER_LEVEL1 - 1; i >= cy ; --i )
    {
	KSpreadCell** cl = m_cluster[ i * KSPREAD_CLUSTER_LEVEL1 + cx ];
	if ( cl )
        {
	    work = TRUE;

	    int top = 0;
	    if ( i == cy )
		top = dy;
	    int bottom = KSPREAD_CLUSTER_LEVEL2 - 1;
	    if ( i == KSPREAD_CLUSTER_LEVEL1 - 1 )
		bottom = KSPREAD_CLUSTER_LEVEL2 - 2;
	    for( int k = bottom; k >= top; --k )
	    {
		KSpreadCell* c = cl[ k * KSPREAD_CLUSTER_LEVEL2 + dx ];
		if ( c )
	        {
		    remove( c->column(), c->row() );
		    c->move( c->column(), c->row() + 1 );
		    insert( c, c->column(), c->row() );
		}
	    }
	}
    }

    setAutoDelete( a );

    return TRUE;
}

bool KSpreadCluster::insertColumn( int col )
{
    if ( col >= KSPREAD_CLUSTER_MAX || col < 0 )
	return FALSE;

    // Is there a cell at the right most position ?
    // In this case the shift is impossible.
    for( int t1 = 0; t1 < KSPREAD_CLUSTER_LEVEL1; ++t1 )
    {
	KSpreadCell** cl = m_cluster[ t1 * KSPREAD_CLUSTER_LEVEL1 + KSPREAD_CLUSTER_LEVEL1 - 1 ];
	if ( cl )
	    for( int t2 = 0; t2 < KSPREAD_CLUSTER_LEVEL2; ++t2 )
		if ( cl[ t2 * KSPREAD_CLUSTER_LEVEL2 + KSPREAD_CLUSTER_LEVEL2 - 1 ] )
		    return FALSE;
    }

    for( int t1 = 0; t1 < KSPREAD_CLUSTER_LEVEL1; ++t1 )
    {
	bool work = TRUE;
	for( int t2 = 0; work && t2 < KSPREAD_CLUSTER_LEVEL2; ++t2 )
	    shiftRow( QPoint( col, t1 * KSPREAD_CLUSTER_LEVEL2 + t2 ), work );
    }

    return TRUE;
}

bool KSpreadCluster::insertRow( int row )
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
	return FALSE;

    // Is there a cell at the bottom most position ?
    // In this case the shift is impossible.
    for( int t1 = 0; t1 < KSPREAD_CLUSTER_LEVEL1; ++t1 )
    {
	KSpreadCell** cl = m_cluster[ KSPREAD_CLUSTER_LEVEL1 * ( KSPREAD_CLUSTER_LEVEL1 - 1 ) + t1 ];
	if ( cl )
	    for( int t2 = 0; t2 < KSPREAD_CLUSTER_LEVEL2; ++t2 )
		if ( cl[ KSPREAD_CLUSTER_LEVEL2 * ( KSPREAD_CLUSTER_LEVEL2 - 1 ) + t2 ] )
		    return FALSE;
    }

    for( int t1 = 0; t1 < KSPREAD_CLUSTER_LEVEL1; ++t1 )
    {
	bool work = TRUE;
	for( int t2 = 0; work && t2 < KSPREAD_CLUSTER_LEVEL2; ++t2 )
	    shiftColumn( QPoint( t1 * KSPREAD_CLUSTER_LEVEL2 + t2, row ), work );
    }

    return TRUE;
}

void KSpreadCluster::unshiftColumn( const QPoint& marker, bool& work )
{
    work = FALSE;

    if ( marker.x() >= KSPREAD_CLUSTER_MAX || marker.x() < 0 ||
	 marker.y() >= KSPREAD_CLUSTER_MAX || marker.y() < 0 )
	return;

    int cx = marker.x() / KSPREAD_CLUSTER_LEVEL2;
    int cy = marker.y() / KSPREAD_CLUSTER_LEVEL2;
    int dx = marker.x() % KSPREAD_CLUSTER_LEVEL2;
    int dy = marker.y() % KSPREAD_CLUSTER_LEVEL2;

    bool a = autoDelete();
    setAutoDelete( FALSE );

    // Move cells in this column one up.
    for( int i = cy; i < KSPREAD_CLUSTER_LEVEL1; ++i )
    {
	KSpreadCell** cl = m_cluster[ i * KSPREAD_CLUSTER_LEVEL1 + cx ];
	if ( cl )
        {
	    work = TRUE;

	    int top = 0;
	    if ( i == cy )
		top = dy + 1;
	    int bottom = KSPREAD_CLUSTER_LEVEL2 - 1;
	    for( int k = top; k <= bottom; ++k )
	    {
		KSpreadCell* c = cl[ k * KSPREAD_CLUSTER_LEVEL2 + dx ];
		if ( c )
	        {
		    remove( c->column(), c->row() );
		    c->move( c->column(), c->row() - 1 );
		    insert( c, c->column(), c->row() );
		}
	    }
	}
    }

    setAutoDelete( a );
}

void KSpreadCluster::unshiftRow( const QPoint& marker, bool& work )
{
    work = FALSE;

    if ( marker.x() >= KSPREAD_CLUSTER_MAX || marker.x() < 0 ||
	 marker.y() >= KSPREAD_CLUSTER_MAX || marker.y() < 0 )
	return;

    int cx = marker.x() / KSPREAD_CLUSTER_LEVEL2;
    int cy = marker.y() / KSPREAD_CLUSTER_LEVEL2;
    int dx = marker.x() % KSPREAD_CLUSTER_LEVEL2;
    int dy = marker.y() % KSPREAD_CLUSTER_LEVEL2;

    bool a = autoDelete();
    setAutoDelete( FALSE );

    // Move cells in this row one to the left.
    for( int i = cx; i < KSPREAD_CLUSTER_LEVEL1; ++i )
    {
	KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + i ];
	if ( cl )
        {
	    work = TRUE;

	    int left = 0;
	    if ( i == cx )
		left = dx + 1;
	    int right = KSPREAD_CLUSTER_LEVEL2 - 1;
	    for( int k = left; k <= right; ++k )
	    {
		KSpreadCell* c = cl[ dy * KSPREAD_CLUSTER_LEVEL2 + k ];
		if ( c )
	        {
		    remove( c->column(), c->row() );
		    c->move( c->column() - 1, c->row() );
		    insert( c, c->column(), c->row() );
		}
	    }
	}
    }

    setAutoDelete( a );
}

void KSpreadCluster::removeColumn( int col )
{
    if ( col >= KSPREAD_CLUSTER_MAX || col < 0 )
	return;

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    for( int y1 = 0; y1 < KSPREAD_CLUSTER_LEVEL1; ++y1 )
    {
	KSpreadCell** cl = m_cluster[ y1 * KSPREAD_CLUSTER_LEVEL2 + cx ];
	if ( cl )
	    for( int y2 = 0; y2 < KSPREAD_CLUSTER_LEVEL2; ++y2 )
		if ( cl[ y2 * KSPREAD_CLUSTER_LEVEL2 + dx ] )
		    remove( col, y1 * KSPREAD_CLUSTER_LEVEL2 + y2 );
    }

    for( int t1 = 0; t1 < KSPREAD_CLUSTER_LEVEL1; ++t1 )
    {
	bool work = TRUE;
	for( int t2 = 0; work && t2 < KSPREAD_CLUSTER_LEVEL2; ++t2 )
	    unshiftRow( QPoint( col, t1 * KSPREAD_CLUSTER_LEVEL2 + t2 ), work );
    }
}

void KSpreadCluster::removeRow( int row )
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
	return;

    int cy = row / KSPREAD_CLUSTER_LEVEL2;
    int dy = row % KSPREAD_CLUSTER_LEVEL2;

    for( int x1 = 0; x1 < KSPREAD_CLUSTER_LEVEL1; ++x1 )
    {
	KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL2 + x1 ];
	if ( cl )
	    for( int x2 = 0; x2 < KSPREAD_CLUSTER_LEVEL2; ++x2 )
		if ( cl[ dy * KSPREAD_CLUSTER_LEVEL2 + x2 ] )
		    remove( x1 * KSPREAD_CLUSTER_LEVEL2 + x2, row );
    }

    for( int t1 = 0; t1 < KSPREAD_CLUSTER_LEVEL1; ++t1 )
    {
	bool work = TRUE;
	for( int t2 = 0; work && t2 < KSPREAD_CLUSTER_LEVEL2; ++t2 )
	    unshiftColumn( QPoint( t1 * KSPREAD_CLUSTER_LEVEL2 + t2, row ), work );
    }
}

void KSpreadCluster::clearColumn( int col )
{
    if ( col >= KSPREAD_CLUSTER_MAX || col < 0 )
	return;

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    for( int y1 = 0; y1 < KSPREAD_CLUSTER_LEVEL1; ++y1 )
    {
	KSpreadCell** cl = m_cluster[ y1 * KSPREAD_CLUSTER_LEVEL2 + cx ];
	if ( cl )
	    for( int y2 = 0; y2 < KSPREAD_CLUSTER_LEVEL2; ++y2 )
		if ( cl[ y2 * KSPREAD_CLUSTER_LEVEL2 + dx ] )
		    remove( col, y1 * KSPREAD_CLUSTER_LEVEL2 + y2 );
    }
}

void KSpreadCluster::clearRow( int row )
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
	return;

    int cy = row / KSPREAD_CLUSTER_LEVEL2;
    int dy = row % KSPREAD_CLUSTER_LEVEL2;

    for( int x1 = 0; x1 < KSPREAD_CLUSTER_LEVEL1; ++x1 )
    {
	KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL2 + x1 ];
	if ( cl )
	    for( int x2 = 0; x2 < KSPREAD_CLUSTER_LEVEL2; ++x2 )
		if ( cl[ dy * KSPREAD_CLUSTER_LEVEL2 + x2 ] )
		    remove( x1 * KSPREAD_CLUSTER_LEVEL2 + x2, row );
    }
}

/****************************************************
 *
 * KSpreadColumnCluster
 *
 ****************************************************/

KSpreadColumnCluster::KSpreadColumnCluster()
    : m_first( 0 ), m_autoDelete( FALSE )
{
    m_cluster = (ColumnLayout***)malloc( KSPREAD_CLUSTER_LEVEL1 * sizeof( ColumnLayout** ) );

    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
	    m_cluster[ x ] = 0;
}

KSpreadColumnCluster::~KSpreadColumnCluster()
{
    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
    {
	ColumnLayout** cl = m_cluster[ x ];
	if ( cl )
        {
	    free( cl );
	    m_cluster[ x ] = 0;
	}
    }
	
    if ( m_autoDelete )
    {
	ColumnLayout* cell = m_first;
	while( cell )
        {
	    ColumnLayout* n = cell->next();
	    delete cell;
	    cell = n;
	}
    }


    free( m_cluster );
}

ColumnLayout* KSpreadColumnCluster::lookup( int col )
{
    if ( col >= KSPREAD_CLUSTER_MAX || col < 0 )
	return 0;

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    ColumnLayout** cl = m_cluster[ cx ];
    if ( !cl )
	return 0;

    return cl[ dx ];
}

const ColumnLayout* KSpreadColumnCluster::lookup( int col ) const
{
    if ( col >= KSPREAD_CLUSTER_MAX || col < 0 )
	return 0;

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    ColumnLayout** cl = m_cluster[ cx ];
    if ( !cl )
	return 0;

    return cl[ dx ];
}

void KSpreadColumnCluster::clear()
{
    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
    {
	ColumnLayout** cl = m_cluster[ x ];
	if ( cl )
        {
	    free( cl );
	    m_cluster[ x ] = 0;
	}
    }

    if ( m_autoDelete )
    {
	ColumnLayout* cell = m_first;
	while( cell )
        {
	    ColumnLayout* n = cell->next();
	    delete cell;
	    cell = n;
	}
    }

    m_first = 0;
}

void KSpreadColumnCluster::insertElement( ColumnLayout* lay, int col )
{
    if ( col >= KSPREAD_CLUSTER_MAX || col < 0 )
	return;

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    ColumnLayout** cl = m_cluster[ cx ];
    if ( !cl )
    {
	cl = (ColumnLayout**)malloc( KSPREAD_CLUSTER_LEVEL2 * sizeof( ColumnLayout*  ) );
	m_cluster[ cx ] = cl;
	
	for( int a = 0; a < KSPREAD_CLUSTER_LEVEL2; ++a )
	    cl[ a ] = 0;
    }

    if ( cl[ dx ] )
	removeElement( col );

    cl[ dx ] = lay;

    if ( m_first )
    {
	lay->setNext( m_first );
	m_first->setPrevious( lay );
    }
    m_first = lay;
}

void KSpreadColumnCluster::removeElement( int col )
{
    if ( col >= KSPREAD_CLUSTER_MAX || col < 0 )
	return;

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    ColumnLayout** cl = m_cluster[ cx ];
    if ( !cl )
	return;

    ColumnLayout* c = cl[ dx ];
    if ( !c )
	return;

    cl[ dx ] = 0;

    if ( m_autoDelete )
    {
	if ( m_first == c )
	    m_first = c->next();
	delete c;
    }
    else
    {
	if ( m_first == c )
	    m_first = c->next();
	if ( c->previous() )
	    c->previous()->setNext( c->next() );
	if ( c->next() )
	    c->next()->setPrevious( c->previous() );
	c->setNext( 0 );
	c->setPrevious( 0 );
    }
}

bool KSpreadColumnCluster::insertColumn( int col )
{
    if ( col >= KSPREAD_CLUSTER_MAX || col < 0 )
	return FALSE;

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    // Is there a column layout at the right most position ?
    // In this case the shift is impossible.
    ColumnLayout** cl = m_cluster[ KSPREAD_CLUSTER_LEVEL1 - 1 ];
    if ( cl && cl[ KSPREAD_CLUSTER_LEVEL2 - 1 ] )
	return FALSE;

    bool a = autoDelete();
    setAutoDelete( FALSE );

    for( int i = KSPREAD_CLUSTER_LEVEL1 - 1; i >= cx ; --i )
    {
	ColumnLayout** cl = m_cluster[ i ];
	if ( cl )
        {
	    int left = 0;
	    if ( i == cx )
		left = dx;
	    int right = KSPREAD_CLUSTER_LEVEL2 - 1;
	    if ( i == KSPREAD_CLUSTER_LEVEL1 - 1 )
		right = KSPREAD_CLUSTER_LEVEL2 - 2;
	    for( int k = right; k >= left; --k )
	    {
		ColumnLayout* c = cl[ k ];
		if ( c )		
	        {
		    removeElement( c->column() );
		    c->setColumn( c->column() + 1 );
		    insertElement( c, c->column() );
		}
	    }
	}
    }

    setAutoDelete( a );

    return TRUE;
}

bool KSpreadColumnCluster::removeColumn( int column )
{
    if ( column >= KSPREAD_CLUSTER_MAX || column < 0 )
	return FALSE;

    int cx = column / KSPREAD_CLUSTER_LEVEL2;
    int dx = column % KSPREAD_CLUSTER_LEVEL2;

    removeElement( column );

    bool a = autoDelete();
    setAutoDelete( FALSE );

    for( int i = cx; i < KSPREAD_CLUSTER_LEVEL1; ++i )
    {
	ColumnLayout** cl = m_cluster[ i ];
	if ( cl )
        {
	    int left = 0;
	    if ( i == cx )
		left = dx + 1;
	    int right = KSPREAD_CLUSTER_LEVEL2 - 1;
	    for( int k = left; k <= right; ++k )
	    {
		ColumnLayout* c = cl[ k ];
		if ( c )
	        {
		    removeElement( c->column() );
		    c->setColumn( c->column() - 1 );
		    insertElement( c, c->column() );
		}
	    }
	}
    }

    setAutoDelete( a );

    return TRUE;
}

void KSpreadColumnCluster::setAutoDelete( bool a )
{
    m_autoDelete = a;
}

bool KSpreadColumnCluster::autoDelete() const
{
    return m_autoDelete;
}

/****************************************************
 *
 * KSpreadRowCluster
 *
 ****************************************************/

KSpreadRowCluster::KSpreadRowCluster()
    : m_first( 0 ), m_autoDelete( FALSE )
{
    m_cluster = (RowLayout***)malloc( KSPREAD_CLUSTER_LEVEL1 * sizeof( RowLayout** ) );

    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
	    m_cluster[ x ] = 0;
}

KSpreadRowCluster::~KSpreadRowCluster()
{
    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
    {
	RowLayout** cl = m_cluster[ x ];
	if ( cl )
        {
	    free( cl );
	    m_cluster[ x ] = 0;
	}
    }
	
    if ( m_autoDelete )
    {
	RowLayout* cell = m_first;
	while( cell )
        {
	    RowLayout* n = cell->next();
	    delete cell;
	    cell = n;
	}
    }

    free( m_cluster );
}

const RowLayout* KSpreadRowCluster::lookup( int row ) const
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
	return 0;

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    RowLayout** cl = m_cluster[ cx ];
    if ( !cl )
	return 0;

    return cl[ dx ];
}

RowLayout* KSpreadRowCluster::lookup( int row )
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
	return 0;

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    RowLayout** cl = m_cluster[ cx ];
    if ( !cl )
	return 0;

    return cl[ dx ];
}

void KSpreadRowCluster::clear()
{
    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
    {
	RowLayout** cl = m_cluster[ x ];
	if ( cl )
        {
	    free( cl );
	    m_cluster[ x ] = 0;
	}
    }

    if ( m_autoDelete )
    {
	RowLayout* cell = m_first;
	while( cell )
        {
	    RowLayout* n = cell->next();
	    delete cell;
	    cell = n;
	}
    }

    m_first = 0;
}

void KSpreadRowCluster::insertElement( RowLayout* lay, int row )
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
	return;

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    RowLayout** cl = m_cluster[ cx ];
    if ( !cl )
    {
	cl = (RowLayout**)malloc( KSPREAD_CLUSTER_LEVEL2 * sizeof( RowLayout*  ) );
	m_cluster[ cx ] = cl;
	
	for( int a = 0; a < KSPREAD_CLUSTER_LEVEL2; ++a )
	    cl[ a ] = 0;
    }

    if ( cl[ dx ] )
	removeElement( row );

    cl[ dx ] = lay;

    if ( m_first )
    {
	lay->setNext( m_first );
	m_first->setPrevious( lay );
    }
    m_first = lay;
}

void KSpreadRowCluster::removeElement( int row )
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
	return;

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    RowLayout** cl = m_cluster[ cx ];
    if ( !cl )
	return;

    RowLayout* c = cl[ dx ];
    if ( !c )
	return;

    cl[ dx ] = 0;

    if ( m_autoDelete )
    {
	if ( m_first == c )
	    m_first = c->next();
	delete c;
    }
    else
    {
	if ( m_first == c )
	    m_first = c->next();
	if ( c->previous() )
	    c->previous()->setNext( c->next() );
	if ( c->next() )
	    c->next()->setPrevious( c->previous() );
	c->setNext( 0 );
	c->setPrevious( 0 );
    }
}

bool KSpreadRowCluster::insertRow( int row )
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
	return FALSE;

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    // Is there a row layout at the right most position ?
    // In this case the shift is impossible.
    RowLayout** cl = m_cluster[ KSPREAD_CLUSTER_LEVEL1 - 1 ];
    if ( cl && cl[ KSPREAD_CLUSTER_LEVEL2 - 1 ] )
	return FALSE;

    bool a = autoDelete();
    setAutoDelete( FALSE );

    for( int i = KSPREAD_CLUSTER_LEVEL1 - 1; i >= cx ; --i )
    {
	RowLayout** cl = m_cluster[ i ];
	if ( cl )
        {
	    int left = 0;
	    if ( i == cx )
		left = dx;
	    int right = KSPREAD_CLUSTER_LEVEL2 - 1;
	    if ( i == KSPREAD_CLUSTER_LEVEL1 - 1 )
		right = KSPREAD_CLUSTER_LEVEL2 - 2;
	    for( int k = right; k >= left; --k )
	    {
		RowLayout* c = cl[ k ];
		if ( c )
	        {
		    removeElement( c->row() );
		    c->setRow( c->row() + 1 );
		    insertElement( c, c->row() );
		}
	    }
	}
    }

    setAutoDelete( a );

    return TRUE;
}

bool KSpreadRowCluster::removeRow( int row )
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
	return FALSE;

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    removeElement( row );

    bool a = autoDelete();
    setAutoDelete( FALSE );

    for( int i = cx; i < KSPREAD_CLUSTER_LEVEL1; ++i )
    {
	RowLayout** cl = m_cluster[ i ];
	if ( cl )
        {
	    int left = 0;
	    if ( i == cx )
		left = dx + 1;
	    int right = KSPREAD_CLUSTER_LEVEL2 - 1;
	    for( int k = left; k <= right; ++k )
	    {
		RowLayout* c = cl[ k ];
		if ( c )
	        {
		    removeElement( c->row() );
		    c->setRow( c->row() - 1 );
		    insertElement( c, c->row() );
		}
	    }
	}
    }

    setAutoDelete( a );

    return TRUE;
}

void KSpreadRowCluster::setAutoDelete( bool a )
{
    m_autoDelete = a;
}

bool KSpreadRowCluster::autoDelete() const
{
    return m_autoDelete;
}
