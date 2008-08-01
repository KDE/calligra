/* This file is part of the KDE project
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
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

#ifndef KPRPAGE_H
#define KPRPAGE_H

#include <KoPAPage.h>

#include "KPrAnimationController.h"

class KPrDocument;
class KPrPageApplicationData;
class KPrNotes;

class KPrPage : public KoPAPage, public KPrAnimationController
{
public:
    KPrPage( KoPAMasterPage * masterPage, KPrDocument * document );
    virtual ~KPrPage();

    /**
     * Get the page data
     *
     * This method is static that you don't need to cast the page to a KPrPage first.
     * As every KPrPage needs to have a KPrPageApplicationData this call fails with a
     * assertion when it is not possible to retrieve.
     */
    static KPrPageApplicationData * pageData( KoPAPageBase * page );

    KPrNotes *pageNotes();

    void addShape( KoShape * shape );
    void removeShape( KoShape * shape );

protected:
    virtual void saveOdfPageStyleData( KoGenStyle &style, KoPASavingContext &paContext ) const;

    virtual void loadOdfPageTag( const KoXmlElement &element, KoPALoadingContext &loadingContext );

    /// reimplemented
    virtual bool saveOdfPresentationNotes(KoPASavingContext &paContext) const;

private:
    class Private;
    Private * const d;
};

#endif /* KPRPAGE_H */
