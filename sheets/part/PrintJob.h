/* This file is part of the KDE project
 * Copyright 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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
    QList<KoShape*> shapesOnPage(int pageNumber) override;
    QList<QWidget*> createOptionWidgets() const override;

private:
    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif
