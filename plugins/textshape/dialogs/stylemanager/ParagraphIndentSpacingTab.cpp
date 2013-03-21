/* This file is part of the KDE project
 * Copyright (C) 2007, 2009 Thomas Zander <zander@kde.org>
 * Copyright (c) 2003 David Faure <faure@kde.org>
 * Copyright (C)  2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
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

#include "ParagraphIndentSpacingTab.h"
#include "ui_ParagraphIndentSpacingTab.h"

#include <KoParagraphStyle.h>
#include <KDebug>

ParagraphIndentSpacingTab::ParagraphIndentSpacingTab(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ParagraphIndentSpacingTab)
    , m_fontMetricsChecked(false)
{
    ui->setupUi(this);

    //indentation
    connect(ui->leftLabel, SIGNAL(toggled(bool)), this, SLOT(slotLeftIndentEnabled(bool)));
    connect(ui->left, SIGNAL(valueChangedPt(qreal)), this, SLOT(slotLeftIndentChanged(qreal)));
    connect(ui->rightLabel, SIGNAL(toggled(bool)), this, SLOT(slotRightIndentEnabled(bool)));
    connect(ui->right, SIGNAL(valueChangedPt(qreal)), this, SLOT(slotRightIndentChanged(qreal)));
    connect(ui->firstLineLabel, SIGNAL(toggled(bool)), this, SLOT(slotFirstLineIndentEnabled(bool)));
    connect(ui->first, SIGNAL(valueChangedPt(qreal)), this, SLOT(slotFirstLineIndentChanged(qreal)));
    connect(ui->autoTextIndent, SIGNAL(toggled(bool)), this, SLOT(slotAutoTextIndentChecked(bool)));

    //line spacing
    // Keep order in sync with lineSpacingType() and display()
    ui->lineSpacing->addItem(i18nc("Line spacing value", "Single"));
    ui->lineSpacing->addItem(i18nc("Line spacing value", "1.5 Lines"));
    ui->lineSpacing->addItem(i18nc("Line spacing value", "Double"));
    ui->lineSpacing->addItem(i18nc("Line spacing type", "Proportional"));    // called Proportional like in OO
    ui->lineSpacing->addItem(i18nc("Line spacing type", "Additional"));    // normal distance + absolute value
    ui->lineSpacing->addItem(i18nc("Line spacing type", "Fixed"));
    ui->lineSpacing->addItem(i18nc("Line spacing type", "At least"));
    connect(ui->lineSpacingLabel, SIGNAL(toggled(bool)), this, SLOT(slotLineSpacingEnabled(bool)));
    connect(ui->lineSpacing, SIGNAL(currentIndexChanged(int)), this, SLOT(slotLineSpacingChanged(int)));
    connect(ui->proportional, SIGNAL(valueChanged(int)), this, SLOT(slotLineSpacingPercentChanged(int)));
    connect(ui->custom, SIGNAL(valueChangedPt(qreal)), this, SLOT(slotLineSpacingValueChanged(qreal)));
    connect(ui->useFont, SIGNAL(toggled(bool)), this, SLOT(slotUseFontMetricsChecked(bool)));
//    blockSignals(true);
//    slotLineSpacingChanged(0);
//    blockSignals(false);

    //paragraph spacing
    connect(ui->beforeLabel, SIGNAL(toggled(bool)), this, SLOT(slotTopParagraphSpacingEnabled(bool)));
    connect(ui->before, SIGNAL(valueChangedPt(qreal)), this, SLOT(slotTopParagraphSpacingChanged(qreal)));
    connect(ui->afterLabel, SIGNAL(toggled(bool)), this, SLOT(slotBottomParagraphSpacingEnabled(bool)));
    connect(ui->after, SIGNAL(valueChangedPt(qreal)), this, SLOT(slotBottomParagraphSpacingChanged(qreal)));
}

ParagraphIndentSpacingTab::~ParagraphIndentSpacingTab()
{
    delete ui;
    ui = 0;
}

void ParagraphIndentSpacingTab::slotLeftIndentEnabled(bool enabled)
{
    ui->left->setEnabled(enabled);
    emit leftIndentEnabled(enabled);
}

void ParagraphIndentSpacingTab::slotLeftIndentChanged(qreal value)
{
    emit leftIndentChanged(value);
}

void ParagraphIndentSpacingTab::slotRightIndentEnabled(bool enabled)
{
    ui->right->setEnabled(enabled);
    emit rightIndentEnabled(enabled);
}

void ParagraphIndentSpacingTab::slotRightIndentChanged(qreal value)
{
    emit rightIndentChanged(value);
}

void ParagraphIndentSpacingTab::slotFirstLineIndentEnabled(bool enabled)
{
    ui->first->setEnabled(enabled && !ui->autoTextIndent->isChecked());
    ui->autoTextIndent->setEnabled(enabled);
    emit firstLineIndentEnabled(enabled);
}

void ParagraphIndentSpacingTab::slotFirstLineIndentChanged(qreal value)
{
    emit firstLineIndentChanged(value);
}

void ParagraphIndentSpacingTab::slotAutoTextIndentChecked(bool checked)
{
    ui->first->setEnabled(ui->firstLineLabel->isChecked() && !checked);
    emit autoTextIndentChecked(checked);
}

void ParagraphIndentSpacingTab::slotLineSpacingEnabled(bool enabled)
{
    ui->lineSpacing->setEnabled(enabled);
    ui->spacingStack->setEnabled(enabled);
    ui->useFont->setEnabled(enabled && ui->lineSpacing->currentIndex() != 5);
    emit lineSpacingEnabled(enabled);
}

void ParagraphIndentSpacingTab::slotLineSpacingChanged(int item)
{
    kDebug() << "slot line spacing changed.";
    kDebug() << "item: " << item;
    kDebug() << "useFont: " << ui->useFont->isChecked();
    kDebug() << "proportional value: " << ui->proportional->value();
    kDebug() << "custom value: " << ui->custom->value();
    switch (item) {
    case 0:
        emit lineSpacingChanged(LineHeightProportional, 120, ui->useFont->isChecked());
        break;
    case 1:
        emit lineSpacingChanged(LineHeightProportional, 180, ui->useFont->isChecked());
        break;
    case 2:
        emit lineSpacingChanged(LineHeightProportional, 240, ui->useFont->isChecked());
        break;
    case 3:
        emit lineSpacingChanged(LineHeightProportional, ui->proportional->value(), ui->useFont->isChecked());
        break;
    case 4:
        if (ui->custom->value() == 0.0) {
            emit lineSpacingChanged(LineHeightProportional, 100, ui->useFont->isChecked()); //we need to save it as percentage
        }
        else {
            emit lineSpacingChanged(LineSpacingAdditional, ui->custom->value(), ui->useFont->isChecked());
        }
        break;
    case 5:
        emit lineSpacingChanged(LineHeightFixed, ui->custom->value(), false);
        break;
    case 6:
        emit lineSpacingChanged(LineHeightMinimum, ui->custom->value(), ui->useFont->isChecked());
        break;
    }
}

void ParagraphIndentSpacingTab::slotLineSpacingPercentChanged(int percent)
{
    Q_UNUSED(percent);
    slotLineSpacingChanged(ui->lineSpacing->currentIndex());
//    emit lineSpacingPercentChanged(percent);
}

void ParagraphIndentSpacingTab::slotLineSpacingValueChanged(qreal value)
{
    Q_UNUSED(value);
    slotLineSpacingChanged(ui->lineSpacing->currentIndex());
//    emit lineSpacingValueChanged(value);
}

void ParagraphIndentSpacingTab::slotUseFontMetricsChecked(bool checked)
{
    Q_UNUSED(checked);
    slotLineSpacingChanged(ui->lineSpacing->currentIndex());
//    emit useFontMetricsChecked(checked);
}

void ParagraphIndentSpacingTab::slotTopParagraphSpacingEnabled(bool enabled)
{
    ui->before->setEnabled(enabled);
    emit topParagraphSpacingEnabled(enabled);
}

void ParagraphIndentSpacingTab::slotTopParagraphSpacingChanged(qreal value)
{
    emit topParagraphSpacingChanged(value);
}

void ParagraphIndentSpacingTab::slotBottomParagraphSpacingEnabled(bool enabled)
{
    ui->after->setEnabled(enabled);
    emit bottomParagraphSpacingEnabled(enabled);
}

void ParagraphIndentSpacingTab::slotBottomParagraphSpacingChanged(qreal value)
{
    emit bottomParagraphSpacingChanged(value);
}

void ParagraphIndentSpacingTab::setDisplay(KoParagraphStyle *style)
{
    if (!style) {
        return;
    }
    blockSignals(true);
    bool checked;
    // indentation
    checked = style->hasProperty(QTextFormat::BlockLeftMargin);
    ui->leftLabel->setChecked(checked);
    slotLeftIndentEnabled(checked);
    ui->left->changeValue(style->leftMargin());
    checked = style->hasProperty(QTextFormat::BlockRightMargin);
    ui->rightLabel->setChecked(checked);
    slotRightIndentEnabled(checked);
    ui->right->changeValue(style->rightMargin());
    checked = style->hasProperty(QTextFormat::TextIndent);
    ui->firstLineLabel->setChecked(checked);
    ui->autoTextIndent->setChecked(style->autoTextIndent()); //set this first as first line indent check for this to enable/disable the spinBox
    slotFirstLineIndentEnabled(checked);
    ui->first->changeValue(style->textIndent());

    //line spacing
    ui->proportional->setValue(style->lineHeightPercent());

    if (style->hasProperty(KoParagraphStyle::FixedLineHeight) && style->lineHeightAbsolute() != 0) {
        // this is the strongest
        ui->lineSpacing->setCurrentIndex(5);
        ui->spacingStack->setCurrentWidget(ui->unitsPage);
        ui->custom->changeValue(style->lineHeightAbsolute());
        ui->useFont->setEnabled(false);
    } else if (style->hasProperty(KoParagraphStyle::LineSpacing) && style->lineSpacing() != 0) {
        // if LineSpacing is set then percent is ignored.
        ui->lineSpacing->setCurrentIndex(4);
        ui->spacingStack->setCurrentWidget(ui->unitsPage);
        ui->custom->changeValue(qMax(qreal(0.1), style->lineSpacing()));
    } else if (style->hasProperty(KoParagraphStyle::PercentLineHeight) && style->lineHeightPercent() != 0) {
        int percent = style->lineHeightPercent();
        if (percent == 120) {
            ui->lineSpacing->setCurrentIndex(0); // single
            ui->proportional->setEnabled(false);
        }
        else if (percent == 180) {
            ui->lineSpacing->setCurrentIndex(1); // 1.5
            ui->proportional->setEnabled(false);
        }
        else if (percent == 240) {
            ui->lineSpacing->setCurrentIndex(2); // double
            ui->proportional->setEnabled(false);
        }
        else {
            ui->lineSpacing->setCurrentIndex(3); // proportional
            ui->proportional->setEnabled(ui->lineSpacingLabel->isChecked());
        }
        ui->spacingStack->setCurrentWidget(ui->percentPage);
    } else if (style->hasProperty(KoParagraphStyle::MinimumLineHeight) && style->minimumLineHeight() != 0) {
        ui->lineSpacing->setCurrentIndex(6);
        ui->spacingStack->setCurrentWidget(ui->unitsPage);
        ui->custom->changeValue(style->minimumLineHeight());
    } else {
        ui->lineSpacing->setCurrentIndex(0); // nothing set, default is 'single' just like for geeks.
        ui->proportional->setValue(120);
        ui->proportional->setEnabled(false);
    }
    ui->useFont->setChecked(style->lineSpacingFromFont() && ui->lineSpacing->currentIndex() != 5);
    m_fontMetricsChecked = style->lineSpacingFromFont();

    checked = (style->hasProperty(KoParagraphStyle::FixedLineHeight) || style->hasProperty(KoParagraphStyle::LineSpacing) || style->hasProperty(KoParagraphStyle::PercentLineHeight) ||style->hasProperty(KoParagraphStyle::MinimumLineHeight));
    ui->lineSpacingLabel->setChecked(checked);
    slotLineSpacingEnabled(checked); //we do this after setting the line spacing widgets because the useFontMetrics check box enabling depends on the line spacing type


    //paragraph spacing
    checked = style->hasProperty(QTextFormat::BlockTopMargin);
    ui->beforeLabel->setChecked(checked);
    slotTopParagraphSpacingEnabled(checked);
    ui->before->changeValue(style->topMargin());
    checked = style->hasProperty(QTextFormat::BlockBottomMargin);
    ui->afterLabel->setChecked(checked);
    slotBottomParagraphSpacingEnabled(checked);
    ui->after->changeValue(style->bottomMargin());

    blockSignals(false);
}

void ParagraphIndentSpacingTab::save(KoParagraphStyle *style)
{
    // general note; we have to unset values by setting it to zero instead of removing the item
    // since this dialog may be used on a copy style, which will be applied later. And removing
    // items doesn't work for that.
    if (!m_textIndentInherited){
        style->setTextIndent(QTextLength(QTextLength::FixedLength, ui->first->value()));
    }
    if (!m_leftMarginInherited){
        style->setLeftMargin(QTextLength(QTextLength::FixedLength, ui->left->value()));
    }
    if (!m_rightMarginIngerited){
        style->setRightMargin(QTextLength(QTextLength::FixedLength, ui->right->value()));
    }
    if (!m_topMarginInherited){
        style->setTopMargin(QTextLength(QTextLength::FixedLength, ui->before->value()));
    }
    if (!m_bottomMarginInherited){
        style->setBottomMargin(QTextLength(QTextLength::FixedLength, ui->after->value()));
    }
    if (!m_autoTextIndentInherited){
        style->setAutoTextIndent(ui->autoTextIndent->isChecked());
    }
    if (!m_spacingInherited) {
        style->setLineHeightAbsolute(0); // since it trumps percentage based line heights, unset it.
        style->setMinimumLineHeight(QTextLength(QTextLength::FixedLength, 0));
        style->setLineSpacing(0);
        switch (ui->lineSpacing->currentIndex()) {
        case 0: style->setLineHeightPercent(120); break;
        case 1: style->setLineHeightPercent(180); break;
        case 2: style->setLineHeightPercent(240); break;
        case 3: style->setLineHeightPercent(ui->proportional->value()); break;
        case 4:
            if (ui->custom->value() == 0.0) { // then we need to save it differently.
                style->setLineHeightPercent(100);
            } else {
                style->setLineSpacing(ui->custom->value());
            }
            break;
        case 5:
            style->setLineHeightAbsolute(ui->custom->value());
            break;
        case 6:
            style->setMinimumLineHeight(QTextLength(QTextLength::FixedLength, ui->custom->value()));
            break;
        }
        style->setLineSpacingFromFont(ui->lineSpacing->currentIndex() != 5 && ui->useFont->isChecked());
    }
}

void ParagraphIndentSpacingTab::setUnit(const KoUnit &unit)
{
    ui->first->setUnit(unit);
    ui->left->setUnit(unit);
    ui->right->setUnit(unit);
    ui->before->setUnit(unit);
    ui->after->setUnit(unit);
    ui->custom->setUnit(unit);
}

#include <ParagraphIndentSpacingTab.moc>
