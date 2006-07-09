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
#ifndef __KDCHARTTABLEINTERFACE_H__
#define __KDCHARTTABLEINTERFACE_H__


class QTable;

#include <qvariant.h>
#include <qobject.h>
#include <qdatetime.h>

#include <kdchart_export.h>

#if defined( SUN7 ) || defined (_SGIAPI)
  #include <float.h>
  #include <limits.h>
#else
  #include <cfloat>
  #include <climits>
#endif


/**
    Use special value KDCHART_POS_INFINITE to indicate positive infinite values.

    If your own table class (derived from KDChartTableDataBase) does
    not store your positive infinite values as KDCHART_POS_INFINITE
    please make sure to reimplement \sa isPosInfinite() accordingly.

    \sa isPosInfinite, isNegInfinite
*/
#define KDCHART_POS_INFINITE DBL_MAX

/**
    Use special value KDCHART_NEG_INFINITE to indicate negative infinite values.

    If your own table class (derived from KDChartTableDataBase) does
    not store your negative infinite values as KDCHART_NEG_INFINITE
    please make sure to reimplement \sa isNegInfinite() accordingly.

    \sa isNegInfinite, isPosInfinite
*/
#define KDCHART_NEG_INFINITE -DBL_MAX


/**
   \file KDChartTableBase.h

   \brief Provides a table class holding all data values
   that are to be used in a chart.

   By subclassing KDChartTableDataBase you may provide your own
   methods to access data stored somewhere else instead of
   using the setCell function to move them into KD Chart's cells.

   \note See the files in doc/tutorial/step07/ for a sample implementation you might want to use as starting-point for your own data handling class.
*/

///KD Chart's build-in table data for an easy way of storing data values.
class KDCHART_EXPORT KDChartTableDataBase :public QObject
{
    Q_OBJECT
public:
    /**
      Default constructor.

      Creates an empty table and sets the sorted flag to false.
      */
    KDChartTableDataBase() :
        QObject( 0 ),
        _sorted(false),
        _useUsedRows(false),
        _useUsedCols(false) {}
    /**
      Default copy constructor.

      Just initializes the QObject part of this class and copies the sorted flag.
      */
    KDChartTableDataBase( const KDChartTableDataBase& other ) :QObject(0)
    {
        _sorted      = other._sorted;
        _useUsedRows = other._useUsedRows;
        _useUsedCols = other._useUsedCols;
        _usedRows    = other._usedRows;
        _usedCols    = other._usedCols;
    }
    /**
      Default destructor.

      Does nothing, only defined to have it virtual.
      */
    virtual ~KDChartTableDataBase() {}

public slots:
    /**
      Returns the number of rows in the table.

      \note This pure-virtual function has to be implemented by
      each class derived from KDChartTableDataBase.

      \returns the number of rows in the table.

      \sa setRows, usedRows, cols
      */
    virtual uint rows() const = 0;
    /**
      Returns the number of cols in the table.

      \note This pure-virtual function has to be implemented by
      each class derived from KDChartTableDataBase.

      \returns the number of cols in the table.

      \sa setCols, usedCols, rows
      */
    virtual uint cols() const = 0;

    /**
      Stores data in a cell.

      \note This pure-virtual function has to be implemented by
      each class derived from KDChartTableDataBase.

      \param _row the row number of the cell to store the data object into.
      \param _col the column number of the cell to store the data object into.
      \param _value1 the first value to be stored, normally the Y value, possible types: int, double, QString
      QString might be used in case you want to use this cell's content for axis label
      \param _value2 the second value to be stored, normally the X value (if any), possible types: int, double, QDateTime

      \sa cellCoords, cellContent, setProp
      */
    virtual void setCell( uint _row, uint _col,
                          const QVariant& _value1,
                          const QVariant& _value2=QVariant() ) = 0;
    /**
      Specifies the property set ID for a cell.

      \note This pure-virtual function has to be implemented by
      each class derived from KDChartTableDataBase.
    
      \param _row the row number of the cell.
      \param _col the column number of the cell.
      \param _propSet the property set ID to be stored for this data cell, defaults to zero for normal data.
    
      \sa cellProp, cellContent, setCell
      */
    virtual void setProp( uint _row, uint _col,
                          int _propSet=0 ) = 0;

    /**
      Returns one of the coordinate data value(s) stored in a cell.

      \note This pure-virtual function has to be implemented by
      each class derived from KDChartTableDataBase.

      \param _row the row number of the cell to be retrieved.
      \param _col the column number of the cell to be retrieved.
      \param _value the coordinate variable to be filled by this method.
      \param coordinate the number of the coordinate to be retrieved, normally
      1 is the Y value and 2 is the X value.

      \returns TRUE if the row and col are addressing a cell in the table.

      \sa cellCoords, cellProp, cellContent, cellVal, setCell, setProp
      */
    virtual bool cellCoord( uint _row, uint _col,
                            QVariant& _value,
                            int coordinate=1 ) const = 0;

