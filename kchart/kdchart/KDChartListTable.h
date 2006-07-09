/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDChart licenses may use this file in
 ** accordance with the KDChart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#ifndef __KDCHARTLISTTABLE_H__
#define __KDCHARTLISTTABLE_H__

#include <qvaluelist.h>
#include <qshared.h>
#include <qtable.h>

#include <KDChartDataIntern.h>
#include <KDChartTableBase.h>

class KDCHART_EXPORT KDChartListTablePrivate : public QShared
{
    public:
        KDChartListTablePrivate() : QShared() {
            row_count = 0;
            col_count = 0;
        }

        KDChartListTablePrivate( uint _rows, uint _cols ) : QShared() {
            for ( uint i = 0; i < _rows; i++ )
                row_list.append( int() );
            for ( uint j = 0; j < _cols; j++ )
                col_list.append( int() );
            for ( uint k = 0; k < _rows * _cols; k++ )
                matrix.append( KDChartData() );
            col_count = _cols;
            row_count = _rows;
        }

        KDChartListTablePrivate( const KDChartListTablePrivate& _t ) :
            QShared(),
        matrix( _t.matrix ),
        row_list( _t.row_list ),
        col_list( _t.col_list ),
        col_count( _t.col_count ),
        row_count( _t.row_count ) {}
        ~KDChartListTablePrivate() {}

        void expand( uint _rows, uint _cols ) {
            if ( _rows > row_count ) {
                for ( uint r = 0; r < _rows - row_count; ++r ) {
                    row_list.append( int() );
                    for ( uint i = 0; i < col_count; ++i )
                        matrix.append( KDChartData() );
                }
                row_count = _rows;
            }
            if ( _cols > col_count ) {
                uint old = col_count;
                col_count = _cols;
                for ( uint c = 0; c < _cols - old; ++c ) {
                    col_list.append( int() );
                    for ( uint i = 0; i < row_count; ++i )
                        matrix.insert( matrix.at( i * col_count + old + c ), KDChartData() );
                }
            }
        }

        const KDChartData& cell( uint _row, uint _col ) const
        {
            Q_ASSERT( _row < row_count && _col < col_count );
            return matrix[ static_cast < int > ( _row * col_count + _col ) ];
        }
        KDChartData& cell( uint _row, uint _col )
        {
            Q_ASSERT( _row < row_count && _col < col_count );
            return matrix[ static_cast < int > ( _row * col_count + _col ) ];
        }
        
        void setCell( uint _row, uint _col, const KDChartData& _element )
        {
            Q_ASSERT( _row < row_count && _col < col_count );
            matrix[ static_cast < int > ( _row * col_count + _col ) ].setAll( _element );
        }

        void clearCell( uint _row, uint _col ) {
            Q_ASSERT( _row < row_count && _col < col_count );
            matrix[ static_cast < int > ( _row * col_count + _col ) ].clearValue();
        }

        void clearAllCells() {
            for ( uint r = 0; r < row_count; ++r )
                for ( uint c = 0; c < col_count; ++c )
                    matrix[ static_cast < int > ( r * col_count + c ) ].clearValue();
        }

        int& row( uint _row ) {
            Q_ASSERT( _row < row_count );
            return row_list[ _row ];
        }

        const int& row( uint _row ) const {
            Q_ASSERT( _row < row_count );
            return row_list[ _row ];
        }

        void setRow( uint _row, const int& _v ) {
            Q_ASSERT( _row < row_count );
            row_list[ _row ] = _v;
        }

        int& col( uint _col ) {
            Q_ASSERT( _col < col_count );
            return col_list[ _col ];
        }


        const int& col( uint _col ) const {
            Q_ASSERT( _col < col_count );
            return col_list[ _col ];
        }

        void setCol( uint _col, const int& _v ) {
            Q_ASSERT( _col < col_count );
            col_list[ _col ] = _v;
        }

