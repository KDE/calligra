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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

/**
 * $Id$
 */

#ifndef KTABLE_H
#define KTABLE_H

#include <qvaluelist.h>
#include <qshared.h>

template<class RowT, class ColT, class CellT> class KTablePrivate : public QShared
{
 public:
  KTablePrivate() : QShared(){ row_count = 0; col_count = 0; }
  KTablePrivate( uint _rows, uint _cols ) : QShared() {
    for( uint i = 0; i < _rows; i++ )
      row_list.append( RowT() );
    for( uint j = 0; j < _cols; j++ )
      col_list.append( ColT() );
    for( uint k = 0; k < _rows * _cols; k++ )
      matrix.append( CellT() );
    col_count = _cols;
    row_count = _rows;
  }
  KTablePrivate( const KTablePrivate& _t ) : QShared() {
    col_count = _t.col_count;
    row_count = _t.row_count;
    matrix = _t.matrix;
    row_list = _t.row_list;
    col_list = _t.col_list;
  }
  ~KTablePrivate() { }

  void expand( uint _rows, uint _cols ) {
    if ( _rows > row_count ) {
      for( uint r = 0; r < _rows - row_count; ++r ) {
	row_list.append( RowT() );
	for( uint i = 0; i < col_count; ++i )
	  matrix.append( CellT() );
      }
      row_count = _rows;
    }
    if ( _cols > col_count ) {
      uint old = col_count;
      col_count = _cols;
      for( uint c = 0; c < _cols - old; ++c ) {
	col_list.append( ColT() );
	for( uint i = 0; i < row_count; ++i )
	  matrix.insert( matrix.at( i * col_count + old + c ), CellT() );
      }
    }
  }

  CellT& cell( uint _row, uint _col ) {
    ASSERT( _row < row_count && _col < col_count );
    return matrix[ (int)(_row * col_count + _col) ];
  }
  const CellT& cell( uint _row, uint _col ) const {
    ASSERT( _row < row_count && _col < col_count );
    return matrix[ (int)(_row * col_count + _col) ];
  }
  void setCell( uint _row, uint _col, const CellT& _element ) {
    ASSERT( _row < row_count && _col < col_count );
    matrix[ (int)(_row * col_count + _col) ] = _element;
  }

  RowT& row( uint _row ) {
    ASSERT( _row < row_count );
    return row_list[ _row ];
  }
  const RowT& row( uint _row ) const {
    ASSERT( _row < row_count );
    return row_list[ _row ];
  }
  void setRow( uint _row, const RowT& _v )
  {
    ASSERT( _row < row_count );
    row_list[ _row ] = _v;
  }

  ColT& col( uint _col ) {
    ASSERT( _col < col_count );
    return col_list[ _col ];
  }
  const ColT& col( uint _col ) const {
    ASSERT( _col < col_count );
    return col_list[ _col ];
  }
  void setCol( uint _col, const ColT& _v )
  {
    ASSERT( _col < col_count );
    col_list[ _col ] = _v;
  }

  void insertColumn( uint _c ) {
    ASSERT( _c <= col_count );
    ++col_count;
    typename QValueList<CellT>::Iterator it;
    for( uint i = 0; i < row_count; ++i )
    {
      it = matrix.at( i * col_count + _c );
      matrix.insert( it, CellT() );
    }

    typename QValueList<ColT>::Iterator it2 = col_list.at( _c );
    col_list.insert( it2, ColT() );
  }

  void insertRow( uint _r ) {
    ASSERT( _r <= row_count );
    ++row_count;
    typename QValueList<CellT>::Iterator it = matrix.at( _r * col_count );
    for( uint i = 0; i < col_count; ++i )
      matrix.insert( it, CellT() );

    QValueList<RowT>::Iterator it2 = row_list.at( _r );
    row_list.insert( it2, RowT() );
  }

  void removeColumn( uint _c ) {
    ASSERT( _c < col_count );
    --col_count;
    typename QValueList<CellT>::Iterator it;
    for( uint i = 0; i < row_count; ++i )
    {
      it = matrix.at( i * col_count + _c );
      matrix.remove( it );
    }

    typename QValueList<ColT>::Iterator it2 = col_list.at( _c );
    col_list.remove( it2 );
  }

  void removeRow( uint _r ) {
    ASSERT( _r < row_count );
    --row_count;
    typename QValueList<CellT>::Iterator it = matrix.at( _r * col_count );
    for( uint i = 0; i < col_count; ++i )
      it = matrix.remove( it );

    QValueList<RowT>::Iterator it2 = row_list.at( _r );
    row_list.remove( it2 );
  }

  QValueList<CellT> matrix;
  QValueList<RowT> row_list;
  QValueList<ColT> col_list;

  uint col_count;
  uint row_count;
};


