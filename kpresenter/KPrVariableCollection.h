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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef kprvariable_h
#define kprvariable_h

#include <QString>
#include <qasciidict.h>
#include <KoVariable.h>

#include "KPrTextDocument.h"
class KPrDocument;
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
    virtual KoVariable* loadOasisField( KoTextDocument* textdoc, const QDomElement& tag, KoOasisContext& context );
};

/**
 * "current page number" and "number of pages" variables
 */
class KPrPgNumVariable : public KoPageVariable
{
public:
    KPrPgNumVariable( KoTextDocument *textdoc, short int subtype, KoVariableFormat *varFormat,
                      KoVariableCollection *_varColl, KPrDocument *doc );

    virtual void recalc();

private:
    KPrDocument *m_doc;
};


class KPrStatisticVariable : public KoStatisticVariable
{
public:
    KPrStatisticVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KPrDocument *doc );
    virtual void recalc();
    virtual QString text(bool realValue=false);

protected:
    KPrDocument *m_doc;
};
#endif
