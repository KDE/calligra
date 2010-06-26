/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2003 Philipp Müller <philipp.mueller@gmx.de>
   Copyright 1998, 1999 Torben Weis <weis@kde.org>,

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

#ifndef KSPREAD_HEADER_FOOTER
#define KSPREAD_HEADER_FOOTER

#include <KoPageLayout.h>

#include <QString>

#include "kspread_export.h"

namespace KSpread
{
class Sheet;

class KSPREAD_EXPORT HeaderFooter
{
public:
    explicit HeaderFooter(Sheet* sheet);
    ~HeaderFooter();

    QString headLeft(int _p, const QString &_t)const {
        if (m_headLeft.isNull()) return "";
        return completeHeading(m_headLeft, _p, _t);
    }
    QString headMid(int _p, const QString &_t)const {
        if (m_headMid.isNull()) return "";
        return completeHeading(m_headMid, _p, _t);
    }
    QString headRight(int _p, const QString &_t)const {
        if (m_headRight.isNull()) return "";
        return completeHeading(m_headRight, _p, _t);
    }
    QString footLeft(int _p, const QString &_t)const {
        if (m_footLeft.isNull()) return "";
        return completeHeading(m_footLeft, _p, _t);
    }
    QString footMid(int _p, const QString &_t)const {
        if (m_footMid.isNull()) return "";
        return completeHeading(m_footMid, _p, _t);
    }
    QString footRight(int _p, const QString &_t)const {
        if (m_footRight.isNull()) return "";
        return completeHeading(m_footRight, _p, _t);
    }

    QString headLeft()const {
        if (m_headLeft.isNull()) return ""; return m_headLeft;
    }
    QString headMid()const {
        if (m_headMid.isNull()) return ""; return m_headMid;
    }
    QString headRight()const {
        if (m_headRight.isNull()) return ""; return m_headRight;
    }
    QString footLeft()const {
        if (m_footLeft.isNull()) return ""; return m_footLeft;
    }
    QString footMid()const {
        if (m_footMid.isNull()) return ""; return m_footMid;
    }
    QString footRight()const {
        if (m_footRight.isNull()) return ""; return m_footRight;
    }

    /**
     * Replaces in _text all _search text parts by _replace text parts.
     * Included is a test to not change if _search == _replace.
     * The arguments should not include neither the beginning "<" nor the leading ">", this is already
     * included internally.
     */
    void replaceHeadFootLineMacro(QString &_text, const QString &_search, const QString &_replace) const;

    /**
     * Replaces in _text all page macros by the i18n-version of the macros
     */
    QString localizeHeadFootLine(const QString &_text) const;

    /**
     * Replaces in _text all i18n-versions of the page macros by the internal version of the macros
     */
    QString delocalizeHeadFootLine(const QString &_text) const;

    /**
     * Sets the head and foot line of the print out
     */
    void setHeadFootLine(const QString &_headl, const QString &_headm, const QString &_headr,
                         const QString &_footl, const QString &_footm, const QString &_footr);

private:
    /**
     * Replaces macros like <name>, <file>, <date> etc. in the string and
     * returns the modified one.
     *
     * @param _page is the page number for which the heading is produced.
     * @param _Sheet is the name of the Sheet for which we generate the headings.
     */
    QString completeHeading(const QString &_data, int _page, const QString &_sheet) const ;

    Sheet *m_pSheet;

    /**
     * Header string. The string may contains makros. That means
     * it has to be processed before printing.
     */
    QString m_headLeft;

    /**
     * Header string. The string may contains makros. That means
     * it has to be processed before printing.
     */
    QString m_headRight;

    /**
     * Header string. The string may contains makros. That means
     * it has to be processed before printing.
     */
    QString m_headMid;

    /**
     * Footer string. The string may contains makros. That means
     * it has to be processed before printing.
     */
    QString m_footLeft;

    /**
     * Footer string. The string may contains makros. That means
     * it has to be processed before printing.
     */
    QString m_footRight;

    /**
     * Footer string. The string may contains makros. That means
     * it has to be processed before printing.
     */
    QString m_footMid;
};

} // namespace KSpread

#endif // KSPREAD_HEADER_FOOTER
