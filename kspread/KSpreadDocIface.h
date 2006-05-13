/* This file is part of the KDE project

   Copyright 2002 Laurent Montel <montel@kde.org>
   Copyright 2002 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2000 David Faure <faure@kde.org>
   Copyright 1999 Torben Weis <weis@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_DOC_IFACE_H
#define KSPREAD_DOC_IFACE_H

#include <QString>
#include <QColor>
#include <QStringList>

#include <dcopref.h>

#include <KoDocumentIface.h>

namespace KSpread
{
class Doc;

class DocIface : virtual public KoDocumentIface
{
    K_DCOP
public:
    DocIface( Doc* );

k_dcop:
    virtual DCOPRef map();

    bool showTabBar()const;

    void setShowVerticalScrollBar(bool _show);
    void setShowHorizontalScrollBar(bool _show);
    void setShowColumnHeader(bool _show);
    void setShowRowHeader(bool _show);
    void setShowTabBar(bool _show);

    void changeDefaultGridPenColor( const QColor &_col);
    bool showFormulaBar()const;
    bool showStatusBar()const;


    QColor pageBorderColor() const;
    void changePageBorderColor( const QColor & _color);
    void addIgnoreWordAll( const QString &word);
    void clearIgnoreWordAll( );
    QStringList spellListIgnoreAll() const;

    void addStringCompletion(const QString & stringCompletion);
    int zoom() const;

    void setMoveToValue(const QString & move);
    QString moveToValue()const;

    void setTypeOfCalc( const QString & calc );
    QString typeOfCalc() const;

private:
    Doc* doc;
};

} // namespace KSpread

#endif
