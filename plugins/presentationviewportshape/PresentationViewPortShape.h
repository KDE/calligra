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

#ifndef PRESENTATIONVIEWPORTSHAPE_H
#define PRESENTATIONVIEWPORTSHAPE_H

#include <KoShapeContainer.h>
#include <qpainterpath.h>
#include <KoShape.h>
#include <KoPathShape.h>
#include <SvgShape.h>

#define PresentationViewPortShapeId "PresentationViewPortShape"

class KoParameterShape;

class PresentationViewPortShape : public KoShape, public SvgShape
{
public:
  /**
   * @brief Constructor
   * @brief initializes a basic PresentationViewPortShape shaped like a [ ]
   */
    
    PresentationViewPortShape();
    ~PresentationViewPortShape();
    
    /** Reimplemented methods */
    
    virtual void paint(QPainter &painter, const KoViewConverter &converter);

    virtual void saveOdf(KoShapeSavingContext &context) const;

    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

        /// reimplemented from SvgShape
    virtual bool saveSvg(SvgSavingContext &context);

    /// reimplemented from SvgShape
    virtual bool loadSvg(const KoXmlElement &element, SvgLoadingContext &context);

    virtual QPainterPath outline() const;

    virtual QSizeF size() const;

    //Methods for the frame object
    void initializeAnimationProperties(); 
    void initializeTransitionProfiles();
    /**
     * Parses frame information from a KoXmlElement,
     * And saves it into this frame.
     */
    void parseAnimationProperties(const KoXmlElement& e); 
        
    QString toString();
    void setRefId(const QString& refid);
    
    int sequence();
    QString attribute(const QString& attrName);
    int transitionProfileIndex(const QString& profile);
    
    bool setAttribute(const QString& attrName, const QString& attrValue);
    
private:     
     /**
     * @return a default path in the shape of '[ ]'
     */
     QPainterPath createShapePath(const QSizeF& size);

     QString m_ns;
     QMap<QString, QString> m_animationAttributes;
     QMap<QString, int> m_transitionProfiles;
};
#endif

    