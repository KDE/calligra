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

#ifndef __KOISPELL_H__
#define __KOISPELL_H__

#include "koSconfig.h"

#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>
#include "koSpell.h"

class KProcIO;
class KProcess;
class KOSpellDlg;

class KOISpell :  public KOSpell
{
    Q_OBJECT

public:

    KOISpell(QWidget *parent, const QString &caption,
             QObject *receiver, const char *slot, KOSpellConfig *kcs=0,
             bool progressbar = TRUE, bool modal = FALSE );

    virtual void cleanUp ();


    virtual bool check (const QString &_buffer, bool usedialog = TRUE);

    virtual QStringList resultCheckWord( const QString &/*_word*/ );


    virtual bool checkList (QStringList *_wordlist, bool usedialog = TRUE);


    virtual bool checkWord (const QString &_buffer, bool usedialog = FALSE);


    virtual bool ignore (const QString & word);

    virtual bool addPersonal (const QString & word);


    void setProgressResolution (unsigned res);

    virtual ~KOISpell();

    static int modalCheck( QString& text, KOSpellConfig * kcs );

protected slots:
    /* All of those signals from KProcIO get sent here. */
    void KSpell2 (KProcIO *);
    void checkWord2 (KProcIO *);
    void checkWord3 ();
    void check2 (KProcIO *);
    void checkList2 ();
    void checkList3a (KProcIO *);
    void checkListReplaceCurrent ();
    void checkList4 ();
    void dialog2 (int dlgresult);
    void check3 ();

    void slotStopCancel (int);
    void ispellExit (KProcess *);
    void emitDeath();
    void ispellErrors (KProcess *, char *, int);

private slots:
    /**
     * Used for @ref modalCheck().
     */
    void slotModalReady();

    void slotModalDone( const QString & );
    void slotSpellCheckerCorrected( const QString & oldText, const QString & newText, unsigned int );
    void  slotModalSpellCheckerFinished( );

signals:
    void dialog3 ();
    /**
     * Emitted during a @ref check().
     * @p i is between 1 and 100.
     */
    void progress (unsigned int i);

protected:

    KProcIO *proc;

    bool usedialog;
    bool texmode;
    bool dlgon;
    bool personaldict;
    bool dialogwillprocess;
    bool progressbar;
    bool dialogsetup;


    QString dialog3slot;

    int trystart;
    int maxtrystart;
    unsigned int progres;
    unsigned int curprog;

    /**
     * Used for @ref #modalCheck.
     */
    bool modaldlg;
    static QString modaltext;
    static int modalreturn;
    static QWidget* modalWidgetHack;
    static QStringList modalListText;

    int parseOneResponse (const QString &_buffer, QString &word, QStringList &sugg);
    QString funnyWord (const QString & word);
    void dialog (const QString & word, QStringList & sugg, const char* _slot);

    void setUpDialog ( bool reallyusedialogbox = TRUE);

    void emitProgress ();
    bool cleanFputs (const QString & s, bool appendCR=TRUE);
    bool cleanFputsWord (const QString & s, bool appendCR=TRUE);
    void startIspell();
    bool writePersonalDictionary ();
};


#endif
