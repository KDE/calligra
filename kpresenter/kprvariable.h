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
#include <koVariable.h>

#include "kprtextdocument.h"
class KPresenterDoc;
class KoVariable;
class KoPgNumVariable;
class KoTextFormat;
class KoVariableSettings;

class KPrVariableCollection : public KoVariableCollection
{
 public:
    KPrVariableCollection(KoVariableSettings *_setting);
    virtual KoVariable *createVariable( int type, int subtype, KoVariableFormatCollection * coll, KoVariableFormat *varFormat,KoTextDocument *textdoc, KoDocument * doc, bool _forceDefaultFormat=false );
    QPtrList<KAction> variableActionList();
};

/**
 * "current page number" and "number of pages" variables
 */
class KPrPgNumVariable : public QObject,public KoPgNumVariable
{
    Q_OBJECT
public:
    KPrPgNumVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat ,KoVariableCollection *_varColl, KPresenterDoc *doc );

    virtual void recalc();
    virtual void setVariableSubType( short int type);
    QPtrList<KAction> actionList();

protected slots:
    void slotChangeSubType();

 private:
    KPresenterDoc *m_doc;
};


class KPrFieldVariable : public QObject, public KoFieldVariable
{
    Q_OBJECT
public:
    KPrFieldVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KPresenterDoc *_doc );

    QPtrList<KAction> actionList();

protected slots:
    void slotChangeSubType();

private:
    KPresenterDoc *m_doc;

};

/**
 * Date-related variables
 */
class KPrDateVariable : public QObject, public KoDateVariable
{
    Q_OBJECT
public:
    KPrDateVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat, KoVariableCollection *_varColl, KPresenterDoc *_doc );

    QPtrList<KAction> actionList();

protected slots:
    void slotChangeSubType();
    void slotChangeFormat();

private:
    KPresenterDoc *m_doc;

};

/**
 * Time-related variables
 */
class KPrTimeVariable : public QObject, public KoTimeVariable
{

    Q_OBJECT
public:
    KPrTimeVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat, KoVariableCollection *_varColl, KPresenterDoc *_doc );

    QPtrList<KAction> actionList();

protected slots:
    void slotChangeSubType();
    void slotChangeFormat();

private:
    KPresenterDoc *m_doc;

};


#endif
