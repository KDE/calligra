/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2013 Sascha Suelzer <s.suelzer@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KORESOURCEMODEL_H
#define KORESOURCEMODEL_H

#include <QSharedPointer>

#include "KoResourceModelBase.h"
#include "kowidgets_export.h"

class KoAbstractResourceServerAdapter;
class KoResource;

/// The resource model managing the resource data
class KOWIDGETS_EXPORT KoResourceModel : public KoResourceModelBase
{
    Q_OBJECT
public:
    explicit KoResourceModel(QSharedPointer<KoAbstractResourceServerAdapter> resourceAdapter, QObject *parent = nullptr);
    ~KoResourceModel() override;

    /// reimplemented
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /// reimplemented
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    /// reimplemented
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /// reimplemented
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    /// Sets the number of columns to display
    void setColumnCount(int columnCount);

    /// Extensions to Qt::ItemDataRole.
    enum ItemDataRole {
        /// A larger thumbnail for displaying in a tooltip. 200x200 or so.
        LargeThumbnailRole = 33
    };

    QModelIndex indexFromResource(KoResource *resource) const override;

    /// facade for KoAbstractResourceServerAdapter
    QString extensions() const;
    void importResourceFile(const QString &filename);
    void importResourceFile(const QString &filename, bool fileCreation);
    bool removeResource(KoResource *resource) override;
    void removeResourceFile(const QString &filename);
    QStringList assignedTagsList(KoResource *resource) const override;
    void addTag(KoResource *resource, const QString &tag) override;
    void deleteTag(KoResource *resource, const QString &tag) override;
    QStringList tagNamesList() const override;
    QStringList searchTag(const QString &lineEditText);
    void enableResourceFiltering(bool enable) override;
    void setCurrentTag(const QString &currentTag) override;
    void searchTextChanged(const QString &searchString) override;
    void updateServer() override;
    int resourcesCount() const override;
    QList<KoResource *> currentlyVisibleResources() const override;
    QList<KoResource *> serverResources() const override;
    void tagCategoryMembersChanged() override;
    void tagCategoryAdded(const QString &tag) override;
    void tagCategoryRemoved(const QString &tag) override;

    QString serverType() const;

Q_SIGNALS:
    /// XXX: not sure if this is the best place for these
    void tagBoxEntryModified();
    void tagBoxEntryAdded(const QString &tag);
    void tagBoxEntryRemoved(const QString &tag);

    void beforeResourcesLayoutReset(KoResource *activateAfterReformat);
    void afterResourcesLayoutReset();

private:
    void doSafeLayoutReset(KoResource *activateAfterReformat);

private Q_SLOTS:
    void resourceAdded(KoResource *resource) override;
    void resourceRemoved(KoResource *resource) override;
    void resourceChanged(KoResource *resource) override;
    void tagBoxEntryWasModified() override;
    void tagBoxEntryWasAdded(const QString &tag) override;
    void tagBoxEntryWasRemoved(const QString &tag) override;

private:
    QSharedPointer<KoAbstractResourceServerAdapter> m_resourceAdapter;
    int m_columnCount;
    QString m_currentTag;
};

#endif // KORESOURCEMODEL_H
