/* This file is part of the KDE libraries
   Copyright (C) 1997 David Sweet <dsweet@kde.org>
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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
#ifndef __KOSPELL_H__
#define __KOSPELL_H__

#include "koSconfig.h"

#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>

class QTextCodec;
class KOSpellDlg;
class AspellSpeller;
class AspellConfig;
/**
 * KDE Spellchecker
 *
 * A KDE programmer's interface to International ISpell 3.1.  (GPL 1997)
 * A static method, @ref modalCheck() is provided for convenient
 *  access to the spellchecker.
 *
 * @author David Sweet <dsweet@kde.org>
 * @see KOSpellConfig
 */

class KOSpell : public QObject
{
    Q_OBJECT

public:

    /**
     * Possible states of the spell checker.
     *
     * @li @p Starting - After creation of KSpell.
     * @li @p Running - After the ready signal has been emitted.
     * @li @p Cleaning - After @ref cleanUp() has been called.
     * @li @p Finished - After @ref cleanUp() has been completed.
     *
     * The following error states exist:
     *
     * @li @p Error -  An error occurred in the @p Starting state.
     * @li @p Crashed - An error occurred in the @p Running state.
     **/
    enum spellStatus { Starting = 0, Running, Cleaning, Finished, Error, Crashed };

    /**
     * Starts the spellchecker.
     *
     * KSpell emits @ref ready() when it has verified that
     * ISpell/ASpell is working properly. Pass the name of a slot -- do not pass zero!
     * Be sure to call @ref cleanUp() when you are done with KSpell.
     *
     * If KSpell could not be started correctly, @ref death() is emitted.
     *
     * @param parent      Parent of @ref KSpellConfig dialog..
     * @param caption     Caption of @ref KSpellConfig dialog.
     * @param receiver    Receiver object for the ready(KSpell *) signal.
     * @param slot        Receiver's slot, will be connected to the ready(KSpell *) signal.
     * @param kcs         Configuration for KSpell.
     * @param modal       Indicates modal or non-modal dialog.
     */
    KOSpell(QWidget *parent, const QString &caption,KOSpellConfig *kcs=0,
             bool modal = FALSE, bool _autocorrect =FALSE );

    /**
     * Returns the status of KSpell.
     *
     * @see spellStatus()
     */
    spellStatus status() const { return m_status; }

    /**
     * Sets the auto-delete flag. If this is set, the KSpell object
     * is automatically deleted after emitting @ref death().
     */
    void setAutoDelete(bool _autoDelete) { autoDelete = _autoDelete; }

    /**
     *  Spellchecks a buffer of many words in plain text
     *  format.
     *
     * The @p _buffer is not modified.  The signal @ref done() will be
     *  sent when @ref check() is finished and the argument will be a
     *  spell-corrected version of @p _buffer.
     *
     * The spell check may be stopped by the user before the entire buffer
     *  has been checked.  You can check @ref lastPosition() to see how far
     *  in @p _buffer @ref check() reached before stopping.
     */

    virtual bool check (const QString &_buffer, bool usedialog = TRUE);

    /**
     * Returns the position (when using @ref check())  or word
     * number (when using @ref checkList()) of
     * the last word checked.
     */
    int lastPosition() const
        { return lastpos;}

    /**
     * Spellchecks a single word.
     *
     * checkWord() is the most flexible function.  Some applications
     *  might need this flexibility but will sacrifice speed when
     *  checking large numbers of words.  Consider @ref checkList() for
     *  checking many words.
     *
     *  Use this method for implementing  "online" spellchecking (i.e.,
     *  spellcheck as-you-type).
     *
     * checkWord() returns @p false if @p buffer is not a single word (e.g.
     *  if it contains white space), otherwise it returns @p true;
     *
     * If @p usedialog is set to @p true, KSpell will open the standard
     *  dialog if the word is not found.  The dialog results can be queried
     *  by using  @ref dlgResult() and @ref replacement().
     *
     *  The signal @ref corrected() is emitted when the check is
     *  complete.  You can look at @ref suggestions() to see what the
     *  suggested replacements were.
     */
    //virtual bool checkWord (const QString &_buffer, bool usedialog = FALSE);

    /**
     * Hides the dialog box.
     *
     * You'll need to do this when you are done with @ref checkWord();
     */
    void hide ();

    /**
     * Returns list of suggested word replacements.
     *
     * After calling @ref checkWord() (an in response to
     *  a @ref misspelled() signal you can
     *  use this to get the list of
     *  suggestions (if any were available).
     */
    QStringList suggestions () const { return sugg; }

