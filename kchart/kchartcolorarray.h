/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHARTCOLORARRAY
#define KCHARTCOLORARRAY

#include <qvector.h>
#include <qcolor.h>

typedef QVector<QColor> _KChartColorArray;

class KChartColorArray : protected _KChartColorArray
{
public:
    KChartColorArray() :
	_KChartColorArray( 0 ) { setAutoDelete( true ); }

    void setColor( uint pos, QColor );
    QColor color( uint pos ) const;
    uint count() const { return _KChartColorArray::count(); }
};

inline void KChartColorArray::setColor( uint pos, QColor color )
{
    if( pos >= size() )
	resize( pos + 1 );

    insert( pos, new QColor( color ) );
}


inline QColor KChartColorArray::color( uint pos ) const
{
    return *( at( pos ) );
}

#endif
