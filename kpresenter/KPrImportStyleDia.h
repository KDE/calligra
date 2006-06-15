// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#ifndef __KPRImportStyleDia__
#define __KPRImportStyleDia__

#include <kdialog.h>
#include <KoImportStyleDia.h>
#include <qstringlist.h>
class QLineEdit;
class Q3ListBox;
class KPrDocument;
class QPushButton;
class KoParagStyle;

class KPrImportStyleDia : public KoImportStyleDia
{
    Q_OBJECT
public:
    KPrImportStyleDia( KPrDocument *_doc, KoStyleCollection* currentCollection, QWidget *parent, const char *name = 0 );
    ~KPrImportStyleDia();

protected:
    virtual void loadFile();

private:
    KPrDocument *m_doc;
};

#endif
