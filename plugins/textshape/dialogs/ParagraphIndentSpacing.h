/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef PARAGRAPHINDENTSPACING_H
#define PARAGRAPHINDENTSPACING_H

#include <ui_ParagraphIndentSpacing.h>

#include <QWidget>

class KoParagraphStyle;
class KoUnit;

class ParagraphIndentSpacing : public QWidget
{
    Q_OBJECT
public:
    explicit ParagraphIndentSpacing(QWidget *parent);
    // open and display the style
    void setDisplay(KoParagraphStyle *style, bool directFormattingMode);
    void setUnit(const KoUnit &unit);

    // save widget state to style
    void save(KoParagraphStyle *style);

Q_SIGNALS:
    void parStyleChanged();

private Q_SLOTS:
    void lineSpacingChanged(int);
    void spacingValueChanged();
    void spacingPercentChanged();
    void useFontMetrices(bool);
    void autoTextIndentChanged(int state);
    void firstIndentValueChanged();
    void leftMarginValueChanged();
    void rightMarginValueChanged();
    void bottomMarginValueChanged();
    void topMarginValueChanged();
    void firstLineMarginChanged(qreal margin);
    void leftMarginChanged(qreal margin);
    void rightMarginChanged(qreal margin);

private:
    Ui::ParagraphIndentSpacing widget;

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
