/*
 *    This file is part of the KDE project
 *    SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
 *
 *    SPDX-License-Identifier: LGPL-2.0-or-later
 * */

#ifndef KORESOURCEITEMCHOOSERCONTEXTMENU_H
#define KORESOURCEITEMCHOOSERCONTEXTMENU_H

#include <QLabel>
#include <QMenu>
#include <QWidgetAction>

class KLineEdit;
class KoResource;

class ContextMenuExistingTagAction : public QAction
{
    Q_OBJECT
public:
    explicit ContextMenuExistingTagAction(KoResource *resource, const QString &tag, QObject *parent = nullptr);
    ~ContextMenuExistingTagAction() override = default;

Q_SIGNALS:
    void triggered(KoResource *resource, const QString &tag);

protected Q_SLOTS:
    void onTriggered();

private:
    KoResource *m_resource;
    QString m_tag;
};

/*!
 *  A line edit QWidgetAction.
 *  Default behavior: Closes its parent upon triggering.
 */
class KoLineEditAction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit KoLineEditAction(QObject *parent);
    ~KoLineEditAction() override = default;
    void setIcon(const QIcon &icon);
    void closeParentOnTrigger(bool closeParent);
    bool closeParentOnTrigger();
    void setPlaceholderText(const QString &clickMessage);
    void setText(const QString &text);
    void setVisible(bool showAction);

Q_SIGNALS:
    void triggered(const QString &tag);

protected Q_SLOTS:
    void onTriggered(const QString &text);

private:
    bool m_closeParentOnTrigger;
    QLabel *m_label;
    KLineEdit *m_editBox;
};

class NewTagAction : public KoLineEditAction
{
    Q_OBJECT
public:
    explicit NewTagAction(KoResource *resource, QMenu *parent);
    ~NewTagAction() override = default;

Q_SIGNALS:
    void triggered(KoResource *resource, const QString &tag);

protected Q_SLOTS:
    void onTriggered(const QString &tagName);

private:
    KoResource *m_resource;
};

class KoResourceItemChooserContextMenu : public QMenu
{
    Q_OBJECT
public:
    explicit KoResourceItemChooserContextMenu(KoResource *resource,
                                              const QStringList &resourceTags,
                                              const QString &currentlySelectedTag,
                                              const QStringList &allTags);
    ~KoResourceItemChooserContextMenu() override = default;

Q_SIGNALS:
    /// Emitted when a resource should be added to an existing tag.
    void resourceTagAdditionRequested(KoResource *resource, const QString &tag);
    /// Emitted when a resource should be removed from an existing tag.
    void resourceTagRemovalRequested(KoResource *resource, const QString &tag);
    /// Emitted when a resource should be added to a new tag, which will need to be created.
    void resourceAssignmentToNewTagRequested(KoResource *resource, const QString &tag);
};

#endif // KORESOURCEITEMCHOOSERCONTEXTMENU_H
