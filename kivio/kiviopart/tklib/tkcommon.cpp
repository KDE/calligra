#include "tkcommon.h"

float XmlReadFloat( const QDomElement &e, const QString &att, const float def)
{
    // Check if this value exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return def;

    // Read the attribute
    QString val = e.attribute( att );
    bool ok=false;

    // Make sure it is a floating point value.  If not, return the default
    float fVal = val.toFloat( &ok );
    if( !ok )
    {
	return def;
    }

    return fVal;
}


/**
 * Write a floating point value to a @ref QDomElement
 *
 * @param e The @ref QDomElement to write to
 * @param att The attribute to write
 * @param val The value of the attribute to write
 *
 * This will write a floating point value to a @ref QDomElement.
 */
void XmlWriteFloat( QDomElement &e, const QString &att, const float &val )
{
    e.setAttribute( att, (double)val );
}


/**
 * Read an int value from a @ref QDomElement.
 *
 * @param e The @ref QDomElement to read from
 * @param att The attribute to locate
 * @param def The default value to return if the attribute is not found
 *
 * This will read an int attribute from a @ref QDomElement, and
 * if it is not found, return the default value.
 */
int XmlReadInt( const QDomElement &e, const QString &att, const int def)
{
    // Check if this value exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return def;

    // Read the attribute
    QString val = e.attribute( att );
    bool ok=false;

    // Make sure it is a floating point value.  If not, return the default
    int iVal = val.toInt( &ok );
    if( !ok )
    {
	  return def;
    }

    // Return the value
    return iVal;
}


/**
 * Write a int value to a @ref QDomElement
 *
 * @param e The @ref QDomElement to write to
 * @param att The attribute to write
 * @param val The value of the attribute to write
 *
 * This will write an int value to a @ref QDomElement.
 */
void XmlWriteInt( QDomElement &e, const QString &att, const int &val )
{
    e.setAttribute( att, (int)val );
}


/**
 * Read an uint value from a @ref QDomElement.
 *
 * @param e The @ref QDomElement to read from
 * @param att The attribute to locate
 * @param def The default value to return if the attribute is not found
 *
 * This will read an uint attribute from a @ref QDomElement, and
 * if it is not found, return the default value.
 */
uint XmlReadUInt( const QDomElement &e, const QString &att, const uint def)
{
    // Check if this value exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return def;

    // Read the attribute
    QString val = e.attribute( att );
    bool ok=false;

    // Make sure it is a floating point value.  If not, return the default
    uint iVal = val.toUInt( &ok );
    if( !ok )
    {
        return def;
    }

    // Return the value
    return iVal;
}


/**
 * Write an uint value to a @ref QDomElement
 *
 * @param e The @ref QDomElement to write to
 * @param att The attribute to write
 * @param val The value of the attribute to write
 *
 * This will write an uint value to a @ref QDomElement.
 */
void XmlWriteUInt( QDomElement &e, const QString &att, const uint &val )
{
    e.setAttribute( att, (uint)val );
}


/**
 * Read a @ref QString from a @ref QDomElement.
 *
 * @param e The @ref QDomElement to read from
 * @param att The attribute to locate
 * @param def The default value to return if the attribute is not found
 *
 * This will read a QString attribute from a @ref QDomElement, and
 * if it is not found, return the default value.
 */
QString XmlReadString( const QDomElement &e, const QString &att, const QString def )
{
    // Check if the attribute exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return def;
    // Otherwise return the attribute
    return e.attribute( att );
}


/**
 * Write a QString to a @ref QDomElement
 *
 * @param e The @ref QDomElement to write to
 * @param att The attribute to write
 * @param val The value of the attribute to write
 *
 * This will write a QString to a @ref QDomElement.
 */
void XmlWriteString( QDomElement &e,  const QString &att,  const QString &val )
{
    e.setAttribute( att, val );
}


/**
 * Read a double value from a @ref QDomElement.
 *
 * @param e The @ref QDomElement to read from
 * @param att The attribute to locate
 * @param def The default value to return if the attribute is not found
 *
 * This will read a double attribute from a @ref QDomElement, and
 * if it is not found, return the default value.
 */
double XmlReadDouble( const QDomElement &e, const QString &att, const double def)
{
    // Check if this value exists, if not, return the default
    if( e.hasAttribute( att )==false )
        return def;

    // Read the attribute
    QString val = e.attribute( att );
    bool ok=false;

    // Make sure it is a floating point value.  If not, return the default
    double dVal = val.toDouble( &ok );
    if( !ok )
    {
        return def;
    }

    // Return the value
    return dVal;
}


/**
 * Write a double value to a @ref QDomElement
 *
 * @param e The @ref QDomElement to write to
 * @param att The attribute to write
 * @param val The value of the attribute to write
 *
 * This will write a double value to a @ref QDomElement.
 */
void XmlWriteDouble( QDomElement &e, const QString &att, const double &val )
{
    e.setAttribute( att, (double)val );
}
