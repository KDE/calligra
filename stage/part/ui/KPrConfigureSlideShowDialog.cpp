/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrConfigureSlideShowDialog.h"

#include <KLocalizedString>

#include "KPrCustomSlideShows.h"
#include "KPrDocument.h"
#include "KPrView.h"
#include "KPrViewModeSlidesSorter.h"
#include "StageDebug.h"

KPrConfigureSlideShowDialog::KPrConfigureSlideShowDialog(KPrDocument *document, KPrView *parent)
    : KoDialog(parent)
    , m_document(document)
    , m_view(parent)
{
    QWidget *widget = new QWidget(this);

    ui.setupUi(widget);

    ui.slidesComboBox->addItem(i18n("All slides"));
    KPrCustomSlideShows *customSlideShows = document->customSlideShows();
    ui.slidesComboBox->addItems(customSlideShows->names());

    setMainWidget(widget);

    setCaption(i18n("Configure Slide Show"));

    QString activeCustomSlideShow = document->activeCustomSlideShow();
    if (activeCustomSlideShow.isEmpty()) {
        ui.slidesComboBox->setCurrentIndex(0);
    } else {
        QList<QString> customSlideShows = document->customSlideShows()->names();
        int index = customSlideShows.indexOf(activeCustomSlideShow) + 1;
        Q_ASSERT(index < ui.slidesComboBox->count());
        ui.slidesComboBox->setCurrentIndex(index);
    }

    connect(ui.editSlidesButton, &QAbstractButton::clicked, this, &KPrConfigureSlideShowDialog::editCustomSlideShow);
}

QString KPrConfigureSlideShowDialog::activeCustomSlideShow() const
{
    if (ui.slidesComboBox->currentIndex() != 0) {
        return ui.slidesComboBox->currentText();
    }

    return QString();
}

void KPrConfigureSlideShowDialog::editCustomSlideShow()
{
    m_view->slidesSorter()->setActiveCustomSlideShow(ui.slidesComboBox->currentIndex());
    m_view->showSlidesSorter();
    accept();
}