    /**
      Returns one of the coordinate data value(s) stored in a cell.

      This convenience method calls the bool cellCoord() function and returns
      the result if it returned successfully - otherwise it returns an invalid QVariant.

      \note If you \em know that a cell is containing valid double data
      you may quickly access them like this:
\verbatim const double yVal = data.cellVal( r, c    ).toDouble();
const double xVal = data.cellVal( r, c, 2 ).toDouble(); \endverbatim

      \param _row the row number of the cell to be retrieved.
      \param _col the column number of the cell to be retrieved.
      \param coordinate the number of the coordinate to be retrieved, normally
      1 is the Y value and 2 is the X value.

      \returns cell contens if the row and col are addressing a cell in the
      table, otherwise an invalid QVariant is returned.

      \sa cellCoords, cellProp, cellContent, setCell, setProp
      */
    virtual QVariant cellVal( uint _row, uint _col, int coordinate=1 ) const {
        QVariant value;
        if( cellCoord( _row, _col, value, coordinate ) )
            return value;
        else
            return QVariant();
    }

    /**
      Returns the property set ID stored in a cell.

      \note This pure-virtual function has to be implemented by
      each class derived from KDChartTableDataBase.

      \param _prop the property set ID of the cell to be retrieved.

      \returns TRUE if the row and col are addressing a cell in the table.

      \sa cellCoord, cellCoords, cellContent, setCell, setProp
      */
    virtual bool cellProp( uint _row, uint _col,
                           int& _prop ) const = 0;

    /**
      Increases the number of rows (and/or columns, resp.) stored in this table.

      \note This pure-virtual function has to be implemented by
      each class derived from KDChartTableDataBase.

      \note The old content of the table must be preserved (e.g. by copying
      the data into the cells of the new table).

      \param _row the new number of rows.
      \param _col the new number of columns.

      \sa cell
      */
    virtual void expand( uint _rows, uint _cols ) = 0;



    //  E N D   O F   pure-virtual function declarations



    /**
      \note To improve runtime speed this virtual function
      may be reimplemented by classes derived from KDChartTableDataBase.

      \sa cellCoords, cellProp, setCell, setProp
      */
    virtual bool cellContent( uint _row, uint _col,
                              QVariant& _value1,
                              QVariant& _value2,
                              int&      _prop ) const
    {
        return cellCoords(_row,_col, _value1,_value2) &&
               cellProp(_row,_col, _prop);
    }
    /**
      \note To improve runtime speed this virtual function
      may be reimplemented by classes derived from KDChartTableDataBase.

      \sa cellCoord, cellProp, cellContent, setCell, setProp
      */
    virtual bool cellCoords( uint _row, uint _col,
                             QVariant& _value1,
                             QVariant& _value2 ) const
    {
        return cellCoord(_row,_col, _value1, 1) &&
               cellCoord(_row,_col, _value2, 2);
    }


    /**
      Sets the number of rows in the table that actually contain data.

      \note You might want to re-implement this function in derived classes,
      the default implementation just stores the new number of used rows.

      \param _rows the number of rows in the table that actually contain data.
    
      \sa usedRows, rows, cols
      */
    virtual void setUsedRows( uint _rows );
    /**
      Returns the number of rows in the table that actually contain data.
    
      \returns the number of rows in the table that actually contain data.

      \sa setUsedRows, rows, cols
      */
    virtual uint usedRows() const;

    /**
      Sets the number of cols in the table that actually contain data.

      \note You might want to re-implement this function in derived classes,
      the default implementation just stores the new number of used cols.

      \param _cols the number of cols in the table that actually contain data.

      \sa usedCols, rows, cols
      */
    virtual void setUsedCols( uint _cols );
    /**
      Returns the number of cols in the table that actually contain data.

      \returns the number of cols in the table that actually contain data.

      \sa setUsedCols, rows, cols
      */
    virtual uint usedCols() const;


    /**
      Returns the number of cols the table has been scrolled by.

      Default implementation of this always returns zero, so make sure
      to return the appropriate value if your class derived from KDChartTableDataBase
      is supporting internal data scrolling technics.

      See the KD Chart Programmers Manual for details described in the
      Data Scrolling chapter.

      \returns the number of cols the table has been scrolled by.

      \sa cols, rows
      */
    virtual uint colsScrolledBy() const
    {
        return 0;
    }


