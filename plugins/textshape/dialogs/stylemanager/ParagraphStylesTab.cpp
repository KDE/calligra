/* This file is part of the KDE project
 * Copyright (C) 2012 Pierre Stirnweiss <pstirnweiss@googlemail.org>
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

#include "ParagraphStylesTab.h"
#include "ui_ParagraphStylesTab.h"

#include "StylesManagerStylesModel.h"

#include <dialogs/StylesModel.h>
#include <dialogs/StylesDelegate.h>

#include <KoStyleManager.h>
#include <KoStyleThumbnailer.h>

#include <KDebug>

ParagraphStylesTab::ParagraphStylesTab(QWidget *parent) :
    QWidget(parent)
  , ui(new Ui::ParagraphStylesTab)
  , m_styleManager(0)
  , m_thumbnailer(new KoStyleThumbnailer())
  , m_sourceModel(0)
  , m_stylesDelegate(new StylesDelegate())
  , m_paragraphStylesModel(0)
{
    ui->setupUi(this);

    //TODO create/extend delegate for saving individual styles, highlight modified styles,...
//    ui->paragraphListView->setItemDelegate(m_stylesDelegate);

    connect(ui->paragraphListView, SIGNAL(activated(QModelIndex)), this, SLOT(slotStyleSelected(QModelIndex)));
    connect(ui->newStyleButton, SIGNAL(clicked()), this, SLOT(slotCreateNewStyle()));
    connect(ui->saveAllStylesButton, SIGNAL(clicked()), this, SLOT(slotSaveStyle()));

    //Character highlighting
    connect(ui->characterHighlighting, SIGNAL(capitalizationEnabled(bool)), this, SLOT(slotCapitalizationEnabled(bool)));
    connect(ui->characterHighlighting, SIGNAL(capitalizationChanged(QFont::Capitalization)), this, SLOT(slotCapitalizationChanged(QFont::Capitalization)));
    connect(ui->characterHighlighting, SIGNAL(underlineEnabled(bool)), this, SLOT(slotUnderlineEnabled(bool)));
    connect(ui->characterHighlighting, SIGNAL(underlineChanged(KoCharacterStyle::LineType,KoCharacterStyle::LineStyle,QColor)), this, SLOT(slotUnderlineChanged(KoCharacterStyle::LineType,KoCharacterStyle::LineStyle,QColor)));
    connect(ui->characterHighlighting, SIGNAL(strikethroughEnabled(bool)), this, SLOT(slotStrikethroughEnabled(bool)));
    connect(ui->characterHighlighting, SIGNAL(strikethroughChanged(KoCharacterStyle::LineType,KoCharacterStyle::LineStyle,QColor)), this, SLOT(slotStrikethroughChanged(KoCharacterStyle::LineType,KoCharacterStyle::LineStyle,QColor)));
    connect(ui->characterHighlighting, SIGNAL(positionEnabled(bool)), this, SLOT(slotPositionEnabled(bool)));
    connect(ui->characterHighlighting, SIGNAL(positionChanged(QTextCharFormat::VerticalAlignment)), this, SLOT(slotPositionChanged(QTextCharFormat::VerticalAlignment)));
    connect(ui->characterHighlighting, SIGNAL(backgroundColorEnabled(bool)), this, SLOT(slotBackgroundColorEnabled(bool)));
    connect(ui->characterHighlighting, SIGNAL(backgroundColorChanged(QColor)), this, SLOT(slotBackgroundColorChanged(QColor)));
    connect(ui->characterHighlighting, SIGNAL(textColorEnabled(bool)), this, SLOT(slotTextColorEnabled(bool)));
    connect(ui->characterHighlighting, SIGNAL(textColorChanged(QColor)), this, SLOT(slotTextColorChanged(QColor)));
    connect(ui->characterHighlighting, SIGNAL(fontEnabled(bool)), this, SLOT(slotFontEnabled(bool)));
    connect(ui->characterHighlighting, SIGNAL(fontChanged(QFont)), this, SLOT(slotFontChanged(QFont)));

    //Paragraph indent and spacing
    connect(ui->indentSpacing, SIGNAL(leftIndentEnabled(bool)), this, SLOT(slotLeftIndentEnabled(bool)));
    connect(ui->indentSpacing, SIGNAL(leftIndentChanged(qreal)), this, SLOT(slotLeftIndentChanged(qreal)));
    connect(ui->indentSpacing, SIGNAL(rightIndentEnabled(bool)), this, SLOT(slotRightIndentEnabled(bool)));
    connect(ui->indentSpacing, SIGNAL(rightIndentChanged(qreal)), this, SLOT(slotRightIndentChanged(qreal)));
    connect(ui->indentSpacing, SIGNAL(firstLineIndentEnabled(bool)), this, SLOT(slotFirstLineIndentEnabled(bool)));
    connect(ui->indentSpacing, SIGNAL(firstLineIndentChanged(qreal)), this, SLOT(slotFirstLineIndentChanged(qreal)));
    connect(ui->indentSpacing, SIGNAL(autoTextIndentChecked(bool)), this, SLOT(slotAutoTextIndentChecked(bool)));
    connect(ui->indentSpacing, SIGNAL(lineSpacingEnabled(bool)), this, SLOT(slotLineSpacingEnabled(bool)));
    connect(ui->indentSpacing, SIGNAL(lineSpacingChanged(ParagraphIndentSpacingTab::LineSpacingType,qreal,bool)), this, SLOT(slotLineSpacingChanged(ParagraphIndentSpacingTab::LineSpacingType,qreal,bool)));
    connect(ui->indentSpacing, SIGNAL(topParagraphSpacingEnabled(bool)), this, SLOT(slotTopParagraphSpacingEnabled(bool)));
    connect(ui->indentSpacing, SIGNAL(topParagraphSpacingChanged(qreal)), this, SLOT(slotTopParagraphSpacingChanged(qreal)));
    connect(ui->indentSpacing, SIGNAL(bottomParagraphSpacingEnabled(bool)), this, SLOT(slotBottomParagraphSpacingEnabled(bool)));
    connect(ui->indentSpacing, SIGNAL(bottomParagraphSpacingChanged(qreal)), this, SLOT(slotBottomParagraphSpacingChanged(qreal)));
}

ParagraphStylesTab::~ParagraphStylesTab()
{
    delete ui;
    delete m_stylesDelegate;
}

void ParagraphStylesTab::setDisplay(KoParagraphStyle *style)
{
    ui->paragraphListView->update(ui->paragraphListView->currentIndex());
    ui->characterHighlighting->setDisplay(style);
    ui->indentSpacing->setDisplay(style);
}

void ParagraphStylesTab::setStyleManager(KoStyleManager *manager)
{
    Q_ASSERT(manager);
    if (!manager) {
        return; //return gracefully but shouldn't happen
    }
    m_styleManager = manager;

    m_sourceModel = new StylesModel(m_styleManager, AbstractStylesModel::ParagraphStyle);
    m_sourceModel->setStyleThumbnailer(m_thumbnailer);

    m_paragraphStylesModel = new StylesManagerStylesModel();
    m_paragraphStylesModel->setStylesModel(m_sourceModel);
    m_paragraphStylesModel->setStyleThumbnailer(m_thumbnailer);
    m_paragraphStylesModel->setStyleManager(m_styleManager);
    ui->paragraphListView->setModel(m_paragraphStylesModel);
}

void ParagraphStylesTab::slotStyleSelected(const QModelIndex &index)
{
    KoParagraphStyle *style = static_cast<KoParagraphStyle*>(index.data(AbstractStylesModel::ParagraphStylePointer).value<void*>());
    if (style) {
        setDisplay(style);
    }
}

void ParagraphStylesTab::slotCreateNewStyle()
{
    KoParagraphStyle *newStyle = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->slotCreateNewStyle(ui->paragraphListView->currentIndex()));
    if (newStyle) {
        ui->paragraphListView->setCurrentIndex(m_paragraphStylesModel->indexForCharacterStyle(*newStyle));
        setDisplay(newStyle);
    }

}

void ParagraphStylesTab::slotSaveStyle() //TODO reselect the style, create a save all style in lieu of this one and connect appropriatly
{
//    KoParagraphStyle *style = static_cast<KoParagraphStyle*>(ui->paragraphListView->currentIndex().data(AbstractStylesModel::ParagraphStylePointer).value<void*>());
//    if (style) {
        m_paragraphStylesModel->saveStyle(ui->paragraphListView->currentIndex());

//    }
}

/*TODOs:
- inherit style on new style instead of cloning
- create our own KFontChooser. the stock kde one does not allow to disable individual font properties
*/

