/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#ifndef PARAGRAPHINDENTSPACINGTAB_H
#define PARAGRAPHINDENTSPACINGTAB_H

#include <KoUnit.h>

#include <QWidget>

class KoParagraphStyle;

namespace Ui {
    class ParagraphIndentSpacingTab;
}

class ParagraphIndentSpacingTab : public QWidget
{
    Q_OBJECT
public:
    explicit ParagraphIndentSpacingTab(QWidget *parent = 0);
    ~ParagraphIndentSpacingTab();
    // open and display the style
    void setDisplay(KoParagraphStyle *style);
    void setUnit(const KoUnit &unit);

    // save widget state to style
    void save(KoParagraphStyle *style);

signals:
    //indentation signals
    void leftIndentEnabled(bool enabled);
    void leftIndentChanged(qreal value);
    void rightIndentEnabled(bool enabled);
    void rightIndentChanged(qreal value);
    void firstLineIndentEnabled(bool enabled);
    void firstLineIndentChanged(qreal value);
    void autoTextIndentChecked(bool checked);
    //line spacing signals
    void lineSpacingEnabled(bool enabled);
    void lineSpacingChanged(int item);
    void lineSpacingValueChanged(qreal value);
    void lineSpacingPercentChanged(int percent);
    void useFontMetricsChecked(bool checked);
    //paragraph spacing signals
    void topParagraphSpacingEnabled(bool enabled);
    void topParagraphSpacingChanged(qreal value);
    void bottomParagraphSpacingEnabled(bool enabled);
    void bottomParagraphSpacingChanged(qreal value);

private slots:
    //indentation slots
    void slotLeftIndentEnabled(bool enabled);
    void slotLeftIndentChanged(qreal value);
    void slotRightIndentEnabled(bool enabled);
    void slotRightIndentChanged(qreal value);
    void slotFirstLineIndentEnabled(bool enabled);
    void slotFirstLineIndentChanged(qreal value);
    void slotAutoTextIndentChecked(bool checked);
    //line spacing slots
    void slotLineSpacingEnabled(bool enabled);
    void slotLineSpacingChanged(int item);
    void slotLineSpacingValueChanged(qreal value);
    void slotLineSpacingPercentChanged(int percent);
    void slotUseFontMetricsChecked(bool checked);
    //paragraph spacing slots
    void slotTopParagraphSpacingEnabled(bool enabled);
    void slotTopParagraphSpacingChanged(qreal value);
    void slotBottomParagraphSpacingEnabled(bool enabled);
    void slotBottomParagraphSpacingChanged(qreal value);

private:
    Ui::ParagraphIndentSpacingTab *ui;

    KoParagraphStyle *m_style;
    bool m_fontMetricsChecked;
    bool m_rightMarginIngerited;
    bool m_leftMarginInherited;
    bool m_topMarginInherited;
    bool m_bottomMarginInherited;
    bool m_textIndentInherited;
    bool m_autoTextIndentInherited;
    bool m_spacingInherited;
};

#endif
