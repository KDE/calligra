/* This file is part of the KDE project
 * Copyright (C) 2007, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
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
#ifndef KWPRINTINGDIALOG_H
#define KWPRINTINGDIALOG_H

#include <KWView.h>

#include <KoPrintingDialog.h>
#include <KoImageData.h>

#include <QMap>
#include <QRectF>

class KoShapeManager;
class KWDocument;

class KWPrintingDialog : public KoPrintingDialog
{
public:
    KWPrintingDialog(KWDocument *document, KoShapeManager *shapeManager, KWView *view);
    ~KWPrintingDialog() override;

    QList<QWidget *> createOptionWidgets() const override;

    QAbstractPrintDialog::PrintDialogOptions printDialogOptions() const override;

protected:
    QRectF preparePage(int pageNumber) override;
    QList<KoShape*> shapesOnPage(int pageNumber) override;
    void printingDone() override;
    int documentFirstPage() const override;
    int documentLastPage() const override;
    int documentCurrentPage() const override;

private:
    KWDocument *m_document;
    KWView *m_view;
};

#endif
