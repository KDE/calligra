
/* This file is part of the KDE project
 * Copyright (C) 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (  at your option ) any later version.
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

#ifndef KPRPLACEHOLDER_H
#define KPRPLACEHOLDER_H

#include <QString>
#include <QRectF>

#include "KoXmlReaderForward.h"
class KoXmlWriter;

class KPrPlaceholder
{
public:
    KPrPlaceholder();
    ~KPrPlaceholder();

    /**
     * @brief Load the presentation:placeholder
     *
     * as OO uses absolute values and in stage we use relative values for the placeholders
     * we need to pass the page size in case the placeholder is saved in absolute values so it 
     * can be converted to relative ones.
     */
    bool loadOdf( const KoXmlElement &element, const QRectF & pageSize );
    void saveOdf( KoXmlWriter & xmlWriter );

    QString presentationObject() const;

    /**
     * Calculate object rect according to the page size
     *
     * @param pageSize The size of the page
     */
    QRectF rect( const QSizeF & pageSize );

    /**
     * Fix wrongly saved data from OO
     *
     * fix for wrong saved data from OO somehow they save negative values for width and height sometimes
     * It will take the values from rect to update the width or height if they are negative
     * <style:presentation-page-layout style:name="AL10T12">
     *   <presentation:placeholder presentation:object="title" svg:x="2.057cm" svg:y="1.743cm" svg:width="23.911cm" svg:height="3.507cm"/>
     *   <presentation:placeholder presentation:object="outline" svg:x="2.057cm" svg:y="5.838cm" svg:width="11.669cm" svg:height="13.23cm"/>
     *   <presentation:placeholder presentation:object="object" svg:x="14.309cm" svg:y="5.838cm" svg:width="-0.585cm" svg:height="6.311cm"/>
     *   <presentation:placeholder presentation:object="object" svg:x="14.309cm" svg:y="12.748cm" svg:width="-0.585cm" svg:height="-0.601cm"/>
     * </style:presentation-page-layout>
     * 
     */
    void fix( const QRectF & rect );

    /**
     * Check the placeholders are identical 
     */
    bool operator==( const KPrPlaceholder & other ) const;

    bool operator<( const KPrPlaceholder & other ) const;

    /**
     * Compare the position of the placeholder
     */
    static bool comparePosition( const KPrPlaceholder & p1, const KPrPlaceholder & p2 );

private:
    qreal percent( const KoXmlElement & element, const char * type, qreal absolute );

    QString m_presentationObject;
    /**
     * Describes the position on the page as a relative value to the page size
     * So the values should be between 0 and 100 might be bigger but does not make 
     * much sense.
     */
    QRectF m_relativeSize;
};

#endif /* KPRPLACEHOLDER_H */
