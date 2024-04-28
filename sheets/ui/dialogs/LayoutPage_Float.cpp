/* This file is part of the KDE project
             SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
             SPDX-FileCopyrightText: 2004-2022 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999-2002 Harri Porten <porten@kde.org>
             SPDX-FileCopyrightText: 2000-2001 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
             SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "LayoutPage_Float.h"
#include "LayoutWidgets.h"

#include "core/Style.h"
#include "engine/Localization.h"
#include <core/Map.h>
#include <core/ValueFormatter.h>

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QPainter>
#include <QRadioButton>
#include <QSpinBox>

#include <KComboBox>
#include <KLineEdit>
#include <QListWidgetItem>

using namespace Calligra::Sheets;

LayoutPageFloat::LayoutPageFloat(QWidget *parent, Localization *locale, ValueFormatter *formatter)
    : QWidget(parent)
    , m_locale(locale)
    , m_formatter(formatter)
{
    m_bFormatColorChanged = false;

    QVBoxLayout *layout = new QVBoxLayout(this);

    QGroupBox *grp = new QGroupBox(i18n("Format"), this);
    QGridLayout *grid = new QGridLayout(grp);

    int fHeight = grp->fontMetrics().height();
    grid->addItem(new QSpacerItem(0, fHeight / 2), 0, 0); // groupbox title

    generic = new QRadioButton(i18n("Generic"), grp);
    generic->setWhatsThis(
        i18n("This is the default format and Calligra Sheets autodetects the actual data type depending on the current cell data. By default, Calligra Sheets "
             "right justifies numbers, dates and times within a cell and left justifies anything else."));
    grid->addWidget(generic, 1, 0);

    number = new QRadioButton(i18n("Number"), grp);
    number->setWhatsThis(
        i18n("The number notation uses the notation you globally choose in System Settings -> Common Appearance and Behavior -> Locale -> Country/Region & "
             "Language -> Numbers tab. Numbers are right justified by default."));
    grid->addWidget(number, 2, 0);

    percent = new QRadioButton(i18n("Percent"), grp);
    percent->setWhatsThis(
        i18n("When you have a number in the current cell and you switch from the dcell format from Generic to Percent, the current cell number will be "
             "multiplied by 100%.\nFor example if you enter 12 and set the cell format to Percent, the number will then be 1,200 %. Switching back to Generic "
             "cell format will bring it back to 12.\nYou can also use the Percent icon in the Format Toolbar."));
    grid->addWidget(percent, 3, 0);

    money = new QRadioButton(i18n("Money"), grp);
    money->setWhatsThis(
        i18n("The Money format converts your number into money notation using the settings globally fixed in System Settings -> Common Appearance and Behavior "
             "-> Locale -> Country/Region & Language -> Money. The currency symbol will be displayed and the precision will be the one set in System "
             "Settings.\nYou can also use the Currency icon in the Format Toolbar to set the cell formatting to look like your current currency."));
    grid->addWidget(money, 4, 0);

    scientific = new QRadioButton(i18n("Scientific"), grp);
    scientific->setWhatsThis(
        i18n("The scientific format changes your number using the scientific notation. For example, 0.0012 will be changed to 1.2E-03. Going back using "
             "Generic cell format will display 0.0012 again."));
    grid->addWidget(scientific, 5, 0);

    fraction = new QRadioButton(i18n("Fraction"), grp);
    fraction->setWhatsThis(
        i18n("The fraction format changes your number into a fraction. For example, 0.1 can be changed to 1/8, 2/16, 1/10, etc. You define the type of "
             "fraction by choosing it in the field on the right. If the exact fraction is not possible in the fraction mode you choose, the nearest closest "
             "match is chosen.\n For example: when we have 1.5 as number, we choose Fraction and Sixteenths 1/16 the text displayed into cell is \"1 8/16\" "
             "which is an exact fraction. If you have 1.4 as number in your cell and you choose Fraction and Sixteenths 1/16 then the cell will display \"1 "
             "6/16\" which is the nearest closest Sixteenth fraction."));
    grid->addWidget(fraction, 6, 0);

    date = new QRadioButton(i18n("Date"), grp);
    date->setWhatsThis(
        i18n("To enter a date, you should enter it in one of the formats set in System Settings -> Common Appearance and Behavior -> Locale -> Country/Region "
             "& Language -> Time & Dates. There are two formats set here: the date format and the short date format.\nJust like you can drag down numbers you "
             "can also drag down dates and the next cells will also get dates."));
    grid->addWidget(date, 7, 0);

    time = new QRadioButton(i18n("Time"), grp);
    time->setWhatsThis(
        i18n("This formats your cell content as a time. To enter a time, you should enter it in the Time format set in System Settings -> Common Appearance "
             "and Behavior -> Locale -> Country/Region & Language -> Date & Time. In the Cell Format dialog box you can set how the time should be displayed "
             "by choosing one of the available time format options. The default format is the system format set in System Settings. When the number in the "
             "cell does not make sense as a time, Calligra Sheets will display 00:00 in the global format you have in System Settings."));
    grid->addWidget(time, 8, 0);

    datetime = new QRadioButton(i18n("Datetime"), grp);
    datetime->setWhatsThis(
        i18n("To enter a datetime, you should enter it in one of the formats set in System Settings -> Common Appearance and Behavior -> Locale -> "
             "Country/Region & Language -> Time & Dates. There are two formats set here: the date format and the short date format.\nJust like you can drag "
             "down numbers you can also drag down dates and the next cells will also get datetimes."));
    grid->addWidget(datetime, 9, 0);

    textFormat = new QRadioButton(i18n("Text"), grp);
    textFormat->setWhatsThis(
        i18n("This formats your cell content as text. This can be useful if you want a number treated as text instead as a number, for example for a ZIP code. "
             "Setting a number as text format will left justify it. When numbers are formatted as text, they cannot be used in calculations or formulas. It "
             "also change the way the cell is justified."));
    grid->addWidget(textFormat, 10, 0);

    customFormat = new QRadioButton(i18n("Custom"), grp);
    customFormat->setWhatsThis(i18n("The custom format does not work yet. To be enabled in the next release."));
    grid->addWidget(customFormat, 11, 0);

    QGroupBox *box2 = new QGroupBox(grp);
    box2->setTitle(i18n("Preview"));
    QGridLayout *grid3 = new QGridLayout(box2);

    exampleLabel = new QLabel(box2);
    exampleLabel->setWhatsThis(i18n("This will display a preview of your choice so you can know what it does before clicking the OK button to validate it."));
    grid3->addWidget(exampleLabel, 0, 1);

    grid->addWidget(box2, 9, 1, 3, 1);

    customFormatEdit = new KLineEdit(grp);
    grid->addWidget(customFormatEdit, 0, 1);

    listFormat = new QListWidget(grp);
    grid->addWidget(listFormat, 1, 1, 8, 1);
    listFormat->setWhatsThis(i18n("Displays choices of format for the fraction, date or time formats."));
    layout->addWidget(grp);

    /* *** */

    QGroupBox *box = new QGroupBox(this);

    grid = new QGridLayout(box);

    postfix = new KLineEdit(box);
    postfix->setWhatsThis(i18n("You can add here a Postfix such as a $HK symbol to the end of each cell content in the checked format."));
    grid->addWidget(postfix, 2, 1);
    precision = new QSpinBox(box);
    precision->setSpecialValueText(i18n("variable"));
    precision->setRange(-1, 10);
    precision->setSingleStep(1);
    precision->setWhatsThis(
        i18n("You can control how many digits are displayed after the decimal point for numeric values. This can also be changed using the Increase precision "
             "or Decrease precision icons in the Format toolbar. "));
    grid->addWidget(precision, 1, 1);

    prefix = new KLineEdit(box);
    prefix->setWhatsThis(i18n("You can add here a Prefix such as a $ symbol at the start of each cell content in the checked format."));
    grid->addWidget(prefix, 0, 1);

    format = new KComboBox(box);
    format->setWhatsThis(i18n("You can choose whether positive values are displayed with a leading + sign and whether negative values are shown in red."));
    grid->addWidget(format, 0, 3);

    QLabel *tmpQLabel;
    tmpQLabel = new QLabel(box);
    grid->addWidget(tmpQLabel, 2, 0);
    tmpQLabel->setText(i18n("Postfix:"));

    tmpQLabel = new QLabel(box);
    grid->addWidget(tmpQLabel, 0, 0);
    tmpQLabel->setText(i18n("Prefix:"));

    tmpQLabel = new QLabel(box);
    grid->addWidget(tmpQLabel, 1, 0);
    tmpQLabel->setText(i18n("Precision:"));

    QPixmap formatOnlyNegSignedPixmap = paintFormatPixmap("123.456", Qt::black, "-123.456", Qt::black);
    QPixmap formatRedOnlyNegSignedPixmap = paintFormatPixmap("123.456", Qt::black, "-123.456", Qt::red);
    QPixmap formatRedNeverSignedPixmap = paintFormatPixmap("123.456", Qt::black, "123.456", Qt::red);
    QPixmap formatAlwaysSignedPixmap = paintFormatPixmap("+123.456", Qt::black, "-123.456", Qt::black);
    QPixmap formatRedAlwaysSignedPixmap = paintFormatPixmap("+123.456", Qt::black, "-123.456", Qt::red);

    format->setIconSize(QSize(150, 14));
    format->insertItem(0, formatOnlyNegSignedPixmap, "");
    format->insertItem(1, formatRedOnlyNegSignedPixmap, "");
    format->insertItem(2, formatRedNeverSignedPixmap, "");
    format->insertItem(3, formatAlwaysSignedPixmap, "");
    format->insertItem(4, formatRedAlwaysSignedPixmap, "");

    tmpQLabel = new QLabel(box);
    grid->addWidget(tmpQLabel, 0, 2);
    tmpQLabel->setText(i18n("Format:"));

    currencyLabel = new QLabel(box);
    grid->addWidget(currencyLabel, 1, 2);
    currencyLabel->setText(i18n("Currency:"));

    currency = new KComboBox(box);
    grid->addWidget(currency, 1, 3);

    m_currencies = Currency::symbols();
    int index = 0;
    // fill the currency combo box
    currency->insertItem(0, i18n("Automatic"));
    for (QString curr : m_currencies) {
        Currency c(curr);
        QString symbol = c.symbol();
        QString text = curr;
        if (symbol.size() && symbol != curr)
            text += " (" + c.symbol() + ")";
        index++;
        currency->insertItem(index, text);
    }
    currency->setCurrentIndex(0);

    layout->addWidget(box);

    connect(generic, &QAbstractButton::clicked, this, &LayoutPageFloat::slotChangeState);
    connect(fraction, &QAbstractButton::clicked, this, &LayoutPageFloat::slotChangeState);
    connect(money, &QAbstractButton::clicked, this, &LayoutPageFloat::slotChangeState);
    connect(datetime, &QAbstractButton::clicked, this, &LayoutPageFloat::slotChangeState);
    connect(date, &QAbstractButton::clicked, this, &LayoutPageFloat::slotChangeState);
    connect(scientific, &QAbstractButton::clicked, this, &LayoutPageFloat::slotChangeState);
    connect(number, &QAbstractButton::clicked, this, &LayoutPageFloat::slotChangeState);
    connect(percent, &QAbstractButton::clicked, this, &LayoutPageFloat::slotChangeState);
    connect(time, &QAbstractButton::clicked, this, &LayoutPageFloat::slotChangeState);
    connect(textFormat, &QAbstractButton::clicked, this, &LayoutPageFloat::slotChangeState);
    connect(customFormat, &QAbstractButton::clicked, this, &LayoutPageFloat::slotChangeState);

    connect(listFormat, &QListWidget::itemSelectionChanged, this, &LayoutPageFloat::makeformat);
    connect(precision, QOverload<int>::of(&QSpinBox::valueChanged), this, &LayoutPageFloat::slotChangeValue);
    connect(prefix, &QLineEdit::textChanged, this, &LayoutPageFloat::makeformat);
    connect(postfix, &QLineEdit::textChanged, this, &LayoutPageFloat::makeformat);
    connect(currency, &KComboBox::textActivated, this, &LayoutPageFloat::currencyChanged);
    connect(format, QOverload<int>::of(&KComboBox::activated), this, &LayoutPageFloat::formatChanged);
    connect(format, QOverload<int>::of(&KComboBox::activated), this, &LayoutPageFloat::makeformat);
    slotChangeState();
    this->resize(400, 400);
}

