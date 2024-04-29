/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Pierre Stirnweiss <pierre.stirnweiss_calligra@gadz.org>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOCHANGETRACKER_H
#define KOCHANGETRACKER_H

#include "kotext_export.h"
#include <KoXmlReaderForward.h>

#include <QMetaType>
#include <QObject>
#include <QVector>

#include <KoGenChange.h>

class KUndo2MagicString;

class KoGenChanges;
class KoChangeTrackerElement;
class KoFormatChangeInformation;

class QTextCursor;
class QTextFormat;
class QString;
class QTextDocumentFragment;
class QTextList;

class KOTEXT_EXPORT KoChangeTracker : public QObject
{
    Q_OBJECT
public:
    enum ChangeSaveFormat { ODF_1_2 = 0, DELTAXML, UNKNOWN = 9999 };

    explicit KoChangeTracker(QObject *parent = nullptr);
    ~KoChangeTracker() override;

    void setRecordChanges(bool enabled);
    bool recordChanges() const;

    void setDisplayChanges(bool enabled);
    bool displayChanges() const;

    /// XXX: these three are called "getXXX" but do change the state of the change tracker
    int getFormatChangeId(const KUndo2MagicString &title, const QTextFormat &format, const QTextFormat &prevFormat, int existingChangeId);
    int getInsertChangeId(const KUndo2MagicString &title, int existingChangeId);
    int getDeleteChangeId(const KUndo2MagicString &title, const QTextDocumentFragment &selection, int existingChangeId);

    void setFormatChangeInformation(int formatChangeId, KoFormatChangeInformation *formatInformation);
    KoFormatChangeInformation *formatChangeInformation(int formatChangeId) const;

    KoChangeTrackerElement *elementById(int id) const;
    bool removeById(int id, bool freeMemory = true);

    // Returns all the deleted changes
    int getDeletedChanges(QVector<KoChangeTrackerElement *> &deleteVector) const;

    bool containsInlineChanges(const QTextFormat &format) const;
    int mergeableId(KoGenChange::Type type, const KUndo2MagicString &title, int existingId) const;

    QColor getInsertionBgColor() const;
    QColor getDeletionBgColor() const;
    QColor getFormatChangeBgColor() const;

    void setInsertionBgColor(const QColor &bgColor);
    void setDeletionBgColor(const QColor &color);
    void setFormatChangeBgColor(const QColor &color);

    /// Splits a changeElement. This creates a duplicate changeElement with a different changeId. This is used because we do not support overlapping change
    /// regions. The function returns the new changeId
    int split(int changeId);

    bool isParent(int testedParentId, int testedChildId) const;
    void setParent(int child, int parent);
    int parent(int changeId) const;

    int createDuplicateChangeId(int existingChangeId);
    bool isDuplicateChangeId(int duplicateChangeId) const;
    int originalChangeId(int duplicateChangeId) const;

    void acceptRejectChange(int changeId, bool set);

    /// Load/save methods
    bool saveInlineChange(int changeId, KoGenChange &change);

    /**
     * @brief saveInlineChanges saves all the changes in the internal map, except
     * for the delete changes, which are changed independently using saveInlineChange.
     * @return an updated table of numerical, internal changeid's to xml:id strings.
     */
    QMap<int, QString> saveInlineChanges(QMap<int, QString> changeTransTable, KoGenChanges &genChanges);

    void loadOdfChanges(const KoXmlElement &element);
    int getLoadedChangeId(const QString &odfId) const;

    static QTextDocumentFragment generateDeleteFragment(const QTextCursor &cursor);
    static void insertDeleteFragment(QTextCursor &cursor);
    static int fragmentLength(const QTextDocumentFragment &fragment);

    QString authorName() const;
    void setAuthorName(const QString &authorName);

    ChangeSaveFormat saveFormat() const;
    void setSaveFormat(ChangeSaveFormat saveFormat);

private:
    static bool checkListDeletion(const QTextList &list, const QTextCursor &cursor);
    class Private;
    Private *const d;
};

Q_DECLARE_METATYPE(KoChangeTracker *)

#endif
