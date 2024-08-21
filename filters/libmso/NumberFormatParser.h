/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * SPDX-FileCopyrightText: 2010 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Christoph Schleifenbaum christoph@kdab.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
#ifndef NUMBERFORMATPARSER_H
#define NUMBERFORMATPARSER_H

#include <KoGenStyle.h>

class QColor;
class QLocale;
class QString;
class KoGenStyles;

class NumberFormatParser
{
public:
    static QColor color(const QString &name);
    static QLocale locale(int langid);

    static KoGenStyle parse(const QString &numberFormat, KoGenStyles *styles = nullptr, KoGenStyle::Type type = KoGenStyle::ParagraphAutoStyle);
    static bool isDateFormat(const QString &numberFormat);
};

#endif
