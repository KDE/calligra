/* $Id */

#include "KChartColorArray.h"

void KChartColorArray::setColor( uint pos, QColor color )
{
  if( pos >= size() )
	resize( pos + 1 );

  insert( pos, new QColor( color ) );
}


QColor KChartColorArray::color( uint pos ) const
{
  return *( at( pos ) );
}
