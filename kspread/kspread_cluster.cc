/* This file is part of the KDE project
   Copyright (C) 2000 Torben Weis <weis@kde.org>

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

#include <stdlib.h>

#include <kdebug.h>

#include "kspread_cluster.h"
#include "kspread_cell.h"

/****************************************************
 *
 * KSpreadCluster
 *
 ****************************************************/

/* Generate a matrix LEVEL1 with the size LEVEL1*LEVEL1 */
KSpreadCluster::KSpreadCluster()
    : m_first( 0 ), m_autoDelete( FALSE )
{
    m_cluster = (KSpreadCell***)malloc( KSPREAD_CLUSTER_LEVEL1 * KSPREAD_CLUSTER_LEVEL1 * sizeof( KSpreadCell** ) );

    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
	for( int y = 0; y < KSPREAD_CLUSTER_LEVEL1; ++y )
	    m_cluster[ y * KSPREAD_CLUSTER_LEVEL1 + x ] = 0;
}

/* Delete the matrix LEVEL1 and all existing LEVEL2 matrizes */
KSpreadCluster::~KSpreadCluster()
{
// Can't we use clear(), to remove double code - Philipp?
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

KSpreadCell* KSpreadCluster::lookup( int x, int y ) const
{
    if ( x >= KSPREAD_CLUSTER_MAX || x < 0 || y >= KSPREAD_CLUSTER_MAX || y < 0 )
    {
	kdDebug(36001) << "KSpreadCluster::lookup: invalid column or row value (col: "
		       << x << "  | row: " << y << ")" << endl;
	return 0;
    }
    int cx = x / KSPREAD_CLUSTER_LEVEL2;
    int cy = y / KSPREAD_CLUSTER_LEVEL2;
    int dx = x % KSPREAD_CLUSTER_LEVEL2;
    int dy = y % KSPREAD_CLUSTER_LEVEL2;

    KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + cx ];
    if ( !cl )
	return 0;

    return cl[ dy * KSPREAD_CLUSTER_LEVEL2 + dx ];
}

