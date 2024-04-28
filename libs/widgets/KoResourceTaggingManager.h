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

#ifndef KORESOURCETAGGINGMANAGER_H
#define KORESOURCETAGGINGMANAGER_H

#include <QObject>

class QWidget;
class QString;
class QPoint;

class KoTagFilterWidget;
class KoTagChooserWidget;
class KoResourceModel;
class KoResource;

class KoResourceTaggingManager : public QObject
{
    Q_OBJECT

public:
    explicit KoResourceTaggingManager(KoResourceModel *model, QWidget *parent);
    ~KoResourceTaggingManager() override;
    void showTaggingBar(bool show);
    QStringList availableTags() const;
    QString currentTag();
    void contextMenuRequested(KoResource *currentResource, QPoint pos);
    void allowTagModification(bool set);
    bool allowTagModification();
    KoTagFilterWidget *tagFilterWidget();
    KoTagChooserWidget *tagChooserWidget();
private Q_SLOTS:
    void undeleteTag(const QString &tagToUndelete);
    void purgeTagUndeleteList();
    void contextCreateNewTag(KoResource *resource, const QString &tag);
    void contextCreateNewTag(const QString &tag);
    void syncTagBoxEntryRemoval(const QString &tag);
    void syncTagBoxEntryAddition(const QString &tag);
    void syncTagBoxEntries();
    void tagSaveButtonPressed();
    void contextRemoveTagFromResource(KoResource *resource, const QString &tag);
    void contextAddTagToResource(KoResource *resource, const QString &tag);
    void renameTag(const QString &oldName, const QString &newName);
    void tagChooserIndexChanged(const QString &lineEditText);
    void tagSearchLineEditTextChanged(const QString &lineEditText);
    void removeTagFromComboBox(const QString &tag);

private:
    void contextMenuRequested(KoResource *resource, const QStringList &resourceTags, const QPoint &pos);
    void enableContextMenu(bool enable);
    void removeResourceTag(KoResource *resource, const QString &tagName);
    void addResourceTag(KoResource *resource, const QString &tagName);
    void updateTaggedResourceView();
    class Private;
    Private *const d;
};

#endif // KORESOURCETAGGINGINTERFACE_H
