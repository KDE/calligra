/* This file is part of the KDE libraries
   Copyright (C) 1997 David Sweet <dsweet@kde.org>

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
#ifndef __KSPELL_H__
#define __KSPELL_H__

#include "ksconfig.h"

#include <qobject.h>
#include <qstringlist.h>
#include <qstring.h>

class QTextCodec;
class KProcIO;
class KProcess;

/**
 * KDE Spellchecker
 *
 * A KDE programmer's interface to International ISpell 3.1.  (GPL 1997)
 * A static method, @ref modalCheck() is provided for convenient
 *  access to the spellchecker.
 *
 * @author David Sweet <dsweet@kde.org>
 * @version $Id$
 * @see KSpellConfig
 */

class KoSpell : public QObject
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

	enum spellStatus
	{
		Starting = 0,
		Running,
		Cleaning,
		Finished,
		Error,
		Crashed
	};

	enum Spelling
	{
		SpellingOk,
		SpellingIgnore,
		Misspelled,
		SpellingError,
		SpellingDone
	};

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
	* @param progressbar Indicates if progress bar should be shown.
	* @param modal       Indicates modal or non-modal dialog.
	**/

	KoSpell(QWidget *parent, QObject *receiver, const char *slot, KSpellConfig *kcs=0);

	/**
	* Returns the status of KSpell.
	*
	* @see spellStatus()
	**/
	spellStatus status() const { return m_status; }

	/**
	* Cleans up ISpell.
	*
	* Write out the personal dictionary and close ISpell's
	*  stdin.  A @ref death() signal will be emitted when the cleanup is
	*  complete, but this method will return immediately.
	**/
	virtual void cleanUp ();

	/**
	* Spellchecks a buffer of many words in plain text
	* format.
	*
	* The @p _buffer is not modified.  The signal @ref done() will be
	* sent when @ref check() is finished and the argument will be a
	* spell-corrected version of @p _buffer.
	*
	* The spell check may be stopped by the user before the entire buffer
	* has been checked.  You can check @ref lastPosition() to see how far
	* in @p _buffer @ref check() reached before stopping.
	**/

	virtual bool check(const QString &buffer);

	Spelling parseLine(const QString &line, QString &word, int &pos);

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
	KSpellConfig ksConfig () const;

	/**
	* The destructor instructs ISpell/ASpell to write out the personal
	*  dictionary and then terminates ISpell/ASpell.
	*/
	virtual ~KoSpell();

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
	void misspelling (const QString & originalword, int pos);

	/**
	* Emitted after KSpell has verified that ISpell/ASpell is running
	* and working properly.
	*/
	void ready(KoSpell *);

	void done();

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
	void KoSpell2(KProcIO *);
	void check2 (KProcIO *);
	void ispellExit (KProcess *);
	void emitDeath();
	void ispellErrors (KProcess *, char *, int);

protected:
	QStringList	m_buffer;
        QStringList ignorelist;
	int trystart;
	int maxtrystart;
	KProcIO *proc;
	QWidget *parent;
	KSpellConfig *ksconfig;
	QTextCodec* codec;

	spellStatus m_status;

	QString funnyWord (const QString & word);

	void startIspell();
	bool writePersonalDictionary ();

	private:
	class KoSpellPrivate;
	KoSpellPrivate *d;
};

#endif
