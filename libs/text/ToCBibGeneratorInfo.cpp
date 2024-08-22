/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Smit Patel <smitpatel24@gmail.com>
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ToCBibGeneratorInfo.h"

#include <KoUnit.h>
#include <KoXmlWriter.h>

IndexEntry::IndexEntry(const QString &_styleName, IndexEntry::IndexEntryName _name)
    : styleName(_styleName)
    , name(_name)
{
}

IndexEntry *IndexEntry::clone()
{
    IndexEntry *newIndexEntry = new IndexEntry(styleName, name);
    return newIndexEntry;
}

IndexEntry::~IndexEntry() = default;

void IndexEntry::addAttributes(KoXmlWriter *writer) const
{
    Q_UNUSED(writer);
}

void IndexEntry::saveOdf(KoXmlWriter *writer) const
{
    switch (name) {
    case LINK_START:
        writer->startElement("text:index-entry-link-start");
        break;
    case CHAPTER:
        writer->startElement("text:index-entry-chapter");
        break;
    case BIBLIOGRAPHY:
        writer->startElement("text:index-entry-bibliography");
        break;
    case SPAN:
        writer->startElement("text:index-entry-span");
        break;
    case TEXT:
        writer->startElement("text:index-entry-text");
        break;
    case TAB_STOP:
        writer->startElement("text:index-entry-tab-stop");
        break;
    case PAGE_NUMBER:
        writer->startElement("text:index-entry-page-number");
        break;
    case LINK_END:
        writer->startElement("text:index-entry-link-end");
        break;
    case UNKNOWN:
        break;
    }

    if (!styleName.isNull()) {
        writer->addAttribute("text:style-name", styleName);
    }

    addAttributes(writer);
    writer->endElement();
}

IndexEntryBibliography::IndexEntryBibliography(const QString &_styleName)
    : IndexEntry(_styleName, IndexEntry::BIBLIOGRAPHY)
    , dataField(QString())
{
}

IndexEntry *IndexEntryBibliography::clone()
{
    IndexEntryBibliography *newIndexEntry = new IndexEntryBibliography(styleName);
    newIndexEntry->dataField = dataField;
    return newIndexEntry;
}

void IndexEntryBibliography::addAttributes(KoXmlWriter *writer) const
{
    if (!dataField.isNull()) {
        writer->addAttribute("text:bibliography-data-field", dataField);
    }
}

IndexEntrySpan::IndexEntrySpan(const QString &_styleName)
    : IndexEntry(_styleName, IndexEntry::SPAN)
{
}

IndexEntry *IndexEntrySpan::clone()
{
    IndexEntrySpan *newIndexEntry = new IndexEntrySpan(styleName);
    newIndexEntry->text = text;
    return newIndexEntry;
}

void IndexEntrySpan::addAttributes(KoXmlWriter *writer) const
{
    if (!text.isNull() && !text.isEmpty()) {
        writer->addTextNode(text);
    }
}

IndexEntryTabStop::IndexEntryTabStop(const QString &_styleName)
    : IndexEntry(_styleName, IndexEntry::TAB_STOP)
{
}

IndexEntry *IndexEntryTabStop::clone()
{
    IndexEntryTabStop *newIndexEntry = new IndexEntryTabStop(styleName);
    newIndexEntry->tab = tab;
    newIndexEntry->m_position = m_position;
    return newIndexEntry;
}

void IndexEntryTabStop::addAttributes(KoXmlWriter *writer) const
{
    writer->addAttribute("style:leader-char", tab.leaderText);
    // If the value of this attribute is left, the style:position attribute shall also be present.
    // Otherwise, this attribute shall be omitted.
    if (tab.type == QTextOption::LeftTab) {
        writer->addAttribute("style:type", "left");
        writer->addAttribute("style:position", m_position);
    } else {
        Q_ASSERT(tab.type == QTextOption::RightTab);
        writer->addAttribute("style:type", "right");
    }
}

void IndexEntryTabStop::setPosition(const QString &position)
{
    m_position = position;
    tab.position = KoUnit::parseValue(position);
}

void BibliographyEntryTemplate::saveOdf(KoXmlWriter *writer) const
{
    writer->startElement("text:bibliography-entry-template");
    writer->addAttribute("text:style-name", styleName);
    writer->addAttribute("text:bibliography-type", bibliographyType);
    foreach (IndexEntry *e, indexEntries) {
        e->saveOdf(writer);
    }

    writer->endElement();
}

void IndexTitleTemplate::saveOdf(KoXmlWriter *writer) const
{
    writer->startElement("text:index-title-template");
    writer->addAttribute("text:style-name", styleName);
    if (!text.isEmpty() && !text.isNull()) {
        writer->addTextNode(text);
    }
    writer->endElement();
}

void IndexSourceStyle::saveOdf(KoXmlWriter *writer) const
{
    writer->startElement("text:index-source-style");
    if (!styleName.isNull()) {
        writer->addAttribute("text:style-name", styleName);
    }
    writer->endElement();
}

IndexSourceStyles::IndexSourceStyles() = default;

