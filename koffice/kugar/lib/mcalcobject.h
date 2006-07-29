/***************************************************************************
             mcalcobject.h  -  Kugar report calculation field object
             -------------------
   begin     : Thu Sep 2 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MCALCOBJECT_H
#define MCALCOBJECT_H

#include "mfieldobject.h"

/**Kugar report calculation field object
  *@author Mutiny Bay Software
  */

namespace Kugar
{

class MCalcObject : public MFieldObject
{

public:
    /** Calculation type constants */
    enum CalculationType { Count = 0, Sum, Average, Variance, StandardDeviation};

    /** Constructor */
    MCalcObject();
    /** Copy constructor */
    MCalcObject( const MCalcObject& mCalcObject );
    /** Assignment operator */
    MCalcObject operator=( const MCalcObject& mCalcObject );
    /** Destructor */
    ~MCalcObject();

    /** Sets the field's  calculation type */
    void setCalculationType( int type );
    /** Returns the field's calculation type */
    int getCalculationType();

private:
    /** Calculation type */
    int calcType;

private:
    /** Copies member data from one object to another.
      * Used by the copy constructor and assignment operator
      */
    void copy( const MCalcObject* mCalcObject );

};

}

#endif
