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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef variable_h
#define variable_h

#include <qstring.h>
#include <qdatetime.h>
#include <qasciidict.h>
#include <koVariable.h>

#include <kwtextdocument.h>
class KWDocument;
class KoVariable;
class KoPgNumVariable;
class KoSerialLetterVariable;
class QDomElement;
namespace Qt3 {
class QTextFormat;
}


/**
 * "current page number" and "number of pages" variables
 */
class KWPgNumVariable : public KoPgNumVariable
{
public:
    KWPgNumVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat ,KoVariableCollection *_varColl, KWDocument *doc );

    virtual void recalc();
    virtual QString text();
 private:
    KWDocument *m_doc;
};


/**
 * Serial letter variable
 */
class KWSerialLetterVariable : public KoSerialLetterVariable
{
public:
    KWSerialLetterVariable( KoTextDocument *textdoc, const QString &name, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KWDocument *doc );
    
    virtual QString text();
    virtual QString value() const;
    virtual void recalc();
 private:
    KWDocument *m_doc;
};


#endif
