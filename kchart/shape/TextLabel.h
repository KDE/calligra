/* This file is part of the KDE project

   Copyright 2007 Johannes Simon <johannes.simon@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KCHART_TEXTLABEL_H
#define KCHART_TEXTLABEL_H

// Local
#include "ChartShape.h"

// KOffice
#include "../karbon/plugins/simpletextshape/SimpleTextShape.h"

namespace KChart {

class TextLabel : public SimpleTextShape
{
public:
    TextLabel( ChartShape *parent );
    ~TextLabel();
    
    //QFont font();                      // provided by SimpleTextShape
    //void setFont( const QFont &font ); // provided by SimpleTextShape
    
    /**
     * @brief Returns the font size in points
     * 
     * This method is provided for convenience. All settings
     * related to the font can be retrieved and set with font()
     * and setFont(), respectively.
     */
    double fontSize() const;
    
    /**
     * @brief Sets the font size in points
     * 
     * This method is provided for convenience. All settings
     * related to the font can be retrieved and set with font()
     * and setFont(), respectively.
     */
    void setFontSize( double size );
    
    /**
     * @brief Sets the type of the label
     * 
     * A label item can either represent a chart's title,
     * subtitle or footer.
     */
    void setType( LabelType type );
    
    
    bool loadOdf( KoXmlElement &labelElement, KoShapeLoadingContext &context );
    void saveOdf( KoXmlWriter &bodyWriter, KoGenStyles &mainStyles );
    
private:
    class Private;
    Private *const d;
};

} // Namespace KChart

#endif // KCHART_TEXTLABEL_H
