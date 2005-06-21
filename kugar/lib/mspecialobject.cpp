/***************************************************************************
             mspecialobject.cpp  -  Kugar report special field object
             -------------------
   begin     : Mon Aug 23 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#include "mspecialobject.h"
#include "mutil.h"

namespace Kugar
{

/** Constructor */
MSpecialObject::MSpecialObject() : MLabelObject()
{
    // Set the defaults
    type = MSpecialObject::Date;
    format = MUtil::MDY_SLASH;
}

/** Copy constructor */
MSpecialObject::MSpecialObject( const MSpecialObject& mSpecialObject ) : MLabelObject( ( MLabelObject & ) mSpecialObject )
{
    copy( &mSpecialObject );
}

/** Assignment operator */
MSpecialObject MSpecialObject::operator=( const MSpecialObject& mSpecialObject )
{
    if ( &mSpecialObject == this )
        return * this;

    // Copy the derived class's data
    copy( &mSpecialObject );

    // Copy the base class's data
    ( ( MLabelObject & ) * this ) = mSpecialObject;

    return *this;
}

/** Destructor */
MSpecialObject::~MSpecialObject()
{}

/** Sets the field's data string with a date */
void MSpecialObject::setText( QDate d )
{
    text = MUtil::formatDate( d, format );
}

/** Sets the field's data string with a page number */
void MSpecialObject::setText( int page )
{
    text.setNum( page );
}

/** Sets the field's data type */
void MSpecialObject::setType( int t )
{
    type = t;
}

/** Gets the field's type */
int MSpecialObject::getType()
{
    return type;
}

/** Sets the field's date formatting */
void MSpecialObject::setDateFormat( int f )
{
    format = f;
}

/** Copies member data from one object to another.
      Used by the copy constructor and assignment operator */
void MSpecialObject::copy( const MSpecialObject* mSpecialObject )
{
    // Copy the fields's data type and format
    type = mSpecialObject->type;
}

}
