/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Benjamin Port <port.benjamin@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPDFPRINTJOB_H
#define KPRPDFPRINTJOB_H

#include <KoPAPrintJob.h>

#include <QPrinter>

class KPrView;

class KPrPdfPrintJob : public KoPAPrintJob
{
    Q_OBJECT
public:
    explicit KPrPdfPrintJob(KPrView *view);
    ~KPrPdfPrintJob() override;

public Q_SLOTS:
    void startPrinting(KoPrintJob::RemovePolicy removePolicy = DoNotDelete) override;
};

#endif // KPRPDFPRINTJOB_H
