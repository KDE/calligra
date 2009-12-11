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

namespace KSpread
{
class View;
class SheetSelectPage;

/**
 * Manages printing on the document level.
 */
class PrintJob : public KoPrintingDialog
{
public:
    PrintJob(View *view);
    virtual ~PrintJob();

    virtual int documentFirstPage() const;
    virtual int documentLastPage() const;
    virtual QAbstractPrintDialog::PrintDialogOptions printDialogOptions() const;

public Q_SLOTS:
    virtual void startPrinting(RemovePolicy removePolicy = DoNotDelete);

protected:
    virtual QRectF preparePage(int pageNumber);
    virtual void printPage(int pageNumber, QPainter &painter);
    virtual QList<KoShape*> shapesOnPage(int pageNumber);
    virtual QList<QWidget*> createOptionWidgets() const;

private:
    class Private;
    Private * const d;
};

}  //KSPread namespace

#endif
