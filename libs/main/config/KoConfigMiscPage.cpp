/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002, 2003 Laurent Montel <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007, 2010-2011 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoConfigMiscPage.h"

#include <KoComponentData.h>
#include <KoDocument.h>
#include <KoDocumentResourceManager.h>
#include <KoPart.h>
#include <KoUnit.h>
#include <KoUnitDoubleSpinBox.h>

#include <KComboBox>

#include <KConfig>
#include <KConfigGroup>

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSpinBox>

class Q_DECL_HIDDEN KoConfigMiscPage::Private
{
public:
    Private(KoDocument *doc, KoDocumentResourceManager *documentResources)
        : doc(doc)
        , docResources(documentResources)
    {
    }

    KoDocument *doc;
    KSharedConfigPtr config;
    KoDocumentResourceManager *docResources;

    KoUnit oldUnit;
    QComboBox *unit;
    QSpinBox *handleRadius;
    uint oldHandleRadius;
    QSpinBox *grabSensitivity;
    uint oldGrabSensitivity;
    KoUnitDoubleSpinBox *pasteOffset;
    qreal oldPasteOffset;
    QCheckBox *pasteAtCursor;
    bool oldPasteAtCursor;
};

KoConfigMiscPage::KoConfigMiscPage(KoDocument *doc, KoDocumentResourceManager *documentResources, char *name)
    : d(std::make_unique<Private>(doc, documentResources))
{
    setObjectName(name);

    d->config = d->doc->documentPart()->componentData().config();

    d->oldGrabSensitivity = d->docResources->grabSensitivity();
    d->oldHandleRadius = d->docResources->handleRadius();
    d->oldPasteOffset = d->docResources->pasteOffset();
    d->oldPasteAtCursor = d->docResources->pasteAtCursor();

    const KoUnit documentUnit = d->doc->unit();

    auto miscLayout = new QFormLayout;

    // #################"laurent
    // don't load unitType from config file because unit is
    // depend from words file => unit can be different from config file

    d->unit = new KComboBox(this);
    d->unit->addItems(KoUnit::listOfUnitNameForUi(KoUnit::HidePixel));
    miscLayout->addRow(i18n("Units:"), d->unit);
    d->oldUnit = documentUnit;
    d->unit->setCurrentIndex(d->oldUnit.indexInListForUi(KoUnit::HidePixel));

    d->handleRadius = new QSpinBox(this);
    d->handleRadius->setRange(3, 20);
    d->handleRadius->setSingleStep(1);
    d->handleRadius->setSuffix(" px");
    d->handleRadius->setValue(d->oldHandleRadius);
    miscLayout->addRow(i18n("Handle radius:"), d->handleRadius);

    d->grabSensitivity = new QSpinBox(this);
    d->grabSensitivity->setRange(3, 20);
    d->grabSensitivity->setSingleStep(1);
    d->grabSensitivity->setSuffix(" px");
    d->grabSensitivity->setValue(d->oldGrabSensitivity);
    miscLayout->addRow(i18n("Grab sensitivity:"), d->grabSensitivity);

    d->pasteOffset = new KoUnitDoubleSpinBox(this);
    d->pasteOffset->setMinMaxStep(-1000, 1000, 0.1);
    d->pasteOffset->setValue(d->oldPasteOffset);
    d->pasteOffset->setUnit(documentUnit);
    d->pasteOffset->setDisabled(d->oldPasteAtCursor);
    miscLayout->addRow(i18n("Paste offset:"), d->pasteOffset);

    d->pasteAtCursor = new QCheckBox(this);
    d->pasteAtCursor->setChecked(d->oldPasteAtCursor);
    d->pasteAtCursor->setText(i18nc("@option:check", "Paste at Cursor"));
    miscLayout->addRow({}, d->pasteAtCursor);

    connect(d->unit, QOverload<int>::of(&QComboBox::activated), this, &KoConfigMiscPage::slotUnitChanged);
    connect(d->pasteAtCursor, &QAbstractButton::clicked, d->pasteOffset, &QWidget::setDisabled);

    auto hbox = new QHBoxLayout(this);
    hbox->addStretch();
    hbox->addLayout(miscLayout);
    hbox->addStretch();
}

KoConfigMiscPage::~KoConfigMiscPage() = default;

void KoConfigMiscPage::apply()
{
    KConfigGroup miscGroup = d->config->group("Misc");

    int currentUnitIndex = d->unit->currentIndex();
    if (d->oldUnit.indexInListForUi(KoUnit::HidePixel) != currentUnitIndex) {
        d->oldUnit = KoUnit::fromListForUi(currentUnitIndex, KoUnit::HidePixel);
        d->doc->setUnit(d->oldUnit);
        miscGroup.writeEntry("Units", d->oldUnit.symbol());
    }

    uint currentHandleRadius = d->handleRadius->value();
    if (currentHandleRadius != d->oldHandleRadius) {
        miscGroup.writeEntry("HandleRadius", currentHandleRadius);
        d->docResources->setHandleRadius(currentHandleRadius);
    }

    uint currentGrabSensitivity = d->grabSensitivity->value();
    if (currentGrabSensitivity != d->oldGrabSensitivity) {
        miscGroup.writeEntry("GrabSensitivity", currentGrabSensitivity);
        d->docResources->setGrabSensitivity(currentGrabSensitivity);
    }

    qreal currentCopyOffset = d->pasteOffset->value();
    if (currentCopyOffset != d->oldPasteOffset) {
        miscGroup.writeEntry("CopyOffset", currentCopyOffset);
        d->docResources->setPasteOffset(currentCopyOffset);
    }

    const bool currentPasteAtCursor = d->pasteAtCursor->isChecked();
    if (currentPasteAtCursor != d->oldPasteAtCursor) {
        miscGroup.writeEntry("PasteAtCursor", currentPasteAtCursor);
        d->docResources->enablePasteAtCursor(currentPasteAtCursor);
    }
}

void KoConfigMiscPage::slotDefault()
{
    d->unit->setCurrentIndex(0);
}

void KoConfigMiscPage::slotUnitChanged(int u)
{
    const KoUnit unit = KoUnit::fromListForUi(u, KoUnit::HidePixel);

    d->pasteOffset->blockSignals(true);
    d->pasteOffset->setUnit(unit);
    d->pasteOffset->blockSignals(false);

    Q_EMIT unitChanged(unit);
}