QPixmap LayoutPageFloat::paintFormatPixmap(const char *_string1, const QColor &_color1, const char *_string2, const QColor &_color2)
{
    QPixmap pixmap(150, 14);
    pixmap.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pixmap);
    painter.setPen(_color1);
    painter.drawText(2, 11, _string1);
    painter.setPen(_color2);
    painter.drawText(75, 11, _string2);
    painter.end();

    return pixmap;
}

void LayoutPageFloat::formatChanged(int)
{
    m_bFormatColorChanged = true;
}

void LayoutPageFloat::slotChangeValue(int)
{
    makeformat();
}
void LayoutPageFloat::slotChangeState()
{
    QStringList list;
    listFormat->clear();
    currency->hide();
    currencyLabel->hide();

    // start with enabled, they get disabled when inappropriate further down
    precision->setEnabled(true);
    prefix->setEnabled(true);
    postfix->setEnabled(true);
    format->setEnabled(true);
    if (generic->isChecked() || number->isChecked() || percent->isChecked() || scientific->isChecked() || textFormat->isChecked())
        listFormat->setEnabled(false);
    else if (money->isChecked()) {
        listFormat->setEnabled(false);
        precision->setValue(2);
        currency->show();
        currencyLabel->show();
    } else if (datetime->isChecked()) {
        format->setEnabled(false);
        precision->setEnabled(false);
        prefix->setEnabled(false);
        postfix->setEnabled(false);
        listFormat->setEnabled(true);
        initDatetime();
    } else if (date->isChecked()) {
        format->setEnabled(false);
        precision->setEnabled(false);
        prefix->setEnabled(false);
        postfix->setEnabled(false);
        listFormat->setEnabled(true);
        initDate();
    } else if (fraction->isChecked()) {
        precision->setEnabled(false);
        listFormat->setEnabled(true);
        list += i18n("Halves 1/2");
        list += i18n("Quarters 1/4");
        list += i18n("Eighths 1/8");
        list += i18n("Sixteenths 1/16");
        list += i18n("Tenths 1/10");
        list += i18n("Hundredths 1/100");
        list += i18n("One digit 5/9");
        list += i18n("Two digits 15/22");
        list += i18n("Three digits 153/652");
        listFormat->addItems(list);
        if (cellFormatType == Format::fraction_half)
            listFormat->setCurrentRow(0);
        else if (cellFormatType == Format::fraction_quarter)
            listFormat->setCurrentRow(1);
        else if (cellFormatType == Format::fraction_eighth)
            listFormat->setCurrentRow(2);
        else if (cellFormatType == Format::fraction_sixteenth)
            listFormat->setCurrentRow(3);
        else if (cellFormatType == Format::fraction_tenth)
            listFormat->setCurrentRow(4);
        else if (cellFormatType == Format::fraction_hundredth)
            listFormat->setCurrentRow(5);
        else if (cellFormatType == Format::fraction_one_digit)
            listFormat->setCurrentRow(6);
        else if (cellFormatType == Format::fraction_two_digits)
            listFormat->setCurrentRow(7);
        else if (cellFormatType == Format::fraction_three_digits)
            listFormat->setCurrentRow(8);
        else
            listFormat->setCurrentRow(0);
    } else if (time->isChecked()) {
        precision->setEnabled(false);
        prefix->setEnabled(false);
        postfix->setEnabled(false);
        format->setEnabled(false);
        listFormat->setEnabled(true);
        initTime();
    }

    if (customFormat->isChecked()) {
        customFormatEdit->setHidden(false);
        precision->setEnabled(false);
        prefix->setEnabled(false);
        postfix->setEnabled(false);
        format->setEnabled(false);
        listFormat->setEnabled(true);
    } else
        customFormatEdit->setHidden(true);

    makeformat();
}