/// ///////////////////CharacterHighlightingTab slots

void ParagraphStylesTab::slotCapitalizationEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->KoCharacterStyle::hasProperty(QTextFormat::FontCapitalization)) {
            style->KoCharacterStyle::setFontCapitalization(style->KoCharacterStyle::fontCapitalization()); //set the capitalisation of the parent/default paragraph style. If none exists, 0 is returned by the function, which correspond to the "normal" mixed case font rendering.
        }
        else if (!enabled && style->KoCharacterStyle::hasProperty(QTextFormat::FontCapitalization)) {
            style->KoCharacterStyle::remove(QTextFormat::FontCapitalization);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotCapitalizationChanged(QFont::Capitalization capitalization)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setFontCapitalization(capitalization);

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotUnderlineEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->KoCharacterStyle::hasProperty(KoCharacterStyle::UnderlineType)) {
            style->KoCharacterStyle::setUnderlineType(style->underlineType()); //set the uderline of the parent/default paragraph style. If none exists, 0 is returned by the function, which correspond to no underline.
            style->KoCharacterStyle::setUnderlineStyle(style->underlineStyle());
            style->KoCharacterStyle::setUnderlineColor(style->underlineColor());
        }
        else if (!enabled && style->KoCharacterStyle::hasProperty(KoCharacterStyle::UnderlineType)) {
            style->KoCharacterStyle::remove(KoCharacterStyle::UnderlineType);
            style->KoCharacterStyle::remove(KoCharacterStyle::UnderlineStyle);
            style->KoCharacterStyle::remove(QTextFormat::TextUnderlineColor);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotUnderlineChanged(KoCharacterStyle::LineType lineType, KoCharacterStyle::LineStyle lineStyle, QColor lineColor)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setUnderlineType(lineType); //set the uderline of the parent/default paragraph style. If none exists, 0 is returned by the function, which correspond to no underline.
        style->KoCharacterStyle::setUnderlineStyle(lineStyle);
        style->KoCharacterStyle::setUnderlineColor(lineColor);

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotStrikethroughEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->KoCharacterStyle::hasProperty(KoCharacterStyle::StrikeOutType)) {
            style->KoCharacterStyle::setStrikeOutType(style->strikeOutType()); //set the uderline of the parent/default paragraph style. If none exists, 0 is returned by the function, which correspond to no underline.
            style->KoCharacterStyle::setStrikeOutStyle(style->strikeOutStyle());
            style->KoCharacterStyle::setStrikeOutColor(style->strikeOutColor());
        }
        else if (!enabled && style->KoCharacterStyle::hasProperty(KoCharacterStyle::StrikeOutType)) {
            style->KoCharacterStyle::remove(KoCharacterStyle::StrikeOutType);
            style->KoCharacterStyle::remove(KoCharacterStyle::StrikeOutStyle);
            style->KoCharacterStyle::remove(KoCharacterStyle::StrikeOutColor);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotStrikethroughChanged(KoCharacterStyle::LineType lineType, KoCharacterStyle::LineStyle lineStyle, QColor lineColor)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setStrikeOutType(lineType); //set the strikeout of the parent/default paragraph style. If none exists, 0 is returned by the function, which correspond to no strikeout.
        style->KoCharacterStyle::setStrikeOutStyle(lineStyle);
        style->KoCharacterStyle::setStrikeOutColor(lineColor);

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotPositionEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->KoCharacterStyle::hasProperty(QTextFormat::TextVerticalAlignment)) {
            style->KoCharacterStyle::setVerticalAlignment(style->KoCharacterStyle::verticalAlignment()); //set the vertical alignment of the parent/default paragraph style. If none exists, 0 is returned by the function, which correspond to the "normal" alignment.
        }
        else if (!enabled && style->KoCharacterStyle::hasProperty(QTextFormat::TextVerticalAlignment)) {
            style->KoCharacterStyle::remove(QTextFormat::TextVerticalAlignment);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotPositionChanged(QTextCharFormat::VerticalAlignment alignment)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setVerticalAlignment(alignment);

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotBackgroundColorEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->KoCharacterStyle::hasProperty(QTextFormat::BackgroundBrush)) {
            style->KoCharacterStyle::setBackground(style->background()); //set the background of the parent/default paragraph style. If none exists, an invalid brush is returned.
        }
        else if (!enabled && style->KoCharacterStyle::hasProperty(QTextFormat::BackgroundBrush)) {
            style->KoCharacterStyle::remove(QTextFormat::BackgroundBrush);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotBackgroundColorChanged(const QColor color)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setBackground(QBrush(color));

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotTextColorEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->KoCharacterStyle::hasProperty(QTextFormat::ForegroundBrush)) {
            style->KoCharacterStyle::setForeground(style->foreground()); //set the foreground of the parent/default paragraph style. If none exists, an invalid brush is returned.
        }
        else if (!enabled && style->KoCharacterStyle::hasProperty(QTextFormat::ForegroundBrush)) {
            style->KoCharacterStyle::remove(QTextFormat::ForegroundBrush);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotTextColorChanged(QColor color)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setForeground(QBrush(color));

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotFontEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && (!style->KoCharacterStyle::hasProperty(QTextFormat::FontFamily) || !style->KoCharacterStyle::hasProperty(QTextFormat::FontItalic) || !style->KoCharacterStyle::hasProperty(QTextFormat::FontWeight) || !style->KoCharacterStyle::hasProperty(QTextFormat::FontPointSize))) {
            style->KoCharacterStyle::setFontFamily(style->fontFamily()); //set the font of the parent/default paragraph style.
            style->KoCharacterStyle::setFontItalic(style->fontItalic());
            style->KoCharacterStyle::setFontWeight(style->fontWeight());
            style->KoCharacterStyle::setFontPointSize(style->fontPointSize());
        }
        else if (!enabled && (style->KoCharacterStyle::hasProperty(QTextFormat::FontFamily) || style->KoCharacterStyle::hasProperty(QTextFormat::FontItalic) || style->KoCharacterStyle::hasProperty(QTextFormat::FontWeight) || style->KoCharacterStyle::hasProperty(QTextFormat::FontPointSize))) {
            style->KoCharacterStyle::remove(QTextFormat::FontFamily);
            style->KoCharacterStyle::remove(QTextFormat::FontItalic);
            style->KoCharacterStyle::remove(QTextFormat::FontWeight);
            style->KoCharacterStyle::remove(QTextFormat::FontPointSize);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotFontChanged(const QFont &font)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setFontFamily(font.family());
        style->KoCharacterStyle::setFontItalic(font.italic());
        style->KoCharacterStyle::setFontWeight(font.weight());
        style->KoCharacterStyle::setFontPointSize(font.pointSize());

        setDisplay(style);
    }
}

