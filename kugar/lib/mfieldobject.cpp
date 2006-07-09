/***************************************************************************
           mfieldobject.cpp  -  Kugar report field object
           -------------------
 begin     : Mon Aug 23 1999
 copyright : (C) 1999 by Mutiny Bay Software
 email     : info@mutinybaysoftware.com
***************************************************************************/

#include "mfieldobject.h"
#include "mutil.h"

#include "inputmask.h"

namespace Kugar
{

/** Constructor */
MFieldObject::MFieldObject() : MLabelObject()
{
    // Set the defaults
    fieldName = "";
    dataType = MFieldObject::String;
    format = MUtil::MDY_SLASH;
    precision = 0;
    currency = 36;
    negativeValueColor.setRgb( 255, 0, 0 );
    comma = 0;
    m_inputMask = new InputMask();
}

/** Copy constructor */
MFieldObject::MFieldObject( const MFieldObject& mFieldObject ) : MLabelObject( ( MLabelObject & ) mFieldObject )
{
    copy( &mFieldObject );
}

/** Assignment operator */
MFieldObject MFieldObject::operator=( const MFieldObject& mFieldObject )
{
    if ( &mFieldObject == this )
        return * this;

    // Copy the derived class's data
    copy( &mFieldObject );

    // Copy the base class's data
    ( ( MLabelObject & ) * this ) = mFieldObject;

    return *this;
}

/** Destructor */
MFieldObject::~MFieldObject()
{}

/** Returns the bound data field name */
QString MFieldObject::getFieldName()
{
    return fieldName;
}

/** Sets the bound data field */
void MFieldObject::setFieldName( const QString field )
{
    fieldName = field;
}

/** Sets the field's data string */
void MFieldObject::setText( const QString txt )
{
    int ret;
    QDate d;
    int pos;
    QString month, day, year;
    QRegExp regexp( "[0-9][0-9](-|//)[0-9][0-9](-|//)[0-9][0-9][0-9][0-9]" );

    // Check for empty string
    if ( txt.isEmpty() && dataType == MFieldObject::Date )
    {
        text = QString::null;
        return ;
    }

    // Set the data
    switch ( dataType )
    {
    case MFieldObject::String:
        text = txt;
        if ( !getInputMask().isEmpty() )
            text = m_inputMask->formatText( txt );
        break;
    case MFieldObject::Integer:
        text.setNum( txt.toDouble(), 'f', 0 );
        if ( comma )
            formatCommas();
        formatNegValue();
        break;
    case MFieldObject::Float:
        text.setNum( txt.toDouble(), 'f', precision );
        if ( comma )
            formatCommas();
        formatNegValue();
        break;
    case MFieldObject::Date:
        // Check if we can read the date and if so, reformat it ...

        pos = regexp.search( txt.mid( 0 ) );
        ret = regexp.matchedLength();
        //ret = regexp.match(txt);

        if ( ret != 1 )
        {
            year = txt.right( 4 );
            month = txt.left( 2 );
            day = txt.mid( 3, 2 );

            d.setYMD( year.toInt(), month.toInt(), day.toInt() );
            //        d = QDate::fromString(txt, Qt::LocalDate);
            text = MUtil::formatDate( d, format );
        }
        else
            text = txt;
        break;
    case MFieldObject::Currency:
        int prec = precision < 2 ? precision : 2;
        bool ok;
        text.setNum( txt.toDouble( &ok ), 'f', prec );
        if( !ok )
            text = QString::null;
        else
        {
            if ( comma )
                formatCommas();
            formatNegValue();
            text = QString( currency + text );
        }
        break;
    }
}

QString MFieldObject::getInputMask() const
{
    return m_inputMask->mask();
}

void MFieldObject::setInputMask( const QString &inputMask )
{
    m_inputMask->setMask( inputMask );
}

/** Sets the field's data type */
void MFieldObject::setDataType( int t )
{
    dataType = t;
    if ( dataType == MFieldObject::Integer ||
            dataType == MFieldObject::Float ||
            dataType == MFieldObject::Currency )
        saveColor = foregroundColor;
}

/** Sets the field's date formatting */
void MFieldObject::setDateFormat( int f )
{
    format = f;
}

/** Sets the field's precision */
void MFieldObject::setPrecision( int p )
{
    precision = p;
}

/** Sets the field's currency symbol */
void MFieldObject::setCurrency( const QChar c )
{
    if ( c.isNull() )
        currency = 36;
    else
        currency = c;
}

/** Sets the object's negative value color - default is red*/
void MFieldObject::setNegValueColor( int r, int g, int b )
{
    negativeValueColor.setRgb( r, g, b );
}

/** Sets if object should delimit numeric values with commas */
void MFieldObject::setCommaSeparator( int c )
{
    comma = c;
}

/** Formats a string representation of a negative number using the negative value color */
void MFieldObject::formatNegValue()
{
    if ( text.toDouble() < 0 )
        foregroundColor = negativeValueColor;
    else
        foregroundColor = saveColor;
}

/** Formats the string representation of a number with comma separators */
void MFieldObject::formatCommas()
{
    //     text = text.replace(".", ",");
    QString tmp;
    int i, j;
    int offset;

    // If a neg value
    if ( text.toDouble() < 0 )
        offset = 1;
    else
        offset = 0;

    // Look for decimal point
    int pos = text.findRev( "." );

    // If a decimal was not found, start at end of string
    if ( pos == -1 )
        pos = text.length();
    else
        tmp = text.mid( pos, text.length() );

    // Move through the string and insert the commas
    for ( i = pos - 1, j = 0; i >= offset; i-- )
    {
        tmp = text.mid( i, 1 ) + tmp;
        j++;
        if ( ( j == 3 ) && ( i - 1 >= offset ) )
        {
            tmp = tr( "," ) + tmp;
            j = 0;
        }
    }

    // If neg value, add back the sign
    if ( offset )
        tmp = "-" + tmp;

    // Set the new string to the field
    text = tmp;
}

/** Copies member data from one object to another.
      Used by the copy constructor and assignment operator */
void MFieldObject::copy( const MFieldObject* mFieldObject )
{
    // Copy the fields's data type and format
    fieldName = mFieldObject->fieldName;
    dataType = mFieldObject->dataType;
    format = mFieldObject->format;
    precision = mFieldObject->precision;
    currency = mFieldObject->currency;
    negativeValueColor = mFieldObject->negativeValueColor;
    saveColor = mFieldObject->saveColor;
    comma = mFieldObject->comma;
}

}
