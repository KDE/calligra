#include "KDChartTableDataWrapper.h"

KDChartTableDataWrapper::KDChartTableDataWrapper( KDChartTableData* data ) :QObject(0), _data(data)
{
}
/*
KDChartData* KDChartTableDataWrapper::cell( uint row, uint col )
{
    KDChartData& item = _data->cell( row, col );
    return &item;
}

void KDChartTableDataWrapper::setCell( uint _row, uint _col, double _element )
{
    // Without this, the user has to wrap the element in a KDChartData
    // In C++ this is handled bu default constructors.
    _data->setCell( _row, _col, _element );

}

void KDChartTableDataWrapper::setCell( uint _row, uint _col, QString _element )
{
    // Without this, the user has to wrap the element in a KDChartData
    // In C++ this is handled bu default constructors.
    _data->setCell( _row, _col, _element );
}

void KDChartTableDataWrapper::setCell( uint _row, uint _col, const KDChartData& _element )
{
    // When overriding setCell above, I also had to override this one. I believe that must be due to a bug in QSA
    _data->setCell( _row, _col, _element );
}

*/
