/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009-2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPATEXTPAGE_H
#define KOPATEXTPAGE_H

#include <KoTextPage.h>

#include "kopageapp_export.h"

class KoPAPageBase;

class KOPAGEAPP_EXPORT KoPATextPage : public KoTextPage
{
public:
    KoPATextPage(int pageNumber, KoPAPageBase *page);

    ~KoPATextPage() override;

    int pageNumber() const override;
    int visiblePageNumber(PageSelection select = CurrentPage, int adjustment = 0) const override;

    KoPAPageBase *page() const;

    QRectF rect() const override;

private:
    int m_pageNumber;
    KoPAPageBase *m_page;
};

#endif /* KOPATEXTPAGE_H */
