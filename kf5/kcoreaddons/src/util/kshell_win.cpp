/*
    This file is part of the KDE libraries

    Copyright (c) 2007 Bernhard Loos <nhuh.put@web.de>
    Copyright (c) 2007,2008 Oswald Buddenhagen <ossi@kde.org>

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
    Boston, MA 02110-1301, USA. 
*/

#include "kshell.h"
#include "kshell_p.h"

#include <QString>
#include <QStringList>
#include <QtCore/QDir>

/*
 * A short introduction into cmd semantics:
 * - Variable expansion is done first, without regard to *any* escaping -
 *   if something looks like an existing variable, it is replaced.
 * - Then follows regular tokenization by the shell. &, &&, | and || are
 *   command delimiters. ( and ) are command grouping operators; they are
 *   recognized only a the start resp. end of a command; mismatched )s are
 *   an error if any (s are present. <, > are just like under UNIX - they can
 *   appear *anywhere* in a command, perform their function and are cut out.
 *   @ at the start of a command is eaten (local echo off - no function as
 *   far as cmd /c is concerned). : at the start of a command declares a label,
 *   which effectively means the remainder of the line is a comment - note that
 *   command separators are not recognized past that point.
 *   ^ is the escape char for everything including itself.
 *   cmd ignores *all* special chars between double quotes, so there is no
 *   way to escape the closing quote. Note that the quotes are *not* removed
 *   from the resulting command line.
 * - Then follows delayed variable expansion if it is enabled and at least
 *   one exclamation mark is present. This involves another layer of ^
 *   escaping, regardless of quotes. (Win2k+)
 * - Then follows argument splitting as described in
 *   http://msdn2.microsoft.com/en-us/library/ms880421.aspx .
 *   Note that this is done by the called application and therefore might
 *   be subject to completely different semantics, in fact.
 */

inline static bool isMetaChar(ushort c)
{
    static const uchar iqm[] = {
        0x00, 0x00, 0x00, 0x00, 0x40, 0x03, 0x00, 0x50,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
    }; // &()<>|

    return (c < sizeof(iqm) * 8) && (iqm[c / 8] & (1 << (c & 7)));
}

inline static bool isSpecialChar(ushort c)
{
    // Chars that should be quoted (TM). This includes:
    // - control chars & space
    // - the shell meta chars &()<>^|
    // - the potential separators ,;=
    static const uchar iqm[] = {
        0xff, 0xff, 0xff, 0xff, 0x41, 0x13, 0x00, 0x78,
        0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x10
    };

    return (c < sizeof(iqm) * 8) && (iqm[c / 8] & (1 << (c & 7)));
}

inline static bool isWhiteSpace(ushort c)
{
    return c == ' ' || c == '\t';
}

QStringList KShell::splitArgs(const QString &_args, Options flags, Errors *err)
{
    QString args(_args);
    QStringList ret;

    const QLatin1Char bs('\\'), dq('\"');

    if (flags & AbortOnMeta) {
        args.remove(PERCENT_ESCAPE);
        if (args.indexOf(QLatin1Char('%')) >= 0) {
            if (err)
                *err = FoundMeta;
            return QStringList();
        }

        args = _args;
        args.replace(PERCENT_ESCAPE, QLatin1String("%"));

        if (!args.isEmpty() && args[0].unicode() == '@')
            args.remove(0, 1);

        for (int p = 0; p < args.length(); p++) {
            ushort c = args[p].unicode();
            if (c == '^') {
                args.remove(p, 1);
            } else if (c == '"') {
                while (++p < args.length() && args[p].unicode() != '"')
                    ;
            } else if (isMetaChar(c)) {
                if (err)
                    *err = FoundMeta;
                return QStringList();
            }
        }
    }

    if (err)
        *err = NoError;

    int p = 0;
    const int length = args.length();
    forever {
        while (p < length && isWhiteSpace(args[p].unicode()))
            ++p;
        if (p == length)
            return ret;

        QString arg;
        bool inquote = false;
        forever {
            bool copy = true; // copy this char
            int bslashes = 0; // number of preceding backslashes to insert
            while (p < length && args[p] == bs) {
                ++p;
                ++bslashes;
            }
            if (p < length && args[p] == dq) {
                if (bslashes % 2 == 0) {
                    // Even number of backslashes, so the quote is not escaped.
                    if (inquote) {
                        if (p + 1 < length && args[p + 1] == dq) {
                            // Two consecutive quotes make a literal quote.
                            // This is not documented on MSDN.
                            ++p;
                        } else {
                            // Closing quote
                            copy = false;
                            inquote = !inquote;
                        }
                    } else {
                        // Opening quote
                        copy = false;
                        inquote = !inquote;
                    }
                }
                bslashes /= 2;
            }

            while (--bslashes >= 0)
                arg.append(bs);

            if (p == length || (!inquote && isWhiteSpace(args[p].unicode()))) {
                ret.append(arg);
                if (inquote) {
                    if (err)
                        *err = BadQuoting;
                    return QStringList();
                }
                break;
            }

            if (copy)
                arg.append(args[p]);
            ++p;
        }
    }
    //not reached
}

QString KShell::quoteArgInternal(const QString &arg, bool _inquote)
{
    // Escape quotes, preceding backslashes are doubled. Surround with quotes.
    // Note that cmd does not understand quote escapes in quoted strings,
    // so the quoting needs to be "suspended".
    const QLatin1Char bs('\\'), dq('\"');
    QString ret;
    bool inquote = _inquote;
    int bslashes = 0;
    for (int p = 0; p < arg.length(); p++) {
        if (arg[p] == bs) {
            bslashes++;
        } else if (arg[p] == dq) {
            if (inquote) {
                ret.append(dq);
                inquote = false;
            }
            for (; bslashes; bslashes--)
                ret.append(QLatin1String("\\\\"));
            ret.append(QLatin1String("\\^\""));
        } else {
            if (!inquote) {
                ret.append(dq);
                inquote = true;
            }
            for (; bslashes; bslashes--)
                ret.append(bs);
            ret.append(arg[p]);
        }
    }
    ret.replace(QLatin1Char('%'), PERCENT_ESCAPE);
    if (bslashes) {
        // Ensure that we don't have directly trailing backslashes,
        // so concatenating with another string won't cause surprises.
        if (!inquote && !_inquote)
            ret.append(dq);
        for (; bslashes; bslashes--)
            ret.append(QLatin1String("\\\\"));
        ret.append(dq);
        if (inquote && _inquote)
            ret.append(dq);
    } else if (inquote != _inquote) {
        ret.append(dq);
    }
    return ret;
}

QString KShell::quoteArg(const QString &arg)
{
    if (arg.isEmpty())
        return QString::fromLatin1("\"\"");

    // Ensure that we don't have directly trailing backslashes,
    // so concatenating with another string won't cause surprises.
    if (arg.endsWith(QLatin1Char('\\')))
        return quoteArgInternal(arg, false);

    for (int x = arg.length() - 1; x >= 0; --x)
        if (isSpecialChar(arg[x].unicode()))
            return quoteArgInternal(arg, false);

    // Escape quotes. Preceding backslashes are doubled.
    // Note that the remaining string is not quoted.
    QString ret(arg);
    ret.replace(QRegExp(QLatin1String("(\\\\*)\"")), QLatin1String("\\1\\1\\^\""));
    ret.replace(QLatin1Char('%'), PERCENT_ESCAPE);
    return ret;
}