template<class RowT, class ColT, class CellT> class KTable
{
private:
  typedef KTablePrivate<RowT,ColT,CellT> Priv;

public:
  /**
   * Typedefs
   */
  typedef typename QValueList<CellT>::Iterator Iterator;
  typedef typename QValueList<CellT>::ConstIterator ConstIterator;

  typedef typename QValueList<RowT>::Iterator RowIterator;
  typedef typename QValueList<RowT>::ConstIterator ConstRowIterator;

  typedef typename QValueList<ColT>::Iterator ColIterator;
  typedef typename QValueList<ColT>::ConstIterator ConstColIterator;

  /**
   * API
   */
  KTable() { sh = new Priv; }
  KTable( uint _rows, uint _cols ) { sh = new Priv( _rows, _cols ); }
  KTable( const KTable& _t ) { sh = _t.sh; sh->ref(); }
  ~KTable() { if ( sh->deref() ) delete sh; }

  KTable& operator=( const KTable& t ) { t.sh->ref(); if ( sh->deref() ) delete sh; sh = t.sh; return *this; }

  Iterator begin() { return sh->matrix.begin(); }
  ConstIterator begin() const { return sh->matrix.begin(); }
  Iterator end() { return sh->matrix.end(); }
  ConstIterator end() const { return sh->matrix.end(); }

  ColIterator colBegin() { return sh->col_list.begin(); }
  ConstColIterator colBegin() const { return sh->col_list.begin(); }
  ColIterator colEnd() { return sh->col_list.end(); }
  ConstColIterator colEnd() const { return sh->col_list.end(); }

  RowIterator rowBegin() { return sh->row_list.begin(); }
  ConstRowIterator rowBegin() const { return sh->row_list.begin(); }
  RowIterator rowEnd() { return sh->row_list.end(); }
  ConstRowIterator rowEnd() const { return sh->row_list.end(); }

  bool isEmpty() const { return ( sh->col_count == 0 && sh->row_count == 0 ); }

  uint cols() const { return sh->col_count; }
  uint rows() const { return sh->row_count; }

  CellT& cell( uint _row, uint _col ) { detach(); return sh->cell( _row, _col ); }
  const CellT& cell( uint _row, uint _col ) const { return sh->cell( _row, _col ); }
  void setCell( uint _row, uint _col, const CellT& _element ) { detach(); sh->setCell( _row, _col, _element ); }

  RowT& row( uint _row ) { detach(); return sh->row( _row ); }
  const RowT& row( uint _row ) const { return sh->row( _row ); }
  void setRow( uint _row, const RowT& _v ) { detach(); sh->setRow( _row, _v ); }

  ColT& col( uint _col ) { detach(); return sh->col( _col ); }
  const ColT& col( uint _col ) const { return sh->col( _col ); }
  void setCol( uint _col, const ColT& _v ) { detach(); sh->setCol( _col, _v ); }

  void insertColumn( uint _c ) { detach(); sh->insertColumn( _c ); }
  void insertRow( uint _r ) { detach(); sh->insertRow( _r ); }
  void removeColumn( uint _c ) { detach(); sh->removeColumn( _c ); }
  void removeRow( uint _r ) { detach(); sh->removeRow( _r ); }

  void expand( uint _rows, uint _cols ) { detach(); sh->expand( _rows, _cols ); }

private:
  /**
   * Helpers
   */
  void detach() { if ( sh->count > 1 ) { sh->deref(); sh = new Priv( *sh ); } }

  /**
   * Variables
   */
  Priv* sh;
};

#endif