void LayoutPageFloat::initDate()
{
    const QDate tmpDate(QDate(2000, 2, 9));
    QListWidgetItem *currentItem = nullptr;
    for (int i = Format::DatesBegin; i < Format::DatesEnd; ++i) {
        auto s = m_formatter->dateFormat(tmpDate, (Format::Type)i);
        if (s.isEmpty()) {
            continue;
        }
        auto item = new QListWidgetItem(s);
        item->setData(Qt::UserRole, i);
        listFormat->addItem(item);
        if (i == cellFormatType) {
            currentItem = item;
        }
    }
    currentItem ? listFormat->setCurrentItem(currentItem) : listFormat->setCurrentRow(0);
}

void LayoutPageFloat::initTime()
{
    QDateTime tmpTime(QDate(1900, 1, 1), QTime(8, 5, 6, 35), Qt::UTC);

    QListWidgetItem *currentItem = nullptr;
    for (int i = Format::TimesBegin; i < Format::TimesEnd; ++i) {
        auto s = m_formatter->timeFormat(tmpTime, (Format::Type)i);
        if (s.isEmpty()) {
            continue;
        }
        auto item = new QListWidgetItem(s);
        item->setData(Qt::UserRole, i);
        listFormat->addItem(item);
        if (i == cellFormatType) {
            currentItem = item;
        }
    }
    currentItem ? listFormat->setCurrentItem(currentItem) : listFormat->setCurrentRow(0);
}

