/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPAPageLayoutDialog.h"

#include "KoPADocument.h"
#include "KoPageLayout.h"

#include <KoUnit.h>

KoPAPageLayoutDialog::KoPAPageLayoutDialog(KoPADocument *document, const KoPageLayout &pageLayout, QWidget *parent)
    : KoPageLayoutDialog(parent, pageLayout)
    , m_document(document)
{
    setPageSpread(false);
    showPageSpread(false);
    setUnit(document->unit());
}

KoPAPageLayoutDialog::~KoPAPageLayoutDialog()
{
    // delete m_pageThumbnailModel;
}

bool KoPAPageLayoutDialog::applyToDocument() const
{
    return KoPageLayoutDialog::applyToDocument();
}