/// ///////////////////////////ParagraphIndentSpacingTab slots

//indentation slots
void ParagraphStylesTab::slotLeftIndentEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->hasProperty(QTextFormat::BlockLeftMargin)) {
            style->setLeftMargin(QTextLength(QTextLength::FixedLength, style->leftMargin())); //set the left margin of the parent/default paragraph style. If none exists, 0 is returned.
        }
        else if (!enabled && style->hasProperty(QTextFormat::BlockLeftMargin)) {
            style->remove(QTextFormat::BlockLeftMargin);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotLeftIndentChanged(qreal value)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->setLeftMargin(QTextLength(QTextLength::FixedLength, value));

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotRightIndentEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->hasProperty(QTextFormat::BlockRightMargin)) {
            style->setRightMargin(QTextLength(QTextLength::FixedLength, style->rightMargin())); //set the right margin of the parent/default paragraph style. If none exists, 0 is returned.
        }
        else if (!enabled && style->hasProperty(QTextFormat::BlockRightMargin)) {
            style->remove(QTextFormat::BlockRightMargin);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotRightIndentChanged(qreal value)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->setRightMargin(QTextLength(QTextLength::FixedLength, value));

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotFirstLineIndentEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->hasProperty(QTextFormat::TextIndent)) {
            style->setTextIndent(QTextLength(QTextLength::FixedLength, style->textIndent())); //set the first line indent of the parent/default paragraph style. If none exists, 0 is returned.
        }
        else if (!enabled && style->hasProperty(QTextFormat::TextIndent)) {
            style->remove(QTextFormat::TextIndent);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotFirstLineIndentChanged(qreal value)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->setTextIndent(QTextLength(QTextLength::FixedLength, value));

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotAutoTextIndentChecked(bool checked)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->setAutoTextIndent(checked);

        setDisplay(style);
    }
}

