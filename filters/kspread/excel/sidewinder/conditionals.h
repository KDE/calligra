/* Sidewinder - Portable library for spreadsheet
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
 * Boston, MA 02110-1301, USA
*/

#ifndef SWINDER_CONDITIONALS_H
#define SWINDER_CONDITIONALS_H

#include "value.h"
#include "format.h"

#include <QRegion>

namespace Swinder
{

class Conditional : private Format
{
public:
    enum Type {
        None, Formula, Between, Outside, Equal, NotEqual, Greater, Less, GreaterOrEqual, LessOrEqual
    };

    Type cond;
    Value value1;
    Value value2;

    Conditional();
    ~Conditional();
    Conditional(const Conditional& c);
    Conditional& operator=(const Conditional& c);
    Conditional& assign(const Conditional& c);

    bool hasValueFormat() const;
    using Format::valueFormat;
    void setValueFormat(const QString& valueFormat);

    const FormatFont& font() const
    { return Format::font(); }

    bool hasFontItalic() const;
    void setFontItalic(bool italic);

    bool hasFontStrikeout() const;
    void setFontStrikeout(bool s);

    bool hasFontBold() const;
    void setFontBold(bool b);

    bool hasFontSubSuperscript() const;
    void setFontSubscript(bool s);
    void setFontSuperscript(bool s);

    bool hasFontUnderline() const;
    void setFontUnderline(bool u);

    bool hasFontColor() const;
    void setFontColor(const QColor& color);
private:
    class Private;
    Private* d; // can't never be 0
};

class ConditionalFormat
{
public:
    ConditionalFormat();
    ~ConditionalFormat();

    void setRegion(const QRegion& region);
    QRegion region() const;

    QList<Conditional> conditionals() const;
    void addConditional(const Conditional& c);
private:
    class Private;
    Private* d; // can't never be 0
};

} // namespace Swinder

#endif // SWINDER_CONDITIONALS_H
