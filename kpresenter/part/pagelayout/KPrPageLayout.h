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

#ifndef KPRPAGELAYOUT_H
#define KPRPAGELAYOUT_H

#include <QList>
#include <QString>

class QRectF;
class KPrPlaceholder;
class KoXmlElement;
class KoPALoadingContext;
class KoPASavingContext;

class KPrPageLayout
{
public:
    KPrPageLayout();
    ~KPrPageLayout();

    /**
     * Load the page layout
     *
     * @param
     * @param
     * @return 
     */
    bool loadOdf( const KoXmlElement &element, const QRectF & pageRect );

    /**
     * save the page layout to odf
     *
     * @return the style name used for this page layout
     */
    QString saveOdf( KoPASavingContext & context ) const;

private:
    // The display name of the layout
    QString m_name;
    // placeholders used in the layout
    QList<KPrPlaceholder *> m_placeholders;
};

#endif /* KPRPAGELAYOUT_H */
