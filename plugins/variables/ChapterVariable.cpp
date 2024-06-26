/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Sebastian Sauer <mail@dipe.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ChapterVariable.h"

#include "VariablesDebug.h"

#include <KoParagraphStyle.h>
#include <KoProperties.h>
#include <KoShape.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoTextBlockData.h>
#include <KoTextDocumentLayout.h>
#include <KoTextLayoutRootArea.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

#include <KLocalizedString>

#include <QAbstractTextDocumentLayout>
#include <QComboBox>
#include <QFontMetricsF>
#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>
#include <QTextDocument>
#include <QTextInlineObject>

ChapterVariable::ChapterVariable()
    : KoVariable(true)
    , m_format(ChapterName)
    , m_level(1)
{
}

void ChapterVariable::readProperties(const KoProperties *props)
{
    m_format = (FormatTypes)props->intProperty("format");
    m_level = qMax(1, props->intProperty("level"));
}

void ChapterVariable::resize(const QTextDocument *_document, QTextInlineObject &object, int _posInDocument, const QTextCharFormat &format, QPaintDevice *pd)
{
    QTextDocument *document = const_cast<QTextDocument *>(_document);
    int posInDocument = _posInDocument;
    bool checkBackwards = true;
    QTextFrame::iterator startIt, endIt;

    KoTextDocumentLayout *lay = qobject_cast<KoTextDocumentLayout *>(document->documentLayout());
    KoTextDocumentLayout *ref = lay->referencedLayout();
    if (ref) {
        KoTextLayoutRootArea *rootArea = lay->rootAreaForPosition(posInDocument);
        if (!rootArea) {
            KoVariable::resize(_document, object, _posInDocument, format, pd);
            return; // not ready yet
        }
        KoTextPage *page = rootArea->page();
        if (!page) {
            KoVariable::resize(_document, object, _posInDocument, format, pd);
            return; // should not happen
        }
        int pagenumber = page->pageNumber();
        foreach (KoTextLayoutRootArea *a, ref->rootAreas()) {
            KoTextPage *p = a->page();
            if (!p || p->pageNumber() != pagenumber)
                continue;
            startIt = a->startTextFrameIterator();
            endIt = a->endTextFrameIterator();
            if (startIt.currentBlock().isValid())
                posInDocument = startIt.currentBlock().position();
            else if (startIt.currentFrame())
                posInDocument = startIt.currentFrame()->firstCursorPosition().position();
            else // abort
                break;
            document = ref->document();
            checkBackwards = false; // check forward
            break;
        }
        if (document == _document) {
            KoVariable::resize(_document, object, _posInDocument, format, pd);
            return; // should not happen
        }
    }

    QTextBlock block = document->findBlock(posInDocument);
    while (block.isValid()) {
        if (block.blockFormat().hasProperty(KoParagraphStyle::OutlineLevel)) {
            int level = block.blockFormat().intProperty(KoParagraphStyle::OutlineLevel);
            if (level == m_level) {
                KoTextBlockData data(block);
                switch (m_format) {
                case ChapterName:
                    setValue(block.text());
                    break;
                case ChapterNumber:
                    setValue(data.counterText());
                    break;
                case ChapterNumberName:
                    setValue(QString("%1 %2").arg(data.counterText(), block.text()));
                    break;
                case ChapterPlainNumber:
                    setValue(data.counterPlainText());
                    break;
                case ChapterPlainNumberName:
                    setValue(QString("%1 %2").arg(data.counterPlainText(), block.text()));
                    break;
                default:
                    break;
                }
                break; // job done
            }
        }

        if (checkBackwards) {
            block = block.previous();
        } else {
            block = block.next();

            // If we search forwards and reached the end of the page then we continue searching backwards
            // at the beginning of the page.
            if (!block.isValid() || (endIt.currentBlock().isValid() && block.position() > endIt.currentBlock().position())
                || (endIt.currentFrame() && block.position() > endIt.currentFrame()->firstCursorPosition().block().position())) {
                if (startIt.currentBlock().isValid())
                    block = startIt.currentBlock();
                else if (startIt.currentFrame())
                    block = startIt.currentFrame()->firstCursorPosition().block();
                else // abort
                    break;
                checkBackwards = true;
            }
        }
    }

    KoVariable::resize(_document, object, _posInDocument, format, pd);
}

void ChapterVariable::saveOdf(KoShapeSavingContext &context)
{
    KoXmlWriter *writer = &context.xmlWriter();
    writer->startElement("text:chapter ", false);
    switch (m_format) {
    case ChapterName:
        writer->addAttribute("text:display", "name");
        break;
    case ChapterNumber:
        writer->addAttribute("text:display", "number");
        break;
    case ChapterNumberName:
        writer->addAttribute("text:display", "number-and-name");
        break;
    case ChapterPlainNumber:
        writer->addAttribute("text:display", "plain-number");
        break;
    case ChapterPlainNumberName:
        writer->addAttribute("text:display", "plain-number-and-name");
        break;
    default:
        break;
    }
    writer->addAttribute("text:outline-level", m_level);
    writer->addTextNode(value());
    writer->endElement(); // text:chapter
}

bool ChapterVariable::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(context);

    const QString display = element.attributeNS(KoXmlNS::text, "display", QString());
    if (display == "name") {
        m_format = ChapterName;
    } else if (display == "number") {
        m_format = ChapterNumber;
    } else if (display == "number-and-name") {
        m_format = ChapterNumberName;
    } else if (display == "plain-number") {
        m_format = ChapterPlainNumber;
    } else if (display == "plain-number-and-name") {
        m_format = ChapterPlainNumberName;
    } else { // fallback
        m_format = ChapterNumberName;
    }

    m_level = qMax(1, element.attributeNS(KoXmlNS::text, "outline-level", QString()).toInt());

    return true;
}

QWidget *ChapterVariable::createOptionsWidget()
{
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);
    layout->setColumnStretch(1, 1);
    widget->setLayout(layout);

    QLabel *formatLabel = new QLabel(i18n("Format:"), widget);
    formatLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(formatLabel, 0, 0);
    QComboBox *formatEdit = new QComboBox(widget);
    formatLabel->setBuddy(formatEdit);
    formatEdit->addItems(QStringList() << i18n("Number") << i18n("Name") << i18n("Number and name") << i18n("Number without separator")
                                       << i18n("Number and name without separator"));
    formatEdit->setCurrentIndex(2);
    layout->addWidget(formatEdit, 0, 1);

    QLabel *levelLabel = new QLabel(i18n("Level:"), widget);
    levelLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(levelLabel, 1, 0);
    QSpinBox *levelEdit = new QSpinBox(widget);
    levelLabel->setBuddy(levelEdit);
    levelEdit->setMinimum(1);
    levelEdit->setValue(m_level);
    layout->addWidget(levelEdit, 1, 1);

    connect(formatEdit, &QComboBox::currentIndexChanged, this, &ChapterVariable::formatChanged);
    connect(levelEdit, &QSpinBox::valueChanged, this, &ChapterVariable::levelChanged);

    return widget;
}

void ChapterVariable::formatChanged(int format)
{
    m_format = (FormatTypes)format;
}

void ChapterVariable::levelChanged(int level)
{
    m_level = level;
}