void LayoutPageFloat::initDatetime()
{
    const QDateTime tmpDate(QDate(2000, 2, 9), QTime(6, 7, 8), Qt::UTC);
    QListWidgetItem *currentItem = nullptr;
    for (int i = Format::DateTimesBegin; i < Format::DateTimesEnd; ++i) {
        auto s = m_formatter->dateTimeFormat(tmpDate, (Format::Type)i);
        if (s.isEmpty()) {
            continue;
        }
        auto item = new QListWidgetItem(s);
        item->setData(Qt::UserRole, i);
        listFormat->addItem(item);
        if (i == cellFormatType) {
            currentItem = item;
        }
    }
    currentItem ? listFormat->setCurrentItem(currentItem) : listFormat->setCurrentRow(0);
}

void LayoutPageFloat::currencyChanged(const QString &)
{
    int index = currency->currentIndex() - 1;
    if (index < 0)
        m_currency = Currency();
    else
        m_currency = Currency(m_currencies[index]);

    makeformat();
}

void LayoutPageFloat::updateFormatType()
{
    if (generic->isChecked()) {
        newFormatType = Format::Generic;
    } else if (number->isChecked()) {
        newFormatType = Format::Number;
    } else if (percent->isChecked()) {
        newFormatType = Format::Percentage;
    } else if (money->isChecked()) {
        newFormatType = Format::Money;
    } else if (scientific->isChecked()) {
        newFormatType = Format::Scientific;
    } else if (fraction->isChecked()) {
        newFormatType = Format::fraction_half;
        switch (listFormat->currentRow()) {
        case 0:
            newFormatType = Format::fraction_half;
            break;
        case 1:
            newFormatType = Format::fraction_quarter;
            break;
        case 2:
            newFormatType = Format::fraction_eighth;
            break;
        case 3:
            newFormatType = Format::fraction_sixteenth;
            break;
        case 4:
            newFormatType = Format::fraction_tenth;
            break;
        case 5:
            newFormatType = Format::fraction_hundredth;
            break;
        case 6:
            newFormatType = Format::fraction_one_digit;
            break;
        case 7:
            newFormatType = Format::fraction_two_digits;
            break;
        case 8:
            newFormatType = Format::fraction_three_digits;
            break;
        }
    } else if (datetime->isChecked()) {
        const auto item = listFormat->currentItem();
        newFormatType = item ? (Format::Type)(item->data(Qt::UserRole).toInt()) : Format::DateTimesBegin;
    } else if (date->isChecked()) {
        const auto item = listFormat->currentItem();
        newFormatType = item ? (Format::Type)(item->data(Qt::UserRole).toInt()) : Format::DatesBegin;
    } else if (time->isChecked()) {
        const auto item = listFormat->currentItem();
        newFormatType = item ? (Format::Type)(item->data(Qt::UserRole).toInt()) : Format::TimesBegin;
    } else if (textFormat->isChecked()) {
        newFormatType = Format::Text;
    } else if (customFormat->isChecked()) {
        newFormatType = Format::Custom;
    }
}

