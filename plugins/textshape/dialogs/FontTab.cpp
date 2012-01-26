/* This file is part of the KDE project
   Copyright (C)  2001, 2002 Montel Laurent <lmontel@mandrakesoft.com>
   Copyright (C)  2006 Thomas Zander <zander@kde.org>
   Copyright (C)  2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "FontTab.h"

#include <kfontdialog.h>

#include <KoCharacterStyle.h>

#include <QFontDatabase>
#include <QStringList>
#include <QVBoxLayout>

#include <KDebug>
#include <KoTextDebug.h>

FontTab::FontTab(bool uniqueFormat, QWidget* parent)
        : QWidget(parent),
        m_uniqueFormat(uniqueFormat)
{
    QLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    QStringList list;
    KFontChooser::getFontList(list, KFontChooser::SmoothScalableFonts);
    m_fontChooser = new KFontChooser(this, KFontChooser::DisplayFrame, list, false);
    m_fontChooser->setSampleBoxVisible(false);

    layout->addWidget(m_fontChooser);

    connect(m_fontChooser, SIGNAL(fontSelected(const QFont &)), this, SLOT(slotFontChanged(const QFont &)));
}

void FontTab::slotFontChanged(const QFont &font)
{
    if (!m_ignoreSignals) {
        if (m_originalFont.family() != font.family()){
            m_fontFamilyInherited = false;
        }
        if (m_originalFont.pointSize() != font.pointSize()) {
            m_fontSizeInherited = false;
        }
        if (m_originalFont.weight() != font.weight()) {
            m_fontBoldInherited = false;
        }
        if (m_originalFont.italic() != font.italic()) {
            m_fontItalicInherited = false;
        }
    }
    emit fontChanged(font);
}

void FontTab::setDisplay(const KoCharacterStyle* displayStyle)
{
//First deal with fonts which don't have the italic property but oblique instead.
    m_ignoreSignals = true;

    m_fontFamilyInherited = !displayStyle->hasProperty(QTextFormat::FontFamily);
    m_fontSizeInherited = !displayStyle->hasProperty(QTextFormat::FontPointSize);
    m_fontBoldInherited = !displayStyle->hasProperty(QTextFormat::FontWeight);
    m_fontItalicInherited = !displayStyle->hasProperty(QTextFormat::FontItalic);
    QFont font = displayStyle->font();
    QFontDatabase dbase;
    QStringList availableStyles = dbase.styles(font.family());
    if (font.italic() && !(availableStyles.contains(QString("Italic"))) && availableStyles.contains(QString("Oblique")))
        font.setStyle(QFont::StyleOblique);

    m_fontChooser->setFont(font);
    m_originalFont = font;
    m_ignoreSignals = false;
}

void FontTab::save(KoCharacterStyle* style) const
{
    KFontChooser::FontDiffFlags fontDiff = m_fontChooser->fontDiffFlags();
    if (!m_fontFamilyInherited && (m_uniqueFormat || (fontDiff & KFontChooser::FontDiffFamily))){
        style->setFontFamily(m_fontChooser->font().family());
    }
    if (!m_fontSizeInherited && (m_uniqueFormat || (fontDiff & KFontChooser::FontDiffSize))){
        style->setFontPointSize(m_fontChooser->font().pointSize());
    }
    if (!m_fontBoldInherited && (m_uniqueFormat || (fontDiff & KFontChooser::FontDiffStyle))) {
        style->setFontWeight(m_fontChooser->font().weight());
    }
    if (!m_fontItalicInherited && (m_uniqueFormat || (fontDiff & KFontChooser::FontDiffStyle))) {
        style->setFontItalic(m_fontChooser->font().italic()); //TODO should set style instead of italic
    }
}

#include <FontTab.moc>
