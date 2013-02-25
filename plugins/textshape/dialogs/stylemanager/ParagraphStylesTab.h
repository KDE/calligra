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

#ifndef PARAGRAPHSTYLESTAB_H
#define PARAGRAPHSTYLESTAB_H

#include <KoCharacterStyle.h>
#include <KoParagraphStyle.h>

#include <QModelIndex>
#include <QWidget>

class StylesModel;
class StylesDelegate;
class StylesManagerStylesModel;

class KoStyleManager;
class KoStyleThumbnailer;

namespace Ui {
class ParagraphStylesTab;
}

class ParagraphStylesTab : public QWidget
{
    Q_OBJECT

public:
    explicit ParagraphStylesTab(QWidget *parent = 0);
    ~ParagraphStylesTab();

    void setStyleManager(KoStyleManager *manager);
    void setDisplay(KoParagraphStyle *style);

private slots:
    void slotStyleSelected(const QModelIndex &index);
    void slotCreateNewStyle();
    void slotSaveStyle();

    void slotCapitalizationEnabled(bool enabled);
    void slotCapitalizationChanged(QFont::Capitalization capitalization);
    void slotUnderlineEnabled(bool enabled);
    void slotUnderlineChanged(KoCharacterStyle::LineType lineType, KoCharacterStyle::LineStyle lineStyle, QColor lineColor);
    void slotStrikethroughEnabled(bool enabled);
    void slotStrikethroughChanged(KoCharacterStyle::LineType lineType, KoCharacterStyle::LineStyle lineStyle, QColor lineColor);
    void slotPositionEnabled(bool enabled);
    void slotPositionChanged(QTextCharFormat::VerticalAlignment alignment);
    void slotBackgroundColorEnabled(bool enabled);
    void slotBackgroundColorChanged(const QColor color);
    void slotTextColorEnabled(bool enabled);
    void slotTextColorChanged(QColor color);
    void slotFontEnabled(bool enabled);
    void slotFontChanged(const QFont &font);

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
    Ui::ParagraphStylesTab *ui;

    KoStyleManager *m_styleManager;
    KoStyleThumbnailer *m_thumbnailer;

    StylesModel *m_sourceModel;
    StylesDelegate *m_stylesDelegate;
    StylesManagerStylesModel *m_paragraphStylesModel;
};

#endif // PARAGRAPHSTYLESTAB_H
