/* This file is part of the KDE project
    * Copyright (C) 2011 Aakriti Gupta <aakriti.a.gupta@gmail.com>
    *
    * This library is free software; you can redistribute it and/or
    * modify it under the terms of the GNU Library General Public
    * License as published by the Free Software Foundation; either
    * version 2 of the License, or (at your option) any later version.
    *
    * This library is distributed in the hope that it will be useful,
    * but WITHOUT ANY WARRANTY; without even the implied warranty of
    * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    * Library General Public License for more details.
    *
    * You should have received a copy of the GNU Library General Public License
    * along with this library; see the file COPYING.LIB.  If not, write to
    * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    * Boston, MA 02110-1301, USA.
    */

#ifndef SVG_PARSER_STAGE_H
#define SVG_PARSER_STAGE_H

#include "SvgParser_generic.h"
#include "SvgAnimationData.h"

class SvgParser_generic;
class KoResourceManager;
class Frame;

/**
 * Inherits SvgParser_generic
 * Parses an SVg document in Stage.
 */
class SvgParser_Stage : public SvgParser_generic
{
public:
  
    SvgParser_Stage(KoResourceManager *documentResourceManager);
    virtual ~SvgParser_Stage();

    /**
     * @param e The XMl element with attributes for Frames
     */
    void parseAppData(const KoXmlElement& e);
    
    KoShape* createAppData(const KoXmlElement& e);
    
    /**
     * Adds a Frame object to each shape that had contained Frame data
     */
    void setAppData();
    
    
private:
   
    QList<QPointF> parsePathPoints(const KoXmlElement& e);  
    QTransform parseTransformation(const KoXmlElement& e);  
  /**
     * @brief m_frameList - 
     * Contains all the Frame objects created from the data 
     * parsed from the SVG document
     */
    QList<Frame*> m_frameList;
    QString *NS; //Namespace
};
#endif /*SVG_PARSER_STAGE_H*/