/* Paste a cell in LEVEL2 (it's more paste than insert) */
void KSpreadCluster::insert( KSpreadCell* cell, int x, int y )
{
    if ( x >= KSPREAD_CLUSTER_MAX || x < 0 || y >= KSPREAD_CLUSTER_MAX || y < 0 )
    {
	kdDebug(36001) << "KSpreadCluster::insert: invalid column or row value (col: "
		       << x << "  | row: " << y << ")" << endl;
	return;
    }

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

/* Removes the cell of a matrix, the matrix itself keeps unchanged */
void KSpreadCluster::remove( int x, int y )
{
    if ( x >= KSPREAD_CLUSTER_MAX || x < 0 || y >= KSPREAD_CLUSTER_MAX || y < 0 )
    {
	kdDebug(36001) << "KSpreadCluster::remove: invalid column or row value (col: "
		       << x << "  | row: " << y << ")" << endl;
	return;
    }

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

KSpreadCell* KSpreadCluster::firstCell() const
{
    return m_first;
}

bool KSpreadCluster::shiftRow( const QPoint& marker, bool& work )
{
    work = FALSE;

    if ( marker.x() >= KSPREAD_CLUSTER_MAX || marker.x() < 0 ||
	 marker.y() >= KSPREAD_CLUSTER_MAX || marker.y() < 0 )
    {
	kdDebug(36001) << "KSpreadCluster::shiftRow: invalid column or row value (col: "
		       << marker.x() << "  | row: " << marker.y() << ")" << endl;
	return FALSE;
    }

    int cx = marker.x() / KSPREAD_CLUSTER_LEVEL2;
    int cy = marker.y() / KSPREAD_CLUSTER_LEVEL2;
    int dx = marker.x() % KSPREAD_CLUSTER_LEVEL2;
    int dy = marker.y() % KSPREAD_CLUSTER_LEVEL2;

    // Is there a cell at the bottom most position ?
    // In this case the shift is impossible.
    KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + KSPREAD_CLUSTER_LEVEL1 - 1 ];
    if ( cl && cl[ dy * KSPREAD_CLUSTER_LEVEL2 + KSPREAD_CLUSTER_LEVEL2 - 1 ] )
	return FALSE;

    bool a = autoDelete();
    setAutoDelete( FALSE );

    // Move cells in this row one down.
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
    {
	kdDebug(36001) << "KSpreadCluster::shiftColumn: invalid column or row value (col: "
		       << marker.x() << "  | row: " << marker.y() << ")" << endl;
	return FALSE;
    }

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

    // Move cells in this column one right.
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
    {
	kdDebug(36001) << "KSpreadCluster::insertColumn: invalid column value (col: "
		       << col << ")" << endl;
	return FALSE;
    }

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
    {
	kdDebug(36001) << "KSpreadCluster::insertRow: invalid row value (row: "
		       << row << ")" << endl;
	return FALSE;
    }

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
    {
	kdDebug(36001) << "KSpreadCluster::unshiftColumn: invalid column or row value (col: "
		       << marker.x() << "  | row: " << marker.y() << ")" << endl;
	return;
    }

    int cx = marker.x() / KSPREAD_CLUSTER_LEVEL2;
    int cy = marker.y() / KSPREAD_CLUSTER_LEVEL2;
    int dx = marker.x() % KSPREAD_CLUSTER_LEVEL2;
    int dy = marker.y() % KSPREAD_CLUSTER_LEVEL2;

    bool a = autoDelete();
    setAutoDelete( FALSE );

    // Move cells in this column one column to the left.
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
    {
	kdDebug(36001) << "KSpreadCluster::unshiftRow: invalid column or row value (col: "
		       << marker.x() << "  | row: " << marker.y() << ")" << endl;
	return;
    }

    int cx = marker.x() / KSPREAD_CLUSTER_LEVEL2;
    int cy = marker.y() / KSPREAD_CLUSTER_LEVEL2;
    int dx = marker.x() % KSPREAD_CLUSTER_LEVEL2;
    int dy = marker.y() % KSPREAD_CLUSTER_LEVEL2;

    bool a = autoDelete();
    setAutoDelete( FALSE );

    // Move cells in this row one row up.
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
    {
	kdDebug(36001) << "KSpreadCluster::removeColumn: invalid column value (col: "
		       << col << ")" << endl;
	return;
    }

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    for( int y1 = 0; y1 < KSPREAD_CLUSTER_LEVEL1; ++y1 )
    {
	KSpreadCell** cl = m_cluster[ y1 * KSPREAD_CLUSTER_LEVEL1 + cx ];
	if ( cl )
	    for( int y2 = 0; y2 < KSPREAD_CLUSTER_LEVEL2; ++y2 )
		if ( cl[ y2 * KSPREAD_CLUSTER_LEVEL2 + dx ] )
		    remove( col, y1 * KSPREAD_CLUSTER_LEVEL1 + y2 );
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
    {
	kdDebug(36001) << "KSpreadCluster::removeRow: invalid row value (row: "
		       << row << ")" << endl;
	return;
    }

    int cy = row / KSPREAD_CLUSTER_LEVEL2;
    int dy = row % KSPREAD_CLUSTER_LEVEL2;

    for( int x1 = 0; x1 < KSPREAD_CLUSTER_LEVEL1; ++x1 )
    {
	KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + x1 ];
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
  {
    kdDebug(36001) << "KSpreadCluster::clearColumn: invalid column value (col: "
    << col << ")" << endl;
    return;
  }

  int cx = col / KSPREAD_CLUSTER_LEVEL2;
  int dx = col % KSPREAD_CLUSTER_LEVEL2;

  for( int cy = 0; cy < KSPREAD_CLUSTER_LEVEL1; ++cy )
  {
    KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + cx ];
    if ( cl )
      for( int dy = 0; dy < KSPREAD_CLUSTER_LEVEL2; ++dy )
        if ( cl[ dy * KSPREAD_CLUSTER_LEVEL2 + dx ] )
        {
          int row = cy * KSPREAD_CLUSTER_LEVEL2 + dy ;
          remove( col, row );
        }
  }
}

void KSpreadCluster::clearRow( int row )
{
  if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
  {
    kdDebug(36001) << "KSpreadCluster::clearRow: invalid row value (row: "
        << row << ")" << endl;
    return;
  }

  int cy = row / KSPREAD_CLUSTER_LEVEL2;
  int dy = row % KSPREAD_CLUSTER_LEVEL2;

  for( int cx = 0; cx < KSPREAD_CLUSTER_LEVEL1; ++cx )
  {
    KSpreadCell** cl = m_cluster[ cy * KSPREAD_CLUSTER_LEVEL2 + cx ];
    if ( cl )
      for( int dx = 0; dx < KSPREAD_CLUSTER_LEVEL2; ++dx )
        if ( cl[ dy * KSPREAD_CLUSTER_LEVEL2 + dx ] )
        {
          int column = cx * KSPREAD_CLUSTER_LEVEL2 + dx ;
          remove( column, row );
        }
  }
}


KSpreadCell* KSpreadCluster::getFirstCellColumn(int col) const
{
  KSpreadCell* cell = lookup(col, 1);

  if (cell == NULL)
  {
    cell = getNextCellDown(col, 1);
  }
  return cell;
}

KSpreadCell* KSpreadCluster::getLastCellColumn(int col) const
{
  KSpreadCell* cell = lookup(col, KS_rowMax);

  if (cell == NULL)
  {
    cell = getNextCellUp(col, KS_rowMax);
  }
  return cell;
}

KSpreadCell* KSpreadCluster::getFirstCellRow(int row) const
{
  KSpreadCell* cell = lookup(1, row);

  if (cell == NULL)
  {
    cell = getNextCellRight(1, row);
  }
  return cell;
}

KSpreadCell* KSpreadCluster::getLastCellRow(int row) const
{
  KSpreadCell* cell = lookup(KS_colMax, row);

  if (cell == NULL)
  {
    cell = getNextCellLeft(KS_colMax, row);
  }
  return cell;
}

KSpreadCell* KSpreadCluster::getNextCellUp(int col, int row) const
{
  int cx = col / KSPREAD_CLUSTER_LEVEL2;
  int cy = (row - 1) / KSPREAD_CLUSTER_LEVEL2;
  int dx = col % KSPREAD_CLUSTER_LEVEL2;
  int dy = (row - 1) % KSPREAD_CLUSTER_LEVEL2;

  while (cy >= 0)
  {
    if ( m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + cx ] != NULL )
    {
      while (dy >= 0)
      {

        if ( m_cluster[ cy*KSPREAD_CLUSTER_LEVEL1 + cx]
             [ dy*KSPREAD_CLUSTER_LEVEL2 + dx] != NULL )
        {
          return m_cluster[ cy*KSPREAD_CLUSTER_LEVEL1 + cx ]
            [ dy*KSPREAD_CLUSTER_LEVEL2 + dx];
        }
        dy--;
      }
    }
    cy--;
    dy = KSPREAD_CLUSTER_LEVEL2 - 1;
  }
  return NULL;
}

