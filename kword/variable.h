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
#include <kwtextdocument.h>
class KWDocument;
class KWVariable;
class QDomElement;
namespace Qt3 {
class QTextFormat;
}
using namespace Qt3;

// Always add new types at the _end_ of this list.
enum VariableType { VT_NONE = -1,
                    VT_DATE_FIX = 0, VT_DATE_VAR = 1, VT_TIME_FIX = 2, VT_TIME_VAR = 3, VT_PGNUM = 4,
                    VT_NUMPAGES = 5, VT_CUSTOM = 6, VT_SERIALLETTER = 7, VT_FIELD = 8 };

// For VT_FIELD
enum VariableSubType { VST_NONE = -1,
                       VST_FILENAME = 0, VST_AUTHORNAME = 1, VST_EMAIL = 2, VST_COMPANYNAME = 3 };

//enum VariableFormatType { VFT_DATE = 0, VFT_TIME = 1, VFT_PGNUM = 2, VFT_NUMPAGES = 3, VFT_CUSTOM = 4,
//                          VFT_SERIALLETTER = 5 };

/**
 * Class: KWVariableFormat
 * Base class for a variable format - held by KWDocument.
 * The variable itself is implemented by KWVariable
 * The reason for formats to be separated is that it allows to
 * customize the formats - for instance, KWVariablePgNumFormat has
 * 'pre' and 'post' variables.
 */
class KWVariableFormat
{
public:
    KWVariableFormat() {}
    virtual ~KWVariableFormat() {}
//    virtual VariableFormatType getType() const = 0;
    virtual QString convert( KWVariable *_var ) = 0;
};

/******************************************************************/
/* Class: KWVariableDateFormat                                    */
/******************************************************************/
class KWVariableDateFormat : public KWVariableFormat
{
public:
    KWVariableDateFormat() : KWVariableFormat() {}

//    virtual VariableFormatType getType() const
//    { return VFT_DATE; }

    virtual QString convert( KWVariable *_var );

};

/******************************************************************/
/* Class: KWVariableTimeFormat                                    */
/******************************************************************/
class KWVariableTimeFormat : public KWVariableFormat
{
public:
    KWVariableTimeFormat() : KWVariableFormat() {}

//    virtual VariableFormatType getType() const
//    { return VFT_TIME; }

    virtual QString convert( KWVariable *_var );

};

/******************************************************************/
/* Class: KWVariablePgNumFormat                                   */
/******************************************************************/
class KWVariablePgNumFormat : public KWVariableFormat
{
public:
    KWVariablePgNumFormat() { pre = "-"; post = "-"; }

//    virtual VariableFormatType getType() const
//    { return VFT_PGNUM; }

    virtual QString convert( KWVariable *_var );

    // Needs a UI !
    void setPre( const QString &_pre ) { pre = _pre; }
    void setPost( const QString &_post ) { pre = _post; }

    QString getPre() const { return pre; }
    QString getPost() const { return post; }

protected:
    QString pre, post;

};

/******************************************************************/
/* Class: KWVariableFieldFormat                                */
/******************************************************************/
class KWVariableFieldFormat : public KWVariableFormat
{
public:
    KWVariableFieldFormat() : KWVariableFormat() {}

//    virtual VariableFormatType getType() const
//    { return VFT_FIELD; }

    virtual QString convert( KWVariable *_var );

};

/******************************************************************/
/* Class: KWVariableCustomFormat                                   */
/******************************************************************/
class KWVariableCustomFormat : public KWVariableFormat
{
public:
    KWVariableCustomFormat() {}

//    virtual VariableFormatType getType() const
//    { return VFT_CUSTOM; }

    virtual QString convert( KWVariable *_var );

};

/******************************************************************/
/* Class: KWVariableSerialLetterFormat                            */
/******************************************************************/
class KWVariableSerialLetterFormat : public KWVariableFormat
{
public:
    KWVariableSerialLetterFormat() {}

//    virtual VariableFormatType getType() const
//    { return VFT_SERIALLETTER; }

    virtual QString convert( KWVariable *_var );

};


// ----------------------------------------------------------------------------------------------

#include <qrichtext_p.h>
using namespace Qt3;

/******************************************************************/
/* Class: KWVariable                                              */
/******************************************************************/
class KWVariable : public KWTextCustomItem
{
public:
    KWVariable( KWTextFrameSet *fs, KWVariableFormat *_varFormat );
    virtual ~KWVariable();

