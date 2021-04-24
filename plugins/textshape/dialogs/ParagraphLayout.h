/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PARAGRAPHLAYOUT_H
#define PARAGRAPHLAYOUT_H

#include <ui_ParagraphLayout.h>

#include <QWidget>

class KoParagraphStyle;

class ParagraphLayout : public QWidget
{
    Q_OBJECT
public:
    explicit ParagraphLayout(QWidget *parent);

    void setDisplay(KoParagraphStyle *style, bool directFormattingMode);

    void save(KoParagraphStyle *style);

Q_SIGNALS:
    void parStyleChanged();

private Q_SLOTS:
    void slotAlignChanged();
    void keepTogetherChanged();
    void breakAfterChanged();
    void breakBeforeChanged();
    void thresholdValueChanged();

private:
    Ui::ParagraphLayout widget;
    bool m_alignmentInherited;
    bool m_keepTogetherInherited;
    bool m_breakAfterInherited;
    bool m_breakBeforeInherited;
    bool m_orphanThresholdInherited;
};

#endif
