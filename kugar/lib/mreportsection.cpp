/***************************************************************************
             mreportsection.cpp  -  Kugar report section
             -------------------
   begin     : Mon Aug 23 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
   copyright : (C) 2002 Alexander Dymo
   email     : cloudtemple@mksat.net
***************************************************************************/

#include "mreportsection.h"
#include "mutil.h"

namespace Kugar
{

/** Constructor */
MReportSection::MReportSection()
{
    // Set geometry
    height = 50;

    // Set print frequency
    frequency = MReportSection::EveryPage;

    // Set special field data
    reportDate = QDate::currentDate();
    pageNumber = 0;

    // Set the line list to AutoDelete
    lines.setAutoDelete( true );
    // Set the label list to AutoDelete
    labels.setAutoDelete( true );
    // Set the field list to AutoDelete
    fields.setAutoDelete( true );
    // Set the special field list to AutoDelete
    specialFields.setAutoDelete( true );
}

/** Copy constructor */
MReportSection::MReportSection( const MReportSection& mReportSection ) /*: QObject((QObject &) mReportSection)*/
{
    copy( &mReportSection );
}

/** Assignment operator */
MReportSection MReportSection::operator=( const MReportSection& mReportSection )
{
    if ( &mReportSection == this )
        return * this;

    // Copy the derived class's data
    copy( &mReportSection );

    // Copy the base class's data
    //((QObject &) *this) = mReportSection;

    return *this;
}

/** Destructor */
MReportSection::~MReportSection()
{
    clear();
}

/** Frees all resources allocated by the report section */
void MReportSection::clear()
{
    clearBase();
}

/** Frees base resources allocated by the report section */
void MReportSection::clearBase()
{
    // Clear the line collection
    lines.clear();
    // Clear the label collection
    labels.clear();
    // Clear the field collection
    fields.clear();
    // Clear the special field collection
    specialFields.clear();
    // Clear the calculated field collection
    calculatedFields.clear();
}

/** Adds a new line object to the section's line collection */
void MReportSection::addLine( MLineObject* line )
{
    lines.append( line );
}

/** Adds a new label object to the section's label collection */
void MReportSection::addLabel( MLabelObject* label )
{
    labels.append( label );
}

/** Adds a new field object to the section's label collection */
void MReportSection::addField( MFieldObject * field )
{
    fields.append( field );
}

/** Adds a new special field object to the section's special field collection */
void MReportSection::addSpecialField( MSpecialObject* special )
{
    specialFields.append( special );
}

/** Adds a new calculated field object to the section's calculated field collection */
void MReportSection::addCalculatedField( MCalcObject* calc )
{
    calculatedFields.append( calc );
}

/** Gets the height of the section */
int MReportSection::getHeight()
{
    return height;
}

/** Sets the height of the section */
void MReportSection::setHeight( int h )
{
    height = h;
}

/** Sets the level of the section */
void MReportSection::setLevel( int l )
{
    level = l;
}

/** Gets the level of the section */
int MReportSection::getLevel()
{
    return level;
}

/** Set the current page number - used by special fields */
void MReportSection::setPageNumber( int page )
{
    pageNumber = page;
}

/** Set the current date - used by special fields */
void MReportSection::setReportDate( QDate date )
{
    reportDate = date;
}

/** Sets the print frequency of the section */
void MReportSection::setPrintFrequency( int printFrequency )
{
    frequency = printFrequency;
}

/** Returns the print frequency of the section */
int MReportSection::printFrequency()
{
    return frequency;
}

/** Returns the name of the bound field for the calculated field object at the given index */
QString MReportSection::getCalcFieldName( int idx )
{
    MCalcObject * field = calculatedFields.at( idx );

    return field->getFieldName();
}

/** Sets the data for the specified calculated field */
void MReportSection::setCalcFieldData( int idx, QString data )
{
    MCalcObject * field = calculatedFields.at( idx );

    field->setText( data );
}

/** Sets the data for the all calculated fields - list size must number of calculated fields */
void MReportSection::setCalcFieldData( QPtrList<QMemArray<double> >* values )
{
    MCalcObject * field;
    int i = 0;

    // Calculate and set the calculated field's data
    for ( field = calculatedFields.first(); field != 0; field = calculatedFields.next() )
    {
        switch ( field->getCalculationType() )
        {
        case MCalcObject::Count:
            field->setText( QString::number( MUtil::count( values->at( i ) ) ) );
            break;
        case MCalcObject::Sum:
            field->setText( QString::number( MUtil::sum( values->at( i ) ), 'f', 2 ) );
            break;
        case MCalcObject::Average:
            field->setText( QString::number( MUtil::average( values->at( i ) ) ) );
            break;
        case MCalcObject::Variance:
            field->setText( QString::number( MUtil::variance( values->at( i ) ) ) );
            break;
        case MCalcObject::StandardDeviation:
            field->setText( QString::number( MUtil::stdDeviation( values->at( i ) ) ) );
            break;
        }
        i++;
    }
}

/** Returns the index of the calculated field object for the given bound field */
int MReportSection::getCalcFieldIndex( QString field )
{
    MCalcObject * tmpField;

    // Find the field in the calculated field collection and return the index
    for ( tmpField = calculatedFields.first(); tmpField != 0; tmpField = calculatedFields.next() )
    {
        if ( tmpField->getFieldName() == field )
            break;
    }
    return calculatedFields.at();
}

/** Returns the number of calculated fields in the section */
int MReportSection::getCalcFieldCount()
{
    return calculatedFields.count();
}

/** Draws the section to the specified painter & x/y-offsets */
void MReportSection::draw( QPainter* p, int xoffset, int yoffset )
{
    drawObjects( p, xoffset, yoffset );
}

/** Draws the section base objects to the specified painter & x/y offsets */
void MReportSection::drawObjects( QPainter* p, int xoffset, int yoffset )
{
    MLineObject * line;
    MLabelObject* label;
    MSpecialObject* special;
    MCalcObject* cfield;
    MFieldObject* field;

    // Set the offsets
    int xcalc = xoffset;
    int ycalc = yoffset;

    // Draw the line collection
    for ( line = lines.first(); line != 0; line = lines.next() )
    {
        line->draw( p, xcalc, ycalc );
    }

    // Draw the label collection
    for ( label = labels.first(); label != 0; label = labels.next() )
    {
        label->draw( p, xcalc, ycalc );
    }

    // Draw the field collection
    for ( field = fields.first(); field != 0; field = fields.next() )
    {
        field->draw( p, xcalc, ycalc );
    }

    // Draw the calculated field collection
    for ( cfield = calculatedFields.first(); cfield != 0; cfield = calculatedFields.next() )
    {
        cfield->draw( p, xcalc, ycalc );
    }

    // Draw the special field collection
    for ( special = specialFields.first(); special != 0; special = specialFields.next() )
    {
        switch ( special->getType() )
        {
        case MSpecialObject::Date:
            special->setText( reportDate );
            break;
        case MSpecialObject::PageNumber:
            special->setText( pageNumber );
            break;
        }
        special->draw( p, xcalc, ycalc );
    }
}

/** Copies member data from one object to another.
      Used by the copy constructor and assignment operator */
void MReportSection::copy( const MReportSection* mReportSection )
{
    // Copy the section's geometry
    height = mReportSection->height;

    // Copy the print frequency
    frequency = mReportSection->frequency;

    // Copy the line list
    lines = mReportSection->lines;
    // Copy the label list
    labels = mReportSection->labels;
    // Copy the field list
    fields = mReportSection->fields;
    // Copy the special field list
    specialFields = mReportSection->specialFields;
    // Copy the calculated field list
    calculatedFields = mReportSection->calculatedFields;
}

void MReportSection::setFieldData( QString name, QString data )
{
    MFieldObject * field;
    for ( field = fields.first(); field != 0; field = fields.next() )
    {
        qWarning( "    checking field %s", field->getFieldName().ascii() );
        if ( field->getFieldName() == name )
            field->setText( data );
    }
}

}

