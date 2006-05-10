/***************************************************************************
             mlabelobject.h  -  Kugar report label object
             -------------------
   begin     : Wed Aug 11 1999
   copyright : (C) 1999 by Mutiny Bay Software
   email     : info@mutinybaysoftware.com
***************************************************************************/

#ifndef MLABELOBJECT_H
#define MLABELOBJECT_H

#include <QString>
#include <qfontmetrics.h>

#include "mreportobject.h"

/**Kugar report label object
  *@author Mutiny Bay Software
  */

namespace Kugar
{

class MLabelObject : public MReportObject
{

public:
    /** Font weight constants */
    enum FontWeight { Light = 25, Normal = 50, DemiBold = 63, Bold = 75, Black = 87 };
    /** Horizontal alignment constants */
    enum HAlignment { Left = 0, Center, Right };
    /** Vertial alignment constants */
    enum VAlignment { Top = 0, Middle, Bottom };

    /** Constructor */
    MLabelObject();
    /** Copy constructor */
    MLabelObject( const MLabelObject& mLabelObject );
    /** Assignment operator */
    MLabelObject operator=( const MLabelObject& mLabelObject );
    /** Destructor */
    virtual ~MLabelObject();

protected:
    /** Label text */
    QString text;
    /** Label text font family  */
    QString fontFamily;
    /** Label text font size in points */
    int fontSize;
    /** Label text font weight */
    int fontWeight;
    /** Label text font italic flag */
    bool fontItalic;
    /** Lable text horizontal alignment */
    int hAlignment;
    /** Label text vertical alignment  */
    int vAlignment;
    /** Label text word wrap flag  */
    bool wordWrap;

private:
    /** Label text horizontal margin spacing */
    const int xMargin;
    /** Label text vertical margin spacing */
    const int yMargin;

public:
    /** Sets the label's text string - default is an empty string*/
    virtual void setText( const QString txt );
    /** Sets the label's text font - default is Times,10,Normal,false */
    void setFont( const QString family, int size, int weight, bool italic );
    /** Sets the label's horizontal alignment -default is Left */
    void setHorizontalAlignment( int a );
    /** Sets the label's vertical alignment - default is Top */
    void setVerticalAlignment( int a );
    /** Sets the label's word wrap flag - default is false */
    void setWordWrap( bool state );
    /** Draws the label using the specificed painter & x/y-offsets */
    void draw( QPainter* p, int xoffset, int yoffset );

private:
    /** Copies member data from one object to another.
      * Used by the copy constructor and assignment operator
      */
    void copy( const MLabelObject* mLabelObject );

};

}

#endif