    virtual VariableType getType() const { return VT_NONE; }

    // QTextCustomItem stuff
    virtual Placement placement() const { return PlaceInline; }
    virtual void resize();
    virtual int widthHint() const { return width; }
    virtual int minimumWidth() const { return width; }
    virtual void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg );

    QTextFormat * format() const;

// Somewhat dangerous.
//    void setVariableFormat( KWVariableFormat *_varFormat, bool _deleteOld = false )
//    { if ( _deleteOld && varFormat ) delete varFormat; varFormat = _varFormat; }

//    KWVariableFormat *getVariableFormat() const
//    { return varFormat; }

    // Returns the text to be displayed for this variable
    // It doesn't need to be cached, convert() is fast, and it's the actual
    // value (date, time etc.) that is cached in the variable already.
    QString getText()
    { return varFormat->convert( this ); }

    virtual void recalc() {}

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    static KWVariable * createVariable( int type, int subtype, KWTextFrameSet * textFrameSet );

protected:
    KWDocument *doc;
    KWVariableFormat *varFormat;
};

/******************************************************************/
/* Class: KWPgNumVariable                                         */
/******************************************************************/
class KWPgNumVariable : public KWVariable
{
public:
    KWPgNumVariable( KWTextFrameSet *fs, KWVariableFormat *_varFormat )
        : KWVariable( fs, _varFormat ) { pgNum = 0; }

    virtual VariableType getType() const
    { return VT_PGNUM; }

    // virtual void recalc(); // TODO !
    int getPgNum() const { return pgNum; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

protected:
    int pgNum;
};

/******************************************************************/
/* Class: KWDateVariable                                          */
/******************************************************************/
class KWDateVariable : public KWVariable
{
public:
    KWDateVariable( KWTextFrameSet *fs, bool _fix, QDate _date, KWVariableFormat *_varFormat );
    //KWDateVariable( KWTextFrameSet *fs ) : KWVariable( fs ) {}

    virtual VariableType getType() const
    { return fix ? VT_DATE_FIX : VT_DATE_VAR; }

    virtual void recalc();

    QDate getDate() const { return date; }
    void setDate( const QDate & _date ) { date = _date; }

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
    KWTimeVariable( KWTextFrameSet *fs, bool _fix, QTime _time, KWVariableFormat *_varFormat );
    //KWTimeVariable( KWTextFrameSet *fs ) : KWVariable( fs ) {}

    virtual VariableType getType() const
    { return fix ? VT_TIME_FIX : VT_TIME_VAR; }

    virtual void recalc();

    QTime getTime() const { return time; }
    void setTime( const QTime & _time ) { time = _time; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

protected:
    QTime time;
    bool fix;
};

/******************************************************************/
/* Class: KWFieldVariable                                         */
/* Any variable that is a string, and whose value is automatically*/
/* determined - as opposed to custom variables whose value is     */
/* entered by the user                                            */
/******************************************************************/
class KWFieldVariable : public KWVariable
{
public:
    KWFieldVariable( KWTextFrameSet *fs, int subtype, KWVariableFormat *varFormat );
    virtual VariableType getType() const
    { return VT_FIELD; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual void recalc();
    QString value() const { return m_value; }
protected:
    int m_subtype;
    QString m_value;
};

/******************************************************************/
/* Class: KWCustomVariable                                        */
/******************************************************************/
class KWCustomVariable : public KWVariable
{
public:
    KWCustomVariable( KWTextFrameSet *fs, const QString &name_, KWVariableFormat *_varFormat );
    //KWCustomVariable( KWTextFrameSet *fs ) : KWVariable( fs ) {}

    virtual VariableType getType() const
    { return VT_CUSTOM; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    QString getName() const { return name; }
    QString getValue() const;

    void setValue( const QString &v );

protected:
    QString name;
};

/******************************************************************/
/* Class: KWSerialLetterVariable                                  */
/******************************************************************/
class KWSerialLetterVariable : public KWVariable
{
public:
    KWSerialLetterVariable( KWTextFrameSet *fs, const QString &name_, KWVariableFormat *_varFormat );
    //KWSerialLetterVariable( KWTextFrameSet *fs ) : KWVariable( fs ) {}

    virtual VariableType getType() const
    { return VT_SERIALLETTER; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    QString getName() const { return name; }
    QString getValue() const;

protected:
    QString name;

};

#endif