void LayoutPageFloat::makeformat()
{
    QString tmp;

    updateFormatType();
    QColor color;
    Style::FloatFormat floatFormat = Style::OnlyNegSigned;
    switch (format->currentIndex()) {
    case 0:
        floatFormat = Style::OnlyNegSigned;
        color = Qt::black;
        break;
    case 1:
        floatFormat = Style::OnlyNegSigned;
        color = Qt::red;
        break;
    case 2:
        floatFormat = Style::AlwaysUnsigned;
        color = Qt::red;
        break;
    case 3:
        floatFormat = Style::AlwaysSigned;
        color = Qt::black;
        break;
    case 4:
        floatFormat = Style::AlwaysSigned;
        color = Qt::red;
        break;
    }

    // TODO Use the value from the selected cell? This only works if not editing a style.
    Value val = Value(-12.3456);
    if (Format::isDate(newFormatType) || Format::isTime(newFormatType) || Format::isDateTime(newFormatType)) {
        val = Value(4.1234);
    }
    if (!format->isEnabled())
        color = Qt::black;

    tmp = m_formatter
              ->formatText(val,
                           newFormatType,
                           precision->value(),
                           floatFormat,
                           prefix->isEnabled() ? prefix->text() : QString(),
                           postfix->isEnabled() ? postfix->text() : QString(),
                           newFormatType == Format::Money ? m_currency.symbol() : QString())
              .asString();
    if (tmp.length() > 50)
        tmp = tmp.left(50);
    exampleLabel->setText(tmp.prepend("<font color=" + color.name() + '>').append("</font>"));
}

