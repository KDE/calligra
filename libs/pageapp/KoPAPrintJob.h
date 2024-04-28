/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPAPRINTJOB_H
#define KOPAPRINTJOB_H

#include <KoPrintJob.h>

#include <QPrinter>

#include "kopageapp_export.h"

class KoPAView;
class KoPAPageBase;
class KoPAPageProvider;

/**
 * For now we print to the center of the page honoring the margins.
 * The page is zoomed to be as big as possible.
 */
class KOPAGEAPP_EXPORT KoPAPrintJob : public KoPrintJob
{
    Q_OBJECT
public:
    explicit KoPAPrintJob(KoPAView *view);
    ~KoPAPrintJob() override;

    QPrinter &printer() override;
    QList<QWidget *> createOptionWidgets() const override;

public Q_SLOTS:
    void startPrinting(KoPrintJob::RemovePolicy removePolicy = DoNotDelete) override;

protected:
    QPrinter m_printer;
    QList<KoPAPageBase *> m_pages;
    KoPAPageProvider *m_pageProvider;
};

#endif /* KOPAPRINTJOB_H */
