/***************************************************************************
             mreportdetail.h  -  Kugar report detail section
             -------------------
   begin     : Mon Aug 23 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MREPORTDETAIL_H
#define MREPORTDETAIL_H

#include "mfieldobject.h"
#include "mreportsection.h"

/**Kugar report detail section
  *@author Mutiny Bay Software
  */

namespace Kugar
{

class MReportDetail : public MReportSection
{

public:
    /** Constructor */
    MReportDetail();
    /** Copy constructor */
    MReportDetail( const MReportDetail& mReportDetail );
    /** Assignment operator */
    MReportDetail operator=( const MReportDetail& mReportDetail );
    /** Destructor */
    virtual ~MReportDetail();

protected:
    /** Sections's repeat on new pages */
    bool repeat;
    /** Section's field collection */
    QPtrList<MFieldObject> fields;

public:
    /** Sets whether to repeat the detail on new pages */
    void setRepeat( bool b );
    /** Sets whether to repeat the detail on new pages */
    bool getRepeat() const;
    /** Draws the detail section to the specified painter & x/y-offsets */
    void draw( QPainter* p, int xoffset, int yoffset );
    /** Adds a new field object to the section's field collection */
    void addField( MFieldObject* field );
    /** Sets the data for the field at the specified index */
    void setFieldData( int idx, QString data );
    /** Returns the number of fields in the detail section */
    int getFieldCount();
    /** Returns the name of the bound field for field object at the given index */
    QString getFieldName( int idx );
    /** Frees all resources allocated by the report section */
    void clear();

private:
    /** Copies member data from one object to another.
      * Used by the copy constructor and assignment operator
      */
    void copy( const MReportDetail* mReportDetail );

};

}

#endif




















