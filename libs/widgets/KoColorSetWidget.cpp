/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007, 2012 C. Boemann <cbo@boemann.dk>
   SPDX-FileCopyrightText: 2007-2008 Fredy Yanardi <fyanardi@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KoColorSetWidget.h"
#include "KoColorSetWidget_p.h"

#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPointer>
#include <QScrollArea>
#include <QSize>
#include <QTimer>
#include <QToolButton>
#include <QWidgetAction>

#include <KLocalizedString>
#include <KSharedConfig>

#include <KoColorPatch.h>
#include <KoColorSet.h>
#include <KoColorSpaceRegistry.h>
#include <KoEditColorSetDialog.h>
#include <KoResourceServer.h>
#include <KoResourceServerProvider.h>

void KoColorSetWidget::KoColorSetWidgetPrivate::fillColors()
{
    delete colorSetContainer;
    colorSetContainer = new QWidget();
    colorSetLayout = new QGridLayout();
    colorSetLayout->setContentsMargins(3, 3, 3, 3);
    colorSetLayout->setSpacing(0); // otherwise the use can click where there is none
    colorSetContainer->setBackgroundRole(QPalette::Dark);
    for (int i = 0; i < 16; i++) {
        colorSetLayout->setColumnMinimumWidth(i, 12);
    }
    colorSetContainer->setLayout(colorSetLayout);

    if (colorSet) {
        for (int i = 0, p = 0; i < colorSet->nColors(); i++) {
            KoColorPatch *patch = new KoColorPatch(colorSetContainer);
            patch->setFrameStyle(QFrame::Plain | QFrame::Box);
            patch->setLineWidth(1);
            patch->setColor(colorSet->getColor(i).color);
            connect(patch, &KoColorPatch::triggered, thePublic, [this](KoColorPatch *patch) {
                thePublic->d->colorTriggered(patch);
            });
            colorSetLayout->addWidget(patch, p / 16, p % 16);
            ++p;
        }
    }

    scrollArea->setWidget(colorSetContainer);
}

void KoColorSetWidget::KoColorSetWidgetPrivate::addRemoveColors()
{
    KoResourceServer<KoColorSet> *srv = KoResourceServerProvider::instance()->paletteServer();
    QList<KoColorSet *> palettes = srv->resources();

    Q_ASSERT(colorSet);
    KoEditColorSetDialog *dlg = new KoEditColorSetDialog(palettes, colorSet->name(), thePublic);
    if (dlg->exec() == KoDialog::Accepted) { // always reload the color set
        KoColorSet *cs = dlg->activeColorSet();
        // check if the selected colorset is predefined
        if (cs && !palettes.contains(cs)) {
            int i = 1;
            QFileInfo fileInfo;
            QString savePath = srv->saveLocation();

            do {
                fileInfo.setFile(savePath + QString("%1.gpl").arg(i++, 4, 10, QChar('0')));
            } while (fileInfo.exists());

            cs->setFilename(fileInfo.filePath());
            cs->setValid(true);

            // add new colorset to predefined colorsets
            if (!srv->addResource(cs)) {
                delete cs;
                cs = nullptr;
            }
        }
        if (cs) {
            thePublic->setColorSet(cs);
        }
    }
    delete dlg;
}

void KoColorSetWidget::KoColorSetWidgetPrivate::addRecent(const KoColor &color)
{
    if (numRecents < 6) {
        recentPatches[numRecents] = new KoColorPatch(thePublic);
        recentPatches[numRecents]->setFrameShape(QFrame::Box);
        recentsLayout->insertWidget(numRecents + 1, recentPatches[numRecents]);
        connect(recentPatches[numRecents], &KoColorPatch::triggered, thePublic, [this](KoColorPatch *patch) {
            thePublic->d->colorTriggered(patch);
        });
        numRecents++;
    }
    // shift colors to the right
    for (int i = numRecents - 1; i > 0; i--) {
        recentPatches[i]->setColor(recentPatches[i - 1]->color());
    }

    // Finally set the recent color
    recentPatches[0]->setColor(color);
}

void KoColorSetWidget::KoColorSetWidgetPrivate::activateRecent(int i)
{
    KoColor color = recentPatches[i]->color();

    while (i > 0) {
        recentPatches[i]->setColor(recentPatches[i - 1]->color());
        i--;
    }
    recentPatches[0]->setColor(color);
}

KoColorSetWidget::KoColorSetWidget(QWidget *parent)
    : QFrame(parent)
    , d(new KoColorSetWidgetPrivate())
{
    d->thePublic = this;
    d->colorSet = nullptr;

    d->firstShowOfContainer = true;

    d->mainLayout = new QVBoxLayout();
    d->mainLayout->setContentsMargins(4, 4, 4, 4);
    d->mainLayout->setSpacing(2);

    d->colorSetContainer = nullptr;

    d->numRecents = 0;
    d->recentsLayout = new QHBoxLayout();
    d->mainLayout->addLayout(d->recentsLayout);
    d->recentsLayout->setContentsMargins({});
    d->recentsLayout->addWidget(new QLabel(i18n("Recent:")));
    d->recentsLayout->addStretch(1);

    KoColor color(KoColorSpaceRegistry::instance()->rgb8());
    color.fromQColor(QColor(128, 0, 0));
    d->addRecent(color);

    d->scrollArea = new QScrollArea();
    d->scrollArea->setBackgroundRole(QPalette::Dark);
    d->mainLayout->addWidget(d->scrollArea);
    d->fillColors();

    d->addRemoveButton = new QToolButton(this);
    d->addRemoveButton->setText(i18n("Add / Remove Colors..."));
    d->addRemoveButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(d->addRemoveButton, &QToolButton::clicked, this, [this]() {
        d->addRemoveColors();
    });
    d->mainLayout->addWidget(d->addRemoveButton);

    setLayout(d->mainLayout);

    KoColorSet *colorSet = new KoColorSet();
    d->colorSet = colorSet;
    d->fillColors();
}

KoColorSetWidget::~KoColorSetWidget()
{
    KoResourceServer<KoColorSet> *srv = KoResourceServerProvider::instance()->paletteServer();
    QList<KoColorSet *> palettes = srv->resources();
    if (!palettes.contains(d->colorSet)) {
        delete d->colorSet;
    }
    delete d;
}

void KoColorSetWidget::KoColorSetWidgetPrivate::colorTriggered(KoColorPatch *patch)
{
    int i;

    Q_EMIT thePublic->colorChanged(patch->color(), true);

    for (i = 0; i < numRecents; i++)
        if (patch == recentPatches[i]) {
            activateRecent(i);
            break;
        }

    if (i == numRecents) // we didn't find it above
        addRecent(patch->color());
}

void KoColorSetWidget::setColorSet(KoColorSet *colorSet)
{
    if (colorSet == d->colorSet)
        return;

    KoResourceServer<KoColorSet> *srv = KoResourceServerProvider::instance()->paletteServer();
    QList<KoColorSet *> palettes = srv->resources();
    if (!palettes.contains(d->colorSet)) {
        delete d->colorSet;
    }

    d->colorSet = colorSet;
    d->fillColors();
}

KoColorSet *KoColorSetWidget::colorSet()
{
    return d->colorSet;
}

void KoColorSetWidget::resizeEvent(QResizeEvent *event)
{
    Q_EMIT widgetSizeChanged(event->size());
    QFrame::resizeEvent(event);
}

// have to include this because of Q_PRIVATE_SLOT
#include "moc_KoColorSetWidget.cpp"
