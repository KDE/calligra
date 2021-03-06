/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002 Montel Laurent <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>
   SPDX-FileCopyrightText: 2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CharacterHighlighting.h"

#include <KoText.h>
#include <KoCharacterStyle.h>
#include <KoIcon.h>

#include <QFontDatabase>
#include <QStringList>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTextCharFormat>

enum Position {
    Normal,
    Superscript,
    Subscript,
    Custom
};

CharacterHighlighting::CharacterHighlighting(QWidget* parent)
    : QWidget(parent)
{
    widget.setupUi(this);

    QStringList list;
    KFontChooser::getFontList(list, KFontChooser::ScalableFonts);
    m_fontChooser = new KFontChooser(this, KFontChooser::ShowDifferences, list, false);
    m_fontChooser->setSampleBoxVisible(false);
    widget.fontLayout->addWidget(m_fontChooser);

    widget.capitalizationList->addItems(capitalizationList());
    widget.underlineStyle->addItems(KoText::underlineTypeList());
    widget.underlineLineStyle->addItems(KoText::underlineStyleList());

    widget.positionList->addItems(fontLayoutPositionList());

    widget.strikethroughType->addItems(KoText::underlineTypeList()); //TODO make KoText consistent: either add strikethroughTypeList, or change from underlineTypeList to lineTypeList
    widget.strikethroughLineStyle->addItems(KoText::underlineStyleList()); //TODO idem

    connect(widget.underlineStyle, QOverload<int>::of(&QComboBox::activated), this, &CharacterHighlighting::underlineTypeChanged);
    connect(widget.underlineLineStyle, QOverload<int>::of(&QComboBox::activated), this, &CharacterHighlighting::underlineStyleChanged);
    connect(widget.underlineColor, &KColorButton::changed, this, &CharacterHighlighting::underlineColorChanged);

    connect(widget.strikethroughType, QOverload<int>::of(&QComboBox::activated), this, &CharacterHighlighting::strikethroughTypeChanged);
    connect(widget.strikethroughLineStyle, QOverload<int>::of(&QComboBox::activated), this, &CharacterHighlighting::strikethroughStyleChanged);
    connect(widget.strikethroughColor, &KColorButton::changed, this, &CharacterHighlighting::strikethroughColorChanged);

    connect(widget.capitalizationList, QOverload<int>::of(&QComboBox::activated), this, &CharacterHighlighting::capitalisationChanged);

    connect(widget.positionList, QOverload<int>::of(&QComboBox::activated), this, &CharacterHighlighting::positionChanged);

    connect(m_fontChooser, &KFontChooser::fontSelected, this, &CharacterHighlighting::fontChanged);
    connect(m_fontChooser, &KFontChooser::fontSelected, this, &CharacterHighlighting::charStyleChanged);

    const QIcon clearIcon = koIcon("edit-clear");
    widget.resetTextColor->setIcon(clearIcon);
    widget.resetBackground->setIcon(clearIcon);
    connect(widget.textColor, &KColorButton::changed, this, QOverload<>::of(&CharacterHighlighting::textColorChanged));
    connect(widget.backgroundColor, &KColorButton::changed, this, QOverload<>::of(&CharacterHighlighting::backgroundColorChanged));
    connect(widget.resetTextColor, &QAbstractButton::clicked, this, &CharacterHighlighting::clearTextColor);
    connect(widget.resetBackground, &QAbstractButton::clicked, this, &CharacterHighlighting::clearBackgroundColor);
    connect(widget.enableText, &QAbstractButton::toggled, this, &CharacterHighlighting::textToggled);
    connect(widget.enableBackground, &QAbstractButton::toggled, this, &CharacterHighlighting::backgroundToggled);
}

KoCharacterStyle::LineType CharacterHighlighting::indexToLineType(int index)
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

KoCharacterStyle::LineStyle CharacterHighlighting::indexToLineStyle(int index)
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

