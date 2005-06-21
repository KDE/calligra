/***************************************************************************
            mcalcobject.cpp  -  Kugar report calculation field object
            -------------------
  begin     : Thu Sep 2 1999
  copyright : (C) 1999 by Mutiny Bay Software
  email     : info@mutinybaysoftware.com
***************************************************************************/

#include "mcalcobject.h"

namespace Kugar
{

/** Constructor */
MCalcObject::MCalcObject() : MFieldObject()
{
    // Set the default calculation type
    calcType = MCalcObject::Count;
}

/** Copy constructor */
MCalcObject::MCalcObject( const MCalcObject& mCalcObject ) : MFieldObject( ( MFieldObject & ) mCalcObject )
{
    copy( &mCalcObject );
}

/** Assignment operator */
MCalcObject MCalcObject::operator=( const MCalcObject& mCalcObject )
{
    if ( &mCalcObject == this )
        return * this;

    // Copy the derived class's data
    copy( &mCalcObject );

    // Copy the base class's data
    ( ( MFieldObject & ) * this ) = mCalcObject;

    return *this;
}

/** Destructor */
MCalcObject::~MCalcObject()
{}

/** Sets the field's  calculation type */
void MCalcObject::setCalculationType( int type )
{
    calcType = type;
}

/** Returns the field's calculation type */
int MCalcObject::getCalculationType()
{
    return calcType;
}

/** Copies member data from one object to another.
      Used by the copy constructor and assignment operator */
void MCalcObject::copy( const MCalcObject* mCalcObject )
{
    // Copy the fields's calculation type
    calcType = mCalcObject->calcType;
}

}