IndexSourceStyles::IndexSourceStyles(const IndexSourceStyles &indexSourceStyles)
{
    outlineLevel = indexSourceStyles.outlineLevel;

    foreach (const IndexSourceStyle &style, indexSourceStyles.styles) {
        styles.append(style);
    }
}

IndexSourceStyles &IndexSourceStyles::operator=(const IndexSourceStyles &indexSourceStyles)
{
    outlineLevel = indexSourceStyles.outlineLevel;

    foreach (const IndexSourceStyle &style, indexSourceStyles.styles) {
        styles.append(style);
    }
    return *this;
}

void IndexSourceStyles::saveOdf(KoXmlWriter *writer) const
{
    writer->startElement("text:index-source-styles");
    writer->addAttribute("text:outline-level", outlineLevel);
    foreach (const IndexSourceStyle &s, styles) {
        s.saveOdf(writer);
    }
    writer->endElement();
}

IndexEntryPageNumber::IndexEntryPageNumber(const QString &_styleName)
    : IndexEntry(_styleName, IndexEntry::PAGE_NUMBER)
{
}

IndexEntry *IndexEntryPageNumber::clone()
{
    IndexEntryPageNumber *newIndexEntry = new IndexEntryPageNumber(styleName);
    return newIndexEntry;
}

IndexEntryLinkEnd::IndexEntryLinkEnd(const QString &_styleName)
    : IndexEntry(_styleName, IndexEntry::LINK_END)
{
}

IndexEntry *IndexEntryLinkEnd::clone()
{
    IndexEntryLinkEnd *newIndexEntry = new IndexEntryLinkEnd(styleName);
    return newIndexEntry;
}

TocEntryTemplate::TocEntryTemplate(const TocEntryTemplate &entryTemplate)
{
    outlineLevel = entryTemplate.outlineLevel;
    styleName = entryTemplate.styleName;
    styleId = entryTemplate.styleId;

    foreach (IndexEntry *entry, entryTemplate.indexEntries) {
        indexEntries.append(entry->clone());
    }
}

TocEntryTemplate &TocEntryTemplate::operator=(const TocEntryTemplate &entryTemplate)
{
    outlineLevel = entryTemplate.outlineLevel;
    styleName = entryTemplate.styleName;
    styleId = entryTemplate.styleId;

    foreach (IndexEntry *entry, entryTemplate.indexEntries) {
        indexEntries.append(entry->clone());
    }
    return *this;
}

void TocEntryTemplate::saveOdf(KoXmlWriter *writer) const
{
    writer->startElement("text:table-of-content-entry-template");
    writer->addAttribute("text:outline-level", outlineLevel);
    writer->addAttribute("text:style-name", styleName);

    foreach (IndexEntry *e, indexEntries) {
        e->saveOdf(writer);
    }

    writer->endElement();
}

IndexEntryText::IndexEntryText(const QString &_styleName)
    : IndexEntry(_styleName, IndexEntry::TEXT)
{
}

IndexEntry *IndexEntryText::clone()
{
    IndexEntryText *newIndexEntry = new IndexEntryText(styleName);
    return newIndexEntry;
}

IndexEntryLinkStart::IndexEntryLinkStart(const QString &_styleName)
    : IndexEntry(_styleName, IndexEntry::LINK_START)
{
}

IndexEntry *IndexEntryLinkStart::clone()
{
    return new IndexEntryLinkStart(styleName);
}

IndexEntryChapter::IndexEntryChapter(const QString &_styleName)
    : IndexEntry(_styleName, IndexEntry::CHAPTER)
    , display(QString())
    , outlineLevel(INVALID_OUTLINE_LEVEL)
{
}

IndexEntry *IndexEntryChapter::clone()
{
    IndexEntryChapter *newIndexEntry = new IndexEntryChapter(styleName);
    newIndexEntry->outlineLevel = outlineLevel;
    newIndexEntry->display = display;
    return newIndexEntry;
}

void IndexEntryChapter::addAttributes(KoXmlWriter *writer) const
{
    if (!display.isNull()) {
        writer->addAttribute("text:display", display);
    }
    writer->addAttribute("text:outline-level", outlineLevel);
}

BibliographyEntryTemplate::BibliographyEntryTemplate() = default;

BibliographyEntryTemplate::BibliographyEntryTemplate(const QString &type, const QList<IndexEntry *> &entries)
    : indexEntries(entries)
    , bibliographyType(type)
{
}

BibliographyEntryTemplate::BibliographyEntryTemplate(const BibliographyEntryTemplate &entryTemplate)
{
    styleName = entryTemplate.styleName;
    styleId = entryTemplate.styleId;

    foreach (IndexEntry *entry, entryTemplate.indexEntries) {
        indexEntries.append(entry->clone());
    }

    bibliographyType = entryTemplate.bibliographyType;
}

BibliographyEntryTemplate &BibliographyEntryTemplate::operator=(const BibliographyEntryTemplate &entryTemplate)
{
    styleName = entryTemplate.styleName;
    styleId = entryTemplate.styleId;

    foreach (IndexEntry *entry, entryTemplate.indexEntries) {
        indexEntries.append(entry->clone());
    }

    bibliographyType = entryTemplate.bibliographyType;
    return *this;
}
