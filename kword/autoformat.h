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

#ifndef kwautoformat_h
#define kwautoformat_h

#include <qstring.h>
#include <qcolor.h>
#include <qmap.h>
#include <qvaluelist.h>
#include <qstringlist.h>

class KWDocument;
class KWTextParag;
namespace Qt3 {
    class QTextCursor;
}
using namespace Qt3;
class KWTextFrameSetEdit;

/******************************************************************/
/* Class: KWAutoFormatEntry					  */
/******************************************************************/
class KWAutoFormatEntry
{
public:
    // The text to find is actually the key in KWAutoFormat's map.
    // What we replace it with is replace().
    KWAutoFormatEntry(const QString& replace = QString::null)
        : m_replace( replace ) {}

    QString replace() const { return m_replace; }

protected:
    QString m_replace;
    // For formatting in the replacement - not implemented yet
    //KWSearchContext m_formatOptions;
};

/******************************************************************/
/* Class: KWAutoFormat						  */
/******************************************************************/
class KWAutoFormat
{
public:
    /**
     * There is a single instance of KWAutoFormat per document
     * (and a temporary one in the auto-format dialog).
     */
    KWAutoFormat( KWDocument *_doc );

    /**
     * Called by KWTextFrameSetEdit when a character (@p ch) has been inserted
     * into @p parag, at the given @p index.
     */
    void doAutoFormat( QTextCursor* cursor, KWTextParag *parag, int index, QChar ch );

    static QString getLastWord(KWTextParag *parag, int index);

    //void setEnabled( bool e ) { m_enabled = e; }
    //bool isEnabled() { return m_enabled; }

    // Config for the typographic quotes. Used by the dialog.
    struct TypographicQuotes
    {
	QChar begin, end;
	bool replace; // aka enabled
    };

    // Configuration (on/off/settings). Called by the dialog.
    void configTypographicQuotes( TypographicQuotes _tq );
    void configUpperCase( bool _uc );
    void configUpperUpper( bool _uu );

    TypographicQuotes getConfigTypographicQuotes() const
    { return m_typographicQuotes; }
    bool getConfigUpperCase() const
    { return m_convertUpperCase; }
    bool getConfigUpperUpper() const
    { return m_convertUpperUpper; }

    // Add/remove entries, called by the dialog
    void addAutoFormatEntry( const QString &key, const KWAutoFormatEntry &entry ) {
	m_entries.insert( key, entry );
	buildMaxLen();
    }

    void removeAutoFormatEntry( const QString &key ) {
        m_entries.remove( key );
	buildMaxLen();
    }

    // Iterate over the entries. Called by the dialog
    QMap< QString, KWAutoFormatEntry >::Iterator firstAutoFormatEntry()
    { return m_entries.begin(); }

    QMap< QString, KWAutoFormatEntry >::Iterator lastAutoFormatEntry()
    { return m_entries.end(); }

    // Copy all autoformat entries from another KWAutoFormat. Called by the dialog
    void copyAutoFormatEntries( const KWAutoFormat & other )
    { m_entries = other.m_entries; }

    void copyListException( const QStringList & _list)
	{ upperCaseExceptions=_list;}

    void copyListTwoUpperCaseException( const QStringList &_list)
	{ twoUpperLetterException=_list; }

    QStringList listException() {return upperCaseExceptions;}

    QStringList listTwoUpperLetterException() {return twoUpperLetterException;}

    // Read/save config ( into kwordrc )
    void readConfig();
    void saveConfig();

    static bool isUpper( const QChar &c );
    static bool isLower( const QChar &c );
    static bool isMark( const QChar &c ); // End of sentence
    static bool isSeparator( const QChar &c );

protected:
    bool doAutoCorrect( QTextCursor* textEditCursor, KWTextParag *parag, int index, const QString & word );
    void doUpperCase( QTextCursor* textEditCursor, KWTextParag *parag, int index, const QString & word );
    void doSpellCheck( QTextCursor* textEditCursor, KWTextParag *parag, int index, const QString & word );
    void doTypographicQuotes( QTextCursor* textEditCursor, KWTextParag *parag, int index );
    void buildMaxLen();

private:
    KWDocument *m_doc;

    //bool m_enabled;
    bool m_configRead;
    bool m_convertUpperCase, m_convertUpperUpper;
    bool m_dontUpper;
    TypographicQuotes m_typographicQuotes;

    typedef QMap< QString, KWAutoFormatEntry > KWAutoFormatEntryMap;
    KWAutoFormatEntryMap m_entries;
    QStringList upperCaseExceptions;
    QStringList twoUpperLetterException;
    int m_maxlen;
};

#endif
