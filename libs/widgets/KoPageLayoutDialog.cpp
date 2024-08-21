/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoPageLayoutDialog.h"

#include "KoPageLayoutWidget.h"
#include "KoPagePreviewWidget.h"

#include <KLocalizedString>
#include <WidgetsDebug.h>

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QTimer>

class Q_DECL_HIDDEN KoPageLayoutDialog::Private
{
public:
    Private()
        : pageLayoutWidget(nullptr)
        , documentCheckBox(nullptr)
    {
    }
    KoPageLayoutWidget *pageLayoutWidget;
    QCheckBox *documentCheckBox;
};

KoPageLayoutDialog::KoPageLayoutDialog(QWidget *parent, const KoPageLayout &layout)
    : KPageDialog(parent)
    , d(new Private)
{
    setWindowTitle(i18n("Page Layout"));
    setFaceType(KPageDialog::Tabbed);

    QWidget *widget = new QWidget(this);
    addPage(widget, i18n("Page"));

    QHBoxLayout *lay = new QHBoxLayout(widget);

    d->pageLayoutWidget = new KoPageLayoutWidget(widget, layout);
    d->pageLayoutWidget->showUnitchooser(false);
    lay->addWidget(d->pageLayoutWidget, 1);

    KoPagePreviewWidget *prev = new KoPagePreviewWidget(widget);
    // use not original layout, but "fixed" one (e.g. with 0 values) as now hold by pageLayoutWidget
    prev->setPageLayout(d->pageLayoutWidget->pageLayout());
    lay->addWidget(prev, 1);

    connect(d->pageLayoutWidget, &KoPageLayoutWidget::layoutChanged, prev, &KoPagePreviewWidget::setPageLayout);
    connect(d->pageLayoutWidget, &KoPageLayoutWidget::layoutChanged, this, &KoPageLayoutDialog::setPageLayout);
    connect(d->pageLayoutWidget, &KoPageLayoutWidget::unitChanged, this, &KoPageLayoutDialog::unitChanged);
}

KoPageLayoutDialog::~KoPageLayoutDialog()
{
    delete d;
}

KoPageLayout KoPageLayoutDialog::pageLayout() const
{
    return d->pageLayoutWidget->pageLayout();
}

void KoPageLayoutDialog::setPageLayout(const KoPageLayout &layout)
{
    d->pageLayoutWidget->setPageLayout(layout);
}

void KoPageLayoutDialog::accept()
{
    KPageDialog::accept();
    deleteLater();
}

void KoPageLayoutDialog::reject()
{
    KPageDialog::reject();
    deleteLater();
}

bool KoPageLayoutDialog::applyToDocument() const
{
    return d->documentCheckBox && d->documentCheckBox->isChecked();
}

void KoPageLayoutDialog::showApplyToDocument(bool on)
{
    if (on && d->documentCheckBox == nullptr) {
        for (int i = 0; i < children().count(); ++i) {
            if (QDialogButtonBox *buttonBox = qobject_cast<QDialogButtonBox *>(children()[i])) {
                d->documentCheckBox = new QCheckBox(i18n("Apply to document"), buttonBox);
                d->documentCheckBox->setChecked(true);
                buttonBox->addButton(d->documentCheckBox, QDialogButtonBox::ResetRole);
                break;
            }
        }

        Q_ASSERT(d->pageLayoutWidget);
        connect(d->documentCheckBox, &QCheckBox::toggled, d->pageLayoutWidget, &KoPageLayoutWidget::setApplyToDocument);
    } else if (d->documentCheckBox) {
        d->documentCheckBox->setVisible(on);
    }
}

void KoPageLayoutDialog::showTextDirection(bool on)
{
    d->pageLayoutWidget->showTextDirection(on);
}

KoText::Direction KoPageLayoutDialog::textDirection() const
{
    return d->pageLayoutWidget->textDirection();
}

void KoPageLayoutDialog::setTextDirection(KoText::Direction direction)
{
    d->pageLayoutWidget->setTextDirection(direction);
}

void KoPageLayoutDialog::showPageSpread(bool on)
{
    d->pageLayoutWidget->showPageSpread(on);
}

void KoPageLayoutDialog::setPageSpread(bool pageSpread)
{
    d->pageLayoutWidget->setPageSpread(pageSpread);
}

void KoPageLayoutDialog::showUnitchooser(bool on)
{
    d->pageLayoutWidget->showUnitchooser(on);
}

void KoPageLayoutDialog::setUnit(const KoUnit &unit)
{
    d->pageLayoutWidget->setUnit(unit);
}
