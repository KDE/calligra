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
// (and update KWView::setupActions)
enum VariableType { VT_NONE = -1,
                    VT_DATE = 0, VT_TIME = 2, VT_PGNUM = 4,
                    VT_CUSTOM = 6, VT_SERIALLETTER = 7, VT_FIELD = 8 };

enum VariableFormat { VF_DATE = 0, VF_TIME = 1, VF_STRING = 2, VF_NUM = 3 };

/**
 * Class: KWVariableFormat
 * Base class for a variable format - held by KWDocument.
 * Example of formats are time, date, string, number, floating-point number...
 * The reason for formats to be separated is that it allows to
 * customize the formats, to implement subformats (various date formats, etc.). Still TBD.
 */
class KWVariableFormat
{
public:
    KWVariableFormat() {}
    virtual ~KWVariableFormat() {}
    // TODO load,save...
};

class KWVariableDateFormat : public KWVariableFormat
{
public:
    KWVariableDateFormat() : KWVariableFormat() {}
    QString convert( const QDate & date );
    // TODO various date formats.
};

class KWVariableTimeFormat : public KWVariableFormat
{
public:
    KWVariableTimeFormat() : KWVariableFormat() {}
    QString convert( const QTime & time );
    // TODO various time formats.
};

class KWVariableStringFormat : public KWVariableFormat
{
public:
    KWVariableStringFormat() : KWVariableFormat() {}
    QString convert( const QString & string );
};

class KWVariableNumberFormat : public KWVariableFormat
{
public:
    KWVariableNumberFormat() : KWVariableFormat() {}
    QString convert( int number );
};

/* TODO find a name :)
   and add a UI for it
   class ... : public KWVariableFormat
{
public:
    ...() { pre = "-"; post = "-"; }

    virtual QString convert( KWVariable *_var );

    // Needs a UI !
    void setPre( const QString &_pre ) { pre = _pre; }
    void setPost( const QString &_post ) { pre = _post; }

    QString getPre() const { return pre; }
    QString getPost() const { return post; }

protected:
    QString pre, post;
};*/

// ----------------------------------------------------------------------------------------------

#include <qrichtext_p.h>
using namespace Qt3;

/**
 * A KWVariable is a custom item, i.e. considered as a single character.
 * KWVariable is the abstract base class.
 */
class KWVariable : public KWTextCustomItem
{
public:
    KWVariable( KWTextFrameSet *fs, KWVariableFormat *varFormat );
    virtual ~KWVariable();

    virtual VariableType type() const = 0;

    // QTextCustomItem stuff
    virtual Placement placement() const { return PlaceInline; }
    virtual void resize();
    virtual int widthHint() const { return width; }
    virtual int minimumWidth() const { return width; }
    virtual void draw( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected );

    void setVariableFormat( KWVariableFormat *_varFormat, bool _deleteOld = false )
    { if ( _deleteOld && m_varFormat ) delete m_varFormat; m_varFormat = _varFormat; }

    KWVariableFormat *variableFormat() const
    { return m_varFormat; }

    // Returns the text to be displayed for this variable
    // It doesn't need to be cached, convert() is fast, and it's the actual
    // value (date, time etc.) that is cached in the variable already.
    virtual QString text() = 0;
    // { return varFormat->convert( variantValue() ); } too bad QVariant doesn't have QDate/QTime :(

    // Variables reimplement this method to recalculate their value
    // They must call resize() after having done that.
    virtual void recalc() {}

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    static KWVariable * createVariable( int type, int subtype, KWTextFrameSet * textFrameSet );

protected:
    KWDocument *m_doc;
    KWVariableFormat *m_varFormat;
};

/**
 * Date-related variables
 */
class KWDateVariable : public KWVariable
{
public:
    KWDateVariable( KWTextFrameSet *fs, int subtype, KWVariableFormat *_varFormat );

    virtual VariableType type() const
    { return VT_DATE; }

    enum { VST_DATE_FIX = 0, VST_DATE_CURRENT = 1 };
    static QStringList actionTexts();

    virtual void recalc();

    virtual QString text();
    //QDate date() const { return m_date; }
    void setDate( const QDate & _date ) { m_date = _date; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

protected:
    short int m_subtype;
    QDate m_date;
};

/**
 * Time-related variables
 */
class KWTimeVariable : public KWVariable
{
public:
    KWTimeVariable( KWTextFrameSet *fs, int subtype, KWVariableFormat *varFormat );

    virtual VariableType type() const
    { return VT_TIME; }

    enum { VST_TIME_FIX = 0, VST_TIME_CURRENT = 1 };
    static QStringList actionTexts();

    virtual void recalc();

    //QTime time() const { return m_time; }
    virtual QString text();
    void setTime( const QTime & _time ) { m_time = _time; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

protected:
    short int m_subtype;
    QTime m_time;
};

/**
 * "current page number" and "number of pages" variables
 */
class KWPgNumVariable : public KWVariable
{
public:
    KWPgNumVariable( KWTextFrameSet *fs, int subtype, KWVariableFormat *varFormat );

    virtual VariableType type() const
    { return VT_PGNUM; }

    enum { VST_PGNUM_CURRENT = 0, VST_PGNUM_TOTAL = 1 };
    static QStringList actionTexts();

    // For the 'current page' variable. This is called by KWTextFrameSet::drawFrame.
    void setPgNum( int pgNum ) { m_pgNum = pgNum; }
    short int subtype() const { return m_subtype; }

    virtual void recalc();
    virtual QString text();

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );
protected:
    short int m_subtype;
    int m_pgNum;
};

/**
 * A custom variable is a variable whose value is entered
 * by the user.
 */
class KWCustomVariable : public KWVariable
{
public:
    KWCustomVariable( KWTextFrameSet *fs, const QString &name, KWVariableFormat *varFormat );

    virtual VariableType type() const
    { return VT_CUSTOM; }
    static QStringList actionTexts();

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    QString name() const { return m_name; }
    virtual void recalc();
    virtual QString text() { return value(); } // use a format when they are customizable
    QString value() const;
    void setValue( const QString &v );

protected:
    QString m_name;
};

/**
 * Serial letter variable
 */
class KWSerialLetterVariable : public KWVariable
{
public:
    KWSerialLetterVariable( KWTextFrameSet *fs, const QString &name, KWVariableFormat *varFormat );

    virtual VariableType type() const
    { return VT_SERIALLETTER; }
    static QStringList actionTexts();

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual QString text();
    QString name() const { return m_name; }
    QString value() const;

protected:
    QString m_name;

};

/**
 * Any variable that is a string, and whose value is automatically
 * determined - as opposed to custom variables whose value is
 * entered by the user
 */
class KWFieldVariable : public KWVariable
{
public:
    KWFieldVariable( KWTextFrameSet *fs, int subtype, KWVariableFormat *varFormat );

    // Do not change existing values
    enum FieldSubType { VST_NONE = -1,
                        VST_FILENAME = 0, VST_DIRECTORYNAME = 1,
                        VST_AUTHORNAME = 2, VST_EMAIL = 3, VST_COMPANYNAME = 4,
                        // room for more 'author' page info fields if asked for
                        VST_TITLE = 10, VST_ABSTRACT = 11 };

    virtual VariableType type() const
    { return VT_FIELD; }

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual void recalc();
    virtual QString text() { return value(); } // use a format when they are customizable
    QString value() const { return m_value; }

    static QStringList actionTexts();

protected:
    short int m_subtype;
    QString m_value;
};

#endif
