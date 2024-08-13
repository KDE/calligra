/* This file is part of the KDE project
SPDX-FileCopyrightText: 2002, 2003 Laurent Montel <lmontel@mandrakesoft.com>
SPDX-FileCopyrightText: 2006-2007 Jan Hambrecht <jaham@gmx.net>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoConfigGridPage.h"

#include <KoAspectButton.h>
#include <KoComponentData.h>
#include <KoDocument.h>
#include <KoGridData.h>
#include <KoPart.h>
#include <KoUnit.h>
#include <KoUnitDoubleSpinBox.h>

#include <KConfigGroup>
#include <kcolorbutton.h>

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>

class Q_DECL_HIDDEN KoConfigGridPage::Private
{
public:
    Private(KoDocument *doc)
        : doc(doc)
    {
    }

    KoDocument *doc;

    KoUnitDoubleSpinBox *spaceHorizUSpin;
    KoUnitDoubleSpinBox *spaceVertUSpin;
    QCheckBox *gridChBox;
    QCheckBox *snapChBox;
    KColorButton *gridColorBtn;
    KSharedConfigPtr config;
    KoAspectButton *bnLinkSpacing;
};

KoConfigGridPage::KoConfigGridPage(KoDocument *doc, char *name)
    : d(new Private(doc))
{
    setObjectName(name);

    d->config = d->doc->documentPart()->componentData().config();

    KoUnit unit = d->doc->unit();
    KoGridData &gd = d->doc->gridData();

    QFormLayout *layout = new QFormLayout;
    d->gridChBox = new QCheckBox(i18nc("@option:check", "Show grid"), this);
    d->gridChBox->setChecked(gd.showGrid());
    d->snapChBox = new QCheckBox(i18nc("@option:check", "Snap to grid"), this);
    d->snapChBox->setChecked(gd.snapToGrid());
    d->gridColorBtn = new KColorButton(gd.gridColor(), this);
    d->gridColorBtn->setAlphaChannelEnabled(true);
    layout->addRow(i18n("Grid:"), d->gridChBox);
    layout->addRow({}, d->snapChBox);
    layout->addRow(i18n("Grid color:"), d->gridColorBtn);

    d->spaceHorizUSpin = new KoUnitDoubleSpinBox(this);
    d->spaceHorizUSpin->setMinMaxStep(0.0, 1000, 0.1);
    d->spaceHorizUSpin->setUnit(unit);
    d->spaceHorizUSpin->changeValue(gd.gridX());
    auto hLayout = new QHBoxLayout;
    d->spaceVertUSpin = new KoUnitDoubleSpinBox(this);
    d->spaceVertUSpin->setMinMaxStep(0.0, 1000, 0.1);
    d->spaceVertUSpin->setUnit(unit);
    d->spaceVertUSpin->changeValue(gd.gridY());
    hLayout->addWidget(d->spaceVertUSpin);
    d->bnLinkSpacing = new KoAspectButton(this);
    d->bnLinkSpacing->setKeepAspectRatio(gd.gridX() == gd.gridY());
    hLayout->addWidget(d->bnLinkSpacing);

    layout->addRow(i18nc("Horizontal grid spacing", "Horizontal spacing:"), d->spaceHorizUSpin);
    layout->addRow(i18nc("Vertical grid spacing", "Vertical spacing:"), hLayout);

    setValuesFromGrid(d->doc->gridData());

    connect(d->spaceHorizUSpin, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoConfigGridPage::spinBoxHSpacingChanged);
    connect(d->spaceVertUSpin, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoConfigGridPage::spinBoxVSpacingChanged);

    auto hbox = new QHBoxLayout(this);
    hbox->addStretch();
    hbox->addLayout(layout);
    hbox->addStretch();
}

KoConfigGridPage::~KoConfigGridPage()
{
    delete d;
}

void KoConfigGridPage::slotUnitChanged(const KoUnit &unit)
{
    d->spaceHorizUSpin->blockSignals(true);
    d->spaceVertUSpin->blockSignals(true);
    d->spaceHorizUSpin->setUnit(unit);
    d->spaceVertUSpin->setUnit(unit);
    d->spaceHorizUSpin->blockSignals(false);
    d->spaceVertUSpin->blockSignals(false);
}

void KoConfigGridPage::apply()
{
    KoGridData &gd = d->doc->gridData();
    gd.setGrid(d->spaceHorizUSpin->value(), d->spaceVertUSpin->value());
    gd.setShowGrid(d->gridChBox->isChecked());
    gd.setSnapToGrid(d->snapChBox->isChecked());
    gd.setGridColor(d->gridColorBtn->color());

    KConfigGroup gridGroup = d->config->group("Grid");
    gridGroup.writeEntry("SpacingX", gd.gridX());
    gridGroup.writeEntry("SpacingY", gd.gridY());
    gridGroup.writeEntry("Color", gd.gridColor());
}

void KoConfigGridPage::slotDefault()
{
    KoGridData defGrid;
    setValuesFromGrid(defGrid);
}

void KoConfigGridPage::setValuesFromGrid(const KoGridData &grid)
{
    d->spaceHorizUSpin->changeValue(grid.gridX());
    d->spaceVertUSpin->changeValue(grid.gridY());

    d->gridChBox->setChecked(grid.showGrid());
    d->snapChBox->setChecked(grid.snapToGrid());
    d->gridColorBtn->setColor(grid.gridColor());
}

void KoConfigGridPage::spinBoxHSpacingChanged(qreal v)
{
    if (d->bnLinkSpacing->keepAspectRatio())
        d->spaceVertUSpin->changeValue(v);
}

void KoConfigGridPage::spinBoxVSpacingChanged(qreal v)
{
    if (d->bnLinkSpacing->keepAspectRatio())
        d->spaceHorizUSpin->changeValue(v);
}
