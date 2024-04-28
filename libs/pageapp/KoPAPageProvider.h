/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009-2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAPAGEPROVIDER_H
#define KOPAPAGEPROVIDER_H

#include "kopageapp_export.h"
#include <KoPageProvider.h>

class KoPAPageBase;

class KOPAGEAPP_EXPORT KoPAPageProvider : public KoPageProvider
{
public:
    KoPAPageProvider();
    ~KoPAPageProvider() override;

    KoTextPage *page(KoShape *shape) override;

    void setPageData(int pageNumber, KoPAPageBase *page);

private:
    int m_pageNumber;
    KoPAPageBase *m_page;
};

#endif /* KOPAPAGEPROVIDER_H */
