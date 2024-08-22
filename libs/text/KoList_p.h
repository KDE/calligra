/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Girish Ramakrishnan <girish@forwardbias.in>
 * SPDX-FileCopyrightText: 2008 Pierre Stirnweiss <pierre.stirnweiss_calligra@gadz.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOLIST_P_H
#define KOLIST_P_H

#include "KoList.h"
#include "KoTextBlockData.h"
#include "styles/KoListStyle.h"

#include <QPointer>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextList>
#include <QVariant>
#include <QVector>

class KoListPrivate
{
public:
    KoListPrivate(KoList *q, const QTextDocument *document)
        : q(q)
        , type(KoList::TextList)
        , style(nullptr)
        , textLists(10)
        , textListIds(10)
        , document(document)
        , listToBeContinuedFrom(nullptr)
    {
    }

    ~KoListPrivate() = default;

    static void invalidate(const QTextBlock &block)
    {
        QTextBlock currentBlock = block;
        KoTextBlockData data(currentBlock);
        data.setCounterWidth(-1.0);
    }

    static void invalidateList(const QTextBlock &block)
    {
        for (int i = 0; i < block.textList()->count(); i++) {
            if (block.textList()->item(i) != block) {
                invalidate(block.textList()->item(i));
                break;
            }
        }
    }

    void styleChanged(int level)
    {
        Q_UNUSED(level);
        q->setStyle(style);
    }

    KoList *q;
    KoList::Type type;
    KoListStyle *style;
    QVector<QPointer<QTextList>> textLists;
    QVector<KoListStyle::ListIdType> textListIds;
    const QTextDocument *document;
    QMap<int, QVariant> properties;
    KoList *listToBeContinuedFrom;
};

#endif // KOLIST_P_H
