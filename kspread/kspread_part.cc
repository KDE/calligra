/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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

#include "kspread_part.h"
#include "kspread_gui.h"
#include <iostream.h>

/*****************************************************************************
 *
 * KXcPart
 *
 *****************************************************************************/

KSpreadPart::KKSpreadPart( Shell *_shell ) : Part_impl( _shell )
{
    pKSpread = new KKSpread( this );
    pGui = new KSpreadView( this, pKKSpread );
    setTopLevelWidget( pGui );
    pGui->show();
}

KSpreadPart::~KKSpreadPart()
{
}

CORBA::Boolean KSpreadPart::save( const char *_url, CORBA::Boolean _append )
{
    StoreDevice dev( _append, _url );
    
    Store store( &dev );
    if ( store.ioDevice() == 0L || !store.ioDevice()->isOpen() )
    {
	warning( "Could not open store with URL '%s'\n",_url );
	return false;
    }

    return writeToStore( store );
}

CORBA::Boolean KSpreadPart::load( const char *_url, CORBA::Long _offset, CORBA::Long _size )
{
    StoreDevice dev( _url, _offset, _size );
    Store store( &dev );
    if ( store.ioDevice() == 0L || !store.ioDevice()->isOpen() )
    {
	warning( "Could not open store with URL '%s'\n",_url );
	return false;
    }
    OBJECT root = store.getRootObject();
    return readFromStore( store, root );
}

OBJECT KSpreadPart::writeToStore(Store &store )
{
    TYPE t_KSpreadPart = store.registerType( "KDE:KKSpreadPart:KXclPart" );
    PROPERTY p_parts = store.registerProperty( "KDE:KSpreadPart:Parts" );
    
    OBJECT root = store.newObject( t_KSpreadPart );
    store.setRootObject( root );
    store.setAuthor( "Torben Weis" );

    OBJECT obj = writePartsToStore( store );
    store.writeObjectReferenceValue( root, p_parts, obj );
    store.release();
    
    return true;
}

bool KSpreadPart::readFromStore (Store &store, OBJECT _obj )
{
    TYPE t_KSpreadPart;
    if ( ( t_KSpreadPart = store.findType( "KDE:KKSpreadPart:KXclPart" ) ) == 0 )
    {
	warning("Could not find type KDE:KSpreadPart:KKSpreadPart\n");
	return false;
    }
    PROPERTY p_parts;
    if ( ( p_parts = store.findProperty( "KDE:KSpreadPart:Parts" ) ) == 0 )
    {
	warning("Could not find type KDE:KSpreadPart:Parts\n");
	return false;
    }

    QString author = store.getAuthor();
    printf("Author = '%s'\n",author.data());
    
    OBJECT obj;
    if ( !store.readObjectReferenceValue( _obj, p_parts, obj ) )
	fatal( "Could not find parts value\n" );
    if ( !readPartsFromStore( pGui, store, obj ) )
	fatal( "Could not read list\n");

    store.release();
    
    return true;
}

/*****************************************************************************
 *
 * KSpread
 *
 *****************************************************************************/

PartFrame* KSpread::createPart( const char *_part_name, const QRect &_geometry, QWidget *_parent )
{
  return pPart->createPart( _part_name, _geometry, _parent );
}

/*****************************************************************************
 *
 * KSpreadTable
 *
 *****************************************************************************/

ChartCellBinding* KSpreadTable::createChartCellBinding( PartFrame *_frame, const QRect &_rect )
{
  CORBA::Any *a;
  a = _frame->part()->api( "IDL:Chart:1.0" );

  CHART::Chart_ptr ptr;
  
  if ( *a >>= ptr )
  {
    printf("Chart_ptr = %x\n",ptr);
    printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    CHART::Chart_var v;
    v = CHART::Chart::_duplicate( ptr );
    
    CHART::Matrix matrix;
     matrix.columns = 3;
    matrix.rows = 3;
    double arr[] = { 1.0, 2.0, 3.0, 1.5, 2.5, 3.5, 6, 5, 4 };
    matrix.matrix.length( 9 );
    int i;
    for ( i = 0; i < 9; i++ )
      matrix.matrix[i] = arr[i];
    matrix.columnDescription.length( 3 );
    const char* arr2[] = { "A","B","C" };
    for ( i = 0; i < 3; i++ )
      matrix.columnDescription[i] = arr2[i];
    matrix.rowDescription.length( 3 );
    const char* arr3[] = { "X","Y","Z" };
    for ( i = 0; i < 3; i++ )
      matrix.rowDescription[i] = arr3[i];
    CHART::Range range;
    range.top = 1;
    range.left = 1;
    range.right = 3;
    range.bottom = 3;
    ChartCallback_impl* cb = new ChartCallback_impl( this );
    printf("Doing the filling now\n");
    
    // ptr->fill( range, matrix, CHART::Callback::_duplicate( cb ) );
    ptr->fill_dummy( range, matrix );
    
    printf("Back\n");
    
    return new ChartCellBinding( this, _rect, ptr );
  }
  else
    fatal( "Could not find interface\n");
  
  return 0L;
}

/*****************************************************************************
 *
 * ChartCellBinding
 *
 *****************************************************************************/

ChartCellBinding::ChartCellBinding( KSpreadTable *_table, const QRect &_rect, CHART::Chart_ptr _chart ) :
    CellBinding( _table, _rect.left(), _rect.top(), _rect.right(), _rect.bottom() )
{
    pChart = _chart;
}

void ChartCellBinding::cellChanged( Object* )
{
    if ( bIgnoreChanges )
	return;
    
    // chartPart->update();
}

#include "kspread_part.moc"
