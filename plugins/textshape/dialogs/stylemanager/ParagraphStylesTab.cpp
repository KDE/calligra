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
    connect(ui->indentSpacing, SIGNAL(lineSpacingChanged(int)), this, SLOT(slotLineSpacingChanged(int)));
    connect(ui->indentSpacing, SIGNAL(lineSpacingValueChanged(qreal)), this, SLOT(slotLineSpacingValueChanged(qreal)));
    connect(ui->indentSpacing, SIGNAL(lineSpacingPercentChanged(int)), this, SLOT(slotLineSpacingPercentChanged(int)));
    connect(ui->indentSpacing, SIGNAL(useFontMetricsChecked(bool)), this, SLOT(slotUseFontMetricsChecked(bool)));
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
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
    }
}

void ParagraphStylesTab::slotCreateNewStyle()
{
    KoParagraphStyle *newStyle = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->slotCreateNewStyle(ui->paragraphListView->currentIndex()));
    if (newStyle) {
        ui->paragraphListView->setCurrentIndex(m_paragraphStylesModel->indexForCharacterStyle(*newStyle));
        ui->characterHighlighting->setDisplay(newStyle);
        ui->indentSpacing->setDisplay(newStyle);
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
        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
    }
}

void ParagraphStylesTab::slotCapitalizationChanged(QFont::Capitalization capitalization)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setFontCapitalization(capitalization);

        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
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

        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
    }
}

void ParagraphStylesTab::slotUnderlineChanged(KoCharacterStyle::LineType lineType, KoCharacterStyle::LineStyle lineStyle, QColor lineColor)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setUnderlineType(lineType); //set the uderline of the parent/default paragraph style. If none exists, 0 is returned by the function, which correspond to no underline.
        style->KoCharacterStyle::setUnderlineStyle(lineStyle);
        style->KoCharacterStyle::setUnderlineColor(lineColor);

        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
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

        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
    }
}

void ParagraphStylesTab::slotStrikethroughChanged(KoCharacterStyle::LineType lineType, KoCharacterStyle::LineStyle lineStyle, QColor lineColor)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setStrikeOutType(lineType); //set the strikeout of the parent/default paragraph style. If none exists, 0 is returned by the function, which correspond to no strikeout.
        style->KoCharacterStyle::setStrikeOutStyle(lineStyle);
        style->KoCharacterStyle::setStrikeOutColor(lineColor);

        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
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
        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
    }
}

void ParagraphStylesTab::slotPositionChanged(QTextCharFormat::VerticalAlignment alignment)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setVerticalAlignment(alignment);

        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
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
        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
    }
}

void ParagraphStylesTab::slotBackgroundColorChanged(const QColor color)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setBackground(QBrush(color));

        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
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
        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
    }
}

void ParagraphStylesTab::slotTextColorChanged(QColor color)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        style->KoCharacterStyle::setForeground(QBrush(color));

        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
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

        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
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

        ui->paragraphListView->update(ui->paragraphListView->currentIndex());
        ui->characterHighlighting->setDisplay(style);
        ui->indentSpacing->setDisplay(style);
    }
}

/// ///////////////////////////ParagraphIndentSpacingTab slots

//indentation slots
void ParagraphStylesTab::slotLeftIndentEnabled(bool enabled)
{
    kDebug() << "leftIndent enabled: " << enabled;
}

void ParagraphStylesTab::slotLeftIndentChanged(qreal value)
{
    kDebug() << "new value: " << value;
}

void ParagraphStylesTab::slotRightIndentEnabled(bool enabled)
{
    kDebug() << "rightIndent enabled: " << enabled;
}

void ParagraphStylesTab::slotRightIndentChanged(qreal value)
{
    kDebug() << "new value: " << value;
}

void ParagraphStylesTab::slotFirstLineIndentEnabled(bool enabled)
{
    kDebug() << "firstLineIndent enabled: " << enabled;
}

void ParagraphStylesTab::slotFirstLineIndentChanged(qreal value)
{
    kDebug() << "new value: " << value;
}

void ParagraphStylesTab::slotAutoTextIndentChecked(bool checked)
{
    kDebug() << "checked: " << checked;
}

//line spacing slots
void ParagraphStylesTab::slotLineSpacingEnabled(bool enabled)
{
    kDebug() << "leftSpacing enabled: " << enabled;
}

void ParagraphStylesTab::slotLineSpacingChanged(int item)
{
    kDebug() << "selected item: " << item;
}

void ParagraphStylesTab::slotLineSpacingValueChanged(qreal value)
{
    kDebug() << "new value: " << value;
}

void ParagraphStylesTab::slotLineSpacingPercentChanged(int percent)
{
    kDebug() << "percent: " << percent;
}

void ParagraphStylesTab::slotUseFontMetricsChecked(bool checked)
{
    kDebug() << "checked: " << checked;
}

//paragraph spacing slots
void ParagraphStylesTab::slotTopParagraphSpacingEnabled(bool enabled)
{
    kDebug() << "topSpacing enabled: " << enabled;
}

void ParagraphStylesTab::slotTopParagraphSpacingChanged(qreal value)
{
    kDebug() << "new value: " << value;
}

void ParagraphStylesTab::slotBottomParagraphSpacingEnabled(bool enabled)
{
    kDebug() << "bottomSpacing enabled: " << enabled;
}

void ParagraphStylesTab::slotBottomParagraphSpacingChanged(qreal value)
{
    kDebug() << "new value: " << value;
}