int CharacterHighlighting::lineTypeToIndex(KoCharacterStyle::LineType type)
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

int CharacterHighlighting::lineStyleToIndex(KoCharacterStyle::LineStyle type)
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

void CharacterHighlighting::capitalisationChanged(int item)
{
    switch (item) {
    case 0:
        emit capitalizationChanged(QFont::MixedCase);
        break;
    case 1:
        emit capitalizationChanged(QFont::SmallCaps);
        break;
    case 2:
        emit capitalizationChanged(QFont::AllUppercase);
        break;
    case 3:
        emit capitalizationChanged(QFont::AllLowercase);
        break;
    case 4:
        emit capitalizationChanged(QFont::Capitalize);
        break;
    }
    m_capitalizationInherited = false;
    emit charStyleChanged();
}

void CharacterHighlighting::positionChanged(int item)
{
    Q_UNUSED(item);
    m_positionInherited = false;
    emit charStyleChanged();
}

void CharacterHighlighting::underlineTypeChanged(int item)
{
    widget.underlineLineStyle->setEnabled(item > 0);
    widget.underlineColor->setEnabled(item > 0);
    m_underlineInherited = false;
    emit underlineChanged(indexToLineType(item), indexToLineStyle(widget.underlineLineStyle->currentIndex()), widget.underlineColor->color());
    emit charStyleChanged();
}

void CharacterHighlighting::underlineStyleChanged(int item)
{
    if (widget.underlineStyle->currentIndex())
        emit underlineChanged(indexToLineType(widget.underlineStyle->currentIndex()), indexToLineStyle(item), widget.underlineColor->color());
    m_underlineInherited = false;
    emit charStyleChanged();
}

void CharacterHighlighting::underlineColorChanged(QColor color)
{
    if (widget.underlineStyle->currentIndex())
        emit underlineChanged(indexToLineType(widget.underlineStyle->currentIndex()), indexToLineStyle(widget.underlineLineStyle->currentIndex()), color);
    m_underlineInherited = false;
    emit charStyleChanged();
}

void CharacterHighlighting::strikethroughTypeChanged(int item)
{
    widget.strikethroughLineStyle->setEnabled(item > 0);
    widget.strikethroughColor->setEnabled(item > 0);
    m_strikeoutInherited = false;
    emit strikethroughChanged(indexToLineType(item), indexToLineStyle(widget.strikethroughLineStyle->currentIndex()), widget.strikethroughColor->color());
    emit charStyleChanged();
}

void CharacterHighlighting::strikethroughStyleChanged(int item)
{
    if (widget.strikethroughType->currentIndex())
        emit strikethroughChanged(indexToLineType(widget.strikethroughType->currentIndex()), indexToLineStyle(item), widget.strikethroughColor->color());
    m_strikeoutInherited = false;
    emit charStyleChanged();
}

void CharacterHighlighting::strikethroughColorChanged(QColor color)
{
    if (widget.strikethroughType->currentIndex())
        emit strikethroughChanged(indexToLineType(widget.strikethroughType->currentIndex()), indexToLineStyle(widget.strikethroughLineStyle->currentIndex()), color);
    m_strikeoutInherited = false;
    emit charStyleChanged();
}

void CharacterHighlighting::backgroundColorChanged()
{
    m_backgroundColorReset = false; m_backgroundColorChanged = true;
    if (widget.enableBackground->isChecked() && widget.backgroundColor->color().isValid())
        emit backgroundColorChanged(widget.backgroundColor->color());
    emit charStyleChanged();
}

void CharacterHighlighting::textColorChanged()
{
    m_textColorReset = false; m_textColorChanged = true;
    if (widget.enableText->isChecked() && widget.textColor->color().isValid())
        emit textColorChanged(widget.textColor->color());
    emit charStyleChanged();
}

void CharacterHighlighting::textToggled(bool state)
{
    widget.textColor->setEnabled(state);
    widget.resetTextColor->setEnabled(state);
    emit charStyleChanged();
}

