/*
 * This file is part of Office 2007 Filters for Calligra
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Christoph Schleifenbaum christoph@kdab.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
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
    static QColor color(const QString& name);
    static QLocale locale(int langid);

    static KoGenStyle parse(const QString& numberFormat, KoGenStyles* styles = 0, KoGenStyle::Type type = KoGenStyle::ParagraphAutoStyle);
    static bool isDateFormat(const QString& numberFormat);
};

#endif
