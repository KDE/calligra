/* This file is part of the KDE project
   Copyright (C)  2001, 2002 Montel Laurent <lmontel@mandrakesoft.com>
   Copyright (C)  2006-2007 Thomas Zander <zander@kde.org>
   Copyright (C)  2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
   Copyright (C)  2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

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

#include "CharacterHighlightingTab.h"
#include "ui_CharacterHighlightingTab.h"

#include <KoText.h>
#include <KoCharacterStyle.h>
#include <KoIcon.h>

#include <kfontdialog.h>
#include <QFontDatabase>
#include <QStringList>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTextCharFormat>

#include <KDebug>

enum Position {
    Normal,
    Superscript,
    Subscript,
    Custom
};

CharacterHighlightingTab::CharacterHighlightingTab(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::CharacterHighlightingTab)
{
    ui->setupUi(this);

    QStringList list;
    KFontChooser::getFontList(list, KFontChooser::SmoothScalableFonts);
    m_fontChooser = new KFontChooser(this, KFontChooser::NoDisplayFlags, list, false);
    m_fontChooser->setSampleBoxVisible(false);
    ui->fontLayout->addWidget(m_fontChooser);
    connect(ui->fontLabel, SIGNAL(toggled(bool)), this, SLOT(slotFontEnabled(bool)));
    connect(m_fontChooser, SIGNAL(fontSelected(const QFont &)), this, SIGNAL(fontChanged(const QFont &)));
    connect(m_fontChooser, SIGNAL(fontSelected(const QFont &)), this, SIGNAL(charStyleChanged()));

    //capitalization
    ui->capitalizationList->addItems(capitalizationList());
    connect(ui->capitalizationList, SIGNAL(activated(int)), this, SLOT(slotCapitalisationChanged(int)));
    connect(ui->capitalizationLabel, SIGNAL(toggled(bool)), this, SLOT(slotCapitalisationEnabled(bool)));

    //underline
    ui->underlineStyle->addItems(KoText::lineTypeList());
    ui->underlineLineStyle->addItems(KoText::lineStyleList());
    connect(ui->underlineLabel, SIGNAL(toggled(bool)), this, SLOT(slotUnderlineEnabled(bool)));
    connect(ui->underlineStyle, SIGNAL(activated(int)), this, SLOT(slotUnderlineTypeChanged(int)));
    connect(ui->underlineLineStyle, SIGNAL(activated(int)), this, SLOT(slotUnderlineStyleChanged(int)));
    connect(ui->underlineColor, SIGNAL(changed(QColor)), this, SLOT(slotUnderlineColorChanged(QColor)));

    //position
    ui->positionList->addItems(fontLayoutPositionList());
    connect(ui->positionLabel, SIGNAL(toggled(bool)), this, SLOT(slotPositionEnabled(bool)));
    connect(ui->positionList, SIGNAL(activated(int)), this, SLOT(slotPositionChanged(int)));

    //strikethrough
    ui->strikethroughStyle->addItems(KoText::lineTypeList());
    ui->strikethroughLineStyle->addItems(KoText::lineStyleList());
    connect(ui->strikethroughLabel, SIGNAL(toggled(bool)), this, SLOT(slotStrikethroughEnabled(bool)));
    connect(ui->strikethroughStyle, SIGNAL(activated(int)), this, SLOT(slotStrikethroughTypeChanged(int)));
    connect(ui->strikethroughLineStyle, SIGNAL(activated(int)), this, SLOT(slotStrikethroughStyleChanged(int)));
    connect(ui->strikethroughColor, SIGNAL(changed(QColor)), this, SLOT(slotStrikethroughColorChanged(QColor)));


    const KIcon clearIcon(koIconName("edit-clear"));
    //text color
    ui->resetTextColor->setIcon(clearIcon);
    connect(ui->textColorLabel, SIGNAL(toggled(bool)), this, SLOT(slotTextColorEnabled(bool)));
    connect(ui->textColor, SIGNAL(changed(const QColor&)), this, SLOT(slotTextColorChanged()));
    connect(ui->resetTextColor, SIGNAL(clicked()), this, SLOT(slotClearTextColor()));

    //background color
    ui->resetBackground->setIcon(clearIcon);
    connect(ui->backgroundColorLabel, SIGNAL(toggled(bool)), this, SLOT(slotBackgroundColorEnabled(bool)));
    connect(ui->backgroundColor, SIGNAL(changed(const QColor&)), this, SLOT(slotBackgroundColorChanged()));
    connect(ui->resetBackground, SIGNAL(clicked()), this, SLOT(slotClearBackgroundColor()));
}

CharacterHighlightingTab::~CharacterHighlightingTab()
{
    delete ui;
    ui = 0;
}

KoCharacterStyle::LineType CharacterHighlightingTab::indexToLineType(int index)
{
    KoCharacterStyle::LineType lineType;
    switch (index) {
    case 1: lineType = KoCharacterStyle::SingleLine; break;
    case 2: lineType = KoCharacterStyle::DoubleLine; break;
    case 0:
    default:
        lineType = KoCharacterStyle::NoLineType; break;
    }
    return lineType;
}

KoCharacterStyle::LineStyle CharacterHighlightingTab::indexToLineStyle(int index)
{
    KoCharacterStyle::LineStyle lineStyle;
    switch (index) {
    case 1: lineStyle = KoCharacterStyle::DashLine; break;
    case 2: lineStyle = KoCharacterStyle::DottedLine; break;
    case 3: lineStyle = KoCharacterStyle::DotDashLine; break;
    case 4: lineStyle = KoCharacterStyle::DotDotDashLine; break;
    case 5: lineStyle = KoCharacterStyle::WaveLine; break;
    case 0:
    default:
        lineStyle = KoCharacterStyle::SolidLine; break;
    }
    return lineStyle;
}

int CharacterHighlightingTab::lineTypeToIndex(KoCharacterStyle::LineType type)
{
    int index;
    switch (type) {
    case KoCharacterStyle::NoLineType: index = 0; break;
    case KoCharacterStyle::SingleLine: index = 1; break;
    case KoCharacterStyle::DoubleLine: index = 2; break;
    default: index = 0; break;
    }
    return index;
}

int CharacterHighlightingTab::lineStyleToIndex(KoCharacterStyle::LineStyle type)
{
    int index;
    switch (type) {
    case KoCharacterStyle::SolidLine: index = 0; break;
    case KoCharacterStyle::DashLine: index = 1; break;
    case KoCharacterStyle::DottedLine: index = 2; break;
    case KoCharacterStyle::DotDashLine: index = 3; break;
    case KoCharacterStyle::DotDotDashLine: index = 4; break;
    case KoCharacterStyle::WaveLine: index = 5; break;
    default: index = 0; break;
    }
    return index;
}

void CharacterHighlightingTab::slotCapitalisationEnabled(bool enabled)
{
    ui->capitalizationList->setEnabled(enabled);
    emit capitalizationEnabled(enabled);
}

void CharacterHighlightingTab::slotCapitalisationChanged(int item)
{
//    if (m_uniqueFormat || ui->capitalizationList->currentIndex() >= 0) {
        switch (item) {
        case 0:
            emit capitalizationChanged(QFont::MixedCase);
            m_mixedCaseInherited = false;
            break;
        case 1:
            emit capitalizationChanged(QFont::SmallCaps);
            m_smallCapsInherited = false;
            break;
        case 2:
            emit capitalizationChanged(QFont::AllUppercase);
            m_allUpperCaseInherited = false;
            break;
        case 3:
            emit capitalizationChanged(QFont::AllLowercase);
            m_allLowerCaseInherited = false;
            break;
        case 4:
            emit capitalizationChanged(QFont::Capitalize);
            m_capitalizInherited = false;
            break;
        }
//    }
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotUnderlineEnabled(bool enabled)
{
    ui->underlineStyle->setEnabled(enabled);
    ui->underlineColor->setEnabled(enabled && (ui->underlineStyle->currentIndex()>0));
    ui->underlineLineStyle->setEnabled(enabled && (ui->underlineStyle->currentIndex()>0));
    emit underlineEnabled(enabled);
}

void CharacterHighlightingTab::slotUnderlineTypeChanged(int item)
{
    ui->underlineLineStyle->setEnabled(item > 0 && ui->underlineStyle->isEnabled());
    ui->underlineColor->setEnabled(item > 0 && ui->underlineStyle->isEnabled());
    m_underlineTypeInherited = false;
    emit underlineChanged(indexToLineType(item), indexToLineStyle(ui->underlineLineStyle->currentIndex()), ui->underlineColor->color());
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotUnderlineStyleChanged(int item)
{
    if (ui->underlineStyle->currentIndex()) {
        emit underlineChanged(indexToLineType(ui->underlineStyle->currentIndex()), indexToLineStyle(item), ui->underlineColor->color());
    }
    m_underlineStyleInherited = false;
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotUnderlineColorChanged(QColor color)
{
    if (ui->underlineStyle->currentIndex()) {
        emit underlineChanged(indexToLineType(ui->underlineStyle->currentIndex()), indexToLineStyle(ui->underlineLineStyle->currentIndex()), color);
    }
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotStrikethroughEnabled(bool enabled)
{
    ui->strikethroughStyle->setEnabled(enabled);
    ui->strikethroughColor->setEnabled(enabled && (ui->strikethroughStyle->currentIndex()>0));
    ui->strikethroughLineStyle->setEnabled(enabled && (ui->strikethroughStyle->currentIndex()>0));
    emit strikethroughEnabled(enabled);
}

void CharacterHighlightingTab::slotStrikethroughTypeChanged(int item)
{
    ui->strikethroughLineStyle->setEnabled(item > 0 && ui->strikethroughStyle->isEnabled());
    ui->strikethroughColor->setEnabled(item > 0 && ui->strikethroughStyle->isEnabled());
    m_strikeoutTypeInherited = false;
    emit strikethroughChanged(indexToLineType(item), indexToLineStyle(ui->strikethroughLineStyle->currentIndex()), ui->strikethroughColor->color());
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotStrikethroughStyleChanged(int item)
{
    if (ui->strikethroughStyle->currentIndex()) {
        emit strikethroughChanged(indexToLineType(ui->strikethroughStyle->currentIndex()), indexToLineStyle(item), ui->strikethroughColor->color());
    }
    m_strikeoutStyleInherited = false;
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotStrikethroughColorChanged(QColor color)
{
    if (ui->strikethroughStyle->currentIndex()) {
        emit strikethroughChanged(indexToLineType(ui->strikethroughStyle->currentIndex()), indexToLineStyle(ui->strikethroughLineStyle->currentIndex()), color);
    }
    m_strikeoutcolorInherited = false;
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotPositionEnabled(bool enabled)
{
    ui->positionList->setEnabled(enabled);
    emit positionEnabled(enabled);
}

void CharacterHighlightingTab::slotPositionChanged(int item)
{
    m_positionInherited = false;
    switch(item) {
    case 0:
        emit positionChanged(QTextCharFormat::AlignNormal);
        break;
    case 1:
        emit positionChanged(QTextCharFormat::AlignSuperScript);
        break;
    case 2:
        emit positionChanged(QTextCharFormat::AlignSubScript);
        break;
    }
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotBackgroundColorEnabled(bool enabled)
{
    ui->backgroundColor->setEnabled(enabled);
    ui->resetBackground->setEnabled(enabled);
    emit backgroundColorEnabled(enabled);
}

void CharacterHighlightingTab::slotClearBackgroundColor()
{
    ui->backgroundColor->setColor(ui->backgroundColor->defaultColor());
    m_backgroundColorReset = true;
    emit backgroundColorChanged(QColor(Qt::transparent));
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotBackgroundColorChanged()
{
    m_backgroundColorReset = false; m_backgroundColorChanged = true;
//    if (/*ui->enableBackground->isChecked() && */ui->backgroundColor->color().isValid())
        emit backgroundColorChanged(ui->backgroundColor->color());
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotTextColorEnabled(bool enabled)
{
    ui->textColor->setEnabled(enabled);
    ui->resetTextColor->setEnabled(enabled);
    emit textColorEnabled(enabled);
}

