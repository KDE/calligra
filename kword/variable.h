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

class KWDocument;
class KWVariable;
class KWParag;

enum VariableType { VT_DATE_FIX = 0, VT_DATE_VAR = 1, VT_TIME_FIX = 2, VT_TIME_VAR = 3, VT_PGNUM = 4,
                    VT_NUMPAGES = 5, VT_CUSTOM = 6, VT_SERIALLETTER = 7, VT_NONE };
enum VariableFormatType { VFT_DATE = 0, VFT_TIME = 1, VFT_PGNUM = 2, VFT_NUMPAGES = 3, VFT_CUSTOM = 4,
                          VFT_SERIALLETTER = 5 };

/******************************************************************/
/* Class: KWVariableFormat                                        */
/******************************************************************/

class KWVariableFormat
{
public:
    KWVariableFormat() {}
    virtual ~KWVariableFormat() {}

    virtual VariableFormatType getType() const = 0;

    virtual void setFormat( QString _format )
    { format = _format; }

    virtual QString convert( KWVariable *_var ) = 0;

protected:
    QString format;

};

/******************************************************************/
/* Class: KWVariableDateFormat                                    */
/******************************************************************/

class KWVariableDateFormat : public KWVariableFormat
{
public:
    KWVariableDateFormat() : KWVariableFormat() {}

    virtual VariableFormatType getType() const
    { return VFT_DATE; }

    virtual void setFormat( QString _format );

    virtual QString convert( KWVariable *_var );

};

/******************************************************************/
/* Class: KWVariableTimeFormat                                    */
/******************************************************************/

class KWVariableTimeFormat : public KWVariableFormat
{
public:
    KWVariableTimeFormat() : KWVariableFormat() {}

    virtual VariableFormatType getType() const
    { return VFT_TIME; }

    virtual void setFormat( QString _format );

    virtual QString convert( KWVariable *_var );

};

/******************************************************************/
/* Class: KWVariablePgNumFormat                                   */
/******************************************************************/

class KWVariablePgNumFormat : public KWVariableFormat
{
public:
    KWVariablePgNumFormat() { pre = "-"; post = "-"; }

    virtual VariableFormatType getType() const
    { return VFT_PGNUM; }

    virtual void setFormat( QString _format );

    virtual QString convert( KWVariable *_var );

    void setPre( const QString &_pre ) { pre = _pre; }
    void setPost( const QString &_post ) { pre = _post; }

    QString getPre() const { return pre; }
    QString getPost() const { return post; }

protected:
    QString pre, post;

};

/******************************************************************/
/* Class: KWVariableCustomFormat                                   */
/******************************************************************/

class KWVariableCustomFormat : public KWVariableFormat
{
public:
    KWVariableCustomFormat() {}

    virtual VariableFormatType getType() const
    { return VFT_CUSTOM; }

    virtual void setFormat( QString _format );

    virtual QString convert( KWVariable *_var );

};

/******************************************************************/
/* Class: KWVariableSerialLetterFormat                            */
/******************************************************************/

class KWVariableSerialLetterFormat : public KWVariableFormat
{
public:
    KWVariableSerialLetterFormat() {}

    virtual VariableFormatType getType() const
    { return VFT_SERIALLETTER; }

    virtual void setFormat( QString _format );

    virtual QString convert( KWVariable *_var );

};


// ----------------------------------------------------------------------------------------------




/******************************************************************/
/* Class: KWVariable                                              */
/******************************************************************/

class KWVariable
{
public:
    KWVariable( KWDocument *_doc );
    virtual ~KWVariable();

    virtual KWVariable *copy() {
        KWVariable *v = new KWVariable( doc );
        v->setVariableFormat( varFormat );
        v->setInfo( frameSetNum, frameNum, pageNum, parag );
        return v;
    }

    virtual VariableType getType() const
    { return VT_NONE; }

    void setVariableFormat( KWVariableFormat *_varFormat, bool _deleteOld = false )
    { if ( _deleteOld && varFormat ) delete varFormat; varFormat = _varFormat; }
    KWVariableFormat *getVariableFormat() const
    { return varFormat; }

    QString getText()
    { return varFormat->convert( this ); }

