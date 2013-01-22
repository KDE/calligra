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
}

CharacterHighlightingTab::~CharacterHighlightingTab()
{
    delete ui;
    ui = 0;
}

void CharacterHighlightingTab::init(bool uniqueFormat)
{
    m_uniqueFormat = uniqueFormat;

    QStringList list;
    KFontChooser::getFontList(list, KFontChooser::SmoothScalableFonts);
    m_fontChooser = new KFontChooser(this, (m_uniqueFormat) ? KFontChooser::NoDisplayFlags : KFontChooser::ShowDifferences, list, false);
    m_fontChooser->setSampleBoxVisible(false);
    ui->fontLayout->addWidget(m_fontChooser);

    ui->capitalizationList->addItems(capitalizationList());
    ui->underlineStyle->addItems(KoText::underlineTypeList());
    ui->underlineLineStyle->addItems(KoText::underlineStyleList());

    ui->positionList->addItems(fontLayoutPositionList());

    ui->strikethroughStyle->addItems(KoText::underlineTypeList()); //TODO make KoText consistent: either add strikethroughTypeList, or change from underlineTypeList to lineTypeList
    ui->strikethroughLineStyle->addItems(KoText::underlineStyleList()); //TODO idem

    connect(ui->underlineStyle, SIGNAL(activated(int)), this, SLOT(underlineTypeChanged(int)));
    connect(ui->underlineLineStyle, SIGNAL(activated(int)), this, SLOT(underlineStyleChanged(int)));
    connect(ui->underlineColor, SIGNAL(changed(QColor)), this, SLOT(underlineColorChanged(QColor)));

    connect(ui->strikethroughStyle, SIGNAL(activated(int)), this, SLOT(strikethroughTypeChanged(int)));
    connect(ui->strikethroughLineStyle, SIGNAL(activated(int)), this, SLOT(strikethroughStyleChanged(int)));
    connect(ui->strikethroughColor, SIGNAL(changed(QColor)), this, SLOT(strikethroughColorChanged(QColor)));

    connect(ui->capitalizationList, SIGNAL(activated(int)), this, SLOT(capitalisationChanged(int)));

    connect(ui->positionList, SIGNAL(activated(int)), this, SLOT(positionChanged(int)));

    connect(m_fontChooser, SIGNAL(fontSelected(const QFont &)), this, SIGNAL(fontChanged(const QFont &)));
    connect(m_fontChooser, SIGNAL(fontSelected(const QFont &)), this, SIGNAL(charStyleChanged()));

    const KIcon clearIcon(koIconName("edit-clear"));
    ui->resetTextColor->setIcon(clearIcon);
    ui->resetBackground->setIcon(clearIcon);
    connect(ui->textColor, SIGNAL(changed(const QColor&)), this, SLOT(textColorChanged()));
    connect(ui->backgroundColor, SIGNAL(changed(const QColor&)), this, SLOT(backgroundColorChanged()));
    connect(ui->resetTextColor, SIGNAL(clicked()), this, SLOT(clearTextColor()));
    connect(ui->resetBackground, SIGNAL(clicked()), this, SLOT(clearBackgroundColor()));
    connect(ui->enableText, SIGNAL(toggled(bool)), this, SLOT(textToggled(bool)));
    connect(ui->enableBackground, SIGNAL(toggled(bool)), this, SLOT(backgroundToggled(bool)));
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

void CharacterHighlightingTab::capitalisationChanged(int item)
{
    if (m_uniqueFormat || ui->capitalizationList->currentIndex() >= 0) {
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
    }
    emit charStyleChanged();
}

void CharacterHighlightingTab::positionChanged(int item)
{
    m_positionInherited = false;
    emit charStyleChanged();
}

void CharacterHighlightingTab::underlineTypeChanged(int item)
{
    ui->underlineLineStyle->setEnabled(item > 0);
    ui->underlineColor->setEnabled(item > 0);
    m_underlineTypeInherited = false;
    emit underlineChanged(indexToLineType(item), indexToLineStyle(ui->underlineLineStyle->currentIndex()), ui->underlineColor->color());
    emit charStyleChanged();
}

void CharacterHighlightingTab::underlineStyleChanged(int item)
{
    if (ui->underlineStyle->currentIndex())
        emit underlineChanged(indexToLineType(ui->underlineStyle->currentIndex()), indexToLineStyle(item), ui->underlineColor->color());
    m_underlineStyleInherited = false;
    emit charStyleChanged();
}

void CharacterHighlightingTab::underlineColorChanged(QColor color)
{
    if (ui->underlineStyle->currentIndex())
        emit underlineChanged(indexToLineType(ui->underlineStyle->currentIndex()), indexToLineStyle(ui->underlineLineStyle->currentIndex()), color);
    emit charStyleChanged();
}

void CharacterHighlightingTab::strikethroughTypeChanged(int item)
{
    ui->strikethroughLineStyle->setEnabled(item > 0);
    ui->strikethroughColor->setEnabled(item > 0);
    m_strikeoutTypeInherited = false;
    emit strikethroughChanged(indexToLineType(item), indexToLineStyle(ui->strikethroughLineStyle->currentIndex()), ui->strikethroughColor->color());
    emit charStyleChanged();
}

void CharacterHighlightingTab::strikethroughStyleChanged(int item)
{
    if (ui->strikethroughStyle->currentIndex())
        emit strikethroughChanged(indexToLineType(ui->strikethroughStyle->currentIndex()), indexToLineStyle(item), ui->strikethroughColor->color());
    m_strikeoutStyleInherited = false;
    emit charStyleChanged();
}

void CharacterHighlightingTab::strikethroughColorChanged(QColor color)
{
    if (ui->strikethroughStyle->currentIndex())
        emit strikethroughChanged(indexToLineType(ui->strikethroughStyle->currentIndex()), indexToLineStyle(ui->strikethroughLineStyle->currentIndex()), color);
    m_strikeoutcolorInherited = false;
    emit charStyleChanged();
}

void CharacterHighlightingTab::backgroundColorChanged()
{
    m_backgroundColorReset = false; m_backgroundColorChanged = true;
    if (ui->enableBackground->isChecked() && ui->backgroundColor->color().isValid())
        emit backgroundColorChanged(ui->backgroundColor->color());
    emit charStyleChanged();
}

void CharacterHighlightingTab::textColorChanged()
{
    m_textColorReset = false; m_textColorChanged = true;
    if (ui->enableText->isChecked() && ui->textColor->color().isValid())
        emit textColorChanged(ui->textColor->color());
    emit charStyleChanged();
}

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

void CharacterHighlightingTab::clearTextColor()
{
    ui->textColor->setColor(ui->textColor->defaultColor());
    m_textColorReset = true;
    emit textColorChanged(QColor(Qt::black));
    emit charStyleChanged();
}

void CharacterHighlightingTab::clearBackgroundColor()
{
    ui->backgroundColor->setColor(ui->backgroundColor->defaultColor());
    m_backgroundColorReset = true;
    emit backgroundColorChanged(QColor(Qt::transparent));
    emit charStyleChanged();
}

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
    if (style == 0)
        return;

    QFont font = style->font();
    QFontDatabase dbase;
    QStringList availableStyles = dbase.styles(font.family());
    if (font.italic() && !(availableStyles.contains(QString("Italic"))) && availableStyles.contains(QString("Oblique")))
        font.setStyle(QFont::StyleOblique);
    m_fontChooser->setFont(font);

    m_positionInherited  = !style->hasProperty(QTextFormat::TextVerticalAlignment);
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
    if (!m_uniqueFormat){
        ui->positionList->setEnabled(false);
        ui->positionList->setCurrentIndex(-1);
    }

    m_underlineStyleInherited = !style->hasProperty(KoCharacterStyle::UnderlineStyle);
    m_underlineTypeInherited = !style->hasProperty(KoCharacterStyle::UnderlineType);
    m_strikeoutStyleInherited = !style->hasProperty(KoCharacterStyle::StrikeOutStyle);
    m_strikeoutTypeInherited = !style->hasProperty(KoCharacterStyle::StrikeOutType);
    m_strikeoutcolorInherited = !style->hasProperty(KoCharacterStyle::StrikeOutColor);
    m_mixedCaseInherited = !style->hasProperty(QFont::MixedCase);
    m_smallCapsInherited = !style->hasProperty(QFont::SmallCaps);
    m_allUpperCaseInherited = !style->hasProperty(QFont::AllUppercase);
    m_allLowerCaseInherited = !style->hasProperty(QFont::AllLowercase);
    m_capitalizInherited = !style->hasProperty(QFont::Capitalize);

    //set the underline up
    ui->underlineStyle->setCurrentIndex(1);
    ui->underlineLineStyle->setCurrentIndex(lineStyleToIndex(style->underlineStyle()));
    if (m_uniqueFormat)
        ui->underlineStyle->setCurrentIndex(lineTypeToIndex(style->underlineType()));
    else
        ui->underlineStyle->setCurrentIndex(-1);

    underlineTypeChanged(ui->underlineStyle->currentIndex());
    ui->underlineColor->setColor(style->underlineColor());

    //set the strikethrough up
    ui->strikethroughStyle->setCurrentIndex(1);
    ui->strikethroughLineStyle->setCurrentIndex(lineStyleToIndex(style->strikeOutStyle()));
    if (m_uniqueFormat)
        ui->strikethroughStyle->setCurrentIndex(lineTypeToIndex(style->strikeOutType()));
    else
        ui->strikethroughStyle->setCurrentIndex(-1);
    strikethroughTypeChanged(ui->strikethroughStyle->currentIndex());
    ui->strikethroughColor->setColor(style->strikeOutColor());

    //Now set the capitalisation
    int index;
    switch (style->fontCapitalization()) {
    case QFont::MixedCase: ui->capitalizationList->setCurrentIndex(0);index=0; break;
    case QFont::SmallCaps: ui->capitalizationList->setCurrentIndex(1);index=1; break;
    case QFont::AllUppercase: ui->capitalizationList->setCurrentIndex(2);index=2; break;
    case QFont::AllLowercase: ui->capitalizationList->setCurrentIndex(3);index=3; break;
    case QFont::Capitalize: ui->capitalizationList->setCurrentIndex(4);index=4; break;
    default:
        ui->capitalizationList->setCurrentIndex(0);
        index =0;
        break;
    }

    if(m_uniqueFormat)
        capitalisationChanged(index);
    else{
        ui->capitalizationList->setCurrentIndex(-1);
        ui->capitalizationList->setEnabled(false);
    }

    //Set font decoration display
    ui->enableText->setVisible(!m_uniqueFormat);
    ui->enableText->setChecked(m_uniqueFormat);
    textToggled(m_uniqueFormat);
    ui->enableBackground->setVisible(!m_uniqueFormat);
    ui->enableBackground->setChecked(m_uniqueFormat);
    backgroundToggled(m_uniqueFormat);

    m_textColorChanged = false;
    m_backgroundColorChanged = false;
    m_textColorReset = ! style->hasProperty(QTextFormat::ForegroundBrush);
    if (m_textColorReset || (style->foreground().style() == Qt::NoBrush)) {
        clearTextColor();
    } else {
        ui->textColor->setColor(style->foreground().color());
    }
    m_backgroundColorReset = ! style->hasProperty(QTextFormat::BackgroundBrush);
    if (m_backgroundColorReset || (style->background().style() == Qt::NoBrush)) {
        clearBackgroundColor();
    } else {
        ui->backgroundColor->setColor(style->background().color());
    }
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

    if (ui->enableBackground->isChecked() && m_backgroundColorReset)
        style->setBackground(QBrush(Qt::NoBrush));
    else if (ui->enableBackground->isChecked() && m_backgroundColorChanged)
        style->setBackground(QBrush(ui->backgroundColor->color()));
    if (ui->enableText->isChecked() && m_textColorReset)
        style->setForeground(QBrush(Qt::NoBrush));
    else if (ui->enableText->isChecked() && m_textColorChanged)
        style->setForeground(QBrush(ui->textColor->color()));
}

#include <CharacterHighlightingTab.moc>
