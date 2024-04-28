/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KWPRINTINGDIALOG_H
#define KWPRINTINGDIALOG_H

#include <KWView.h>

#include <KoImageData.h>
#include <KoPrintingDialog.h>

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
    QList<KoShape *> shapesOnPage(int pageNumber) override;
    void printingDone() override;
    int documentFirstPage() const override;
    int documentLastPage() const override;
    int documentCurrentPage() const override;

private:
    KWDocument *m_document;
    KWView *m_view;
};

#endif
