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

#ifndef KPRPAGELAYOUTSHAREDLOADINGDATA_H
#define KPRPAGELAYOUTSHAREDLOADINGDATA_H

#include <KoSharedLoadingData.h>
#include <QMap>
#include <QString>

class KPrPageLayout;

#define KPR_PAGE_LAYOUT_SHARED_LOADING_ID "KPrPageLayoutSharedLoadingId"

class KPrPageLayoutSharedLoadingData
{
public:
    KPrPageLayoutSharedLoadingData();
    virtual ~KPrPageLayoutSharedLoadingData();

    /**
     * Add page layout style name
     *
     * @param pageLayout the page layout
     * @param the styleName of the page layout style
     */
    void addPageLayout( KPrPageLayout * pageLayout, const QString & styleName );

    /**
     * Get the page layout 
     *
     * @param the presentation page layout style name
     * @return the style name for the page layout or 0 if it is not found
     */
    KPrPageLayout * pageLayout( const QString & styleName );

private:
    QMap<QString, KPrPageLayout *> m_nameToPageLayout;
};

#endif /* KPRPAGELAYOUTSHAREDLOADINGDATA_H */
