/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TOCBIBGENERATORINFO_H
#define TOCBIBGENERATORINFO_H

#include "KoText.h"

const int INVALID_OUTLINE_LEVEL = 0;

class KoXmlWriter;

class KOTEXT_EXPORT IndexEntry
{
public:
    enum IndexEntryName { UNKNOWN, LINK_START, CHAPTER, SPAN, TEXT, TAB_STOP, PAGE_NUMBER, LINK_END, BIBLIOGRAPHY };

    explicit IndexEntry(const QString &_styleName, IndexEntryName _name = IndexEntry::UNKNOWN);
    virtual IndexEntry *clone();
    virtual ~IndexEntry();
    virtual void addAttributes(KoXmlWriter *writer) const;
    void saveOdf(KoXmlWriter *writer) const;

    QString styleName;
    IndexEntryName name;
};

class IndexEntryLinkStart : public IndexEntry
{
public:
    explicit IndexEntryLinkStart(const QString &_styleName);
    IndexEntry *clone() override;
};

class IndexEntryChapter : public IndexEntry
{
public:
    explicit IndexEntryChapter(const QString &_styleName);
    IndexEntry *clone() override;
    void addAttributes(KoXmlWriter *writer) const override;

    QString display;
    int outlineLevel;
};

class KOTEXT_EXPORT IndexEntrySpan : public IndexEntry
{
public:
    explicit IndexEntrySpan(const QString &_styleName);
    IndexEntry *clone() override;
    void addAttributes(KoXmlWriter *writer) const override;

    QString text;
};

class IndexEntryText : public IndexEntry
{
public:
    explicit IndexEntryText(const QString &_styleName);
    IndexEntry *clone() override;
};

class KOTEXT_EXPORT IndexEntryTabStop : public IndexEntry
{
public:
    explicit IndexEntryTabStop(const QString &_styleName);
    IndexEntry *clone() override;
    void addAttributes(KoXmlWriter *writer) const override;
    // for saving let's save the original unit,
    // for KoText::Tab we need to convert to PostScript points
    void setPosition(const QString &position);

    KoText::Tab tab;
    QString m_position;
};

class IndexEntryPageNumber : public IndexEntry
{
public:
    explicit IndexEntryPageNumber(const QString &_styleName);
    IndexEntry *clone() override;
};

class IndexEntryLinkEnd : public IndexEntry
{
public:
    explicit IndexEntryLinkEnd(const QString &_styleName);
    IndexEntry *clone() override;
};

class KOTEXT_EXPORT TocEntryTemplate
{
public:
    TocEntryTemplate() = default;
    TocEntryTemplate(const TocEntryTemplate &other);
    TocEntryTemplate &operator=(const TocEntryTemplate &other);
    void saveOdf(KoXmlWriter *writer) const;

    int outlineLevel;
    QString styleName;
    int styleId;
    QList<IndexEntry *> indexEntries;
};

class KOTEXT_EXPORT IndexTitleTemplate
{
public:
    void saveOdf(KoXmlWriter *writer) const;

    QString styleName;
    int styleId;
    QString text;
};

class KOTEXT_EXPORT IndexSourceStyle
{
public:
    void saveOdf(KoXmlWriter *writer) const;

    QString styleName;
    int styleId;
};

class KOTEXT_EXPORT IndexSourceStyles
{
public:
    IndexSourceStyles();
    IndexSourceStyles(const IndexSourceStyles &indexSourceStyles);
    IndexSourceStyles &operator=(const IndexSourceStyles &indexSourceStyles);
    void saveOdf(KoXmlWriter *writer) const;

    int outlineLevel;
    QList<IndexSourceStyle> styles;
};

class KOTEXT_EXPORT IndexEntryBibliography : public IndexEntry
{
public:
    explicit IndexEntryBibliography(const QString &_styleName);
    IndexEntry *clone() override;
    void addAttributes(KoXmlWriter *writer) const override;

    QString dataField;
};

class KOTEXT_EXPORT BibliographyEntryTemplate
{
public:
    BibliographyEntryTemplate();
    BibliographyEntryTemplate(const QString &type, const QList<IndexEntry *> &entries);
    BibliographyEntryTemplate(const BibliographyEntryTemplate &other);
    BibliographyEntryTemplate &operator=(const BibliographyEntryTemplate &);
    void saveOdf(KoXmlWriter *writer) const;

    QString styleName;
    int styleId;
    QList<IndexEntry *> indexEntries;
    QString bibliographyType;
};

Q_DECLARE_METATYPE(IndexEntry::IndexEntryName)
#endif // TOCBIBGENERATORINFO_H
