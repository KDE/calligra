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
#include <defs.h>
#include <koVariable.h>

#include <kwtextdocument.h>
class KWDocument;
class KoVariable;
class KoPgNumVariable;
class KoMailMergeVariable;
class QDomElement;
class KoTextFormat;

class KWVariableCollection : public KoVariableCollection
{
 public:
    KWVariableCollection();
    virtual KoVariable *createVariable( int type, int subtype, KoVariableFormatCollection * coll, KoVariableFormat *varFormat,KoTextDocument *textdoc, KoDocument * doc );
};

/**
 * "current page number" and "number of pages" variables
 */
class KWPgNumVariable : public KoPgNumVariable
{
public:
    KWPgNumVariable( KoTextDocument *textdoc, int subtype, KoVariableFormat *varFormat ,KoVariableCollection *_varColl, KWDocument *doc );

    virtual void recalc();
    virtual void setVariableSubType( short int type);
 private:
    KWDocument *m_doc;
};


/**
 * Mail Merge variable
 */
class KWMailMergeVariable : public KoMailMergeVariable
{
public:
    KWMailMergeVariable( KoTextDocument *textdoc, const QString &name, KoVariableFormat *varFormat,KoVariableCollection *_varColl, KWDocument *doc );

    virtual QString text();
    virtual QString value() const;
    virtual void recalc();
 private:
    KWDocument *m_doc;
};

/**
 * The variable showing the footnote number in superscript, in the text.
 */
class KWFootNoteVariable : public KoVariable
{
public:
    KWFootNoteVariable( KoTextDocument *textdoc, NoteType noteType, KoVariableFormat *varFormat, KoVariableCollection *varColl );

    virtual VariableType type() const
    { return VT_FOOTNOTE; }

    //static QStringList actionTexts();

    /** The frameset that contains the text for this footnote */
    KWTextFrameSet * frameSet() const { return m_frameset; }
    void setFrameSet( KWTextFrameSet* fs ) { Q_ASSERT( !m_frameset ); m_frameset = fs; }

    virtual void saveVariable( QDomElement &parentElem );
    virtual void load( QDomElement &elem );

    virtual QString text();
    // Nothing to do here. Done at a higher level.
    virtual void recalc() { }

    void setNum( int num ) { m_num = num; }

private:
    unsigned short int m_num;
    NoteType m_noteType;
    KWTextFrameSet* m_frameset;
};

#endif