void LayoutPageFloat::apply(Style *style, bool partial)
{
    if (postfix->isEnabled() && shouldApplyLayoutChange(!postfix->text().length(), postfix->text() != m_postfix, partial))
        style->setPostfix(postfix->text());

    if (prefix->isEnabled() && shouldApplyLayoutChange(!prefix->text().length(), prefix->text() != m_prefix, partial))
        style->setPrefix(prefix->text());

    if (shouldApplyLayoutChange(precision->value() == -1, precision->value() != m_precision, partial))
        style->setPrecision(precision->value());

    int fmt = format->currentIndex();
    if (shouldApplyLayoutChange(fmt == 0, m_bFormatColorChanged, partial)) {
        switch (fmt) {
        case 0:
            style->setFloatFormat(Style::OnlyNegSigned);
            style->setFloatColor(Style::AllBlack);
            break;
        case 1:
            style->setFloatFormat(Style::OnlyNegSigned);
            style->setFloatColor(Style::NegRed);
            break;
        case 2:
            style->setFloatFormat(Style::AlwaysUnsigned);
            style->setFloatColor(Style::NegRed);
            break;
        case 3:
            style->setFloatFormat(Style::AlwaysSigned);
            style->setFloatColor(Style::AllBlack);
            break;
        case 4:
            style->setFloatFormat(Style::AlwaysSigned);
            style->setFloatColor(Style::NegRed);
            break;
        }
    }

    if (shouldApplyLayoutChange(newFormatType == Format::Generic, newFormatType != cellFormatType, partial)) {
        style->setFormatType(newFormatType);
        if (money->isChecked()) {
            Currency currency;
            int index = this->currency->currentIndex() - 1;
            if (index < 0)
                currency = Currency();
            else
                currency = Currency(m_currencies[index]);
            style->setCurrency(currency);
        }
        if (newFormatType == Format::Scientific) {
            // FIXME: temporary fix to at least get precision to work
            // TODO: Add min-exponent-digits to dialog
            // Custom format overrides precision, so create a proper one
            QString format = "0.";
            if (precision->value() > 0) {
                for (int i = 0; i < precision->value(); ++i)
                    format.append('0');
            }
            format.append("E+00");
            style->setCustomFormat(format);
        } else {
            // nothing else needs custom format
            style->setCustomFormat(QString());
        }
    }
}