void CharacterHighlighting::backgroundToggled(bool state)
{
    widget.backgroundColor->setEnabled(state);
    widget.resetBackground->setEnabled(state);
    emit charStyleChanged();
}

void CharacterHighlighting::clearTextColor()
{
    widget.textColor->setColor(widget.textColor->defaultColor());
    m_textColorReset = true;
    emit textColorChanged(QColor(Qt::black));
    emit charStyleChanged();
}

void CharacterHighlighting::clearBackgroundColor()
{
    widget.backgroundColor->setColor(widget.backgroundColor->defaultColor());
    m_backgroundColorReset = true;
    emit backgroundColorChanged(QColor(Qt::transparent));
    emit charStyleChanged();
}

QStringList CharacterHighlighting::capitalizationList()
{
    QStringList lst;
    lst <<i18n("Normal");
    lst <<i18n("Small Caps");
    lst <<i18n("Uppercase");
    lst <<i18n("Lowercase");
    lst <<i18n("Capitalize");
    return lst;
}

QStringList CharacterHighlighting::fontLayoutPositionList()
{
    QStringList lst;
    lst <<i18n("Normal");
    lst <<i18n("Superscript");
    lst <<i18n("Subscript");
    return lst;
}
void CharacterHighlighting::setDisplay(KoCharacterStyle *style, bool directFormattingMode)
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
        widget.positionList->setCurrentIndex(1);
        break;
    case QTextCharFormat::AlignSubScript:
        widget.positionList->setCurrentIndex(2);
        break;
    default:
        // TODO check if its custom instead.
        widget.positionList->setCurrentIndex(0);
    }

    m_underlineInherited = directFormattingMode || (!style->hasProperty(KoCharacterStyle::UnderlineStyle)
                            && !style->hasProperty(KoCharacterStyle::UnderlineType)
                            && !style->hasProperty(QTextFormat::TextUnderlineColor));
    m_strikeoutInherited = directFormattingMode || (!style->hasProperty(KoCharacterStyle::StrikeOutStyle)
                            && !style->hasProperty(KoCharacterStyle::StrikeOutType)
                            && !style->hasProperty(KoCharacterStyle::StrikeOutColor));
    m_capitalizationInherited = directFormattingMode || !style->hasProperty(QTextFormat::FontCapitalization);

    //set the underline up
    widget.underlineStyle->setCurrentIndex(1);
    widget.underlineLineStyle->setCurrentIndex(lineStyleToIndex(style->underlineStyle()));
    widget.underlineStyle->setCurrentIndex(lineTypeToIndex(style->underlineType()));
    widget.underlineColor->setColor(style->underlineColor());

    //set the strikethrough up
    widget.strikethroughType->setCurrentIndex(1);
    widget.strikethroughLineStyle->setCurrentIndex(lineStyleToIndex(style->strikeOutStyle()));
    widget.strikethroughType->setCurrentIndex(lineTypeToIndex(style->strikeOutType()));
    widget.strikethroughColor->setColor(style->strikeOutColor());

    //Now set the capitalisation
    switch (style->fontCapitalization()) {
    case QFont::MixedCase:
        widget.capitalizationList->setCurrentIndex(0);
        break;
    case QFont::SmallCaps:
        widget.capitalizationList->setCurrentIndex(1);
        break;
    case QFont::AllUppercase:
        widget.capitalizationList->setCurrentIndex(2);
        break;
    case QFont::AllLowercase:
        widget.capitalizationList->setCurrentIndex(3);
        break;
    case QFont::Capitalize:
        widget.capitalizationList->setCurrentIndex(4);
        break;
    default:
        widget.capitalizationList->setCurrentIndex(0);
        break;
    }

    //Set font decoration display
    m_textColorChanged = false;
    m_textColorReset = ! style->hasProperty(QTextFormat::ForegroundBrush);
    widget.enableText->setChecked(m_textColorReset);
    if (style->foreground().style() == Qt::NoBrush) {
        widget.textColor->setColor(widget.textColor->defaultColor());
    } else {
        widget.textColor->setColor(style->foreground().color());
    }

    m_backgroundColorChanged = false;
    m_backgroundColorReset = ! style->hasProperty(QTextFormat::BackgroundBrush);
    widget.enableBackground->setChecked(m_backgroundColorReset);
    if (style->background().style() == Qt::NoBrush) {
        widget.backgroundColor->setColor(widget.backgroundColor->defaultColor());
    } else {
        widget.backgroundColor->setColor(style->background().color());
    }
}

