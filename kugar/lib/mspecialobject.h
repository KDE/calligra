/***************************************************************************
             mspecialobject.h  -  Kugar report special field object
             -------------------
   begin     : Mon Aug 23 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MSPECIALOBJECT_H
#define MSPECIALOBJECT_H

#include <qdatetime.h>

#include "mlabelobject.h"

/**Kugar report special field object - report date, page number, etc.
  *@author Mutiny Bay Software
  */

namespace Kugar
{

class MSpecialObject : public MLabelObject
{

public:
    /** Data type constants */
    enum SpecialType { Date = 0, PageNumber };

    /** Constructor */
    MSpecialObject();
    /** Copy constructor */
    MSpecialObject( const MSpecialObject& mSpecialObject );
    /** Assignment operator */
    MSpecialObject operator=( const MSpecialObject& mSpecialObject );
    /** Destructor */
    virtual ~MSpecialObject();

protected:
    /** Field type */
    int type;
    /** Format type */
    int format;
public:
    /** Sets the field's data string with a date */
    void setText( QDate d );
    /** Sets the field's data string with a page number */
    void setText( int page );
    /** Sets the field's type */
    void setType( int t );
    /** Gets the field's type */
    int getType();
    /** Sets the field's date formatting */
    void setDateFormat( int f );

private:
    /** Copies member data from one object to another.
      * Used by the copy constructor and assignment operator
      */
    void copy( const MSpecialObject* mSpecialObject );

};

}

#endif

























