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
#include <kaction.h>
#include "qrichtext_p.h"
#include <qvariant.h>

class QDomElement;
// Always add new types at the _end_ of this list (but before VT_ALL of course).
// (and update KWView::setupActions)
enum VariableType { VT_NONE = -1,
                    VT_DATE = 0, VT_TIME = 2, VT_PGNUM = 4,
                    VT_CUSTOM = 6, VT_MAILMERGE = 7, VT_FIELD = 8, VT_LINK = 9,
                    VT_NOTE = 10, VT_FOOTNOTE = 11,
                    VT_ALL=256 };

enum VariableFormat { VF_DATE = 0, VF_TIME = 1, VF_STRING = 2, VF_NUM = 3 };

class KoVariable;

class KoVariableSettings
{
 public:
    KoVariableSettings();
    virtual ~KoVariableSettings() {}
    int startingPage()const{return m_startingpage;}
    void setStartingPage(int _startingpage){ m_startingpage=_startingpage;}

    bool displayLink()const{return m_displayLink;}
    void setDisplayLink( bool b){ m_displayLink=b;}

    bool displayComment()const {return m_displayComment;}
    void setDisplayComment( bool b){ m_displayComment=b;}

    bool underlineLink()const {return m_underlineLink;}
    void setUnderlineLink( bool b){ m_underlineLink=b;}

    virtual void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

 private:
    int m_startingpage;
    bool m_displayLink;
    bool m_displayComment;
    bool m_underlineLink;
};

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
    virtual QString convert(const QVariant& data )const = 0;
};

class KoVariableDateFormat : public KoVariableFormat
{
public:
    KoVariableDateFormat();
    QString convert(const QVariant& data )const;

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
    QString convert(const QVariant& data )const;
    virtual QCString key() const;
    virtual void load( const QCString & /*key*/ );
    QString m_strFormat;
    // TODO custom time formats
};

class KoVariableStringFormat : public KoVariableFormat
{
public:
    KoVariableStringFormat() : KoVariableFormat() {}
    QString convert(const QVariant& data )const;

    virtual QCString key() const;
    virtual void load( const QCString & /*key*/ ) {}
};

class KoVariableNumberFormat : public KoVariableFormat
{
public:
    KoVariableNumberFormat() : KoVariableFormat() {}
    QString convert(const QVariant& data )const;
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

class KoVariable;
class KoVariableFormat;
class KoDocument;
class KoVariableFormatCollection;
class KoTextDocument;
class KoVariableCollection;
class KoVariableCollection : public QObject
{
    Q_OBJECT
public:
    KoVariableCollection(KoVariableSettings *_setting);
    ~KoVariableCollection();
    void registerVariable( KoVariable *var );
    void unregisterVariable( KoVariable *var );
    void recalcVariables(int type);
    void recalcVariables(KoVariable *var);

    // For custom variables
    void setVariableValue( const QString &name, const QString &value );
    QString getVariableValue( const QString &name ) const;

    const QPtrList<KoVariable>& getVariables() const {
        return variables;
    }

    bool customVariableExist(const QString &varname)const ;

    virtual KoVariable *createVariable( int type, int subtype, KoVariableFormatCollection * coll, KoVariableFormat *varFormat,KoTextDocument *textdoc, KoDocument * doc );

    KoVariableSettings *variableSetting(){return m_variableSettings;}

    void setVariableSelected(KoVariable * var);

    QPtrList<KAction> variableActionList();


 signals:
    void repaintVariable();

 public slots:
    void changeTypeOfVariable();
    void changeFormatOfVariable();

 private:
    typedef QMap<KAction *, int> VariableSubTextMap;
    VariableSubTextMap m_variableSubTextMap;

    struct VariableSubFormatDef {
        QString translatedString;
        QString format;
    };
    typedef QMap<KAction *, VariableSubFormatDef> VariableSubFormatMap;
    VariableSubFormatMap m_variableSubFormatMap;

    QPtrList<KoVariable> variables;
    QMap< QString, QString > varValues;
    KoVariableSettings *m_variableSettings;
    KoVariable *m_varSelected;
};


// ----------------------------------------------------------------------------------------------

class KoDocument;
class KoVariable;
class QDomElement;
class KoTextFormat;

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

    // KoTextCustomItem stuff
    virtual Placement placement() const { return PlaceInline; }
    virtual void resize();
    virtual int widthHint() const { return width; }
    virtual int minimumWidth() const { return width; }
    virtual void drawCustomItem( QPainter* p, int x, int y, int cx, int cy, int cw, int ch, const QColorGroup& cg, bool selected, const int offset);
    /**
     * Called by drawCustomItem. Some special variables can reimplement drawCustomItem
     * to change the parameters passed to drawCustomItemHelper
     */
    void drawCustomItemHelper( QPainter* p, int x, int y, const QColorGroup& cg, bool selected, const int offset, KoTextFormat* fmt, const QFont& font, QColor textColor );

    void setVariableFormat( KoVariableFormat *_varFormat );

    KoVariableFormat *variableFormat() const
        { return m_varFormat; }

    KoVariableCollection *variableColl() const
        { return m_varColl; }

    /** Returns the text to be displayed for this variable
     * It doesn't need to be cached, convert() is fast, and it's the actual
     * value (date, time etc.) that is cached in the variable already.
     */
    virtual QString text();

    /** Return the variable value, as a QVariant, before format conversion */
    QVariant varValue() const { return m_varValue; }

    /** Variables reimplement this method to recalculate their value
     * They must call resize() after having done that.
     */
    virtual void recalc() {}

