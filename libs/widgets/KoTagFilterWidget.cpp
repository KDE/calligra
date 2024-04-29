/*
 *    This file is part of the KDE project
 *    SPDX-FileCopyrightText: 2002 Patrick Julien <freak@codepimps.org>
 *    SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *    SPDX-FileCopyrightText: 2007 Sven Langkamp <sven.langkamp@gmail.com>
 *    SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
 *    SPDX-FileCopyrightText: 2011 José Luis Vergara <pentalis@gmail.com>
 *    SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTagFilterWidget.h"

#include <QAction>
#include <QGridLayout>
#include <QPushButton>

#include <KLocalizedString>
#include <klineedit.h>

#include <KoIcon.h>

class KoTagFilterWidget::Private
{
public:
    QString tagSearchBarTooltip_saving_disabled;
    QString tagSearchBarTooltip_saving_enabled;
    KLineEdit *tagSearchLineEdit;
    QPushButton *tagSearchSaveButton;
    QGridLayout *filterBarLayout;
};

KoTagFilterWidget::KoTagFilterWidget(QWidget *parent)
    : QWidget(parent)
    , d(new Private())
{
    d->tagSearchBarTooltip_saving_disabled = i18nc("@info:tooltip",
                                                   "<qt>Entering search terms here will add to, or remove resources from the current tag view."
                                                   "<para>To filter based on the partial, case insensitive name of a resource:<br>"
                                                   "<icode>partialname</icode> or <icode>!partialname</icode>.</para>"
                                                   "<para>In-/exclusion of other tag sets:<br>"
                                                   "<icode>[Tagname]</icode> or <icode>![Tagname]</icode>.</para>"
                                                   "<para>Case sensitive and full name matching in-/exclusion:<br>"
                                                   "<icode>\"ExactMatch\"</icode> or <icode>!\"ExactMatch\"</icode>.</para>"
                                                   "Filter results cannot be saved for the <interface>All Presets</interface> view.<br>"
                                                   "In this view, pressing <interface>Enter</interface> or clearing the filter box will restore all items.<br>"
                                                   "Create and/or switch to a different tag if you want to save filtered resources into named sets.</qt>");

    d->tagSearchBarTooltip_saving_enabled =
        i18nc("@info:tooltip",
              "<qt>Entering search terms here will add to, or remove resources from the current tag view."
              "<para>To filter based on the partial, case insensitive name of a resource:<br>"
              "<icode>partialname</icode> or <icode>!partialname</icode>.</para>"
              "<para>In-/exclusion of other tag sets:<br>"
              "<icode>[Tagname]</icode> or <icode>![Tagname]</icode>.</para>"
              "<para>Case sensitive and full name matching in-/exclusion:<br>"
              "<icode>\"ExactMatch\"</icode> or <icode>!\"ExactMatch\"</icode>.</para>"
              "Pressing <interface>Enter</interface> or clicking the <interface>Save</interface> button will save the changes.</qt>");

    QGridLayout *filterBarLayout = new QGridLayout;

    d->tagSearchLineEdit = new KLineEdit(this);
    d->tagSearchLineEdit->setClearButtonEnabled(true);
    d->tagSearchLineEdit->setPlaceholderText(i18n("Enter resource filters here"));
    d->tagSearchLineEdit->setToolTip(d->tagSearchBarTooltip_saving_disabled);
    d->tagSearchLineEdit->setEnabled(true);

    filterBarLayout->setSpacing(0);
    filterBarLayout->setContentsMargins({});
    filterBarLayout->setColumnStretch(0, 1);
    filterBarLayout->addWidget(d->tagSearchLineEdit, 0, 0);

    d->tagSearchSaveButton = new QPushButton(this);
    d->tagSearchSaveButton->setIcon(koIcon("media-floppy"));
    d->tagSearchSaveButton->setToolTip(i18nc("@info:tooltip", "<qt>Save the currently filtered set as the new members of the current tag.</qt>"));
    d->tagSearchSaveButton->setEnabled(false);

    filterBarLayout->addWidget(d->tagSearchSaveButton, 0, 1);

    connect(d->tagSearchSaveButton, &QAbstractButton::pressed, this, &KoTagFilterWidget::onSaveButtonClicked);
    connect(d->tagSearchLineEdit, &KLineEdit::returnPressed, this, &KoTagFilterWidget::onSaveButtonClicked);
    connect(d->tagSearchLineEdit, &QLineEdit::textChanged, this, &KoTagFilterWidget::onTextChanged);
    allowSave(false);
    this->setLayout(filterBarLayout);
}

KoTagFilterWidget::~KoTagFilterWidget()
{
    delete d;
}
void KoTagFilterWidget::allowSave(bool allow)
{
    if (allow) {
        d->tagSearchSaveButton->show();
        d->tagSearchLineEdit->setToolTip(d->tagSearchBarTooltip_saving_enabled);
    } else {
        d->tagSearchSaveButton->hide();
        d->tagSearchLineEdit->setToolTip(d->tagSearchBarTooltip_saving_disabled);
    }
}

void KoTagFilterWidget::clear()
{
    d->tagSearchLineEdit->clear();
    d->tagSearchSaveButton->setEnabled(false);
}

void KoTagFilterWidget::onTextChanged(const QString &lineEditText)
{
    d->tagSearchSaveButton->setEnabled(!lineEditText.isEmpty());
    Q_EMIT filterTextChanged(lineEditText);
}

void KoTagFilterWidget::onSaveButtonClicked()
{
    Q_EMIT saveButtonClicked();
    clear();
}