        void insertColumn( uint _c ) {
            Q_ASSERT( _c <= col_count );
            ++col_count;
            QValueList < KDChartData > ::Iterator it;
            for ( uint i = 0; i < row_count; ++i ) {
                it = matrix.at( i * col_count + _c );
                matrix.insert( it, KDChartData() );
            }

            QValueList < int > ::Iterator it2 = col_list.at( _c );
            col_list.insert( it2, int() );
        }

        void insertRow( uint _r ) {
            Q_ASSERT( _r <= row_count );
            ++row_count;
            QValueList < KDChartData > ::Iterator it = matrix.at( _r * col_count );
            for ( uint i = 0; i < col_count; ++i )
                matrix.insert( it, KDChartData() );

            QValueList < int > ::Iterator it2 = row_list.at( _r );
            row_list.insert( it2, int() );
        }

        void removeColumn( uint _c ) {
            Q_ASSERT( _c < col_count );
            --col_count;
            QValueList < KDChartData > ::Iterator it;
            for ( uint i = 0; i < row_count; ++i ) {
                it = matrix.at( i * col_count + _c );
                matrix.remove( it );
            }

            QValueList < int > ::Iterator it2 = col_list.at( _c );
            col_list.remove( it2 );
        }

        void removeRow( uint _r ) {
            Q_ASSERT( _r < row_count );
            --row_count;
            QValueList < KDChartData > ::Iterator it = matrix.at( _r * col_count );
            for ( uint i = 0; i < col_count; ++i )
                it = matrix.remove( it );

            QValueList < int > ::Iterator it2 = row_list.at( _r );
            row_list.remove( it2 );
        }

        QValueList < KDChartData > matrix;
        QValueList < int > row_list;
        QValueList < int > col_list;

        uint col_count;
        uint row_count;
};


class KDChartListTableData : public KDChartTableDataBase
{
    private:
        typedef KDChartListTablePrivate Priv;
        uint _usedRows, _usedCols;

    public:
        /**
         * Typedefs
         */
        typedef QValueList < KDChartData > ::Iterator Iterator;
        typedef QValueList < KDChartData > ::ConstIterator ConstIterator;

        typedef QValueList < int > ::Iterator RowIterator;
        typedef QValueList < int > ::ConstIterator ConstRowIterator;

        typedef QValueList < int > ::Iterator ColIterator;
        typedef QValueList < int > ::ConstIterator ConstColIterator;

        /**
         * API
         */
        KDChartListTableData() :
            KDChartTableDataBase()
            {
                sh = new Priv;
                _usedCols = 0;
                _usedRows = 0;
            }
        KDChartListTableData( uint _rows, uint _cols ) :
            KDChartTableDataBase()
            {
                sh = new Priv( _rows, _cols );
                _usedRows = _rows;
                _usedCols = _cols;
            }

        KDChartListTableData( const KDChartListTableData& _t ) :
            KDChartTableDataBase( _t ) {
                _useUsedRows = _t._useUsedRows;
                _useUsedCols = _t._useUsedCols;
                _usedRows = _t._usedRows;
                _usedCols = _t._usedCols;
                sh = _t.sh;
                sh->ref();
                setSorted( _t.sorted() );
            }

        virtual ~KDChartListTableData() {
            if ( sh->deref() )
                delete sh;
        }

        KDChartListTableData& operator=( const KDChartListTableData& t ) {
            if ( &t == this )
                return * this;
            _useUsedRows = t._useUsedRows;
            _useUsedCols = t._useUsedCols;
            _usedRows = t._usedRows;
            _usedCols = t._usedCols;
            t.sh->ref();
            if ( sh->deref() )
                delete sh;
            sh = t.sh;
            setSorted( t.sorted() );
            return *this;
        }

        Iterator begin() {
            return sh->matrix.begin();
        }

        ConstIterator begin() const {
            return sh->matrix.begin();
        }

        Iterator end() {
            return sh->matrix.end();
        }

        ConstIterator end() const {
            return sh->matrix.end();
        }

        ColIterator colBegin() {
            return sh->col_list.begin();
        }

        ConstColIterator colBegin() const {
            return sh->col_list.begin();
        }

