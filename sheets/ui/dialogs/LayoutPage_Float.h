/* This file is part of the KDE project
             SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
             SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
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

#ifndef CALLIGRA_SHEETS_LAYOUT_PAGE_FLOAT
#define CALLIGRA_SHEETS_LAYOUT_PAGE_FLOAT

#include <QWidget>

#include "core/Currency.h"
#include "engine/Format.h"
#include "engine/Value.h"

class KComboBox;
class KLineEdit;
class QLabel;
class QListWidget;
class QRadioButton;
class QSpinBox;

namespace Calligra
{
namespace Sheets
{

class Style;
class Localization;
class ValueFormatter;

/**
 * \ingroup UI
 * Dialog page to select the value formatting.
 */
class LayoutPageFloat : public QWidget
{
    Q_OBJECT
public:
    LayoutPageFloat(QWidget *parent, Localization *locale, ValueFormatter *formatter);
    void apply(Style *style, bool partial);
    void loadFrom(const Style &style, bool partial);

public Q_SLOTS:
    void slotChangeState();
    void makeformat();
    void updateFormatType();
    void initDate();
    void initTime();
    void initDatetime();
    void slotChangeValue(int);
    void formatChanged(int);
    void currencyChanged(const QString &);

protected:
    /**
     * Draws a pixmap showing a text consisting of two parts, @p _string1 and @p _string2 .
     * The parts' colors are given by @p _color1 and @p _color2 .
     */
    QPixmap paintFormatPixmap(const char *_string1, const QColor &_color1, const char *_string2, const QColor &_color2);

    KLineEdit *postfix;
    QSpinBox *precision;
    KLineEdit *prefix;
    KComboBox *format;
    KComboBox *currency;
    QLabel *currencyLabel;
    QRadioButton *generic;
    QRadioButton *number;
    QRadioButton *percent;
    QRadioButton *date;
    QRadioButton *datetime;
    QRadioButton *money;
    QRadioButton *scientific;
    QRadioButton *fraction;
    QRadioButton *time;
    QRadioButton *textFormat;
    QRadioButton *customFormat;
    QListWidget *listFormat;
    KLineEdit *customFormatEdit;
    QLabel *exampleLabel;
    Format::Type cellFormatType, newFormatType;
    int cellFormatColor;
    Localization *m_locale;
    ValueFormatter *m_formatter;

    int m_precision;
    QString m_prefix, m_postfix;
    bool m_bFormatColorChanged;
    Currency m_currency;
    QList<QString> m_currencies;

    QPixmap *formatOnlyNegSignedPixmap;
    QPixmap *formatRedOnlyNegSignedPixmap;
    QPixmap *formatRedNeverSignedPixmap;
    QPixmap *formatAlwaysSignedPixmap;
    QPixmap *formatRedAlwaysSignedPixmap;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_LAYOUT_PAGE_FLOAT