    /**
      Specified whether the table is sorted.

      \sa sorted
    */
    virtual void setSorted(bool sorted);
    /**
      Returns whether the table is sorted.

      \sa setSorted
    */
    virtual bool sorted() const;
    
    /**
        Returns true if the given value represents a positive infinite value.
        
        \note This virtual function may be implemented by
        classes derived from KDChartTableDataBase.
        This should be done if your data are not stored as
        special value KDCHART_POS_INFINITE
        to indicate positive infinite values

        \returns TRUE if the value given is a positive infinite value.
    */
    virtual bool isPosInfinite( double value ) const
    {
        return value == KDCHART_POS_INFINITE;
    }
    
    /**
        Returns true if the given value represents a negative infinite value.
        
        \note This virtual function may be implemented by
        classes derived from KDChartTableDataBase.
        This should be done if your data are not stored as
        special value KDCHART_NEG_INFINITE
        to indicate negative infinite values
        
        \returns TRUE if the value given is a negative infinite value.
    */
    virtual bool isNegInfinite( double value ) const
    {
        return value == KDCHART_NEG_INFINITE;
    }

    /**
        Returns true if the given value represents a normal double value.

        Normal double values are defined as values that are neither positive infinite
        nor negative infinite.  This method is provided to let derived classed use
        their own way to determine when a double value is to be threated as normal.

        \note To improve runtime speed this virtual function
        may be reimplemented by classes derived from KDChartTableDataBase.

        \returns TRUE if the value given is neither positive infinite nor negativr infinite.
    */
    virtual bool isNormalDouble( double value ) const
    {
        return !isPosInfinite( value ) && !isNegInfinite( value );
    }

    /**
        Returns true if the given QVariant value represents a normal double value.

        This method tests if \c value has type QVariant::Double: if no, it returns false;
        if yes, it sets \c dVal accordingly and calls the virtual method
        isNormalDouble( double value ).

        \param value The QVariant value to be tested and converted.
        \param dVal Points to the double variable to be filled with the converted value.

        \returns TRUE if the value given is neither positive infinite nor negative
        infinite, \c value is set to the converted value if the type of \c value
        is QVariant::Double, otherwise it is not modified.
    */
    bool isNormalDouble( QVariant value, double& dVal ) const
    {
        if( QVariant::Double != value.type() )
            return false;
        dVal = value.toDouble();
        return isNormalDouble( dVal );
    }

    virtual void importFromQTable( QTable* table );

    virtual double maxValue( int coordinate=1 ) const;
    virtual double minValue( int coordinate=1, bool bOnlyGTZero=false ) const;

    virtual QDateTime maxDtValue( int coordinate=1 ) const;
    virtual QDateTime minDtValue( int coordinate=1 ) const;

    virtual double maxColSum( int coordinate=1 ) const;
    virtual double minColSum( int coordinate=1 ) const;
    virtual double maxColSum( uint row, uint row2, int coordinate=1 ) const;
    virtual double minColSum( uint row, uint row2, int coordinate=1 ) const;
    virtual double colSum( uint col, int coordinate=1 ) const;
    virtual double colAbsSum( uint col, int coordinate=1 ) const;
    virtual double maxRowSum( int coordinate=1 ) const;
    virtual double minRowSum( int coordinate=1 ) const;
    virtual double rowSum( uint row, int coordinate=1 ) const;
    virtual double rowAbsSum( uint row, int coordinate=1 ) const;
    virtual double maxInColumn( uint col, int coordinate=1 ) const;
    virtual double minInColumn( uint col, int coordinate=1 ) const;
    virtual double maxInRow( uint row, int coordinate=1 ) const;
    virtual double minInRow( uint row, int coordinate=1 ) const;
    virtual double maxInRows( uint row, uint row2, int coordinate=1 ) const;
    virtual double minInRows( uint row, uint row2, int coordinate=1, bool bOnlyGTZero=false ) const;
    virtual QDateTime maxDtInRows( uint row, uint row2, int coordinate=1 ) const;
    virtual QDateTime minDtInRows( uint row, uint row2, int coordinate=1 ) const;
    virtual uint lastPositiveCellInColumn( uint col, int coordinate=1 ) const;
    virtual bool cellsHaveSeveralCoordinates(QVariant::Type* type2Ref) const;
    virtual bool cellsHaveSeveralCoordinates(uint row1=0, uint row2=UINT_MAX,
                                             QVariant::Type* type2Ref=NULL) const;
    virtual QVariant::Type cellsValueType( uint row1, uint row2=UINT_MAX,
                                           int coordinate=1 ) const;
    virtual QVariant::Type cellsValueType( int coordinate=1 ) const;
private:
    bool _sorted;
protected:
    bool _useUsedRows, _useUsedCols;
private:
    uint _usedRows, _usedCols;
};

#endif
