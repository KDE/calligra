/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef LISTITEMSHELPER_H
#define LISTITEMSHELPER_H

#include "kotextlayout_export.h"

#include <KoListStyle.h>

#include <QFontMetricsF>

class QTextList;
class QFont;

namespace Lists
{
enum Capitalisation { Lowercase, Uppercase };
struct ListStyleItem {
    ListStyleItem(const QString &name_, KoListStyle::LabelType labelType_, const QString &suffix_ = QString())
    {
        name = name_;
        labelType = labelType_;
        suffix = suffix_;
    }
    KoListStyle::LabelType labelType;
    QString name;
    QString suffix;
};

}

/// \internal helper class for calculating text-lists prefixes and indents
class ListItemsHelper
{
public:
    ListItemsHelper(QTextList *textList, const QFont &font);
    ~ListItemsHelper() = default;
    /// is meant to take a QTextList and set the indent plus the string to render on each listitem
    void recalculateBlock(QTextBlock &block);
    static bool needsRecalc(QTextList *textList);

private:
    QTextList *m_textList;
    QFontMetricsF m_fm;
};

#endif
