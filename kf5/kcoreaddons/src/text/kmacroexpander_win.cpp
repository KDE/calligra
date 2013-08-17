/*
    This file is part of the KDE libraries

    Copyright (c) 2008 Oswald Buddenhagen <ossi@kde.org>

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

#include "kmacroexpander_p.h"
#include "kshell_p.h"

#include "kshell.h"

#include <QString>
#include <QStringList>

bool KMacroExpanderBase::expandMacrosShellQuote( QString &str, int &pos )
{
    int len;
    int pos2;
    ushort uc;
    ushort ec = d->escapechar.unicode();
    bool shellQuote = false; // shell is in quoted state
    bool crtQuote = false; // c runtime is in quoted state
    bool escaped = false; // previous char was a circumflex
    int bslashes = 0; // previous chars were backslashes
    int parens = 0; // parentheses nesting level
    QStringList rst;
    QString rsts;

    while (pos < str.length()) {
        ushort cc = str.unicode()[pos].unicode();
        if (escaped) // prevent anomalies due to expansion
            goto notcf;
        if (ec != 0) {
            if (cc != ec)
                goto nohit;
            if (!(len = expandEscapedMacro( str, pos, rst )))
                goto nohit;
        } else {
            if (!(len = expandPlainMacro( str, pos, rst )))
                goto nohit;
        }
            if (len < 0) {
                pos -= len;
                continue;
            }
            if (shellQuote != crtQuote) // Silly, isn't it? Ahoy to Redmond.
                return false;
            if (shellQuote) {
                rsts = KShell::quoteArgInternal( rst.join( QLatin1String(" ") ), true );
            } else {
                if (rst.isEmpty()) {
                    str.remove( pos, len );
                    continue;
                }
                rsts = KShell::joinArgs( rst );
            }
            pos2 = 0;
            while (pos2 < rsts.length() &&
                   ((uc = rsts.unicode()[pos2].unicode()) == '\\' || uc == '^'))
                pos2++;
            if (pos2 < rsts.length() && rsts.unicode()[pos2].unicode() == '"') {
                QString bsl;
                bsl.reserve( bslashes );
                for (; bslashes; bslashes--)
                    bsl.append( QLatin1String("\\") );
                rsts.prepend( bsl );
            }
            bslashes = 0;
            rst.clear();
            str.replace( pos, len, rsts );
            pos += rsts.length();
            continue;
      nohit:
        if (!escaped && !shellQuote && cc == '^') {
            escaped = true;
        } else {
          notcf:
            if (cc == '\\') {
                bslashes++;
            } else {
                if (cc == '"') {
                    if (!escaped)
                        shellQuote = !shellQuote;
                    if (!(bslashes & 1))
                        crtQuote = !crtQuote;
                } else if (!shellQuote) {
                    if (cc == '(')
                        parens++;
                    else if (cc == ')')
                        if (--parens < 0)
                            break;
                }
                bslashes = 0;
            }
            escaped = false;
        }
        pos++;
    }
    return true;
}
