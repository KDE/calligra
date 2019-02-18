/* This file is part of the KDE project
 * Copyright (C) 2008 Jan Hambrecht <jaham@gmx.net>
 * Copyright (c) 2013 Sascha Suelzer <s.suelzer@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
    explicit KoResourceModel(QSharedPointer<KoAbstractResourceServerAdapter> resourceAdapter, QObject * parent = 0);
    ~KoResourceModel() Q_DECL_OVERRIDE;

    /// reimplemented
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    /// reimplemented
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const Q_DECL_OVERRIDE;
    /// reimplemented
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    /// reimplemented
    QModelIndex index ( int row, int column = 0, const QModelIndex & parent = QModelIndex() ) const Q_DECL_OVERRIDE;
    /// Sets the number of columns to display
    void setColumnCount( int columnCount );

    /// Extensions to Qt::ItemDataRole.
    enum ItemDataRole
    {
        /// A larger thumbnail for displaying in a tooltip. 200x200 or so.
        LargeThumbnailRole = 33
    };

    QModelIndex indexFromResource(KoResource* resource) const Q_DECL_OVERRIDE;

    /// facade for KoAbstractResourceServerAdapter
    QString extensions() const;
    void importResourceFile(const QString &filename);
    void importResourceFile(const QString &filename, bool fileCreation);
    bool removeResource(KoResource* resource) Q_DECL_OVERRIDE;
    void removeResourceFile(const QString & filename);
    QStringList assignedTagsList(KoResource *resource) const Q_DECL_OVERRIDE;
    void addTag(KoResource* resource, const QString& tag) Q_DECL_OVERRIDE;
    void deleteTag( KoResource* resource, const QString& tag) Q_DECL_OVERRIDE;
    QStringList tagNamesList() const Q_DECL_OVERRIDE;
    QStringList searchTag(const QString& lineEditText);
    void enableResourceFiltering(bool enable) Q_DECL_OVERRIDE;
    void setCurrentTag(const QString& currentTag) Q_DECL_OVERRIDE;
    void searchTextChanged(const QString& searchString) Q_DECL_OVERRIDE;
    void updateServer() Q_DECL_OVERRIDE;
    int resourcesCount() const Q_DECL_OVERRIDE;
    QList<KoResource *> currentlyVisibleResources() const Q_DECL_OVERRIDE;
    QList<KoResource *> serverResources() const Q_DECL_OVERRIDE;
    void tagCategoryMembersChanged() Q_DECL_OVERRIDE;
    void tagCategoryAdded(const QString& tag) Q_DECL_OVERRIDE;
    void tagCategoryRemoved(const QString& tag) Q_DECL_OVERRIDE;

    QString serverType() const;

Q_SIGNALS:
    /// XXX: not sure if this is the best place for these
    void tagBoxEntryModified();
    void tagBoxEntryAdded(const QString& tag);
    void tagBoxEntryRemoved(const QString& tag);

    void beforeResourcesLayoutReset(KoResource *activateAfterReformat);
    void afterResourcesLayoutReset();

private:
    void doSafeLayoutReset(KoResource *activateAfterReformat);

private Q_SLOTS:
    void resourceAdded(KoResource *resource);
    void resourceRemoved(KoResource *resource);
    void resourceChanged(KoResource *resource);
    void tagBoxEntryWasModified();
    void tagBoxEntryWasAdded(const QString& tag);
    void tagBoxEntryWasRemoved(const QString& tag);

private:
    QSharedPointer<KoAbstractResourceServerAdapter> m_resourceAdapter;
    int m_columnCount;
    QString m_currentTag;
};

#endif // KORESOURCEMODEL_H