void LayoutPageFloat::loadFrom(const Style &style, bool /*partial*/)
{
    m_postfix = style.postfix();
    m_prefix = style.prefix();
    m_precision = style.precision();

    precision->setValue(m_precision);
    postfix->setText(m_postfix);
    prefix->setText(m_prefix);

    slotChangeState();

    cellFormatColor = 0;
    if (style.floatFormat() == Style::OnlyNegSigned && style.floatColor() == Style::AllBlack)
        cellFormatColor = 0;
    else if (style.floatFormat() == Style::OnlyNegSigned && style.floatColor() == Style::NegRed)
        cellFormatColor = 1;
    else if (style.floatFormat() == Style::AlwaysUnsigned && style.floatColor() == Style::NegRed)
        cellFormatColor = 2;
    else if (style.floatFormat() == Style::AlwaysSigned && style.floatColor() == Style::AllBlack)
        cellFormatColor = 3;
    else if (style.floatFormat() == Style::AlwaysSigned && style.floatColor() == Style::NegRed)
        cellFormatColor = 4;
    format->setCurrentIndex(cellFormatColor);

    cellFormatType = style.formatType();
    newFormatType = cellFormatType;
    if (!cellFormatType)
        generic->setChecked(true);
    else {
        if (cellFormatType == Format::Number)
            number->setChecked(true);
        else if (cellFormatType == Format::Percentage)
            percent->setChecked(true);
        else if (cellFormatType == Format::Money) {
            money->setChecked(true);
            m_currency = style.currency();
            int pos = m_currencies.indexOf(m_currency.code());
            if (pos >= 0)
                currency->setCurrentIndex(pos + 1);
            else
                currency->setCurrentIndex(0);
        } else if (cellFormatType == Format::Scientific)
            scientific->setChecked(true);
        else if (Format::isDateTime(cellFormatType))
            datetime->setChecked(true);
        else if (Format::isDate(cellFormatType))
            date->setChecked(true);
        else if (Format::isTime(cellFormatType))
            time->setChecked(true);
        else if (Format::isFraction(cellFormatType))
            fraction->setChecked(true);
        else if (cellFormatType == Format::Text)
            textFormat->setChecked(true);
        else if (cellFormatType == Format::Custom)
            customFormat->setChecked(true);
    }

    // enable controls as needed
    slotChangeState();
}
