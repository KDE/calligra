/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPAPAGELAYOUTDIALOG_H
#define KOPAPAGELAYOUTDIALOG_H

#include "KoPageLayoutDialog.h"

class KoPADocument;
struct KoPageLayout;

class KoPAPageLayoutDialog : public KoPageLayoutDialog
{
    Q_OBJECT
public:
    explicit KoPAPageLayoutDialog(KoPADocument *document, const KoPageLayout &pageLayout, QWidget *parent = nullptr);
    ~KoPAPageLayoutDialog() override;
    bool applyToDocument() const;

private:
    KoPADocument *m_document;
};

#endif