//line spacing slots
void ParagraphStylesTab::slotLineSpacingEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && (!style->hasProperty(KoParagraphStyle::FixedLineHeight) || !style->hasProperty(KoParagraphStyle::LineSpacing) || !style->hasProperty(KoParagraphStyle::PercentLineHeight) || !style->hasProperty(KoParagraphStyle::MinimumLineHeight))) {
            style->setLineHeightAbsolute(style->lineHeightAbsolute());
            style->setLineHeightPercent(style->lineHeightPercent());
            style->setLineSpacing(style->lineSpacing());
            style->setMinimumLineHeight(QTextLength(QTextLength::FixedLength, style->minimumLineHeight()));
            style->setLineSpacingFromFont(style->lineSpacingFromFont());
        }
        else if (!enabled && (style->hasProperty(KoParagraphStyle::FixedLineHeight) || style->hasProperty(KoParagraphStyle::LineSpacing) || style->hasProperty(KoParagraphStyle::PercentLineHeight) || style->hasProperty(KoParagraphStyle::MinimumLineHeight))) {
            style->remove(KoParagraphStyle::FixedLineHeight);
            style->remove(KoParagraphStyle::LineSpacing);
            style->remove(KoParagraphStyle::PercentLineHeight);
            style->remove(KoParagraphStyle::MinimumLineHeight);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotLineSpacingChanged(ParagraphIndentSpacingTab::LineSpacingType spacingType, qreal value, bool useFontMetrics)
{
    kDebug() << "slot lineSpacing changed";
    kDebug() << "spacing type: " << spacingType;
    kDebug() << "value: " << value;
    kDebug() << "useFont: " << useFontMetrics;
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        switch (spacingType) {
        case ParagraphIndentSpacingTab::LineHeightProportional:
            style->setLineHeightAbsolute(0); // since it trumps percentage based line heights, unset it.
            style->setMinimumLineHeight(QTextLength(QTextLength::FixedLength, 0));
            style->setLineSpacing(0);
            style->setLineHeightPercent(int(value));
            style->setLineSpacingFromFont(useFontMetrics);
            break;
        case ParagraphIndentSpacingTab::LineSpacingAdditional:
            style->setLineHeightAbsolute(0); // since it trumps percentage based line heights, unset it.
            style->setMinimumLineHeight(QTextLength(QTextLength::FixedLength, 0));
            style->setLineSpacing(value);
            style->setLineHeightPercent(100);
            style->setLineSpacingFromFont(useFontMetrics);
            break;
        case ParagraphIndentSpacingTab::LineHeightFixed:
            style->setMinimumLineHeight(QTextLength(QTextLength::FixedLength, 0));
            style->setLineSpacing(0);
            style->setLineHeightPercent(100);
            style->setLineHeightAbsolute(style->lineHeightAbsolute());
            style->setLineSpacingFromFont(false);
            break;
        case ParagraphIndentSpacingTab::LineHeightMinimum:
            style->setLineHeightAbsolute(0); // since it trumps percentage based line heights, unset it.
            style->setLineHeightPercent(100);
            style->setLineSpacing(0);
            style->setMinimumLineHeight(QTextLength(QTextLength::FixedLength, style->minimumLineHeight()));
            style->setLineSpacingFromFont(false);
            break;
        }

        setDisplay(style);
    }
}

//paragraph spacing slots
void ParagraphStylesTab::slotTopParagraphSpacingEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->hasProperty(QTextFormat::BlockTopMargin)) {
            style->setTopMargin(QTextLength(QTextLength::FixedLength, style->topMargin()));
        }
        else if (!enabled && style->hasProperty(QTextFormat::BlockTopMargin)) {
            style->remove(QTextFormat::BlockTopMargin);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotTopParagraphSpacingChanged(qreal value)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->setTopMargin(QTextLength(QTextLength::FixedLength, value));

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotBottomParagraphSpacingEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->hasProperty(QTextFormat::BlockBottomMargin)) {
            style->setBottomMargin(QTextLength(QTextLength::FixedLength, style->bottomMargin()));
        }
        else if (!enabled && style->hasProperty(QTextFormat::BlockBottomMargin)) {
            style->remove(QTextFormat::BlockBottomMargin);
        }

        setDisplay(style);
    }
}

void ParagraphStylesTab::slotBottomParagraphSpacingChanged(qreal value)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->setBottomMargin(QTextLength(QTextLength::FixedLength, value));

        setDisplay(style);
    }
}
