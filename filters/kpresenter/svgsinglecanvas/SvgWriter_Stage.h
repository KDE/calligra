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

#ifndef SVG_WRITER_STAGE_H
#define SVG_WRITER_STAGE_H

#include "SvgWriter_generic.h"
#include <plugins/presentationviewportshape/PresentationViewPortShape.h>
#include <KoPathShape.h>

class Frame;
class KoShape;
class QTextStream;
class SvgWriter;
class KoShapeLayer;
class SvgAnimationData;

class SvgWriter_Stage : public SvgWriter_generic
{
public:
    /**
     * Creates svg writer to export specified layers
     */
    SvgWriter_Stage(const QList<KoShapeLayer*> &layers, const QSizeF &pageSize) ;
   
    /**
     * Creates svg writer to export specified shapes
     */
    SvgWriter_Stage(const QList<KoShape*> &toplevelShapes, const QSizeF &pageSize);

    /** 
     * Destroys the svg writer
     */
    ~SvgWriter_Stage();
    /**
     * @param shape saves app data conatined in this to a stream
     */
    void saveAppData(KoShape *shape);
    
private:
    void saveAnimationTags(PresentationViewPortShape* shape);
    /**
     * Saves javascript to the SVG doc
     */
    void saveScript();
    //Temporary function, since UI still not ready to test adding of frame data
    void forTesting(KoShape *shape);

    QTextStream* m_frames;
    QString m_script;
};  
#endif /*SVG_dWRITER_STAGE_H*/