/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Victor Lafon <metabolic.ewilan@hotmail.fr>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KORESOURCEMODELBASE_H
#define KORESOURCEMODELBASE_H

#include "kowidgets_export.h"
#include <QAbstractTableModel>

class KoResource;

/// The resource model managing the resource data
class KOWIDGETS_EXPORT KoResourceModelBase : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit KoResourceModelBase(QObject *parent = nullptr);
    ~KoResourceModelBase() override;

    /// reimplemented
    int rowCount(const QModelIndex &parent = QModelIndex()) const override = 0;
    /// reimplemented
    int columnCount(const QModelIndex &parent = QModelIndex()) const override = 0;
    /// reimplemented
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override = 0;
    /// reimplemented
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override = 0;

    virtual QModelIndex indexFromResource(KoResource *resource) const = 0;
    virtual QStringList assignedTagsList(KoResource *resource) const = 0;
    virtual bool removeResource(KoResource *resource) = 0;
    virtual void addTag(KoResource *resource, const QString &tag) = 0;
    virtual void deleteTag(KoResource *resource, const QString &tag) = 0;
    virtual int resourcesCount() const = 0;
    virtual QList<KoResource *> currentlyVisibleResources() const = 0;
    virtual void updateServer() = 0;
    virtual void enableResourceFiltering(bool enable) = 0;
    virtual void searchTextChanged(const QString &searchString) = 0;
    virtual QStringList tagNamesList() const = 0;
    virtual void setCurrentTag(const QString &currentTag) = 0;
    virtual QList<KoResource *> serverResources() const = 0;
    virtual void tagCategoryMembersChanged() = 0;
    virtual void tagCategoryAdded(const QString &tag) = 0;
    virtual void tagCategoryRemoved(const QString &tag) = 0;

private Q_SLOTS:
    virtual void tagBoxEntryWasModified() = 0;
    virtual void tagBoxEntryWasAdded(const QString &tag) = 0;
    virtual void tagBoxEntryWasRemoved(const QString &tag) = 0;
    virtual void resourceAdded(KoResource *resource) = 0;
    virtual void resourceRemoved(KoResource *resource) = 0;
    virtual void resourceChanged(KoResource *resource) = 0;
};
#endif // KORESOURCEMODELBASE_H
