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

#ifndef kovariable_h
#define kovariable_h

#include <qstring.h>
#include <qdatetime.h>
#include <qasciidict.h>
#include <qptrlist.h>
#include <qmap.h>
#include <qobject.h>

// Always add new types at the _end_ of this list.
// (and update KWView::setupActions)
enum VariableType { VT_NONE = -1,
                    VT_DATE = 0, VT_TIME = 2, VT_PGNUM = 4,
                    VT_CUSTOM = 6, VT_SERIALLETTER = 7, VT_FIELD = 8, VT_ALL=256 };

enum VariableFormat { VF_DATE = 0, VF_TIME = 1, VF_STRING = 2, VF_NUM = 3 };

class KoVariable;
/**
 * Class: KoVariableFormat
 * Base class for a variable format - held by KWDocument.
 * Example of formats are time, date, string, number, floating-point number...
 * The reason for formats to be separated is that it allows to
 * customize the formats, to implement subformats (various date formats, etc.). Still TBD.
 */
class KoVariableFormat
{
public:
    KoVariableFormat() {}
    virtual ~KoVariableFormat() {}
    /** Return a key describing this format.
     * Used for the flyweight pattern in KoVariableFormatCollection
     */
    virtual QCString key() const = 0;
    /** Create a format from this key.
     */
    virtual void load( const QCString &key ) = 0;
};

class KoVariableDateFormat : public KoVariableFormat
{
public:
    KoVariableDateFormat();
    QString convert( const QDate & date ) const;
    virtual QCString key() const;
    virtual void load( const QCString &key );
    QString m_strFormat;
private:
    bool m_bShort;
};

class KoVariableTimeFormat : public KoVariableFormat
{
public:
    KoVariableTimeFormat();
    QString convert( const QTime & time ) const;
    virtual QCString key() const;
    virtual void load( const QCString & /*key*/ );
    QString m_strFormat;
    // TODO custom time formats
};

class KoVariableStringFormat : public KoVariableFormat
{
public:
    KoVariableStringFormat() : KoVariableFormat() {}
    QString convert( const QString & string ) const;
    virtual QCString key() const;
    virtual void load( const QCString & /*key*/ ) {}
};

class KoVariableNumberFormat : public KoVariableFormat
{
public:
    KoVariableNumberFormat() : KoVariableFormat() {}
    QString convert( int number ) const;
    virtual QCString key() const;
    virtual void load( const QCString & /*key*/ ) {}
};

/**
 * The collection of formats for variables.
 * Example: date (short or long), time, string (prefix/suffix), number (prefix/suffix, decimals?)...
 * Implements the flyweight pattern to share formats and create them on demand.
 * Each KoDocument holds a KoVariableFormatCollection.
 */
class KoVariableFormatCollection
{
public:
    KoVariableFormatCollection();

    /**
     * Forget (and erase) all the formats this collection knows about
     */
    void clear() { m_dict.clear(); }

    /**
     * Find or create the format for the given @p key
     */
    KoVariableFormat *format( const QCString &key );

    // TODO Refcounting and removing unused formats
    // Not critical, that we don't delete unused formats until closing the doc...
protected:
    KoVariableFormat *createFormat( const QCString &key );

private:
    QAsciiDict<KoVariableFormat> m_dict;
};

/* TODO find a way to integrate with all other formats !
   and add a UI for it
   class ... : public KoVariableFormat
{
public:
    ...() { pre = "-"; post = "-"; }

    virtual QString convert( KWVariable *_var ) const;

    // Needs a UI !
    void setPre( const QString &_pre ) { pre = _pre; }
    void setPost( const QString &_post ) { pre = _post; }

    QString getPre() const { return pre; }
    QString getPost() const { return post; }

protected:
    QString pre, post;
};*/


class KoVariableCollection : public QObject
{
    Q_OBJECT
public:
    KoVariableCollection();
    void registerVariable( KoVariable *var );
    void unregisterVariable( KoVariable *var );
    void recalcVariables(int type);

