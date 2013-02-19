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

//    ui->paragraphListView->setItemDelegate(m_stylesDelegate);
    connect(ui->paragraphListView, SIGNAL(activated(QModelIndex)), this, SLOT(slotStyleSelected(QModelIndex)));
    connect(ui->newStyleButton, SIGNAL(clicked()), this, SLOT(slotCreateNewStyle()));
    connect(ui->saveAllStylesButton, SIGNAL(clicked()), this, SLOT(slotSaveStyle()));

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
        ui->characterHighlighting->setDisplay(newStyle);
        ui->indentSpacing->setDisplay(newStyle);
    }

}

void ParagraphStylesTab::slotSaveStyle() //TODO reselect the style
{
//    KoParagraphStyle *style = static_cast<KoParagraphStyle*>(ui->paragraphListView->currentIndex().data(AbstractStylesModel::ParagraphStylePointer).value<void*>());
//    if (style) {
        m_paragraphStylesModel->saveStyle(ui->paragraphListView->currentIndex());

//    }
}

void ParagraphStylesTab::slotCapitalizationEnabled(bool enabled)
{
    KoParagraphStyle *style = dynamic_cast<KoParagraphStyle*>(m_paragraphStylesModel->unsavedStyle(ui->paragraphListView->currentIndex()));
    if (style) {
        if (enabled && !style->hasProperty(QTextFormat::FontCapitalization)) {
            style->setFontCapitalization(style->fontCapitalization()); //set the capitalisation of the parent/default paragraph style. If none exists, 0 is returned by the function, which correspond to the "normal" mixed case font rendering.
        }
        else if (!enable && style->hasProperty(QTextFormat::FontCapitalization)) {
            style->remove(QTextFormat::FontCapitalization);
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
    kDebug() << "underline enabled: " << enabled;
}

void ParagraphStylesTab::slotUnderlineChanged(KoCharacterStyle::LineType lineType, KoCharacterStyle::LineStyle lineStyle, QColor lineColor)
{
    kDebug() << "underline changed type: " << lineType << " style: " << lineStyle << " color: " << lineColor;
}

void ParagraphStylesTab::slotStrikethroughEnabled(bool enabled)
{
    kDebug() << "strikethrough enabled: " << enabled;
}

void ParagraphStylesTab::slotStrikethroughChanged(KoCharacterStyle::LineType lineType, KoCharacterStyle::LineStyle lineStyle, QColor lineColor)
{
        kDebug() << "strikethrough changed type: " << lineType << " style: " << lineStyle << " color: " << lineColor;
}

void ParagraphStylesTab::slotPositionEnabled(bool enabled)
{
    kDebug() << "position enabled: " << enabled;
}

void ParagraphStylesTab::slotPositionChanged(QTextCharFormat::VerticalAlignment alignment)
{
    kDebug() << "position changed: " << alignment;
}

void ParagraphStylesTab::slotBackgroundColorEnabled(bool enabled)
{
    kDebug() << "backgroundColor enabled: " << enabled;
}

void ParagraphStylesTab::slotBackgroundColorChanged(QColor color)
{
    kDebug() << "backgroundColor changed: " << color;
}

void ParagraphStylesTab::slotTextColorEnabled(bool enabled)
{
    kDebug() << "textColor enabled: " << enabled;
}

void ParagraphStylesTab::slotTextColorChanged(QColor color)
{
    kDebug() << "textColor changed: " << color;
}

void ParagraphStylesTab::slotFontEnabled(bool enabled)
{
    kDebug() << "font enabled: " << enabled;
}

void ParagraphStylesTab::slotFontChanged(const QFont &font)
{
    kDebug() << "font changed: " << font;
}

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
