/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrConfigurePresenterViewDialog.h"

#include <QGuiApplication>
#include <QScreen>

#include <KLocalizedString>

#include "KPrDocument.h"
#include "StageDebug.h"

KPrConfigurePresenterViewDialog::KPrConfigurePresenterViewDialog(KPrDocument *document, QWidget *parent)
    : KoDialog(parent)
    , m_document(document)
{
    QWidget *widget = new QWidget(this);

    ui.setupUi(widget);

    for (auto screen : qGuiApp->screens()) {
        if (screen == qGuiApp->primaryScreen()) {
            ui.monitorComboBox->addItem(i18n("Monitor %1 (primary)", screen->name()));
        } else {
            ui.monitorComboBox->addItem(i18n("Monitor %1", screen->name()));
        }
    }

    if (qGuiApp->screens().count() <= 1)
        ui.presenterViewCheckBox->setEnabled(false);

    setMainWidget(widget);

    setCaption(i18n("Configure Presenter View"));

    ui.monitorComboBox->setCurrentIndex(document->presentationMonitor());
    ui.presenterViewCheckBox->setChecked(document->isPresenterViewEnabled());
}

int KPrConfigurePresenterViewDialog::presentationMonitor()
{
    return ui.monitorComboBox->currentIndex();
}

bool KPrConfigurePresenterViewDialog::presenterViewEnabled()
{
    return (ui.presenterViewCheckBox->checkState() == Qt::Checked);
}
