/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2003 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ParagraphIndentSpacing.h"

#include <KoParagraphStyle.h>
#include <QDebug>

ParagraphIndentSpacing::ParagraphIndentSpacing(QWidget *parent)
    : QWidget(parent)
    , m_fontMetricsChecked(false)
{
    widget.setupUi(this);
    widget.mainLayout->insertStretch(0);
    widget.mainLayout->insertStretch(2);
    widget.formLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    connect(widget.first, &KoUnitDoubleSpinBox::valueChangedPt, this, &ParagraphIndentSpacing::firstLineMarginChanged);
    connect(widget.left, &KoUnitDoubleSpinBox::valueChangedPt, this, &ParagraphIndentSpacing::leftMarginChanged);
    connect(widget.right, &KoUnitDoubleSpinBox::valueChangedPt, this, &ParagraphIndentSpacing::rightMarginChanged);

    // Keep order in sync with lineSpacingType() and display()
    widget.lineSpacing->addItem(i18nc("Line spacing value", "Single"));
    widget.lineSpacing->addItem(i18nc("Line spacing value", "1.5 Lines"));
    widget.lineSpacing->addItem(i18nc("Line spacing value", "Double"));
    widget.lineSpacing->addItem(i18nc("Line spacing type", "Proportional")); // called Proportional like in OO
    widget.lineSpacing->addItem(i18nc("Line spacing type", "Additional")); // normal distance + absolute value
    widget.lineSpacing->addItem(i18nc("Line spacing type", "Fixed"));
    widget.lineSpacing->addItem(i18nc("Line spacing type", "At least"));

    connect(widget.first, &KoUnitDoubleSpinBox::valueChangedPt, this, &ParagraphIndentSpacing::firstIndentValueChanged);
    connect(widget.left, &KoUnitDoubleSpinBox::valueChangedPt, this, &ParagraphIndentSpacing::leftMarginValueChanged);
    connect(widget.right, &KoUnitDoubleSpinBox::valueChangedPt, this, &ParagraphIndentSpacing::rightMarginValueChanged);
    connect(widget.after, &KoUnitDoubleSpinBox::valueChangedPt, this, &ParagraphIndentSpacing::bottomMarginValueChanged);
    connect(widget.before, &KoUnitDoubleSpinBox::valueChangedPt, this, &ParagraphIndentSpacing::topMarginValueChanged);
    connect(widget.lineSpacing, &QComboBox::currentIndexChanged, this, &ParagraphIndentSpacing::lineSpacingChanged);
    connect(widget.useFont, &QAbstractButton::toggled, this, &ParagraphIndentSpacing::useFontMetrices);
    connect(widget.autoTextIndent, &QCheckBox::stateChanged, this, &ParagraphIndentSpacing::autoTextIndentChanged);
    connect(widget.percentPage, &QSpinBox::valueChanged, this, &ParagraphIndentSpacing::spacingPercentChanged);
    connect(widget.unitsPage, &KoUnitDoubleSpinBox::valueChangedPt, this, &ParagraphIndentSpacing::spacingValueChanged);
    lineSpacingChanged(0);
}

