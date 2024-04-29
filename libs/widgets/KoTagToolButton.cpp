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

#include "KoTagToolButton.h"

#include <QGridLayout>
#include <QToolButton>

#include <KLocalizedString>

#include <KoIcon.h>

#include "KoResourceItemChooserContextMenu.h"

class KoTagToolButton::Private
{
public:
    QToolButton *tagToolButton;
    QAction *action_undeleteTag;
    QAction *action_deleteTag;
    KoLineEditAction *action_renameTag;
    QAction *action_purgeTagUndeleteList;
    QString undeleteCandidate;
};

KoTagToolButton::KoTagToolButton(QWidget *parent)
    : QWidget(parent)
    , d(new Private())
{
    QGridLayout *buttonLayout = new QGridLayout(this);
    buttonLayout->setContentsMargins({});
    buttonLayout->setSpacing(0);

    d->tagToolButton = new QToolButton(this);
    d->tagToolButton->setIcon(koIcon("list-add"));
    d->tagToolButton->setToolTip(i18nc("@info:tooltip", "<qt>Show the tag box options.</qt>"));
    d->tagToolButton->setPopupMode(QToolButton::InstantPopup);
    d->tagToolButton->setEnabled(true);

    QMenu *popup = new QMenu(this);

    KoLineEditAction *addTagAction = new KoLineEditAction(popup);
    addTagAction->setPlaceholderText(i18n("New tag"));
    addTagAction->setIcon(koIcon("document-new"));
    addTagAction->closeParentOnTrigger(true);
    popup->addAction(addTagAction);

    connect(addTagAction, QOverload<const QString &>::of(&KoLineEditAction::triggered), this, &KoTagToolButton::newTagRequested);

    d->action_renameTag = new KoLineEditAction(popup);
    d->action_renameTag->setPlaceholderText(i18n("Rename tag"));
    d->action_renameTag->setIcon(koIcon("edit-rename"));
    d->action_renameTag->closeParentOnTrigger(true);
    popup->addAction(d->action_renameTag);

    connect(d->action_renameTag, QOverload<const QString &>::of(&KoLineEditAction::triggered), this, &KoTagToolButton::renamingOfCurrentTagRequested);

    popup->addSeparator();

    d->action_deleteTag = new QAction(popup);
    d->action_deleteTag->setText(i18n("Delete this tag"));
    d->action_deleteTag->setIcon(koIcon("edit-delete"));
    popup->addAction(d->action_deleteTag);

    connect(d->action_deleteTag, &QAction::triggered, this, &KoTagToolButton::deletionOfCurrentTagRequested);

    popup->addSeparator();

    d->action_undeleteTag = new QAction(popup);
    d->action_undeleteTag->setIcon(koIcon("edit-redo"));
    d->action_undeleteTag->setVisible(false);
    popup->addAction(d->action_undeleteTag);

    connect(d->action_undeleteTag, &QAction::triggered, this, &KoTagToolButton::onTagUndeleteClicked);

    d->action_purgeTagUndeleteList = new QAction(popup);
    d->action_purgeTagUndeleteList->setText(i18n("Clear undelete list"));
    d->action_purgeTagUndeleteList->setIcon(koIcon("edit-clear"));
    d->action_purgeTagUndeleteList->setVisible(false);
    popup->addAction(d->action_purgeTagUndeleteList);

    connect(d->action_purgeTagUndeleteList, &QAction::triggered, this, &KoTagToolButton::purgingOfTagUndeleteListRequested);

    connect(popup, &QMenu::aboutToShow, this, &KoTagToolButton::popupMenuAboutToShow);

    d->tagToolButton->setMenu(popup);
    buttonLayout->addWidget(d->tagToolButton);
}

KoTagToolButton::~KoTagToolButton()
{
    delete d;
}

void KoTagToolButton::readOnlyMode(bool activate)
{
    activate = !activate;
    d->action_renameTag->setVisible(activate);
    d->action_deleteTag->setVisible(activate);
}

void KoTagToolButton::setUndeletionCandidate(const QString &deletedTagName)
{
    d->undeleteCandidate = deletedTagName;
    d->action_undeleteTag->setText(i18n("Undelete") + " " + deletedTagName);
    d->action_undeleteTag->setVisible(!deletedTagName.isEmpty());
    d->action_purgeTagUndeleteList->setVisible(!deletedTagName.isEmpty());
}

void KoTagToolButton::onTagUndeleteClicked()
{
    Q_EMIT undeletionOfTagRequested(d->undeleteCandidate);
}
