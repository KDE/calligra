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

#ifndef KPRPAGELAYOUTS_H
#define KPRPAGELAYOUTS_H

#include <QMap>
#include <QString>
#include <KoDataCenter.h>

class QRectF;
class KPrPageLayout;
class KoPALoadingContext;
class KoPASavingContext;

#define PageLayouts "PageLayouts"

class KPrPageLayouts : public KoDataCenter
{
public:
    KPrPageLayouts();
    ~KPrPageLayouts();

    /// reimplemented
    virtual bool completeLoading( KoStore *store );
    /// reimplemented
    virtual bool completeSaving( KoStore *store, KoXmlWriter * manifestWriter );

    bool saveOdf( KoPASavingContext & context );
    // loadOdf is not implemented as loading is done by pageLayout and TODO

    /**
     *
     */
    KPrPageLayout * pageLayout( const QString & name, KoPALoadingContext & loadingContext, const QRectF & pageRect );

    /**
     * load all not yet loaded styles and add application styles
     */
    // TODO

private:
    // the string is the style name associated with the page layout when the file was 
    // loaded from file
    QMap<QString, KPrPageLayout *> m_pageLayouts;
};

#endif /* KPRPAGELAYOUTS_H */
