/* This file is part of the KDE libraries
   Copyright (C) 2002-2003 Laurent Montel <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef __KOASPELL_H__
#define __KOASPELL_H__

#include "koSconfig.h"

#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>

#include "koSpell.h"
#ifdef HAVE_LIBASPELL

class QTextCodec;
class KOSpellDlg;
class AspellSpeller;
class AspellConfig;

class KOASpell : public KOSpell
{
    Q_OBJECT

public:

    KOASpell(QWidget *parent, const QString &caption,KOSpellConfig *kcs=0,
             bool modal = FALSE, bool _autocorrect =FALSE );

    /*
     * Create a simple kospell class without dialogbox
     * we can just use resultCheckWord function to get list
     * of result
     */
    KOASpell( KOSpellConfig *_ksc );

    QStringList resultCheckWord( const QString &_word );

    //virtual bool checkWord (const QString &_buffer, bool usedialog = FALSE);


    virtual bool ignore (const QString & word);
    virtual bool addPersonal (const QString & word);


    virtual ~KOASpell();

    static int modalCheck( QString& text ); // marked as deprecated

    static int modalCheck( QString& text, KOSpellConfig * kcs );

    virtual bool check (const QString &_buffer, bool usedialog = TRUE) ;


protected slots:
    /* All of those signals from KProcIO get sent here. */
    void dialog2 (int dlgresult);

    void emitDeath();

protected:

    AspellSpeller * speller;
    AspellConfig * config;

    bool usedialog;
    bool personaldict;
    bool dialogwillsprocess;

    static QString modaltext;
    static int modalreturn;
    static QWidget* modalWidgetHack;
    static QStringList modalListText;

    void dialog (const QString & word, QStringList & sugg);

    void setUpDialog ();

    bool writePersonalDictionary ();

    bool spellWord( const QString &_word );

    void checkNextWord();
    void nextWord();
    void previousWord();

    void spellCheckReplaceWord( const QString & _word);

    bool initConfig(const QString & language= QString::null);
    void changeSpellLanguage( int index );
    void testIgnoreWord( QString & word, bool haveAnNumber );

    void initSpell(KOSpellConfig *_ksc);
    void correctWord( const QString & originalword, const QString & newword );
    void deleteSpellChecker();

private slots:
    void slotSpellCheckerCorrected( const QString &, const QString &, unsigned int );
    void slotModalDone( const QString &/*_buffer*/ );
    void slotModalSpellCheckerFinished();
};

#endif
#endif