void CharacterHighlighting::save(KoCharacterStyle *style)
{
    if (style == 0)
        return;


    KFontChooser::FontDiffFlags fontDiff = m_fontChooser->fontDiffFlags();
    if (fontDiff & KFontChooser::FontDiffFamily){
        style->setFontFamily(m_fontChooser->font().family());
    }
    if (fontDiff & KFontChooser::FontDiffSize){
        style->setFontPointSize(m_fontChooser->font().pointSize());
    }
    if (fontDiff & KFontChooser::FontDiffStyle) {
        style->setFontWeight(m_fontChooser->font().weight());
        style->setFontItalic(m_fontChooser->font().italic()); //TODO should set style instead of italic
    }

    if (!m_underlineInherited) {
        style->setUnderlineStyle(indexToLineStyle(widget.underlineLineStyle->currentIndex()));
        style->setUnderlineColor(widget.underlineColor->color());
        style->setUnderlineType(indexToLineType(widget.underlineStyle->currentIndex()));
        if (widget.underlineStyle->currentIndex() == 0) {
            style->setUnderlineStyle(KoCharacterStyle::NoLineStyle);
        }
    }

    if (!m_strikeoutInherited) {
        style->setStrikeOutStyle(indexToLineStyle(widget.strikethroughLineStyle->currentIndex()));
        style->setStrikeOutColor(widget.strikethroughColor->color());
        style->setStrikeOutType(indexToLineType(widget.strikethroughType->currentIndex()));
        if (widget.strikethroughType->currentIndex() == 0) {
            style->setStrikeOutStyle(KoCharacterStyle::NoLineStyle);
        }
    }

    if (!m_capitalizationInherited) {
        if (widget.capitalizationList->currentIndex() == 0)
            style->setFontCapitalization(QFont::MixedCase);
        else if (widget.capitalizationList->currentIndex() == 1)
            style->setFontCapitalization(QFont::SmallCaps);
        else if (widget.capitalizationList->currentIndex() == 2)
            style->setFontCapitalization(QFont::AllUppercase);
        else if (widget.capitalizationList->currentIndex() == 3)
            style->setFontCapitalization(QFont::AllLowercase);
        else if (widget.capitalizationList->currentIndex() == 4)
            style->setFontCapitalization(QFont::Capitalize);
    }

    if (!m_positionInherited ) {
        QTextCharFormat::VerticalAlignment va;
        if (widget.positionList->currentIndex() == 0)
            va = QTextCharFormat::AlignNormal;
        else if (widget.positionList->currentIndex() == 2)
            va = QTextCharFormat::AlignSubScript;
        else if (widget.positionList->currentIndex() == 1)
            va = QTextCharFormat::AlignSuperScript;
        else
            va = QTextCharFormat::AlignNormal;
        style->setVerticalAlignment(va);
    }

    if (widget.enableBackground->isChecked() && m_backgroundColorReset)
        style->setBackground(QBrush(Qt::NoBrush));
    else if (widget.enableBackground->isChecked() && m_backgroundColorChanged)
        style->setBackground(QBrush(widget.backgroundColor->color()));
    if (widget.enableText->isChecked() && m_textColorReset)
        style->setForeground(QBrush(Qt::NoBrush));
    else if (widget.enableText->isChecked() && m_textColorChanged)
        style->setForeground(QBrush(widget.textColor->color()));
}