    // For custom variables
    void setVariableValue( const QString &name, const QString &value );
    QString getVariableValue( const QString &name ) const;

    const QPtrList<KoVariable>& getVariables() const {
        return variables;
    }

 signals:
    void repaintVariable();
 private:
    QPtrList<KoVariable> variables;
    QMap< QString, QString > varValues;
};


// ----------------------------------------------------------------------------------------------

#include <kotextdocument.h>
class KoDocument;
class KoVariable;
class QDomElement;
namespace Qt3 {
class QTextFormat;
}

/**
 * A KoVariable is a custom item, i.e. considered as a single character.
 * KoVariable is the abstract base class.
 */
class KoVariable : public KoTextCustomItem
{
public:
    KoVariable( KoTextDocument *fs, KoVariableFormat *varFormat,KoVariableCollection *varColl );
    virtual ~KoVariable();

    virtual VariableType type() const = 0;

    // QTextCustomItem stuff
    virtual Placement placement() const { return PlaceInline; }
    virtual void resize();
    virtual int widthHint() const { return width; }
    virtual int minimumWidth() const { return width; }
    virtual void drawCustomItem( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected, const int offset);

    void setVariableFormat( KoVariableFormat *_varFormat )
    { m_varFormat = _varFormat; }

    KoVariableFormat *variableFormat() const
    { return m_varFormat; }

    /** Returns the text to be displayed for this variable
     * It doesn't need to be cached, convert() is fast, and it's the actual
     * value (date, time etc.) that is cached in the variable already.
     */
    virtual QString text() = 0;
   // { return varFormat->convert( variantValue() ); } too bad QVariant doesn't have QDate/QTime :(

    // Variables reimplement this method to recalculate their value
    // They must call resize() after having done that.
    virtual void recalc() {}

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    /**
     * Create a variable, from its @p type and @p subtype.
     * When @p varFormat is 0, the variable is created with its default format.
     */
    static KoVariable * createVariable( int type, int subtype, KoVariableFormatCollection * coll, KoVariableFormat * varFormat,KoTextDocument *textdoc, KoDocument * doc, KoVariableCollection *varColl );

protected:
    KoVariableFormat *m_varFormat;
    KoVariableCollection *m_varColl;
};

/**
 * Date-related variables
 */
class KoDateVariable : public KoVariable
{
public:
    KoDateVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *_varFormat,KoVariableCollection *_varColl );

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
class KoTimeVariable : public KoVariable
{
public:
    KoTimeVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat, KoVariableCollection *_varColl);

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
 * A custom variable is a variable whose value is entered
 * by the user.
 */
class KoCustomVariable : public KoVariable
{
public:
    KoCustomVariable(KoTextDocument *textdoc , const QString &name, KoVariableFormat *varFormat,KoVariableCollection *_varcoll );

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
 * Any variable that is a string, and whose value is automatically
 * determined - as opposed to custom variables whose value is
 * entered by the user
 */
class KoFieldVariable : public KoVariable
{
public:
    KoFieldVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KoDocument *_doc );

    // Do not change existing values
    enum FieldSubType { VST_NONE = -1,
                        VST_FILENAME = 0, VST_DIRECTORYNAME = 1,
                        VST_AUTHORNAME = 2, VST_EMAIL = 3, VST_COMPANYNAME = 4,
			VST_PATHFILENAME = 5, VST_FILENAMEWITHOUTEXTENSION=6,
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
    KoDocument *m_doc;
};

class KoSerialLetterVariable : public KoVariable
{
public:
    KoSerialLetterVariable( KoTextDocument *textdoc, const QString &name, KoVariableFormat *varFormat, KoVariableCollection *_varColl );

    virtual VariableType type() const
    { return VT_SERIALLETTER; }
    static QStringList actionTexts();

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual QString text();
    QString name() const { return m_name; }
    virtual QString value() const;

protected:
    QString m_name;

};

/**
 * "current page number" and "number of pages" variables
 */
class KoPgNumVariable : public KoVariable
{
public:
    KoPgNumVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat ,KoVariableCollection *_varColl);

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

#endif
