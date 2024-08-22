/*
 *  SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@valdyas.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOTEXTWRITER_P_H
#define KOTEXTWRITER_P_H

#include <QHash>
#include <QMap>
#include <QPair>
#include <QStack>
#include <QString>

#include <KoTextWriter.h>
#include <KoXmlReaderForward.h>

class KoInlineObject;
class KoTextInlineRdf;
class KoList;
class KoShapeSavingContext;
class KoXmlWriter;
class KoTableColumnStyle;
class KoTextSharedSavingData;
class KoTableRowStyle;
class KoDocumentRdfBase;

class QTextDocument;
class QTextTable;
class QTextTableCellFormat;
class QTextList;
class QTextStream;

/**
 * XXX: Apidox!
 */
class TagInformation
{
public:
    TagInformation()
        : tagName(nullptr)
        , attributeList()
    {
    }

    void setTagName(const char *tagName)
    {
        this->tagName = tagName;
    }

    void addAttribute(const QString &attributeName, const QString &attributeValue)
    {
        attributeList.push_back(QPair<QString, QString>(attributeName, attributeValue));
    }

    void addAttribute(const QString &attributeName, int value)
    {
        addAttribute(attributeName, QString::number(value));
    }

    void clear()
    {
        tagName = nullptr;
        attributeList.clear();
    }

    const char *name() const
    {
        return tagName;
    }

    const QVector<QPair<QString, QString>> &attributes() const
    {
        return attributeList;
    }

private:
    const char *tagName;
    QVector<QPair<QString, QString>> attributeList;
};

/**
 * XXX: Apidox!
 */
class Q_DECL_HIDDEN KoTextWriter::Private
{
public:
    explicit Private(KoShapeSavingContext &context);

    ~Private() = default;

    void writeBlocks(QTextDocument *document,
                     int from,
                     int to,
                     QHash<QTextList *, QString> &listStyles,
                     QTextTable *currentTable = nullptr,
                     QTextList *currentList = nullptr);
    QHash<QTextList *, QString> saveListStyles(QTextBlock block, int to);

private:
    enum ElementType { Span, ParagraphOrHeader, ListItem, List, NumberedParagraph, Table, TableRow, TableColumn, TableCell };

    void openTagRegion(KoTextWriter::Private::ElementType elementType, TagInformation &tagInformation);
    void closeTagRegion();

    QString saveParagraphStyle(const QTextBlock &block);
    QString saveParagraphStyle(const QTextBlockFormat &blockFormat, const QTextCharFormat &charFormat);
    QString saveCharacterStyle(const QTextCharFormat &charFormat, const QTextCharFormat &blockCharFormat);
    QString saveTableStyle(const QTextTable &table);
    QString saveTableColumnStyle(const KoTableColumnStyle &columnStyle, int columnNumber, const QString &tableStyleName);
    QString saveTableRowStyle(const KoTableRowStyle &rowStyle, int rowNumber, const QString &tableStyleName);
    QString saveTableCellStyle(const QTextTableCellFormat &cellFormat, int columnNumber, const QString &tableStyleName);

    void saveParagraph(const QTextBlock &block, int from, int to);
    void saveTable(QTextTable *table, QHash<QTextList *, QString> &listStyles, int from, int to);
    QTextBlock &saveList(QTextBlock &block, QHash<QTextList *, QString> &listStyles, int level, QTextTable *currentTable);
    void saveTableOfContents(QTextDocument *document, QHash<QTextList *, QString> &listStyles, QTextBlock toc);
    void saveBibliography(QTextDocument *document, QHash<QTextList *, QString> &listStyles, QTextBlock bib);
    void saveInlineRdf(KoTextInlineRdf *rdf, TagInformation *tagInfos);

    void addNameSpaceDefinitions(QString &generatedXmlString);

    // Common methods
    void writeAttributes(QTextStream &outputXmlStream, KoXmlElement &element);
    void writeNode(QTextStream &outputXmlStream, KoXmlNode &node, bool writeOnlyChildren = false);

    QString createXmlId();

public:
    KoDocumentRdfBase *rdfData;
    KoTextSharedSavingData *sharedData;
    KoStyleManager *styleManager;
    QTextDocument *document;
    int globalFrom; // to and from positions, relevant for finding matching bookmarks etc
    int globalTo;

private:
    KoXmlWriter *writer;

    QStack<const char *> openedTagStack;

    KoShapeSavingContext &context;

    // Things like bookmarks need to be properly turn down during a cut and paste operation
    // when their end markeris not included in the selection. However, when recursing into
    // e.g. the QTextDocument of a table, we need have a clean slate. Hence, a stack of stacks.
    QStack<QStack<KoInlineObject *> *> pairedInlineObjectsStackStack;
    QStack<KoInlineObject *> *currentPairedInlineObjectsStack;

    QMap<KoList *, QString> listXmlIds;

    QMap<KoList *, QString> numberedParagraphListIds;
};

#endif // KOTEXTWRITER_P_H