void CharacterHighlightingTab::slotClearTextColor()
{
    ui->textColor->setColor(ui->textColor->defaultColor());
    m_textColorReset = true;
    emit textColorChanged(QColor(Qt::black));
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotTextColorChanged()
{
    m_textColorReset = false; m_textColorChanged = true;
//    if (/*ui->enableText->isChecked() && */ui->textColor->color().isValid())
        emit textColorChanged(ui->textColor->color());
//    emit charStyleChanged();
}

void CharacterHighlightingTab::slotFontEnabled(bool enabled)
{
//    ui->fontChooser->setEnabled(enabled);
    m_fontChooser->setEnabled(enabled);
    emit fontEnabled(enabled);
}

void CharacterHighlightingTab::slotFontChanged(const QFont &font)
{
    emit slotFontChanged(font);
}

/*
void CharacterHighlightingTab::textToggled(bool state)
{
    ui->textColor->setEnabled(state);
    ui->resetTextColor->setEnabled(state);
    emit charStyleChanged();
}

void CharacterHighlightingTab::backgroundToggled(bool state)
{
    ui->backgroundColor->setEnabled(state);
    ui->resetBackground->setEnabled(state);
    emit charStyleChanged();
}
*/

QStringList CharacterHighlightingTab::capitalizationList()
{
    QStringList lst;
    lst <<i18n("Normal");
    lst <<i18n("Small Caps");
    lst <<i18n("Uppercase");
    lst <<i18n("Lowercase");
    lst <<i18n("Capitalize");
    return lst;
}

QStringList CharacterHighlightingTab::fontLayoutPositionList()
{
    QStringList lst;
    lst <<i18n("Normal");
    lst <<i18n("Superscript");
    lst <<i18n("Subscript");
    return lst;
}
void CharacterHighlightingTab::setDisplay(KoCharacterStyle *style)
{
    if (style == 0) {
        return;
    }

    blockSignals(true);

    bool checked;
    //font
    QFont font = style->font();
    checked = style->hasProperty(QTextFormat::FontFamily) || style->hasProperty(QTextFormat::FontItalic) || style->hasProperty(QTextFormat::FontWeight) || style->hasProperty(QTextFormat::FontPointSize);
    ui->fontLabel->setChecked(checked);
    slotFontEnabled(checked);
    QFontDatabase dbase;
    QStringList availableStyles = dbase.styles(font.family());
    if (font.italic() && !(availableStyles.contains(QString("Italic"))) && availableStyles.contains(QString("Oblique"))) {
        font.setStyle(QFont::StyleOblique);
    }
    m_fontChooser->setFont(font);

    //position
    m_positionInherited  = !style->hasProperty(QTextFormat::TextVerticalAlignment);
    checked = style->hasProperty(QTextFormat::TextVerticalAlignment);
    switch (style->verticalAlignment()) {
    case QTextCharFormat::AlignSuperScript:
        ui->positionList->setCurrentIndex(1);
        break;
    case QTextCharFormat::AlignSubScript:
        ui->positionList->setCurrentIndex(2);
        break;
    default:
        // TODO check if its custom instead.
        ui->positionList->setCurrentIndex(0);
    }
    ui->positionLabel->setChecked(checked);
    slotPositionEnabled(checked);

    //underline
    m_underlineStyleInherited = !style->hasProperty(KoCharacterStyle::UnderlineStyle);
    m_underlineTypeInherited = !style->hasProperty(KoCharacterStyle::UnderlineType);

    checked = style->hasProperty(KoCharacterStyle::UnderlineType);
    ui->underlineStyle->setCurrentIndex(lineTypeToIndex(style->underlineType()));
    ui->underlineLineStyle->setCurrentIndex(lineStyleToIndex(style->underlineStyle()));
    ui->underlineColor->setColor(style->underlineColor());
    ui->underlineLabel->setChecked(checked);
    slotUnderlineEnabled(checked);

    //strikethrough
    m_strikeoutStyleInherited = !style->hasProperty(KoCharacterStyle::StrikeOutStyle);
    m_strikeoutTypeInherited = !style->hasProperty(KoCharacterStyle::StrikeOutType);
    m_strikeoutcolorInherited = !style->hasProperty(KoCharacterStyle::StrikeOutColor);

    checked = style->hasProperty(KoCharacterStyle::StrikeOutType);
    ui->strikethroughStyle->setCurrentIndex(lineTypeToIndex(style->strikeOutType()));
    ui->strikethroughLineStyle->setCurrentIndex(lineStyleToIndex(style->strikeOutStyle()));
    ui->strikethroughColor->setColor(style->strikeOutColor());
    ui->strikethroughLabel->setChecked(checked);
    slotStrikethroughEnabled(checked);

    //capitalisation
    m_mixedCaseInherited = !style->hasProperty(QFont::MixedCase);
    m_smallCapsInherited = !style->hasProperty(QFont::SmallCaps);
    m_allUpperCaseInherited = !style->hasProperty(QFont::AllUppercase);
    m_allLowerCaseInherited = !style->hasProperty(QFont::AllLowercase);
    m_capitalizInherited = !style->hasProperty(QFont::Capitalize);

    checked = style->hasProperty(QTextFormat::FontCapitalization);
    switch (style->fontCapitalization()) {
    case QFont::MixedCase:
        ui->capitalizationList->setCurrentIndex(0);
        break;
    case QFont::SmallCaps:
        ui->capitalizationList->setCurrentIndex(1);
        break;
    case QFont::AllUppercase:
        ui->capitalizationList->setCurrentIndex(2);
        break;
    case QFont::AllLowercase:
        ui->capitalizationList->setCurrentIndex(3);
        break;
    case QFont::Capitalize:
        ui->capitalizationList->setCurrentIndex(4);
        break;
    default:
        ui->capitalizationList->setCurrentIndex(0);
        break;
    }
    ui->capitalizationLabel->setChecked(checked);
    slotCapitalisationEnabled(checked);

    //Set font decoration display
//    ui->enableText->setVisible(!m_uniqueFormat);
//    ui->enableText->setChecked(m_uniqueFormat);
//    slotTextToggled(m_uniqueFormat);
//    ui->enableBackground->setVisible(!m_uniqueFormat);
//    ui->enableBackground->setChecked(m_uniqueFormat);
//    backgroundToggled(m_uniqueFormat);

    //text color
    checked = style->hasProperty(QTextFormat::ForegroundBrush);
    if (!style->hasProperty(QTextFormat::ForegroundBrush) || (style->foreground().style() == Qt::NoBrush)) {
        slotClearTextColor();
    } else {
        ui->textColor->setColor(style->foreground().color());
    }
    ui->textColorLabel->setChecked(checked);
    slotTextColorEnabled(checked);

    //background color
    checked = style->hasProperty(QTextFormat::BackgroundBrush);
    if (!style->hasProperty(QTextFormat::BackgroundBrush) || (style->background().style() == Qt::NoBrush)) {
        slotClearBackgroundColor();
    } else {
        ui->backgroundColor->setColor(style->background().color());
    }
    ui->backgroundColorLabel->setChecked(checked);
    slotBackgroundColorEnabled(checked);

    blockSignals(false);
}

void CharacterHighlightingTab::save(KoCharacterStyle *style)
{
    if (style == 0)
        return;

    kDebug() << "QFont() weight: " << QFont().weight();
    KFontChooser::FontDiffFlags fontDiff = m_fontChooser->fontDiffFlags();
    if (m_uniqueFormat || (fontDiff & KFontChooser::FontDiffFamily)){
        style->setFontFamily(m_fontChooser->font().family());
    }
    if (m_uniqueFormat || (fontDiff & KFontChooser::FontDiffSize)){
        style->setFontPointSize(m_fontChooser->font().pointSize());
    }
    if (m_uniqueFormat || (fontDiff & KFontChooser::FontDiffStyle)) {
        style->setFontWeight(m_fontChooser->font().weight());
        style->setFontItalic(m_fontChooser->font().italic()); //TODO should set style instead of italic
    }

    if (ui->underlineStyle->currentIndex() == 0) {
        style->setUnderlineType(KoCharacterStyle::NoLineType);
        style->setUnderlineStyle(KoCharacterStyle::NoLineStyle);
    } else if (ui->underlineStyle->currentIndex() > 0) {
        if (!m_underlineTypeInherited) {
            style->setUnderlineType(indexToLineType(ui->underlineStyle->currentIndex()));
        }
        if (!m_underlineStyleInherited) {
            style->setUnderlineStyle(indexToLineStyle(ui->underlineLineStyle->currentIndex()));
        }
        style->setUnderlineColor(ui->underlineColor->color());
    }

    if (ui->strikethroughStyle->currentIndex() == 0) {
        style->setStrikeOutType(KoCharacterStyle::NoLineType);
        style->setStrikeOutStyle(KoCharacterStyle::NoLineStyle);
    } else if (ui->strikethroughStyle->currentIndex() > 0) {
        if (!m_strikeoutTypeInherited) {
            style->setStrikeOutType(indexToLineType(ui->strikethroughStyle->currentIndex()));
        }
        if (!m_strikeoutStyleInherited) {
            style->setStrikeOutStyle(indexToLineStyle(ui->strikethroughLineStyle->currentIndex()));
        }
        if (!m_strikeoutcolorInherited) {
            style->setStrikeOutColor(ui->strikethroughColor->color());
        }
    }
    if (m_uniqueFormat || ui->capitalizationList->currentIndex() >= 0) {
        if (ui->capitalizationList->currentIndex() == 0 && !m_mixedCaseInherited)
            style->setFontCapitalization(QFont::MixedCase);
        else if (ui->capitalizationList->currentIndex() == 1 && !m_smallCapsInherited)
            style->setFontCapitalization(QFont::SmallCaps);
        else if (ui->capitalizationList->currentIndex() == 2 && !m_allUpperCaseInherited)
            style->setFontCapitalization(QFont::AllUppercase);
        else if (ui->capitalizationList->currentIndex() == 3 && !m_allLowerCaseInherited)
            style->setFontCapitalization(QFont::AllLowercase);
        else if (ui->capitalizationList->currentIndex() == 4 && !m_capitalizInherited)
            style->setFontCapitalization(QFont::Capitalize);
    }


    QTextCharFormat::VerticalAlignment va;
    if (m_uniqueFormat || ui->positionList->currentIndex() >= 0){
        if (!m_positionInherited ) {
            if (ui->positionList->currentIndex() == 0)
                va = QTextCharFormat::AlignNormal;
            else if (ui->positionList->currentIndex() == 2)
                va = QTextCharFormat::AlignSubScript;
            else if (ui->positionList->currentIndex() == 1)
                va = QTextCharFormat::AlignSuperScript;
            else
                va = QTextCharFormat::AlignNormal;
            style->setVerticalAlignment(va);
        }
    }

    if (/*ui->enableBackground->isChecked() &&*/ m_backgroundColorReset)
        style->setBackground(QBrush(Qt::NoBrush));
    else if (/*ui->enableBackground->isChecked() &&*/ m_backgroundColorChanged)
        style->setBackground(QBrush(ui->backgroundColor->color()));
    if (/*ui->enableText->isChecked() &&*/ m_textColorReset)
        style->setForeground(QBrush(Qt::NoBrush));
    else if (/*ui->enableText->isChecked() && */m_textColorChanged)
        style->setForeground(QBrush(ui->textColor->color()));
}

#include <CharacterHighlightingTab.moc>