KSpreadCell* KSpreadCluster::getNextCellDown(int col, int row) const
{
  int cx = col / KSPREAD_CLUSTER_LEVEL2;
  int cy = (row + 1) / KSPREAD_CLUSTER_LEVEL2;
  int dx = col % KSPREAD_CLUSTER_LEVEL2;
  int dy = (row + 1) % KSPREAD_CLUSTER_LEVEL2;

  while (cy < KSPREAD_CLUSTER_LEVEL1)
  {
    if ( m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + cx ] != NULL )
    {
      while (dy < KSPREAD_CLUSTER_LEVEL2)
      {

        if ( m_cluster[ cy*KSPREAD_CLUSTER_LEVEL1 + cx]
             [ dy*KSPREAD_CLUSTER_LEVEL2 + dx] != NULL )
        {
          return m_cluster[ cy*KSPREAD_CLUSTER_LEVEL1 + cx ]
            [ dy*KSPREAD_CLUSTER_LEVEL2 + dx];
        }
        dy++;
      }
    }
    cy++;
    dy = 0;
  }
  return NULL;
}

KSpreadCell* KSpreadCluster::getNextCellLeft(int col, int row) const
{
  int cx = (col - 1) / KSPREAD_CLUSTER_LEVEL2;
  int cy = row / KSPREAD_CLUSTER_LEVEL2;
  int dx = (col - 1) % KSPREAD_CLUSTER_LEVEL2;
  int dy = row % KSPREAD_CLUSTER_LEVEL2;

  while (cx >= 0)
  {
    if ( m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + cx ] != NULL )
    {
      while (dx >= 0)
      {

        if ( m_cluster[ cy*KSPREAD_CLUSTER_LEVEL1 + cx]
             [ dy*KSPREAD_CLUSTER_LEVEL2 + dx] != NULL )
        {
          return m_cluster[ cy*KSPREAD_CLUSTER_LEVEL1 + cx ]
            [ dy*KSPREAD_CLUSTER_LEVEL2 + dx];
        }
        dx--;
      }
    }
    cx--;
    dx = KSPREAD_CLUSTER_LEVEL2 - 1;
  }
  return NULL;
}

