/* Swinder - Portable library for spreadsheet
   Copyright (C) 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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
   Boston, MA 02110-1301, USA
*/

#include "conditionals.h"

using namespace Swinder;

class Conditional::Private
{
public:
    bool hasValueFormat;
    bool hasFontItalic;
    bool hasFontStrikeout;
    bool hasFontBold;
    bool hasFontSubSuperscript;
    bool hasFontUnderline;
    bool hasFontColor;
};

Conditional::Conditional()
    : d(new Private)
{
    d->hasValueFormat = false;
    d->hasFontItalic = false;
    d->hasFontStrikeout = false;
    d->hasFontBold = false;
    d->hasFontSubSuperscript = false;
    d->hasFontUnderline = false;
    d->hasFontColor = false;
}

Conditional::~Conditional()
{
    delete d;
}

Conditional::Conditional(const Conditional &c)
    : Format(c), d(new Private)
{
    assign(c);
}

Conditional& Conditional::operator=(const Conditional& c)
{
    Format::assign(c);
    return assign(c);
}

Conditional& Conditional::assign(const Conditional &c)
{
    *d = *c.d;
    cond = c.cond;
    value1 = c.value1;
    value2 = c.value2;
    return *this;
}

bool Conditional::hasValueFormat() const
{
    return d->hasValueFormat;
}

void Conditional::setValueFormat(const QString &valueFormat)
{
    d->hasValueFormat = true;
    Format::setValueFormat(valueFormat);
}

bool Conditional::hasFontItalic() const
{
    return d->hasFontItalic;
}

void Conditional::setFontItalic(bool italic)
{
    d->hasFontItalic = true;
    Format::font().setItalic(italic);
}

bool Conditional::hasFontStrikeout() const
{
    return d->hasFontStrikeout;
}

void Conditional::setFontStrikeout(bool s)
{
    d->hasFontStrikeout = true;
    Format::font().setStrikeout(s);
}

bool Conditional::hasFontBold() const
{
    return d->hasFontBold;
}

void Conditional::setFontBold(bool b)
{
    d->hasFontBold = true;
    Format::font().setBold(b);
}

bool Conditional::hasFontSubSuperscript() const
{
    return d->hasFontSubSuperscript;
}

void Conditional::setFontSubscript(bool s)
{
    d->hasFontSubSuperscript = true;
    Format::font().setSubscript(s);
}

void Conditional::setFontSuperscript(bool s)
{
    d->hasFontSubSuperscript = true;
    Format::font().setSuperscript(s);
}

bool Conditional::hasFontUnderline() const
{
    return d->hasFontUnderline;
}

void Conditional::setFontUnderline(bool u)
{
    d->hasFontUnderline = true;
    Format::font().setUnderline(u);
}

bool Conditional::hasFontColor() const
{
    return d->hasFontColor;
}

void Conditional::setFontColor(const QColor &color)
{
    d->hasFontColor = true;
    Format::font().setColor(color);
}

class ConditionalFormat::Private
{
public:
    QRegion region;
    QList<Conditional> conditionals;
};

ConditionalFormat::ConditionalFormat()
    : d(new Private)
{
}

ConditionalFormat::~ConditionalFormat()
{
    delete d;
}

void ConditionalFormat::setRegion(const QRegion &region)
{
    d->region = region;
}

QRegion ConditionalFormat::region() const
{
    return d->region;
}

void ConditionalFormat::addConditional(const Conditional &c)
{
    d->conditionals.append(c);
}

QList<Conditional> ConditionalFormat::conditionals() const
{
    return d->conditionals;
}
