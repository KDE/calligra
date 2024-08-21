/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextBlockData.h"

#include "KoTextBlockBorderData.h"
#include "KoTextBlockPaintStrategyBase.h"

class Q_DECL_HIDDEN KoTextBlockData::Private : public QTextBlockUserData
{
public:
    Private()
        : counterWidth(-1.0)
        , counterSpacing(0)
        , counterIsImage(false)
        , counterIndex(1)
        , border(nullptr)
        , paintStrategy(nullptr)
    {
        layoutedMarkupRanges[KoTextBlockData::Misspell] = false;
        layoutedMarkupRanges[KoTextBlockData::Grammar] = false;
    }

    ~Private() override
    {
        if (border && !border->deref())
            delete border;
        delete paintStrategy;
    }
    qreal counterWidth;
    qreal counterSpacing;
    QString counterPrefix;
    QString counterPlainText;
    QString counterSuffix;
    QString partialCounterText;
    bool counterIsImage;
    int counterIndex;
    QPointF counterPos;
    QTextCharFormat labelFormat;
    KoTextBlockBorderData *border;
    KoTextBlockPaintStrategyBase *paintStrategy;
    QMap<KoTextBlockData::MarkupType, QVector<MarkupRange>> markupRangesMap;
    QMap<KoTextBlockData::MarkupType, bool> layoutedMarkupRanges;
};

KoTextBlockData::KoTextBlockData(QTextBlock &block)
    : d(block.userData() ? dynamic_cast<KoTextBlockData::Private *>(block.userData()) : new Private())
{
    block.setUserData(d);
}

KoTextBlockData::KoTextBlockData(QTextBlockUserData *userData)
    : d(dynamic_cast<KoTextBlockData::Private *>(userData))
{
    Q_ASSERT(d);
}

KoTextBlockData::~KoTextBlockData()
{
    // explicitly do not delete the d-pointer here
}

void KoTextBlockData::appendMarkup(MarkupType type, int firstChar, int lastChar)
{
    Q_ASSERT(d->markupRangesMap[type].isEmpty() || firstChar > d->markupRangesMap[type].last().lastChar + d->markupRangesMap[type].last().lastRebased);

    MarkupRange range;
    range.firstChar = firstChar;
    range.lastChar = lastChar;
    range.firstRebased = 0;
    range.lastRebased = 0;
    if (!d->markupRangesMap[type].isEmpty()) {
        // The document may have been changed (and thus markup has moved) while
        // the plugin has done its job in the background
        range.firstChar += d->markupRangesMap[type].last().firstRebased;
        range.lastChar += d->markupRangesMap[type].last().lastRebased;
    }
    d->layoutedMarkupRanges[type] = false;

    d->markupRangesMap[type].append(range);
}

void KoTextBlockData::clearMarkups(MarkupType type)
{
    d->markupRangesMap[type].clear();
    d->layoutedMarkupRanges[type] = false;
}

KoTextBlockData::MarkupRange KoTextBlockData::findMarkup(MarkupType type, int positionWithin) const
{
    foreach (const MarkupRange &range, d->markupRangesMap[type]) {
        if (positionWithin <= range.lastChar + range.lastRebased) {
            // possible hit
            if (positionWithin >= range.firstChar + range.firstRebased) {
                return range;
            } else {
                return MarkupRange(); // we have passed it without finding
            }
        }
    }
    return MarkupRange(); // either no ranges or not in last either
}

void KoTextBlockData::rebaseMarkups(MarkupType type, int fromPosition, int delta)
{
    QVector<MarkupRange>::Iterator markIt = markupsBegin(type);
    QVector<MarkupRange>::Iterator markEnd = markupsEnd(type);
    while (markIt != markEnd) {
        if (fromPosition <= markIt->lastChar) {
            // we need to modify the end of this
            markIt->lastChar += delta;
            markIt->lastRebased += delta;
        }
        if (fromPosition < markIt->firstChar) {
            // we need to modify the end of this
            markIt->firstChar += delta;
            markIt->firstRebased += delta;
        }
        ++markIt;
    }
}