KSpreadCell* KSpreadCluster::getNextCellRight(int col, int row) const
{
  int cx = (col + 1) / KSPREAD_CLUSTER_LEVEL2;
  int cy = row / KSPREAD_CLUSTER_LEVEL2;
  int dx = (col + 1) % KSPREAD_CLUSTER_LEVEL2;
  int dy = row % KSPREAD_CLUSTER_LEVEL2;

  while (cx < KSPREAD_CLUSTER_LEVEL1)
  {
    if ( m_cluster[ cy * KSPREAD_CLUSTER_LEVEL1 + cx ] != NULL )
    {
      while (dx < KSPREAD_CLUSTER_LEVEL2)
      {

        if ( m_cluster[ cy*KSPREAD_CLUSTER_LEVEL1 + cx]
             [ dy*KSPREAD_CLUSTER_LEVEL2 + dx] != NULL )
        {
          return m_cluster[ cy*KSPREAD_CLUSTER_LEVEL1 + cx ]
            [ dy*KSPREAD_CLUSTER_LEVEL2 + dx];
        }
        dx++;
      }
    }
    cx++;
    dx = 0;
  }
  return NULL;
}

/****************************************************
 *
 * KSpreadColumnCluster
 *
 ****************************************************/

KSpreadColumnCluster::KSpreadColumnCluster()
    : m_first( 0 ), m_autoDelete( FALSE )
{
    m_cluster = (ColumnFormat***)malloc( KSPREAD_CLUSTER_LEVEL1 * sizeof( ColumnFormat** ) );

    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
	m_cluster[ x ] = 0;
}

KSpreadColumnCluster::~KSpreadColumnCluster()
{
    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
    {
	ColumnFormat** cl = m_cluster[ x ];
	if ( cl )
        {
	    free( cl );
	    m_cluster[ x ] = 0;
	}
    }

    if ( m_autoDelete )
    {
	ColumnFormat* cell = m_first;
	while( cell )
        {
	    ColumnFormat* n = cell->next();
	    delete cell;
	    cell = n;
	}
    }


    free( m_cluster );
}

ColumnFormat* KSpreadColumnCluster::lookup( int col )
{
    if ( col >= KSPREAD_CLUSTER_MAX || col < 0 )
    {
	kdDebug(36001) << "KSpreadColumnCluster::lookup: invalid column value (col: "
		       << col << ")" << endl;
	return 0;
    }

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    ColumnFormat** cl = m_cluster[ cx ];
    if ( !cl )
	return 0;

    return cl[ dx ];
}