void ParagraphIndentSpacing::autoTextIndentChanged(int state)
{
    widget.first->setEnabled(state == Qt::Unchecked);
    m_autoTextIndentInherited = false;
    Q_EMIT parStyleChanged();
}
void ParagraphIndentSpacing::firstIndentValueChanged()
{
    m_textIndentInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphIndentSpacing::rightMarginValueChanged()
{
    m_rightMarginIngerited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphIndentSpacing::leftMarginValueChanged()
{
    m_leftMarginInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphIndentSpacing::topMarginValueChanged()
{
    m_topMarginInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphIndentSpacing::bottomMarginValueChanged()
{
    m_bottomMarginInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphIndentSpacing::setDisplay(KoParagraphStyle *style, bool directFormattingMode)
{
    m_style = style;
    // TODO : handle relatives
    widget.first->changeValue(style->textIndent());
    widget.left->changeValue(style->leftMargin());
    widget.right->changeValue(style->rightMargin());
    widget.before->changeValue(style->topMargin());
    widget.after->changeValue(style->bottomMargin());

    m_rightMarginIngerited = directFormattingMode || !style->hasProperty(QTextFormat::BlockRightMargin);
    m_leftMarginInherited = directFormattingMode || !style->hasProperty(QTextFormat::BlockLeftMargin);
    m_topMarginInherited = directFormattingMode || !style->hasProperty(QTextFormat::BlockTopMargin);
    m_bottomMarginInherited = directFormattingMode || !style->hasProperty(QTextFormat::BlockBottomMargin);
    m_autoTextIndentInherited = directFormattingMode || !style->hasProperty(KoParagraphStyle::AutoTextIndent);
    m_textIndentInherited = directFormattingMode || !style->hasProperty(QTextFormat::TextIndent);

    widget.autoTextIndent->setChecked(style->autoTextIndent());

    m_spacingInherited = !(style->hasProperty(KoParagraphStyle::FixedLineHeight) || style->hasProperty(KoParagraphStyle::LineSpacing)
                           || style->hasProperty(KoParagraphStyle::PercentLineHeight) || style->hasProperty(KoParagraphStyle::MinimumLineHeight));

    int index;
    if (style->hasProperty(KoParagraphStyle::FixedLineHeight) && style->lineHeightAbsolute() != 0) {
        // this is the strongest; if this is set we don't care what other properties there are.
        index = 5;
    } else if (style->hasProperty(KoParagraphStyle::LineSpacing) && style->lineSpacing() != 0) {
        // if LineSpacing is set then percent is ignored.
        index = 4;
    } else if (style->hasProperty(KoParagraphStyle::PercentLineHeight) && style->lineHeightPercent() != 0) {
        int percent = style->lineHeightPercent();
        if (percent == 100)
            index = 0; // single
        else if (percent == 150)
            index = 1; // 1.5
        else if (percent == 200)
            index = 2; // double
        else
            index = 3; // proportional
    } else if (style->hasProperty(KoParagraphStyle::MinimumLineHeight) && style->minimumLineHeight() != 0) {
        index = 6;
    } else {
        index = 0; // nothing set, default is 'single' just like for geeks.
    }
    widget.lineSpacing->setCurrentIndex(index);
    // widget.minimumLineSpacing->changeValue(style->minimumLineHeight());
    widget.useFont->setChecked(style->lineSpacingFromFont());
    m_fontMetricsChecked = style->lineSpacingFromFont();
}

void ParagraphIndentSpacing::lineSpacingChanged(int row)
{
    bool percent = false, custom = false;
    qreal customValue = 0.0;
    switch (row) {
    case 0:
    case 1:
    case 2:
        break;
    case 3: // proportional
        percent = true;
        widget.percentPage->setValue(m_style->lineHeightPercent());
        break;
    case 4: // additional
        custom = true;
        customValue = qMax(qreal(0.1), m_style->lineSpacing());
        break;
    case 5: // fixed
        custom = true;
        if (m_style->lineHeightAbsolute() == 0) // unset
            customValue = 12.0; // nice default value...
        else
            customValue = m_style->lineHeightAbsolute();
        break;
    case 6: // minimum
        custom = true;
        customValue = m_style->minimumLineHeight();
        break;
    default:; // other cases don't need the spinboxes
    }

    m_spacingInherited = false;

    if (custom) {
        widget.unitsPage->setEnabled(true);
        widget.spacingStack->setCurrentWidget(widget.unitsPage);
        widget.unitsPage->changeValue(customValue);
    } else {
        widget.spacingStack->setCurrentWidget(widget.percentPage);
        widget.percentPage->setEnabled(percent);
        if (!percent)
            widget.percentPage->setValue(100);
    }

    widget.useFont->setEnabled(row != 5);
    widget.useFont->setChecked(row == 5 ? false : m_fontMetricsChecked);
    Q_EMIT parStyleChanged();
}

void ParagraphIndentSpacing::spacingPercentChanged()
{
    m_spacingInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphIndentSpacing::spacingValueChanged()
{
    m_spacingInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphIndentSpacing::save(KoParagraphStyle *style)
{
    // general note; we have to unset values by setting it to zero instead of removing the item
    // since this dialog may be used on a copy style, which will be applied later. And removing
    // items doesn't work for that.
    if (!m_textIndentInherited) {
        style->setTextIndent(QTextLength(QTextLength::FixedLength, widget.first->value()));
    }
    if (!m_leftMarginInherited) {
        style->setLeftMargin(QTextLength(QTextLength::FixedLength, widget.left->value()));
    }
    if (!m_rightMarginIngerited) {
        style->setRightMargin(QTextLength(QTextLength::FixedLength, widget.right->value()));
    }
    if (!m_topMarginInherited) {
        style->setTopMargin(QTextLength(QTextLength::FixedLength, widget.before->value()));
    }
    if (!m_bottomMarginInherited) {
        style->setBottomMargin(QTextLength(QTextLength::FixedLength, widget.after->value()));
    }
    if (!m_autoTextIndentInherited) {
        style->setAutoTextIndent(widget.autoTextIndent->isChecked());
    }
    if (!m_spacingInherited) {
        style->setLineHeightAbsolute(0); // since it trumps percentage based line heights, unset it.
        style->setMinimumLineHeight(QTextLength(QTextLength::FixedLength, 0));
        style->setLineSpacing(0);
        switch (widget.lineSpacing->currentIndex()) {
        case 0:
            style->setLineHeightPercent(100);
            break;
        case 1:
            style->setLineHeightPercent(150);
            break;
        case 2:
            style->setLineHeightPercent(200);
            break;
        case 3:
            style->setLineHeightPercent(widget.percentPage->value());
            break;
        case 4:
            if (widget.unitsPage->value() == 0.0) { // then we need to save it differently.
                style->setLineHeightPercent(100);
            } else {
                style->setLineSpacing(widget.unitsPage->value());
            }
            break;
        case 5:
            style->setLineHeightAbsolute(widget.unitsPage->value());
            break;
        case 6:
            style->setMinimumLineHeight(QTextLength(QTextLength::FixedLength, widget.unitsPage->value()));
            break;
        }
        style->setLineSpacingFromFont(widget.lineSpacing->currentIndex() != 5 && widget.useFont->isChecked());
    }
}

void ParagraphIndentSpacing::setUnit(const KoUnit &unit)
{
    widget.first->setUnit(unit);
    widget.left->setUnit(unit);
    widget.right->setUnit(unit);
    widget.before->setUnit(unit);
    widget.after->setUnit(unit);
    widget.unitsPage->setUnit(unit);
}

void ParagraphIndentSpacing::useFontMetrices(bool on)
{
    if (widget.lineSpacing->currentIndex() != 5)
        m_fontMetricsChecked = on;
    Q_EMIT parStyleChanged();
}

void ParagraphIndentSpacing::firstLineMarginChanged(qreal margin)
{
    Q_UNUSED(margin);
    Q_EMIT parStyleChanged();
}

void ParagraphIndentSpacing::leftMarginChanged(qreal margin)
{
    Q_UNUSED(margin);
    Q_EMIT parStyleChanged();
}

void ParagraphIndentSpacing::rightMarginChanged(qreal margin)
{
    Q_UNUSED(margin);
    Q_EMIT parStyleChanged();
}
