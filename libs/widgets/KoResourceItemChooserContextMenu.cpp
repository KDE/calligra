
/* This file is part of the KDE project
 *    SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 * */

#include "KoResourceItemChooserContextMenu.h"

#include <QDebug>
#include <QGridLayout>
#include <QLabel>

#include <KLocalizedString>
#include <KoIcon.h>
#include <klineedit.h>

#include "KoResource.h"

KoLineEditAction::KoLineEditAction(QObject *parent)
    : QWidgetAction(parent)
    , m_closeParentOnTrigger(false)
{
    QWidget *pWidget = new QWidget(nullptr);
    QHBoxLayout *pLayout = new QHBoxLayout();
    m_label = new QLabel(nullptr);
    m_editBox = new KLineEdit(nullptr);
    pLayout->addWidget(m_label);
    pLayout->addWidget(m_editBox);
    pWidget->setLayout(pLayout);
    setDefaultWidget(pWidget);

    connect(m_editBox, &KLineEdit::returnKeyPressed, this, &KoLineEditAction::onTriggered);
}

void KoLineEditAction::setIcon(const QIcon &icon)
{
    QPixmap pixmap = QPixmap(icon.pixmap(16, 16));
    m_label->setPixmap(pixmap);
}

void KoLineEditAction::closeParentOnTrigger(bool closeParent)
{
    m_closeParentOnTrigger = closeParent;
}

bool KoLineEditAction::closeParentOnTrigger()
{
    return m_closeParentOnTrigger;
}

void KoLineEditAction::onTriggered(const QString &text)
{
    if (!text.isEmpty()) {
        Q_EMIT triggered(text);
        m_editBox->clear();

        if (m_closeParentOnTrigger) {
            this->parentWidget()->close();
            m_editBox->clearFocus();
        }
    }
}

void KoLineEditAction::setPlaceholderText(const QString &clickMessage)
{
    m_editBox->setPlaceholderText(clickMessage);
}

void KoLineEditAction::setText(const QString &text)
{
    m_editBox->setText(text);
}

void KoLineEditAction::setVisible(bool showAction)
{
    QLayout *currentLayout = defaultWidget()->layout();

    this->QAction::setVisible(showAction);

    for (int i = 0; i < currentLayout->count(); i++) {
        currentLayout->itemAt(i)->widget()->setVisible(showAction);
    }
    defaultWidget()->setVisible(showAction);
}

ContextMenuExistingTagAction::ContextMenuExistingTagAction(KoResource *resource, const QString &tag, QObject *parent)
    : QAction(parent)
    , m_resource(resource)
    , m_tag(tag)
{
    setText(tag);
    connect(this, QOverload<KoResource *, const QString &>::of(&ContextMenuExistingTagAction::triggered), this, &ContextMenuExistingTagAction::onTriggered);
}

void ContextMenuExistingTagAction::onTriggered()
{
    Q_EMIT triggered(m_resource, m_tag);
}
NewTagAction::NewTagAction(KoResource *resource, QMenu *parent)
    : KoLineEditAction(parent)
{
    m_resource = resource;
    setIcon(koIcon("document-new"));
    setPlaceholderText(i18n("New tag"));
    closeParentOnTrigger(true);

    connect(this, QOverload<KoResource *, const QString &>::of(&NewTagAction::triggered), this, [this](KoResource *res, const QString &tag) {
        Q_UNUSED(res);
        onTriggered(tag);
    });
}

void NewTagAction::onTriggered(const QString &tagName)
{
    Q_EMIT triggered(m_resource, tagName);
}

KoResourceItemChooserContextMenu::KoResourceItemChooserContextMenu(KoResource *resource,
                                                                   const QStringList &resourceTags,
                                                                   const QString &currentlySelectedTag,
                                                                   const QStringList &allTags)
{
    QImage image = resource->image();
    QIcon icon(QPixmap::fromImage(image));
    QAction *label = new QAction(resource->name(), this);
    label->setIcon(icon);

    addAction(label);

    QMenu *removableTagsMenu;
    QMenu *assignableTagsMenu;

    QStringList removables = resourceTags;
    QStringList assignables = allTags;

    removables.sort();
    assignables.sort();

    assignableTagsMenu = addMenu(koIcon("list-add"), i18n("Assign to tag"));

    if (!removables.isEmpty()) {
        addSeparator();
        QString currentTag = currentlySelectedTag;
        if (removables.contains(currentTag)) {
            assignables.removeAll(currentTag);
            removables.removeAll(currentTag);
            ContextMenuExistingTagAction *removeTagAction = new ContextMenuExistingTagAction(resource, currentTag, this);
            removeTagAction->setText(i18n("Remove from this tag"));
            removeTagAction->setIcon(koIcon("list-remove"));

            connect(removeTagAction,
                    QOverload<KoResource *, const QString &>::of(&ContextMenuExistingTagAction::triggered),
                    this,
                    &KoResourceItemChooserContextMenu::resourceTagRemovalRequested);
            addAction(removeTagAction);
        }
        if (!removables.isEmpty()) {
            removableTagsMenu = addMenu(koIcon("list-remove"), i18n("Remove from other tag"));
            foreach (const QString &tag, removables) {
                assignables.removeAll(tag);
                ContextMenuExistingTagAction *removeTagAction = new ContextMenuExistingTagAction(resource, tag, this);

                connect(removeTagAction,
                        QOverload<KoResource *, const QString &>::of(&ContextMenuExistingTagAction::triggered),
                        this,
                        &KoResourceItemChooserContextMenu::resourceTagRemovalRequested);
                removableTagsMenu->addAction(removeTagAction);
            }
        }
    }

    foreach (const QString &tag, assignables) {
        ContextMenuExistingTagAction *addTagAction = new ContextMenuExistingTagAction(resource, tag, this);

        connect(addTagAction,
                QOverload<KoResource *, const QString &>::of(&ContextMenuExistingTagAction::triggered),
                this,
                &KoResourceItemChooserContextMenu::resourceTagAdditionRequested);
        assignableTagsMenu->addAction(addTagAction);
    }
    assignableTagsMenu->addSeparator();

    NewTagAction *addTagAction = new NewTagAction(resource, this);
    connect(addTagAction,
            QOverload<KoResource *, const QString &>::of(&NewTagAction::triggered),
            this,
            &KoResourceItemChooserContextMenu::resourceAssignmentToNewTagRequested);
    assignableTagsMenu->addAction(addTagAction);
}
