/* This file is part of the KDE project
 * Copyright (C) 2006-2007, 2010 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Girish Ramakrishnan <girish@forwardbias.in>
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

#include "ListItemsHelper.h"

#include <KoTextBlockData.h>
#include <KoParagraphStyle.h>
#include <KoTextDocument.h>
#include <KoList.h>
#include <KoOdfNumberDefinition.h>

#include <TextLayoutDebug.h>
#include <klocalizedstring.h>
#include <QTextList>


// ------------------- ListItemsHelper ------------
/// \internal helper class for calculating text-lists prefixes and indents
ListItemsHelper::ListItemsHelper(QTextList *textList, const QFont &font)
        : m_textList(textList)
        , m_fm(font, textList->document()->documentLayout()->paintDevice())
{
}

void ListItemsHelper::recalculateBlock(QTextBlock &block)
{
    //warnTextLayout;
    const QTextListFormat format = m_textList->format();
    const KoListStyle::LabelType labelType = static_cast<KoListStyle::LabelType>(format.style());

    const QString prefix = format.stringProperty(KoListStyle::ListItemPrefix);
    const QString suffix = format.stringProperty(KoListStyle::ListItemSuffix);
    const int level = format.intProperty(KoListStyle::Level);
    int dp = format.intProperty(KoListStyle::DisplayLevel);
    if (dp > level)
        dp = level;
    const int displayLevel = dp ? dp : 1;

    QTextBlockFormat blockFormat = block.blockFormat();

    // Look if we have a block that is inside a header. We need to special case them cause header-lists are
    // different from any other kind of list and they do build up there own global list (table of content).
    bool isOutline = blockFormat.intProperty(KoParagraphStyle::OutlineLevel) > 0;

    int startValue = 1;
    if (format.hasProperty(KoListStyle::StartValue))
        startValue = format.intProperty(KoListStyle::StartValue);

    int index = startValue;
    bool fixed = false;
    if (blockFormat.boolProperty(KoParagraphStyle::RestartListNumbering)) {
        index = format.intProperty(KoListStyle::StartValue);
        fixed = true;
    }
    const int paragIndex = blockFormat.intProperty(KoParagraphStyle::ListStartValue);
    if (paragIndex > 0) {
        index = paragIndex;
        fixed = true;
    }

    if (!fixed) {
        //if this is the first item then find if the list has to be continued from any other list
        KoList *listContinued = 0;
        if (m_textList->itemNumber(block) == 0 && KoTextDocument(m_textList->document()).list(m_textList) && (listContinued = KoTextDocument(m_textList->document()).list(m_textList)->listContinuedFrom())) {
            //find the previous list of the same level
            QTextList *previousTextList = listContinued->textLists().at(level - 1).data();
            if (previousTextList) {
                QTextBlock textBlock = previousTextList->item(previousTextList->count() - 1);
                if (textBlock.isValid()) {
                    index = KoTextBlockData(textBlock).counterIndex() + 1; //resume the previous list count
                }
            }
        } else if (m_textList->itemNumber(block) > 0) {
            QTextBlock textBlock = m_textList->item(m_textList->itemNumber(block) - 1);
            if (textBlock.isValid()) {
                index = KoTextBlockData(textBlock).counterIndex() + 1; //resume the previous list count
            }
        }
    }

    qreal width = 0.0;
    KoTextBlockData blockData(block);

    if (blockFormat.boolProperty(KoParagraphStyle::UnnumberedListItem)
        || blockFormat.boolProperty(KoParagraphStyle::IsListHeader)) {
        blockData.setCounterPlainText(QString());
        blockData.setCounterPrefix(QString());
        blockData.setCounterSuffix(QString());
        blockData.setPartialCounterText(QString());
        // set the counter for the current un-numbered list to the counter index of the previous list item.
        // index-1 because the list counter would have already incremented by one
        blockData.setCounterIndex(index - 1);
        if (blockFormat.boolProperty(KoParagraphStyle::IsListHeader)) {
            blockData.setCounterWidth(format.doubleProperty(KoListStyle::MinimumWidth));
            blockData.setCounterSpacing(0);
        }
        return;
    }

    QString item;
    if (displayLevel > 1) {
        int checkLevel = level;
        int tmpDisplayLevel = displayLevel;
        bool counterResetRequired = true;
        for (QTextBlock b = block.previous(); tmpDisplayLevel > 1 && b.isValid(); b = b.previous()) {
            if (b.textList() == 0)
                continue;
            QTextListFormat lf = b.textList()->format();
            if (lf.property(KoListStyle::StyleId) != format.property(KoListStyle::StyleId))
               continue; // uninteresting for us
            if (isOutline != bool(b.blockFormat().intProperty(KoParagraphStyle::OutlineLevel)))
                continue; // also uninteresting cause the one is an outline-listitem while the other is not

            if (! KoListStyle::isNumberingStyle(static_cast<KoListStyle::LabelType>(lf.style()))) {
                continue;
            }

            if (b.blockFormat().boolProperty(KoParagraphStyle::UnnumberedListItem)) {
                continue; //unnumbered listItems are irrelevant
            }

            const int otherLevel  = lf.intProperty(KoListStyle::Level);
            if (isOutline && checkLevel == otherLevel) {
                counterResetRequired = false;
            }

            if (checkLevel <= otherLevel)
                continue;

            KoTextBlockData otherData(b);
            if (!otherData.hasCounterData()) {
                continue;
            }
            if (tmpDisplayLevel - 1 < otherLevel) { // can't just copy it fully since we are
                // displaying less then the full counter
                item += otherData.partialCounterText();
                tmpDisplayLevel--;
                checkLevel--;
                for (int i = otherLevel + 1; i < level; i++) {
                    tmpDisplayLevel--;
                    KoOdfNumberDefinition numberFormat;
                    numberFormat.setFormatSpecification(static_cast<KoOdfNumberDefinition::FormatSpecification>(format.intProperty(KoListStyle::NumberFormat)));
                    numberFormat.setLetterSynchronization(format.boolProperty(KoListStyle::LetterSynchronization));
                    item += "." + numberFormat.formattedNumber(index); // add missing counters.
                }
            } else { // just copy previous counter as prefix
                QString otherPrefix = lf.stringProperty(KoListStyle::ListItemPrefix);
                QString otherSuffix = lf.stringProperty(KoListStyle::ListItemSuffix);
                QString pureCounter = otherData.counterText().mid(otherPrefix.size());
                pureCounter = pureCounter.left(pureCounter.size() - otherSuffix.size());
                item += pureCounter;
                for (int i = otherLevel + 1; i < level; i++) {
                    KoOdfNumberDefinition numberFormat;
                    numberFormat.setFormatSpecification(static_cast<KoOdfNumberDefinition::FormatSpecification>(format.intProperty(KoListStyle::NumberFormat)));
                    numberFormat.setLetterSynchronization(format.boolProperty(KoListStyle::LetterSynchronization));
                    item += "." + numberFormat.formattedNumber(index); // add missing counters.
                }
                tmpDisplayLevel = 0;
                if (isOutline && counterResetRequired) {
                    index = 1;
                }
                break;
            }
        }
        for (int i = 1; i < tmpDisplayLevel; i++) {
            KoOdfNumberDefinition numberFormat;
            numberFormat.setFormatSpecification(static_cast<KoOdfNumberDefinition::FormatSpecification>(format.intProperty(KoListStyle::NumberFormat)));
            numberFormat.setLetterSynchronization(format.boolProperty(KoListStyle::LetterSynchronization));
            item = numberFormat.formattedNumber(index) + "." + item; // add missing counters.
        }
    }
    bool calcWidth = true;
    QString partialCounterText;
    switch (labelType) {
    case KoListStyle::NumberLabelType: {
        KoOdfNumberDefinition::FormatSpecification spec = static_cast<KoOdfNumberDefinition::FormatSpecification>(format.intProperty(KoListStyle::NumberFormat));

        if (!(item.isEmpty() || item.endsWith('.') || item.endsWith(' '))) {
            if (spec == KoOdfNumberDefinition::Numeric || spec == KoOdfNumberDefinition::AlphabeticLowerCase ||
            spec == KoOdfNumberDefinition::AlphabeticUpperCase ||
            spec == KoOdfNumberDefinition::RomanLowerCase ||
            spec == KoOdfNumberDefinition::RomanUpperCase) {
                item += '.';
            }
        }

        KoOdfNumberDefinition numberFormat;
        numberFormat.setFormatSpecification(spec);
        partialCounterText = numberFormat.formattedNumber(index);
        break;
    }
    case KoListStyle::BulletCharLabelType: {
        calcWidth = false;
        if (format.intProperty(KoListStyle::BulletCharacter))
            item = QString(QChar(format.intProperty(KoListStyle::BulletCharacter)));
        width = m_fm.width(item);
        int percent = format.intProperty(KoListStyle::RelativeBulletSize);
        if (percent > 0)
            width = width * (percent / 100.0);
        break;
    }
    case KoListStyle::None:
        calcWidth = false;
        width =  0.0;
        break;
    case KoListStyle::ImageLabelType:
        calcWidth = false;
        width = qMax(format.doubleProperty(KoListStyle::Width), (qreal)1.0);
        break;
    default:  // others we ignore.
        calcWidth = false;
    }

    blockData.setCounterIsImage(labelType == KoListStyle::ImageLabelType);
    blockData.setPartialCounterText(partialCounterText);
    blockData.setCounterIndex(index);
    item += partialCounterText;
    blockData.setCounterPlainText(item);
    blockData.setCounterPrefix(prefix);
    blockData.setCounterSuffix(suffix);
    if (calcWidth)
        width = m_fm.width(item);
    index++;

    width += m_fm.width(prefix + suffix);

    qreal counterSpacing = 0;
    if (format.boolProperty(KoListStyle::AlignmentMode)) {
        // for aligmentmode spacing should be 0
        counterSpacing = 0;
    } else {
        if (labelType != KoListStyle::None) {
            // see ODF spec 1.2 item 20.422
            counterSpacing = format.doubleProperty(KoListStyle::MinimumDistance);
            if (width < format.doubleProperty(KoListStyle::MinimumWidth)) {
                counterSpacing -= format.doubleProperty(KoListStyle::MinimumWidth) - width;
            }
            counterSpacing = qMax(counterSpacing, qreal(0.0));
        }
        width = qMax(width, format.doubleProperty(KoListStyle::MinimumWidth));
    }
    blockData.setCounterWidth(width);
    blockData.setCounterSpacing(counterSpacing);
}

// static
bool ListItemsHelper::needsRecalc(QTextList *textList)
{
    Q_ASSERT(textList);
    QTextBlock tb = textList->item(0);
    KoTextBlockData blockData(tb);
    return !blockData.hasCounterData();
}