    /**
     * Gets the result code of the dialog box.
     *
     * After calling checkWord, you can use this to get the dialog box's
     *  result code.
     * The possible
     *  values are (from kspelldlg.h):
     *    @li KS_CANCEL
     *    @li KS_REPLACE
     *    @li KS_REPLACEALL
     *    @li KS_IGNORE
     *    @li KS_IGNOREALL
     *    @li KS_ADD
     *    @li KS_STOP
     *
     */
    int dlgResult () const
        { return dlgresult; }

    /**
     * Moves the dialog.
     *
     * If the dialog is not currently visible, it will
     *   be placed at this position when it becomes visible.
     * Use this to get the dialog out of the way of a highlighted
     * misspelled word in a document.
     */
    void moveDlg (int x, int y);

    /**
     * Returns the height of the dialog box.
     */
    int heightDlg () const;
    /**
     * Returns the width of the dialog box.
     */
    int widthDlg () const;

    /**
     * Returns the partially spellchecked buffer.
     *
     * You might want the full buffer in its partially-checked state.
     */
    QString intermediateBuffer () const {return newbuffer;}

    /**
     * Tells ISpell/ASpell to ignore this word for the life of this KSpell instance.
     *
     *  @return false if @p word is not a word or there was an error
     *  communicating with ISpell/ASpell.
     */
    virtual bool ignore (const QString & word);

    /**
     * Adds a word to the user's personal dictionary.
     *
     * @return false if @p word
     *  is not a word or there was an error communicating with ISpell/ASpell.
     */
    virtual bool addPersonal (const QString & word);

    /**
     * @return the @ref KSpellConfig object being used by this KSpell instance.
     */
    KOSpellConfig ksConfig () const;


    /**
     * The destructor instructs ISpell/ASpell to write out the personal
     *  dictionary and then terminates ISpell/ASpell.
     */
    virtual ~KOSpell();

    /**
     * Performs a synchronous spellcheck.
     *
     * This method does not return until spellchecking is done or canceled.
     * Your application's GUI will still be updated, however.
     */
    static int modalCheck( QString& text ); // marked as deprecated

    /**
     * Performs a synchronous spellcheck.
     *
     * This method does not return until spellchecking is done or canceled.
     * Your application's GUI will still be updated, however.
     *
     * This overloaded method uses the spell-check configuration passed as parameter.
     */
    static int modalCheck( QString& text, KOSpellConfig * kcs );

    /**
     * Call @ref setIgnoreUpperWords(true) to tell the spell-checker to ignore
     * words that are completely uppercase. They are spell-checked by default.
     */
    void setIgnoreUpperWords(bool b);

    /**
     * Call @ref setIgnoreTitleCase(true) to tell the spell-checker to ignore
     * words with a 'title' case, i.e. starting with an uppercase letter.
     * They are spell-checked by default.
     */
    void setIgnoreTitleCase(bool b);

signals:

    /**
     * Emitted whenever a misspelled word is found by @ref check() or
     *   by @ref checkWord().
     *  If it is emitted by @ref checkWord(), @p pos=0.
     *  If it is emitted by @ref check(), then @p pos indicates the position of
     *   the misspelled word in the (original) @p _buffer.
     *   (The first position is zero.)
     *  If it is emitted by @ref checkList(), @p pos is the index to
     *  the misspelled
     *   word in the @ref QStringList passed to @ref checkList().
     *  Note, that @p originalword can be only a word part, if it's
     *  word with hyphens.
     *
     *  These are called _before_ the dialog is opened, so that the
     *   calling program's GUI may be updated. (e.g. the misspelled word may
     *   be highlighted).
     */
    void misspelling (const QString & originalword, const QStringList & suggestions,
                      unsigned int pos);

    /**
     * Emitted after the "Replace" or "Replace All" buttons of the dialog
     * was pressed, or if the word was
     * corrected without calling the dialog (i.e., the user previously chose
     * "Replace All" for this word).
     *
     * Results from the dialog may be checked with @ref dlgResult()
     *  and @ref replacement().
     *
     * Note, that when using @ref checkList() this signal can occur
     * more then once with same list position, when checking a word with
     * hyphens. In this case @p originalword is the last replacement.
     *
     * @see check()
     */
    void corrected (const QString & originalword, const QString & newword, unsigned int pos);

    /**
     * Emitted when the user pressed "Ignore All" in the dialog.
     * This could be used to make an application or file specific
     * user dictionary.
     *
     */
    void ignoreall (const QString & originalword);