    virtual void setInfo( int _frameSetNum, int _frameNum, int _pageNum, KWParag *_parag )
    { frameSetNum = _frameSetNum; frameNum = _frameNum; pageNum = _pageNum; parag = _parag; }

    virtual void recalc() {}

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

protected:
    KWDocument *doc;
    KWVariableFormat *varFormat;
    QString text;
    int frameSetNum, frameNum, pageNum;
    KWParag *parag;

};

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/

class KWPgNumVariable : public KWVariable
{
public:
    KWPgNumVariable( KWDocument *_doc ) : KWVariable( _doc ) { pgNum = 0; }

    virtual KWVariable *copy() {
        KWPgNumVariable *var = new KWPgNumVariable( doc );
        var->setVariableFormat( varFormat );
        var->setInfo( frameSetNum, frameNum, pageNum, parag );
        return var;
    }

    virtual VariableType getType() const
    { return VT_PGNUM; }

    virtual void recalc() { pgNum = pageNum; }
    long unsigned int getPgNum() const { return pgNum; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

protected:
    long unsigned int pgNum;

};

/******************************************************************/
/* Class: KWDateVariable                                          */
/******************************************************************/

class KWDateVariable : public KWVariable
{
public:
    KWDateVariable( KWDocument *_doc, bool _fix, QDate _date );
    KWDateVariable( KWDocument *_doc ) : KWVariable( _doc ) {}

    virtual KWVariable *copy() {
        KWDateVariable *var = new KWDateVariable( doc, fix, date );
        var->setVariableFormat( varFormat );
        var->setInfo( frameSetNum, frameNum, pageNum, parag );
        return var;
    }

    virtual VariableType getType() const
    { return fix ? VT_DATE_FIX : VT_DATE_VAR; }

    virtual void recalc();

    QDate getDate() const { return date; }
    void setDate( QDate _date ) { date = _date; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

protected:
    QDate date;
    bool fix;

};

/******************************************************************/
/* Class: KWTimeVariable                                          */
/******************************************************************/

class KWTimeVariable : public KWVariable
{
public:
    KWTimeVariable( KWDocument *_doc, bool _fix, QTime _time );
    KWTimeVariable( KWDocument *_doc ) : KWVariable( _doc ) {}

    virtual KWVariable *copy() {
        KWTimeVariable *var = new KWTimeVariable( doc, fix, time );
        var->setVariableFormat( varFormat );
        var->setInfo( frameSetNum, frameNum, pageNum, parag );
        return var;
    }

    virtual VariableType getType() const
    { return fix ? VT_TIME_FIX : VT_TIME_VAR; }

    virtual void recalc();

    QTime getTime() const { return time; }
    void setTime( QTime _time ) { time = _time; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

protected:
    QTime time;
    bool fix;

};

/******************************************************************/
/* Class: KWCustomVariable                                        */
/******************************************************************/

class KWCustomVariable : public KWVariable
{
public:
    KWCustomVariable( KWDocument *_doc, const QString &name_ );
    KWCustomVariable( KWDocument *_doc ) : KWVariable( _doc ) {}

    virtual KWVariable *copy() {
        KWCustomVariable *var = new KWCustomVariable( doc, name );
        var->setVariableFormat( varFormat );
        var->setInfo( frameSetNum, frameNum, pageNum, parag );
        return var;
    }

    virtual VariableType getType() const
    { return VT_CUSTOM; }

    virtual void recalc();

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual QString getName() const;
    virtual QString getValue() const;

    virtual void setValue( const QString &v );

protected:
    QString name;

};

/******************************************************************/
/* Class: KWSerialLetterVariable                                  */
/******************************************************************/

class KWSerialLetterVariable : public KWVariable
{
public:
    KWSerialLetterVariable( KWDocument *_doc, const QString &name_ );
    KWSerialLetterVariable( KWDocument *_doc ) : KWVariable( _doc ) {}

    virtual KWVariable *copy() {
        KWSerialLetterVariable *var = new KWSerialLetterVariable( doc, name );
        var->setVariableFormat( varFormat );
        var->setInfo( frameSetNum, frameNum, pageNum, parag );
        return var;
    }

    virtual VariableType getType() const
    { return VT_SERIALLETTER; }

    virtual void recalc();

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual QString getName() const;
    virtual QString getValue() const;

protected:
    QString name;

};

#endif
