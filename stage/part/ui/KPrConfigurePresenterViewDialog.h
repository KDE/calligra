/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRCONFIGUREPRESENTERVIEWDIALOG_H
#define KPRCONFIGUREPRESENTERVIEWDIALOG_H

#include <KoDialog.h>

#include "ui_KPrConfigurePresenterView.h"

class KPrDocument;

class KPrConfigurePresenterViewDialog : public KoDialog
{
    Q_OBJECT
public:
    explicit KPrConfigurePresenterViewDialog(KPrDocument *document, QWidget *parent = nullptr);

    int presentationMonitor();
    bool presenterViewEnabled();

private Q_SLOTS:

private:
    Ui::KPrConfigurePresenterView ui;
    KPrDocument *m_document;
};

#endif // KPRCONFIGUREPRESENTERVIEWDIALOG_H
