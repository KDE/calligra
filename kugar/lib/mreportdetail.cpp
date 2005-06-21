/***************************************************************************
             mreportdetail.cpp  -  Kugar report detail section
             -------------------
   begin     : Mon Aug 23 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#include "mreportdetail.h"

namespace Kugar
{

/** Constructor */
MReportDetail::MReportDetail() : MReportSection()
{
    // Set the label list to AutoDelete
    fields.setAutoDelete( true );
}

/** Copy constructor */
MReportDetail::MReportDetail( const MReportDetail& mReportDetail ) : MReportSection( ( MReportSection & ) mReportDetail )
{
    copy( &mReportDetail );
}

/** Assignment operator */
MReportDetail MReportDetail::operator=( const MReportDetail& mReportDetail )
{
    if ( &mReportDetail == this )
        return * this;

    // Copy the derived class's data
    copy( &mReportDetail );

    // Copy the base class's data
    ( ( MReportSection & ) * this ) = mReportDetail;

    return *this;
}

/** Destructor */
MReportDetail::~MReportDetail()
{
    clear();
}

/** Frees all resources allocated by the report section */
void MReportDetail::clear()
{
    // Free the base resources
    clearBase();

    // Clear the field collection
    fields.clear();
}

void MReportDetail::setRepeat( bool b )
{
    repeat = b;
}

bool MReportDetail::getRepeat() const
{
    return repeat;
}

/** Draws the detail section to the selected painter & x/y-offsets */
void MReportDetail::draw( QPainter* p, int xoffset, int yoffset )
{
    MFieldObject * field;

    // Set the offsets
    int xcalc = xoffset;
    int ycalc = yoffset;

    // Draw the base objects
    drawObjects( p, xoffset, yoffset );

    // Draw the field collection
    for ( field = fields.first(); field != 0; field = fields.next() )
    {
        field->draw( p, xcalc, ycalc );
    }
}

/** Adds a new field object to the section's field collection */
void MReportDetail::addField( MFieldObject* field )
{
    fields.append( field );
}

/** Sets the data for the field at the specified index */
void MReportDetail::setFieldData( int idx, QString data )
{
    MFieldObject * field = fields.at( idx );

    field->setText( data );
}

/** Returns the number of fields in the detail section */
int MReportDetail::getFieldCount()
{
    return fields.count();
}

/** Returns the name of the bound field for field object at the given index */
QString MReportDetail::getFieldName( int idx )
{
    MFieldObject * field = fields.at( idx );

    return field->getFieldName();
}

/** Copies member data from one object to another.
      Used by the copy constructor and assignment operator */
void MReportDetail::copy( const MReportDetail* mReportDetail )
{
    // Copy the field list
    fields = mReportDetail->fields;
}

}
