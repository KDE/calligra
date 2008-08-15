
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

class KoXmlElement;
class KoXmlWriter;

class KPrPlaceholder
{
public:
    KPrPlaceholder();
    ~KPrPlaceholder();

    /**
     * @brief Load the presentation:placeholder
     *
     * as OO uses absolute values and in kpresenter we use relative values for the placeholders
     * we need to pass the page size in case the placeholder is saved in absolute values so it 
     * can be converted to relative ones.
     */
    bool loadOdf( const KoXmlElement &element, const QRectF & pageSize );
    void saveOdf( KoXmlWriter & xmlWriter );

    QString presentationObject();
    QRectF size( const QRectF & pageSize );

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
