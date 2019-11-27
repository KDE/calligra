/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
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

#ifndef KPRMASTERPAGE_H
#define KPRMASTERPAGE_H

#include <KoPAMasterPage.h>

#include "KPrPageData.h"
#include "KPrShapeAnimations.h"

class KPrMasterPage : public KoPAMasterPage, public KPrPageData
{
public:
    explicit KPrMasterPage(KPrDocument *document);
    ~KPrMasterPage() override;

    /**
     * Get the page type used in the document
     *
     * @return KoPageApp::Slide
     */
    KoPageApp::PageType pageType() const override;

    /// reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

protected:
    /// reimplemented
    void loadOdfPageExtra( const KoXmlElement &element, KoPALoadingContext & loadingContext ) override;
};

#endif /* KPRMASTERPAGE_H */
