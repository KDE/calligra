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
#ifndef __KOSPELL_H__
#define __KOSPELL_H__

#include "koSconfig.h"

#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>

class KOSpellDlg;

class KOSpell : public QObject
{
    Q_OBJECT
public:
    enum spellStatus { Starting = 0, Running, Cleaning, Finished, Error, Crashed, FinishedNoMisspellingsEncountered };
    ~KOSpell();
    //slot necessary for old lib
    //receive used this by default
    static KOSpell *createKoSpell( QWidget *parent, const QString &caption, QObject *receiver, const char *slot,KOSpellConfig *kcs, bool modal, bool _autocorrect );

    static int modalCheck( QString& text, KOSpellConfig * kcs );
    static int modalCheck( QString& text);


   /**
     * Cleans up ISpell.
     *
     * Write out the personal dictionary and close ISpell's
     *  stdin.  A @ref death() signal will be emitted when the cleanup is
     *  complete, but this method will return immediately.
     */
    virtual void cleanUp () {};


    /**
     * Returns the position (when using @ref check())  or word
     * number (when using @ref checkList()) of
     * the last word checked.
     */
    int lastPosition() const
        { return lastpos;}

    /**
     * Sets the auto-delete flag. If this is set, the KSpell object
     * is automatically deleted after emitting @ref death().
     */
    void setAutoDelete(bool _autoDelete) { autoDelete = _autoDelete; }
    /**
     * Returns the status of KSpell.
     *
     * @see spellStatus()
     */
    spellStatus status() const { return m_status; }
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

    virtual bool check (const QString &_buffer, bool usedialog = TRUE) = 0;

    virtual QStringList resultCheckWord( const QString &_word ) = 0;


    /**
     * Tells ISpell/ASpell to ignore this word for the life of this KSpell instance.
     *
     *  @return false if @p word is not a word or there was an error
     *  communicating with ISpell/ASpell.
     */
    virtual bool ignore (const QString & word)=0;

    /**
     * Adds a word to the user's personal dictionary.
     *
     * @return false if @p word
     *  is not a word or there was an error communicating with ISpell/ASpell.
     */
    virtual bool addPersonal (const QString & word)= 0;

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
    /**
     * @return the @ref KSpellConfig object being used by this KSpell instance.
     */
    KOSpellConfig ksConfig () const;

    QString replacement () const { return dlgreplacement; }
    /**
     * Returns the partially spellchecked buffer.
     *
     * You might want the full buffer in its partially-checked state.
     */
    QString intermediateBuffer () const {return newbuffer;}
    /**
     * Hides the dialog box.
     *
     * You'll need to do this when you are done with @ref checkWord();
     */
    void hide ();
    /**
     * Returns the height of the dialog box.
     */
    int heightDlg () const;
    /**
     * Returns the width of the dialog box.
     */
    int widthDlg () const;
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
   * Returns list of suggested word replacements.
   *
   * After calling @ref checkWord() (an in response to
   *  a @ref misspelled() signal you can
   *  use this to get the list of
   *  suggestions (if any were available).
   */
  QStringList suggestions () const { return sugg; }
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

    // emit to start spell check when we use libaspell
    void spellCheckerReady();

private slots:
    virtual void slotSpellCheckerCorrected( const QString &, const QString &, unsigned int ) = 0;
    virtual void slotModalDone( const QString &/*_buffer*/ ) = 0;
    virtual void slotModalSpellCheckerFinished() = 0;

protected:
    void initSpell( KOSpellConfig *_ksc );
    void misspellingWord (const QString & originalword, const QStringList & suggestions, unsigned int pos);

    KOSpell(QWidget *parent, const QString &caption,KOSpellConfig *kcs=0,
            bool modal = FALSE, bool _autocorrect =FALSE );
    KOSpell( KOSpellConfig *_ksc );

    bool endOfResponse;
    bool m_bIgnoreUpperWords;
    bool m_bIgnoreTitleCase;
    bool autoDelete;
    bool m_bNoMisspellingsEncountered;
    unsigned int totalpos;
    unsigned int lastline;
    unsigned int posinline;
    unsigned int lastlastline;
    unsigned int offset;
    KOSpellConfig *ksconfig;
    spellStatus m_status;
    int lastpos;
    bool modaldlg;
    bool autocorrect;
    QString caption;
    QString orig;
    QString origbuffer;
    QString newbuffer;
    QString cwword;
    QString dlgorigword;
    QString dlgreplacement;
    QStringList ignorelist;
    QStringList replacelist;
    QWidget *parent;
    KOSpellDlg *ksdlg;
    QStringList *wordlist;
    QStringList::Iterator wlIt;
    QStringList sugg;
    QTextCodec* codec;
    int dlgresult;
};
#endif