        ColIterator colEnd() {
            return sh->col_list.end();
        }

        ConstColIterator colEnd() const
        {
            return sh->col_list.end();
        }

        RowIterator rowBegin() {
            return sh->row_list.begin();
        }

        ConstRowIterator rowBegin() const {
            return sh->row_list.begin();
        }

        RowIterator rowEnd() {
            return sh->row_list.end();
        }

        ConstRowIterator rowEnd() const {
            return sh->row_list.end();
        }

        bool isEmpty() const {
            return ( sh->col_count == 0 && sh->row_count == 0 );
        }

        uint cols() const {
            return sh->col_count;
        }

        uint rows() const {
            return sh->row_count;
        }

        virtual bool cellCoord( uint _row, uint _col,
                                QVariant& _value,
                                int coordinate=1 ) const
        {
            if( _row >= sh->row_count || _col >= sh->col_count )
                return false;
            _value = sh->cell( _row, _col ).value( coordinate );
            return true;
        }

        virtual bool cellProp( uint _row, uint _col,
                               int& _prop ) const
        {
            if( _row >= sh->row_count || _col >= sh->col_count )
                return false;
            _prop = sh->cell( _row, _col ).propertySet();
            return true;
        }

        virtual void setCell( uint _row, uint _col,
                              const QVariant& _value1,
                              const QVariant& _value2=QVariant() )
        {
            detach();
            const KDChartData element( _value1, _value2 );
            sh->setCell( _row, _col, element );
        }

        virtual void setProp( uint _row, uint _col,
                              int _propSet=0 )
        {
            sh->cell( _row, _col ).setPropertySet( _propSet );
        }
        
        void clearCell( uint _row, uint _col ) {
            detach();
            sh->clearCell( _row, _col );
        }

        void clearAllCells() {
            detach();
            sh->clearAllCells();
        }

        int& row( uint _row ) {
            detach();
            return sh->row( _row );
        }

        const int& row( uint _row ) const {
            return sh->row( _row );
        }

        void setRow( uint _row, const int& _v ) {
            detach();
            sh->setRow( _row, _v );
        }

        int& col( uint _col ) {
            detach();
            return sh->col( _col );
        }

        const int& col( uint _col ) const {
            return sh->col( _col );
        }

        void setCol( uint _col, const int& _v ) {
            detach();
            sh->setCol( _col, _v );
        }

        void insertColumn( uint _c ) {
            detach();
            sh->insertColumn( _c );
            ++_usedCols;
        }

        void insertRow( uint _r ) {
            detach();
            sh->insertRow( _r );
            ++_usedRows;
        }

        void removeColumn( uint _c ) {
            detach();
            sh->removeColumn( _c );
            if( _usedCols  )
                --_usedCols;
        }

        void removeRow( uint _r ) {
            detach();
            sh->removeRow( _r );
            if( _usedRows  )
                --_usedRows;
        }

        void expand( uint _rows, uint _cols ) {
            detach();
            sh->expand( _rows, _cols );
            _usedRows = _rows;
            _usedCols = _cols;
        }

        void setUsedRows( uint _rows ) {
            Q_ASSERT( _rows <= rows() );
            if( _usedRows < _rows )
                setSorted( false );
            _usedRows = _rows;
            _useUsedRows = true;
        }

        uint usedRows() const {
            return _useUsedRows ? _usedRows : rows();
        }

        void setUsedCols( uint _cols ) {
            Q_ASSERT( _cols <= cols() );
            if( _usedCols < _cols )
                setSorted( false );
            _usedCols = _cols;
            _useUsedCols = true;
        }

        uint usedCols() const {
            return _useUsedCols ? _usedCols : cols();
        }

    private:
        /**
         * Helpers
         */
        void detach() {
            if ( sh->count > 1 ) {
                sh->deref();
                sh = new Priv( *sh );
            }
            setSorted( false );
        }

        /**
         * Variables
         */
        Priv* sh;
};

#endif
// __KDCHARTLISTTABLE_H__

