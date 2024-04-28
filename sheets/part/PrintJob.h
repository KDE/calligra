/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef PRINTJOB_H
#define PRINTJOB_H

#include <KoPrintingDialog.h>

namespace Calligra
{
namespace Sheets
{
class View;

/**
 * Manages printing on the document level.
 */
class PrintJob : public KoPrintingDialog
{
public:
    explicit PrintJob(View *view);
    ~PrintJob() override;

    int documentFirstPage() const override;
    int documentLastPage() const override;
    QAbstractPrintDialog::PrintDialogOptions printDialogOptions() const override;

public Q_SLOTS:
    void startPrinting(RemovePolicy removePolicy = DoNotDelete) override;

protected:
    QRectF preparePage(int pageNumber) override;
    void printPage(int pageNumber, QPainter &painter) override;
    QList<KoShape *> shapesOnPage(int pageNumber) override;
    QList<QWidget *> createOptionWidgets() const override;

private:
    class Private;
    Private *const d;
};

} // namespace Sheets
} // namespace Calligra

#endif
