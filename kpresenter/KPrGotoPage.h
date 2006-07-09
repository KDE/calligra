// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef gotopage_h
#define gotopage_h

#include <kdialogbase.h>
#include <qvaluelist.h>

class QWidget;
class QObject;
class QResizeEvent;
class QListBox;
class KPrDocument;

class KPrGotoPage : public KDialogBase
{
public:
    KPrGotoPage( const KPrDocument *doc,
                const QValueList<int> &slides, int start,
                QWidget *parent = 0L, const char *name = 0L );

    static int gotoPage( const KPrDocument *doc,
                         const QValueList<int> &slides, int start,
                         QWidget *parent = 0L );

    int page() const;

private:
    void resetCursor();

    QListBox *spinbox;
    int oldPage;
};

#endif