void KoTextBlockData::setMarkupsLayoutValidity(MarkupType type, bool valid)
{
    d->layoutedMarkupRanges[type] = valid;
}

bool KoTextBlockData::isMarkupsLayoutValid(MarkupType type) const
{
    return d->layoutedMarkupRanges[type];
}

QVector<KoTextBlockData::MarkupRange>::Iterator KoTextBlockData::markupsBegin(MarkupType type)
{
    return d->markupRangesMap[type].begin();
}

QVector<KoTextBlockData::MarkupRange>::Iterator KoTextBlockData::markupsEnd(MarkupType type)
{
    return d->markupRangesMap[type].end();
}

bool KoTextBlockData::hasCounterData() const
{
    return d->counterWidth >= 0 && (!d->counterPlainText.isNull() || d->counterIsImage);
}

qreal KoTextBlockData::counterWidth() const
{
    return qMax(qreal(0), d->counterWidth);
}

void KoTextBlockData::setBorder(KoTextBlockBorderData *border)
{
    if (d->border && !d->border->deref())
        delete d->border;
    d->border = border;
    if (d->border)
        d->border->ref();
}

void KoTextBlockData::setCounterWidth(qreal width)
{
    d->counterWidth = width;
}

qreal KoTextBlockData::counterSpacing() const
{
    return d->counterSpacing;
}

void KoTextBlockData::setCounterSpacing(qreal spacing)
{
    d->counterSpacing = spacing;
}

QString KoTextBlockData::counterText() const
{
    return d->counterPrefix + d->counterPlainText + d->counterSuffix;
}

void KoTextBlockData::clearCounter()
{
    d->partialCounterText.clear();
    d->counterPlainText.clear();
    d->counterPrefix.clear();
    d->counterSuffix.clear();
    d->counterSpacing = 0.0;
    d->counterWidth = 0.0;
    d->counterIsImage = false;
}

void KoTextBlockData::setPartialCounterText(const QString &text)
{
    d->partialCounterText = text;
}

QString KoTextBlockData::partialCounterText() const
{
    return d->partialCounterText;
}

void KoTextBlockData::setCounterPlainText(const QString &text)
{
    d->counterPlainText = text;
}

QString KoTextBlockData::counterPlainText() const
{
    return d->counterPlainText;
}

void KoTextBlockData::setCounterPrefix(const QString &text)
{
    d->counterPrefix = text;
}

QString KoTextBlockData::counterPrefix() const
{
    return d->counterPrefix;
}

void KoTextBlockData::setCounterSuffix(const QString &text)
{
    d->counterSuffix = text;
}

QString KoTextBlockData::counterSuffix() const
{
    return d->counterSuffix;
}

void KoTextBlockData::setCounterIsImage(bool isImage)
{
    d->counterIsImage = isImage;
}

bool KoTextBlockData::counterIsImage() const
{
    return d->counterIsImage;
}

void KoTextBlockData::setCounterIndex(int index)
{
    d->counterIndex = index;
}

int KoTextBlockData::counterIndex() const
{
    return d->counterIndex;
}

void KoTextBlockData::setCounterPosition(const QPointF &position)
{
    d->counterPos = position;
}

QPointF KoTextBlockData::counterPosition() const
{
    return d->counterPos;
}

void KoTextBlockData::setLabelFormat(const QTextCharFormat &format)
{
    d->labelFormat = format;
}

QTextCharFormat KoTextBlockData::labelFormat() const
{
    return d->labelFormat;
}

KoTextBlockBorderData *KoTextBlockData::border() const
{
    return d->border;
}

void KoTextBlockData::setPaintStrategy(KoTextBlockPaintStrategyBase *paintStrategy)
{
    delete d->paintStrategy;
    d->paintStrategy = paintStrategy;
}

KoTextBlockPaintStrategyBase *KoTextBlockData::paintStrategy() const
{
    return d->paintStrategy;
}

bool KoTextBlockData::saveXmlID() const
{
    // as suggested by boemann, http://lists.kde.org/?l=calligra-devel&m=132396354701553&w=2
    return d->paintStrategy != nullptr;
}
