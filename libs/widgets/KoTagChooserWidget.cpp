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

#include "KoTagChooserWidget.h"

#include <QDebug>
#include <QGridLayout>
#include <QToolButton>

#include <KComboBox>
#include <KLocalizedString>

#include <KoIcon.h>

#include "KoResourceItemChooserContextMenu.h"

#include "KoTagToolButton.h"

class Q_DECL_HIDDEN KoTagChooserWidget::Private
{
public:
    KComboBox *comboBox;
    KoTagToolButton *tagToolButton;
    QList<QString> readOnlyTags;
    QList<QString> tags;
};

KoTagChooserWidget::KoTagChooserWidget(QWidget *parent)
    : QWidget(parent)
    , d(new Private())
{
    d->comboBox = new KComboBox(this);
    d->comboBox->setToolTip(i18n("Tag"));
    d->comboBox->setInsertPolicy(KComboBox::InsertAlphabetically);
    d->comboBox->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    QGridLayout *comboLayout = new QGridLayout(this);

    comboLayout->addWidget(d->comboBox, 0, 0);

    d->tagToolButton = new KoTagToolButton(this);
    comboLayout->addWidget(d->tagToolButton, 0, 1);

    comboLayout->setSpacing(0);
    comboLayout->setContentsMargins({});
    comboLayout->setColumnStretch(0, 3);
    this->setEnabled(true);
    clear();

    connect(d->comboBox, &KComboBox::currentTextChanged, this, &KoTagChooserWidget::tagChosen);
    connect(d->tagToolButton, &KoTagToolButton::popupMenuAboutToShow, this, &KoTagChooserWidget::tagOptionsContextMenuAboutToShow);
    connect(d->tagToolButton, &KoTagToolButton::newTagRequested, this, &KoTagChooserWidget::newTagRequested);
    connect(d->tagToolButton, &KoTagToolButton::deletionOfCurrentTagRequested, this, &KoTagChooserWidget::contextDeleteCurrentTag);
    connect(d->tagToolButton, &KoTagToolButton::renamingOfCurrentTagRequested, this, &KoTagChooserWidget::slotTagRenamingRequested);
    connect(d->tagToolButton, &KoTagToolButton::undeletionOfTagRequested, this, &KoTagChooserWidget::tagUndeletionRequested);
    connect(d->tagToolButton, &KoTagToolButton::purgingOfTagUndeleteListRequested, this, &KoTagChooserWidget::tagUndeletionListPurgeRequested);
}

KoTagChooserWidget::~KoTagChooserWidget()
{
    delete d;
}

void KoTagChooserWidget::contextDeleteCurrentTag()
{
    if (selectedTagIsReadOnly()) {
        return;
    }
    Q_EMIT tagDeletionRequested(currentlySelectedTag());
}

void KoTagChooserWidget::slotTagRenamingRequested(const QString &newName)
{
    if (newName.isEmpty() || selectedTagIsReadOnly()) {
        return;
    }
    Q_EMIT tagRenamingRequested(currentlySelectedTag(), newName);
}

void KoTagChooserWidget::setUndeletionCandidate(const QString &tag)
{
    d->tagToolButton->setUndeletionCandidate(tag);
}

void KoTagChooserWidget::setCurrentIndex(int index)
{
    d->comboBox->setCurrentIndex(index);
}

int KoTagChooserWidget::findIndexOf(const QString &tagName)
{
    return d->comboBox->findText(tagName);
}

void KoTagChooserWidget::addReadOnlyItem(const QString &tagName)
{
    d->readOnlyTags.append(tagName);
}

void KoTagChooserWidget::insertItem(const QString &tagName)
{
    QStringList tags = allTags();
    tags.append(tagName);
    tags.sort();
    foreach (const QString &readOnlyTag, d->readOnlyTags) {
        tags.prepend(readOnlyTag);
    }

    int index = tags.indexOf(tagName);
    if (d->comboBox->findText(tagName) == -1) {
        insertItemAt(index, tagName);
        d->tags.append(tagName);
    }
}

void KoTagChooserWidget::insertItemAt(int index, const QString &tag)
{
    d->comboBox->insertItem(index, tag);
}

QString KoTagChooserWidget::currentlySelectedTag()
{
    return d->comboBox->currentText();
}

QStringList KoTagChooserWidget::allTags()
{
    return d->tags;
}

bool KoTagChooserWidget::selectedTagIsReadOnly()
{
    return d->readOnlyTags.contains(d->comboBox->currentText());
}

void KoTagChooserWidget::addItems(QStringList tagNames)
{
    tagNames.sort();
    QStringList items;

    for (const QString &readOnlyTag : std::as_const(d->readOnlyTags)) {
        items.append(readOnlyTag);
    }

    items.append(tagNames);
    d->tags.append(tagNames);

    d->comboBox->addItems(items);
}

void KoTagChooserWidget::clear()
{
    d->comboBox->clear();
}

void KoTagChooserWidget::removeItem(const QString &item)
{
    int pos = d->comboBox->findText(item);
    if (pos >= 0) {
        d->comboBox->removeItem(pos);
    }
}

void KoTagChooserWidget::tagOptionsContextMenuAboutToShow()
{
    /* only enable the save button if the selected tag set is editable */
    d->tagToolButton->readOnlyMode(selectedTagIsReadOnly());
    Q_EMIT popupMenuAboutToShow();
}

void KoTagChooserWidget::showTagToolButton(bool show)
{
    d->tagToolButton->setVisible(show);
}
