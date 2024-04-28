/* Sidewinder - Portable library for spreadsheet
   SPDX-FileCopyrightText: 2010 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SWINDER_CONDITIONALS_H
#define SWINDER_CONDITIONALS_H

#include "format.h"
#include "value.h"

#include <QRegion>

namespace Swinder
{

class Conditional : private Format
{
public:
    enum Type { None, Formula, Between, Outside, Equal, NotEqual, Greater, Less, GreaterOrEqual, LessOrEqual };

    Type cond;
    Value value1;
    Value value2;

    Conditional();
    ~Conditional();
    Conditional(const Conditional &c);
    Conditional &operator=(const Conditional &c);
    Conditional &assign(const Conditional &c);

    bool hasValueFormat() const;
    using Format::valueFormat;
    void setValueFormat(const QString &valueFormat);

    const FormatFont &font() const
    {
        return Format::font();
    }

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
    void setFontColor(const QColor &color);

private:
    class Private;
    Private *d; // can't never be 0
};

class ConditionalFormat
{
public:
    ConditionalFormat();
    ~ConditionalFormat();

    void setRegion(const QRegion &region);
    QRegion region() const;

    QList<Conditional> conditionals() const;
    void addConditional(const Conditional &c);

private:
    class Private;
    Private *d; // can't never be 0
};

} // namespace Swinder

#endif // SWINDER_CONDITIONALS_H
