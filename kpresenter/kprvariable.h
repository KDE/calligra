// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2001 Laurent MONTEL <lmontel@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef kprvariable_h
#define kprvariable_h

#include <qstring.h>
#include <qasciidict.h>
#include <kovariable.h>

#include "kprtextdocument.h"
class KPresenterDoc;
class KoVariable;
class KoPageVariable;
class KoTextFormat;
class KoVariableSettings;

class KPrVariableCollection : public KoVariableCollection
{
public:
    KPrVariableCollection(KoVariableSettings *_setting, KoVariableFormatCollection* coll);
    virtual KoVariable *createVariable( int type, short int subtype, KoVariableFormatCollection * coll,
                                        KoVariableFormat *varFormat,KoTextDocument *textdoc, KoDocument * doc,
                                        int _correct, bool _forceDefaultFormat=false , bool loadFootNote= true);
};

/**
 * "current page number" and "number of pages" variables
 */
class KPrPgNumVariable : public KoPageVariable
{
public:
    KPrPgNumVariable( KoTextDocument *textdoc, short int subtype, KoVariableFormat *varFormat,
                      KoVariableCollection *_varColl, KPresenterDoc *doc );

    virtual void recalc();

private:
    KPresenterDoc *m_doc;
};

#endif
