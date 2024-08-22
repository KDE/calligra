/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
   SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PARAGRAPHDROPCAPS_H
#define PARAGRAPHDROPCAPS_H
#include "ui_ParagraphDropCaps.h"

#include <QWidget>

class KoParagraphStyle;
class KoUnit;

namespace Ui
{
class ParagraphDropCaps;
}

class ParagraphDropCaps : public QWidget
{
    Q_OBJECT

public:
    explicit ParagraphDropCaps(QWidget *parent = nullptr);

    void setDisplay(KoParagraphStyle *style, bool directFormattingMode);
    void save(KoParagraphStyle *style);

    void setUnit(const KoUnit &unit);

Q_SIGNALS:
    void parStyleChanged();

private Q_SLOTS:
    void dropCapsStateChanged();
    void paragraphDistanceChanged(qreal distance);
    void dropsLineSpanChanged(int lineSpan);
    void dropedCharacterCountChanged(int count);

private:
    void setSettingEnabled(bool enabled);

    Ui::ParagraphDropCaps widget;

    bool m_dropCapsInherited;
    bool m_capsDistanceInherited;
    bool m_capsLengthInherited;
    bool m_capsLinesInherited;
};

#endif // PARAGRAPHDROPCAPS_H
