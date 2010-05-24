/* This file is part of the KDE project
 * Copyright (C) 2007-2009 Thorsten Zachmann <zachmann@kde.org>
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

#include "KPrPageData.h"
#include "KPrDeclarations.h"

#include "kpresenter_export.h"

class KoPADocument;
class KPrDocument;
class KPrPageApplicationData;
class KPrNotes;
class KPrPageLayout;
class KPrDeclarations;

class KPRESENTER_EXPORT KPrPage : public KoPAPage, public KPrPageData
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

    /**
     * Get the presentation notes for this page
     *
     * @return the presentation notes
     */
    KPrNotes *pageNotes();

    /// reimplemented
    virtual void shapeAdded( KoShape * shape );

    /// reimplemented
    virtual void shapeRemoved( KoShape * shape );

    /**
     * Set the layout to use on the page
     *
     * @param layout the layout that should be used from now. 
     *        If 0 no layout will be used.
     */
    void setLayout( KPrPageLayout * layout, KoPADocument * document  );

    /**
     * Get the layout used on the page
     *
     * @return layout that is used or 0 if no layout is used
     */
    KPrPageLayout * layout() const;

    /**
     * Get the page type used in the document
     *
     * @return KoPageApp::Slide
     */
    virtual KoPageApp::PageType pageType() const;

    QString declaration(KPrDeclarations::Type type) const;

    /// reimplemented
    virtual bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context);

protected:
    /// reimplemented
    virtual void saveOdfPageContent( KoPASavingContext & paContext ) const;

    /// reimplemented
    virtual void saveOdfPageStyleData( KoGenStyle &style, KoPASavingContext &paContext ) const;

    /// reimplemented
    virtual void loadOdfPageTag( const KoXmlElement &element, KoPALoadingContext &loadingContext );

    /// reimplemented
    virtual void loadOdfPageExtra( const KoXmlElement &element, KoPALoadingContext & loadingContext );

    /// reimplemented
    virtual bool saveOdfPresentationNotes(KoPASavingContext &paContext) const;

    /// reimplemented
    virtual KoShapeManagerPaintingStrategy * getPaintingStrategy() const;

private:
    class Private;
    Private * const d;
};

#endif /* KPRPAGE_H */
