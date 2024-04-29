/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRCONFIGURESLIDESHOWDIALOG_H
#define KPRCONFIGURESLIDESHOWDIALOG_H

#include <KoDialog.h>
class KPrView;

#include "ui_KPrConfigureSlideShow.h"

class KPrDocument;

class KPrConfigureSlideShowDialog : public KoDialog
{
    Q_OBJECT

public:
    explicit KPrConfigureSlideShowDialog(KPrDocument *document, KPrView *parent = nullptr);

    QString activeCustomSlideShow() const;

private Q_SLOTS:
    void editCustomSlideShow();

private:
    Ui::KPrConfigureSlideShow ui;
    KPrDocument *m_document;
    KPrView *m_view;
};

#endif // KPRCONFIGUREPRESENTERVIEWDIALOG_H
