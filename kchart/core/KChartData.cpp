/* $Id$ */

#include "KChartData.h"
#ifdef INCLUDE_MOC_BULLSHIT
#include "KChartData.moc"
#endif

#include <values.h>
#include <math.h> // MAXDOUBLE

#define INITIALDATASETSIZE 20

KChartData::KChartData( int datasets ) :
  _datasets( datasets ),
  _maxpos( 0 )
{
  // Create one data set for the x values
  xdata = new KChartXDataSet( INITIALDATASETSIZE );
  xdata->setAutoDelete( true );
  for( int i = 0; i < INITIALDATASETSIZE; i++ )
	xdata->insert( i, 0 );

  // For each Y data set, create one set with the data and one set
  // that shows the validity of each entry
  ydatasets = new KChartYDataSet[ datasets ]( INITIALDATASETSIZE );
  ydatasetmasks = new QBitArray[ datasets ]( INITIALDATASETSIZE );
  for( int i = 0; i < datasets; i++ )
	ydatasetmasks[ i ].fill( false );
}


KChartData::~KChartData()
{
  delete[] ydatasetmasks;
  delete[] ydatasets;

  // Free the xdata set. No need to delete the individual items
  // (auto delete).
  delete xdata;

}

void KChartData::setXValue( uint pos, const char* value )
{
  // Resize the array if necessary.
  uint oldsize = xdata->size();
  if( oldsize <= pos ) {
	xdata->resize( pos+1 );
	for( uint i = oldsize; i < pos+1; i++ )
	  xdata->insert( i, 0 );
  }

  // Make a private copy of the string.
  QString* string = new QString( value );
  string->detach();

  xdata->insert( pos, string );

  // notify interested parties
  emit dataChanged();
}

QString KChartData::xValue( uint pos ) const
{
  if( xdata->size() <= pos )
	// too small
	return "";
  else
	return *(xdata->at( pos ));
}

void KChartData::setYValue( uint dataset, uint pos, double value )
{
  // Resize the arrays if necessary
  if( ydatasets[ dataset ].size() <= pos )
	ydatasets[ dataset ].resize( pos+1 );
  uint oldsize = ydatasetmasks[ dataset ].size();
  if( oldsize <= pos ) {
	ydatasetmasks[ dataset ].resize( pos+1 );
	for( uint i = oldsize; i < pos+1; i++ )
	  ydatasetmasks[ dataset ].clearBit( i );
  }

  // Insert the value
  //  ydatasets[ dataset ].at( pos ) = value;
  ydatasets[ dataset ][ pos ] = value;
  // And mark it as valid
  ydatasetmasks[ dataset ].setBit( pos, true );

  // We have values until here
  if( pos > _maxpos )
	_maxpos = pos;

  // notify interested parties
  emit dataChanged();
}

double KChartData::yValue( uint dataset, uint pos ) const
{
  // First check if the value is valid.
  if( ydatasetmasks[ dataset ].testBit( pos ) ) {
	double ret = ydatasets[ dataset ][ pos ];
	return ret;
  }
  else
	// Not valid - return an "almost bogus" value
	return MAXDOUBLE;
}


bool KChartData::hasYValue( uint dataset, uint pos ) const
{
  return ydatasetmasks[ dataset ].testBit( pos );
}


double KChartData::maxYValue( uint dataset ) const
{
  double max = yValue( dataset, 0 );
  for( uint i = 1; i <= _maxpos; i++ )
	if( yValue( dataset, i ) > max )
	  max = yValue( dataset, i );

  return max;
}


double KChartData::minYValue( uint dataset ) const
{
  double min = yValue( dataset, 0 );
  for( uint i = 1; i <= _maxpos; i++ )
	if( yValue( dataset, i ) < min )
	  min = yValue( dataset, i );

  return min;
}


void KChartData::minMaxOverallYValue( double& max, double& min ) const
{
  min = yValue( 0, 0 );
  max = yValue( 0, 0 );

  for( uint i = 0; i <= _maxpos; i++ )
	for( int j = 0; j < _datasets; j++ ) {
	  if( yValue( j, i ) > max )
		max = yValue( j, i );
	  if( yValue( j, i ) < min )
		min = yValue( j, i );
	}
}
