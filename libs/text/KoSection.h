/*
 *  SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@valdyas.org>
 *  SPDX-FileCopyrightText: 2014-2015 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#ifndef KOSECTION_H
#define KOSECTION_H

#include "kotext_export.h"

#include <QList>
#include <QMetaType>
#include <QPair>
#include <QScopedPointer>
#include <QString>
#include <QTextCursor>

class KoXmlElement;
class KoShapeSavingContext;
class KoTextSharedLoadingData;
class KoSectionEnd;
class KoElementReference;
class KoTextInlineRdf;

class KoSectionPrivate;
/**
 * Contains the information about the current text:section.
 *
 * The <text:section> element has the following attributes:
 *
 * <ul>
 * <li>text:condition
 * <li>text:display
 * <li>text:name
 * <li>text:protected
 * <li>text:protection-key
 * <li>text:protection-key-digest-algorithm
 * <li>text:style-name
 * <li>xml:id
 * </ul>
 * (odf spec v.12)
 */
class KOTEXT_EXPORT KoSection
{
public:
    ~KoSection();

    /// Returns section name
    QString name() const;
    /// Returns starting and ending position of section in QTextDocument
    QPair<int, int> bounds() const;
    /// Returns section level. Root section has @c 0 level.
    int level() const;

    /** Returns inlineRdf associated with section
     * @return pointer to the KoTextInlineRdf for this section
     */
    KoTextInlineRdf *inlineRdf() const;

    /** Sets KoTextInlineRdf for this section
     * @param inlineRdf pointer to KoTextInlineRdf to set
     */
    void setInlineRdf(KoTextInlineRdf *inlineRdf);

    bool loadOdf(const KoXmlElement &element, KoTextSharedLoadingData *sharedData, bool stylesDotXml);
    void saveOdf(KoShapeSavingContext &context) const;

protected:
    const QScopedPointer<KoSectionPrivate> d_ptr;

private:
    Q_DISABLE_COPY(KoSection)
    Q_DECLARE_PRIVATE(KoSection)

    explicit KoSection(const QTextCursor &cursor, const QString &name, KoSection *parent);

    /// Changes section's name to @param name
    void setName(const QString &name);

    /// Sets paired KoSectionsEnd for this section.
    void setSectionEnd(KoSectionEnd *sectionEnd);

    /**
     * Sets level of section in section tree.
     * Root sections have @c 0 level.
     */
    void setLevel(int level);

    /// Returns a pointer to the parent of the section in tree.
    KoSection *parent() const;

    /// Returns a vector of pointers to the children of the section.
    QVector<KoSection *> children() const;

    /**
     * Specifies if end bound of section should stay on place when inserting text.
     * Used by KoTextLoader on document loading.
     * @see QTextCursor::setKeepPositionOnInsert(bool)
     */
    void setKeepEndBound(bool state);

    /**
     * Inserts @param section to position @param childIdx of children
     */
    void insertChild(int childIdx, KoSection *section);

    /**
     * Removes child on position @param childIdx
     */
    void removeChild(int childIdx);

    friend class KoSectionModel;
    friend class KoTextLoader; // accesses setKeepEndBound() function
    friend class KoSectionEnd;
    friend class TestKoTextEditor; // accesses setKeepEndBound() function
};

Q_DECLARE_METATYPE(KoSection *)
Q_DECLARE_METATYPE(QList<KoSection *>)

#endif // KOSECTION_H
