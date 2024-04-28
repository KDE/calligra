/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 * SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOBOOKMARKMANAGER_H
#define KOBOOKMARKMANAGER_H

#include "kotext_export.h"

#include <QList>
#include <QObject>
#include <QString>

class KoBookmark;
class KoBookmarkManagerPrivate;

/**
 * A manager for all bookmarks in a document. Every bookmark is identified by a unique name.
 * Note that only SinglePosition and StartBookmark bookmarks can be retrieved from this
 * manager. An end bookmark should be retrieved from it's parent (StartBookmark) using
 * KoBookmark::endBookmark()
 * This class also maintains a list of bookmark names so that it can be easily used to
 * show all available bookmark.
 */
class KOTEXT_EXPORT KoBookmarkManager : public QObject
{
    Q_OBJECT
public:
    /// constructor
    KoBookmarkManager();
    ~KoBookmarkManager() override;

    /// @return a bookmark with the specified name, or 0 if there is none
    KoBookmark *bookmark(const QString &name) const;

    /// @return a list of QString containing all bookmark names
    QList<QString> bookmarkNameList() const;

public Q_SLOTS:
    /**
     * Insert a new bookmark to this manager. The name of the bookmark
     * will be set to @p name, no matter what name has been set on
     * it.
     * @param name the name of the bookmark
     * @param bookmark the bookmark object to insert
     */
    void insert(const QString &name, KoBookmark *bookmark);

    /**
     * Remove a bookmark from this manager.
     * @param name the name of the bookmark to remove
     */
    void remove(const QString &name);

    /**
     * Rename a bookmark
     * @param oldName the old name of the bookmark
     * @param newName the new name of the bookmark
     */
    void rename(const QString &oldName, const QString &newName);

private:
    KoBookmarkManagerPrivate *const d;
};

#endif