const ColumnFormat* KSpreadColumnCluster::lookup( int col ) const
{
    if ( col >= KSPREAD_CLUSTER_MAX || col < 0 )
    {
	kdDebug(36001) << "KSpreadColumnCluster::lookup: invalid column value (col: "
		       << col << ")" << endl;
	return 0;
    }

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    ColumnFormat** cl = m_cluster[ cx ];
    if ( !cl )
	return 0;

    return cl[ dx ];
}

void KSpreadColumnCluster::clear()
{
    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
    {
	ColumnFormat** cl = m_cluster[ x ];
	if ( cl )
        {
	    free( cl );
	    m_cluster[ x ] = 0;
	}
    }

    if ( m_autoDelete )
    {
	ColumnFormat* cell = m_first;
	while( cell )
        {
	    ColumnFormat* n = cell->next();
	    delete cell;
	    cell = n;
	}
    }

    m_first = 0;
}

void KSpreadColumnCluster::insertElement( ColumnFormat* lay, int col )
{
    if ( col >= KSPREAD_CLUSTER_MAX || col < 0 )
    {
	kdDebug(36001) << "KSpreadColumnCluster::insertElement: invalid column value (col: "
		       << col << ")" << endl;
	return;
    }

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    ColumnFormat** cl = m_cluster[ cx ];
    if ( !cl )
    {
	cl = (ColumnFormat**)malloc( KSPREAD_CLUSTER_LEVEL2 * sizeof( ColumnFormat*  ) );
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
    {
	kdDebug(36001) << "KSpreadColumnCluster::removeElement: invalid column value (col: "
		       << col << ")" << endl;
	return;
    }

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    ColumnFormat** cl = m_cluster[ cx ];
    if ( !cl )
	return;

    ColumnFormat* c = cl[ dx ];
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
    {
	kdDebug(36001) << "KSpreadColumnCluster::insertColumn: invalid column value (col: "
		       << col << ")" << endl;
	return FALSE;
    }

    int cx = col / KSPREAD_CLUSTER_LEVEL2;
    int dx = col % KSPREAD_CLUSTER_LEVEL2;

    // Is there a column layout at the right most position ?
    // In this case the shift is impossible.
    ColumnFormat** cl = m_cluster[ KSPREAD_CLUSTER_LEVEL1 - 1 ];
    if ( cl && cl[ KSPREAD_CLUSTER_LEVEL2 - 1 ] )
	return FALSE;

    bool a = autoDelete();
    setAutoDelete( FALSE );

    for( int i = KSPREAD_CLUSTER_LEVEL1 - 1; i >= cx ; --i )
    {
	ColumnFormat** cl = m_cluster[ i ];
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
		ColumnFormat* c = cl[ k ];
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
    {
	kdDebug(36001) << "KSpreadColumnCluster::removeColumn: invalid column value (col: "
		       << column << ")" << endl;
	return FALSE;
    }

    int cx = column / KSPREAD_CLUSTER_LEVEL2;
    int dx = column % KSPREAD_CLUSTER_LEVEL2;

    removeElement( column );

    bool a = autoDelete();
    setAutoDelete( FALSE );

    for( int i = cx; i < KSPREAD_CLUSTER_LEVEL1; ++i )
    {
	ColumnFormat** cl = m_cluster[ i ];
	if ( cl )
        {
	    int left = 0;
	    if ( i == cx )
		left = dx + 1;
	    int right = KSPREAD_CLUSTER_LEVEL2 - 1;
	    for( int k = left; k <= right; ++k )
	    {
		ColumnFormat* c = cl[ k ];
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
    m_cluster = (RowFormat***)malloc( KSPREAD_CLUSTER_LEVEL1 * sizeof( RowFormat** ) );

    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
	    m_cluster[ x ] = 0;
}

KSpreadRowCluster::~KSpreadRowCluster()
{
    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
    {
	RowFormat** cl = m_cluster[ x ];
	if ( cl )
        {
	    free( cl );
	    m_cluster[ x ] = 0;
	}
    }

    if ( m_autoDelete )
    {
	RowFormat* cell = m_first;
	while( cell )
        {
	    RowFormat* n = cell->next();
	    delete cell;
	    cell = n;
	}
    }

    free( m_cluster );
}

const RowFormat* KSpreadRowCluster::lookup( int row ) const
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
    {
	kdDebug(36001) << "KSpreadRowCluster::lookup: invalid row value (row: "
		       << row << ")" << endl;
	return 0;
    }

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    RowFormat** cl = m_cluster[ cx ];
    if ( !cl )
	return 0;

    return cl[ dx ];
}

RowFormat* KSpreadRowCluster::lookup( int row )
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
    {
	kdDebug(36001) << "KSpreadRowCluster::lookup: invalid row value (row: "
		       << row << ")" << endl;
	return 0;
    }

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    RowFormat** cl = m_cluster[ cx ];
    if ( !cl )
	return 0;

    return cl[ dx ];
}

void KSpreadRowCluster::clear()
{
    for( int x = 0; x < KSPREAD_CLUSTER_LEVEL1; ++x )
    {
	RowFormat** cl = m_cluster[ x ];
	if ( cl )
        {
	    free( cl );
	    m_cluster[ x ] = 0;
	}
    }

    if ( m_autoDelete )
    {
	RowFormat* cell = m_first;
	while( cell )
        {
	    RowFormat* n = cell->next();
	    delete cell;
	    cell = n;
	}
    }

    m_first = 0;
}

void KSpreadRowCluster::insertElement( RowFormat* lay, int row )
{
    if ( row >= KSPREAD_CLUSTER_MAX || row < 0 )
    {
	kdDebug(36001) << "KSpreadRowCluster::insertElement: invalid row value (row: "
		       << row << ")" << endl;
	return;
    }

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    RowFormat** cl = m_cluster[ cx ];
    if ( !cl )
    {
	cl = (RowFormat**)malloc( KSPREAD_CLUSTER_LEVEL2 * sizeof( RowFormat*  ) );
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
    {
	kdDebug(36001) << "KSpreadRowCluster::removeElement: invalid row value (row: "
		       << row << ")" << endl;
	return;
    }

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    RowFormat** cl = m_cluster[ cx ];
    if ( !cl )
	return;

    RowFormat* c = cl[ dx ];
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
    {
	kdDebug(36001) << "KSpreadRowCluster::insertRow: invalid row value (row: "
		       << row << ")" << endl;
	return FALSE;
    }

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    // Is there a row layout at the bottom most position ?
    // In this case the shift is impossible.
    RowFormat** cl = m_cluster[ KSPREAD_CLUSTER_LEVEL1 - 1 ];
    if ( cl && cl[ KSPREAD_CLUSTER_LEVEL2 - 1 ] )
	return FALSE;

    bool a = autoDelete();
    setAutoDelete( FALSE );

    for( int i = KSPREAD_CLUSTER_LEVEL1 - 1; i >= cx ; --i )
    {
	RowFormat** cl = m_cluster[ i ];
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
		RowFormat* c = cl[ k ];
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
    {
	kdDebug(36001) << "KSpreadRowCluster::removeRow: invalid row value (row: "
		       << row << ")" << endl;
	return FALSE;
    }

    int cx = row / KSPREAD_CLUSTER_LEVEL2;
    int dx = row % KSPREAD_CLUSTER_LEVEL2;

    removeElement( row );

    bool a = autoDelete();
    setAutoDelete( FALSE );

    for( int i = cx; i < KSPREAD_CLUSTER_LEVEL1; ++i )
    {
	RowFormat** cl = m_cluster[ i ];
	if ( cl )
        {
	    int left = 0;
	    if ( i == cx )
		left = dx + 1;
	    int right = KSPREAD_CLUSTER_LEVEL2 - 1;
	    for( int k = left; k <= right; ++k )
	    {
		RowFormat* c = cl[ k ];
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