    /**
     * Emitted when the user pressed "Ignore" in the dialog.
     * Don't know if this could be useful.
     *
     */
    void ignoreword (const QString & originalword);

    /**
     * Emitted when the user pressed "Add" in the dialog.
     * This could be used to make an external user dictionary
     * independent of the ISpell personal dictionary.
     *
     */
    void addword (const QString & originalword);

    /**
     * Emitted when the user pressed "ReplaceAll" in the dialog.
     */
    void replaceall( const QString & origword ,  const QString &replacement );

    void addAutoCorrect (const QString & originalword, const QString & newword);


    /**
     * Emitted after KSpell has verified that ISpell/ASpell is running
     * and working properly.
     */
    void ready(KOSpell *);


    /**
     * Emitted when @ref check() is done.
     *
     * Be sure to copy the results of @p buffer if you need them.
     *  You can only rely
     *  on the contents of buffer for the life of the slot which was signaled
     *  by @ref done().
     */
    void done (const QString &buffer);

    /**
     * Emitted when @ref checkList() is done.
     *
     * If the argument is
     * @p true, then you should update your text from the
     * wordlist, otherwise not.
     */
    void done(bool);

    /**
     * Emitted on terminal errors and after clean up.
     *
     * You can delete the KSpell object in this signal.
     *
     * You can check @ref status() to see what caused the death:
     * @li @p Error - KSpell could not start.
     * @li @p Crashed - KSpell encountered an unexpected error during execution.
     * @li @p Finished - Clean up finished.
     */
    void death( );


protected slots:
    /* All of those signals from KProcIO get sent here. */
    void dialog2 (int dlgresult);

    void emitDeath();

signals:
    void dialog3 ();

protected:

    QWidget *parent;
    KOSpellConfig *ksconfig;
    KOSpellDlg *ksdlg;
    QStringList *wordlist;
    QStringList::Iterator wlIt;
    QStringList ignorelist;
    QStringList replacelist;
    QStringList sugg;
    QTextCodec* codec;

    AspellSpeller * speller;
    AspellConfig * config;

    spellStatus m_status;

    bool usedialog;
    bool texmode;
    bool dlgon;
    bool personaldict;
    bool dialogwillsprocess;
    bool autoDelete;
    bool modaldlg;

    static QString modaltext;
    static int modalreturn;
    static QWidget* modalWidgetHack;
    static QStringList modalListText;


    bool autocorrect;
    QString caption;
    QString orig;
    QString origbuffer;
    QString newbuffer;
    QString cwword;
    QString dlgorigword;
    QString dlgreplacement;

    int dlgresult;
    int trystart;
    int maxtrystart;
    int lastpos;
    unsigned int totalpos;
    unsigned int lastline;
    unsigned int posinline;
    unsigned int lastlastline;
    unsigned int offset;
    unsigned int curprog;

    void dialog (const QString & word, QStringList & sugg);
    QString replacement () const { return dlgreplacement; }

    void setUpDialog ();

    bool writePersonalDictionary ();

    bool spellWord( const QString &_word );

    void checkNextWord();
    void nextWord();
    void previousWord();

    void spellCheckReplaceWord( const QString & _word);
    QStringList resultCheckWord( const QString &_word );

    bool initConfig();
    void changeSpellLanguage( int index );

private slots:
    void slotSpellCheckerCorrected( const QString &, const QString &, unsigned int );
    void slotModalDone( const QString &/*_buffer*/ );
    void slotModalSpellCheckerFinished();

private:
    class KOSpellPrivate;
    KOSpellPrivate *d;
};

/**
 * @libdoc Spelling Checker Library
 *
 * @ref KSpell offers easy access to International ISpell or ASpell
 *  (at the user's option) as well as a spell-checker GUI
 *  ("Add", "Replace", etc.).
 *
 * You can use @ref KSpell to
 *  automatically spell-check an ASCII file as well as to implement
 *  online spell-checking and to spell-check proprietary format and
 *  marked up (e.g. HTML, TeX) documents.  The relevant methods for
 *  these three procedures are @ref check(), @ref checkWord(), and
 *  @ref checkList(), respectively.
 *
 * @ref KSpellConfig holds configuration information about @ref KSpell as well
 *  as acting as an options-setting dialog.
 *
 * KSpell usually works asynchronously. If you do not need that, you should
 * simply use @ref KSpell::modalCheck(). It won't return until the
 * passed string is processed or the spell checking canceled.
 * During modal spell checking your GUI is still repainted, but the user may
 * only interact with the @ref KSpell dialog.
 *
 * @see KSpell, KSpellConfig
 **/

#endif
