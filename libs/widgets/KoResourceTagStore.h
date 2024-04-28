/*  This file is part of the KDE project

    SPDX-FileCopyrightText: 2011 Sven Langkamp <sven.langkamp@gmail.com>
    SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KORESOURCETAGSTORE_H
#define KORESOURCETAGSTORE_H

#include "kowidgets_export.h"
#include <WidgetsDebug.h>

class KoResourceServerBase;
class KoResource;
class QString;

/**
 * KoResourceTagging allows to add and delete tags to resources and also search resources using tags
 */
class KOWIDGETS_EXPORT KoResourceTagStore
{
public:
    /**
     * Constructs a KoResourceTagging object
     *
     */
    explicit KoResourceTagStore(KoResourceServerBase *resourceServer);
    ~KoResourceTagStore();

    QStringList assignedTagsList(const KoResource *resource) const;

    /// remote the given resource from the tagstore
    void removeResource(const KoResource *resource);

    /// Add the given tag to the tag store. The resource can be empty, in which case
    /// the tag is added but unused
    void addTag(KoResource *resource, const QString &tag);

    /// Remove the given tag for the given resource.
    void delTag(KoResource *resource, const QString &tag);

    void delTag(const QString &tag);

    /// @return a list of all the tags in this store
    QStringList tagNamesList() const;

    /// Return a list of filenames for the given tag
    QStringList searchTag(const QString &query) const;

    void loadTags();
    void serializeTags();

private:
    friend class KoResourceTaggingTest;

    void readXMLFile(const QString &tagstore);
    void writeXMLFile(const QString &tagstore);

    /// To check whether the resource belongs to the present server or not
    bool isServerResource(const QString &resourceName) const;

    /// If resource filenames have no extensions, then we add "-krita.extension".
    QString adjustedFileName(const QString &fileName) const;

    /// Removes the adjustements before going to the server
    QStringList removeAdjustedFileNames(QStringList fileNamesList) const;

    class Private;
    Private *const d;
};

#endif // KORESOURCETAGSTORE_H
