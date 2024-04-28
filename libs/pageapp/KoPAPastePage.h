/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAPASTEPAGE_H
#define KOPAPASTEPAGE_H

#include "KoOdfPaste.h"

#include "kopageapp_export.h"

class KoPADocument;
class KoPAPageBase;

class KOPAGEAPP_TEST_EXPORT KoPAPastePage : public KoOdfPaste
{
public:
    /**
     * Paste pages
     *
     * This uses intelligent paste of pages.
     * o When copying a page and the master page of that page already exists the
     *   master page is not created, instead the existing master page is used.
     * o When copying a page and the master page of that page does not yet exists
     *   the master page of that page is also created.
     * o When copying a master page a copy of the page is always created.
     *
     * @param doc The document in which the pages are pasted.
     * @param activePage The page after which the pages are pasted. If 0 at the
     *        pages are inserted at the beginning.
     */
    KoPAPastePage(KoPADocument *doc, KoPAPageBase *activePage);

protected:
    bool process(const KoXmlElement &body, KoOdfReadStore &odfStore) override;

    KoPADocument *m_doc;
    KoPAPageBase *m_activePage;
};

#endif /* KOPAPASTEPAGE_H */