    /** Save the variable. Public API, does the common job and then calls saveVariable. */
    void save( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    /** Part of the KoTextCustomItem interface. Returns the code for a variable, see DTD.
      * Do NOT reimplement in koVariable-derived classes.
      */
    virtual int typeId() const { return 4; }

    virtual QStringList subTypeText();
    virtual QStringList subTypeFormat();
    virtual void setVariableSubType( short int /*subtype*/) {}


protected:
    /** Variable should reimplement this to implement saving. */
    virtual void saveVariable( QDomElement &parentElem ) = 0;
    KoVariableFormat *m_varFormat;
    KoVariableCollection *m_varColl;
    QVariant m_varValue;
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

    void setDate( const QDate & _date ) { m_varValue = QVariant(_date); }

    virtual void saveVariable( QDomElement &parentElem );
    virtual void load( QDomElement &elem );
    virtual QStringList subTypeText();
    virtual QStringList subTypeFormat();
    virtual void setVariableSubType( short int type){m_subtype=type;}

protected:
    short int m_subtype;
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

    void setTime( const QTime & _time ) { m_varValue = QVariant(_time); }

    virtual void saveVariable( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual QStringList subTypeText();
    virtual QStringList subTypeFormat();
    virtual void setVariableSubType( short int type){m_subtype=type;}

protected:
    short int m_subtype;
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

    virtual void saveVariable( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    QString name() const { return m_varValue.toString(); }
    virtual void recalc();
    virtual QString text() { return value(); } // use a format when they are customizable
    QString value() const;
    void setValue( const QString &v );

protected:
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
                        VST_TELEPHONE = 7, VST_FAX = 8, VST_COUNTRY = 9,
                        // room for more 'author' page info fields if asked for
                        VST_TITLE = 10, VST_ABSTRACT = 11,
                        VST_POSTAL_CODE = 12, VST_CITY = 13, VST_STREET = 14};

    virtual VariableType type() const
    { return VT_FIELD; }

    virtual void saveVariable( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual void recalc();
    virtual QString text() { return value(); } // use a format when they are customizable
    QString value() const { return m_varValue.toString(); }

    static QStringList actionTexts();

    virtual QStringList subTypeText();
    virtual void setVariableSubType( short int type){m_subtype=type;}

protected:
    short int m_subtype;
    KoDocument *m_doc;
};

class KoMailMergeVariable : public KoVariable
{
public:
    KoMailMergeVariable( KoTextDocument *textdoc, const QString &name, KoVariableFormat *varFormat, KoVariableCollection *_varColl );

    virtual VariableType type() const
    { return VT_MAILMERGE; }
    static QStringList actionTexts();

    virtual void saveVariable( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual QString text();
    QString name() const { return m_varValue.toString(); }
    virtual QString value() const;

protected:
};

/**
 * "current page number" and "number of pages" variables
 * This is a base class, it must be inherited by applications,
 * to provide recalc().
 * TODO: rename KoPageVariable.
 */
class KoPgNumVariable : public KoVariable
{
public:
    KoPgNumVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat ,KoVariableCollection *_varColl);

    virtual VariableType type() const
    { return VT_PGNUM; }

    enum { VST_PGNUM_CURRENT = 0, VST_PGNUM_TOTAL = 1, VST_CURRENT_SECTION = 2 };
    static QStringList actionTexts();

    virtual QStringList subTypeText();

    virtual void setVariableSubType( short int type);

    // For the 'current page' variable. This is called by the app e.g. when painting
    // a given page (see KWTextFrameSet::drawFrame and KPTextObject::recalcPageNum)
    void setPgNum( int pgNum ) { m_varValue = QVariant( pgNum); }
    // For the 'current section title' variable. Same thing.
    void setSectionTitle( const QString& title ) { m_varValue = QVariant( title); }

    short int subtype() const { return m_subtype; }

    virtual void recalc() = 0;

    virtual void saveVariable( QDomElement &parentElem );
    virtual void load( QDomElement &elem );
protected:
    short int m_subtype;
};

class KoLinkVariable : public KoVariable
{
public:
    KoLinkVariable( KoTextDocument *textdoc, const QString & _linkName, const QString & _ulr,KoVariableFormat *varFormat, KoVariableCollection *_varColl );
    virtual void drawCustomItem( QPainter* p, int x, int y, int /*cx*/, int /*cy*/, int /*cw*/, int /*ch*/, const QColorGroup& cg, bool selected, const int offset );

    virtual VariableType type() const
    { return VT_LINK; }

    static QStringList actionTexts();

    virtual void saveVariable( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual QString text() { return value(); }
    QString value() const { return m_varValue.toString(); }
    QString url() const { return m_url;}

    virtual void recalc();

    void setLink(const QString & _linkName, const QString &_url)
	{
	    m_varValue=QVariant(_linkName);
	    m_url=_url;
	}

protected:
    QString m_url;
};

// A custom item that display a small yellow rect. Right-clicking on it shows the comment.
class KoNoteVariable : public KoVariable
{
public:
    KoNoteVariable( KoTextDocument *textdoc, const QString & _note,KoVariableFormat *varFormat, KoVariableCollection *_varColl );
    virtual void drawCustomItem( QPainter* p, int x, int y, int /*cx*/, int /*cy*/, int /*cw*/, int /*ch*/, const QColorGroup& cg, bool selected, const int offset );

    virtual VariableType type() const
    { return VT_NOTE; }

    static QStringList actionTexts();

    virtual void saveVariable( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual QString text();
    QString note() const { return m_varValue.toString(); }
    void setNote( const QString & _note) { m_varValue = QVariant(_note); }
    virtual void recalc();
};


#endif
