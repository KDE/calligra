/* This file is part of the KDE project
   Copyright (C) 2002 David Faure <david@mandrakesoft.com>
                 2002 Laurent Montel <lmontel@mandrakesoft.com>

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


#include "koBgSpellCheck.h"
#include "koBgSpellCheck.moc"
#include <qtimer.h>
#include <kdebug.h>
#include <kospell.h>
#include <ksconfig.h>
#include <kotextobject.h>
#include <klocale.h>

//#define DEBUG_BGSPELLCHECKING

KoBgSpellCheck::KoBgSpellCheck()
{
    m_pKSpellConfig=0L;
    m_bgSpell.kspell=0L;
    m_bDontCheckUpperWord=false;
    m_bSpellCheckEnabled=false;
    m_bDontCheckTitleCase=false;
    m_bgSpell.currentTextObj=0L;
}

KoBgSpellCheck::~KoBgSpellCheck()
{
    delete m_bgSpell.kspell;
    delete m_pKSpellConfig;
}

void KoBgSpellCheck::enableBackgroundSpellCheck( bool b )
{
    m_bSpellCheckEnabled=b;
    startBackgroundSpellCheck(); // will enable or disable
}

void KoBgSpellCheck::setIgnoreUpperWords( bool b)
{
    stopSpellChecking();
    m_bDontCheckUpperWord = b;
    startBackgroundSpellCheck();
}

void KoBgSpellCheck::setIgnoreTitleCase( bool b)
{
    stopSpellChecking();
    m_bDontCheckTitleCase = b;
    startBackgroundSpellCheck();
}

void KoBgSpellCheck::addIgnoreWordAll( const QString & word)
{
    if( m_spellListIgnoreAll.findIndex( word )==-1)
        m_spellListIgnoreAll.append( word );
    stopSpellChecking();
    spellConfig()->setIgnoreList( m_spellListIgnoreAll );
    startBackgroundSpellCheck();
}

void KoBgSpellCheck::addIgnoreWordAllList( const QStringList & list)
{
    m_spellListIgnoreAll.clear();
    stopSpellChecking();
    spellConfig()->setIgnoreList( list );
    startBackgroundSpellCheck();
}

void KoBgSpellCheck::clearIgnoreWordAll( )
{
    m_spellListIgnoreAll.clear();
    stopSpellChecking();
    spellConfig()->setIgnoreList( m_spellListIgnoreAll );
    startBackgroundSpellCheck();
}

void KoBgSpellCheck::startBackgroundSpellCheck()
{
    if ( !m_bSpellCheckEnabled || !m_bgSpell.currentTextObj )
        return;
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug() << "KoBgSpellCheck::startBackgroundSpellCheck" << endl;
#endif

    m_bgSpell.currentParag = m_bgSpell.currentTextObj->textDocument()->firstParag();
    nextParagraphNeedingCheck();
    //kdDebug() << "fs=" << m_bgSpell.currentTextObj << " parag=" << m_bgSpell.currentParag << endl;

    if ( !m_bgSpell.currentTextObj || !m_bgSpell.currentParag ) {
        // Might be better to launch again upon document modification (key, pasting, etc.) instead of right now
        //kdDebug() << "KWDocument::startBackgroundSpellCheck nothing to check this time." << endl;
        QTimer::singleShot( 1000, this, SLOT( startBackgroundSpellCheck() ) );
        return;
    }

    bool needsWait = false;
    if ( !m_bgSpell.kspell ) // reuse if existing
    {
        m_bgSpell.kspell = new KoSpell(0L, this, SLOT( spellCheckerReady() ), m_pKSpellConfig );

        needsWait = true; // need to wait for ready()
        connect( m_bgSpell.kspell, SIGNAL( death() ),
                 this, SLOT( spellCheckerFinished() ) );
        connect( m_bgSpell.kspell, SIGNAL( misspelling( const QString &, int ) ),
                 this, SLOT( spellCheckerMisspelling( const QString &, int ) ) );
        connect( m_bgSpell.kspell, SIGNAL( done() ),
                 this, SLOT( spellCheckerDone() ) );
    }
    m_bgSpell.kspell->setIgnoreUpperWords( m_bDontCheckUpperWord );
    m_bgSpell.kspell->setIgnoreTitleCase( m_bDontCheckTitleCase );
    // TODO 'ignore list' stuff

    if ( !needsWait )
        spellCheckerReady();
}

void KoBgSpellCheck::spellCheckerReady()
{
    //kdDebug() << "KWDocument::spellCheckerReady" << endl;
    QTimer::singleShot( 10, this, SLOT( spellCheckNextParagraph() ) );
}

// Input: currentTextObj non-null, and currentParag set to the last parag checked
// Output: currentTextObj+currentParag set to next parag to check. Both 0 if end.
void KoBgSpellCheck::nextParagraphNeedingCheck()
{
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug() << "KoBgSpellCheck::nextParagraphNeedingCheck" <<m_bgSpell.currentTextObj <<endl;
#endif
    if ( !m_bgSpell.currentTextObj ) {
        m_bgSpell.currentParag = 0L;
        return;
    }
    KoTextParag* parag = m_bgSpell.currentParag;
    if ( parag && parag->next() )
        parag = parag->next();
    // Skip any unchanged parags
    while ( parag && !parag->string()->needsSpellCheck() )
        parag = parag->next();
    while ( parag && parag->length() <= 1 ) // empty parag
    {
        parag->string()->setNeedsSpellCheck( false ); // nothing to check
        while ( parag && !parag->string()->needsSpellCheck() ) // keep looking
            parag = parag->next();
    }
    if ( parag )
        m_bgSpell.currentParag = parag;
    else
        m_bgSpell.currentParag = 0L; // ###

    if( !m_bgSpell.currentParag)
    {
        KoTextObject *obj=m_bgSpell.currentTextObj;
        m_bgSpell.currentTextObj=nextTextObject( m_bgSpell.currentTextObj );
        if ( m_bgSpell.currentTextObj && m_bgSpell.currentTextObj!=obj)
            m_bgSpell.currentParag = m_bgSpell.currentTextObj->textDocument()->firstParag();
        else
            m_bgSpell.currentParag = 0L;
    }
}

void KoBgSpellCheck::spellCheckNextParagraph()
{
    // TODO handle deletion of paragraphs.... signal from kotextobjects?
    //kdDebug() << "KoBgSpellCheck::spellCheckNextParagraph" << endl;

    nextParagraphNeedingCheck();
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug() << "fs=" << m_bgSpell.currentTextObj << " parag=" << m_bgSpell.currentParag << endl;
#endif
    if ( !m_bgSpell.currentTextObj || !m_bgSpell.currentParag )
    {
#ifdef DEBUG_BGSPELLCHECKING
        kdDebug() << "KoBgSpellCheck::spellCheckNextParagraph scheduling restart" << endl;
#endif
        // We arrived to the end of the paragraphs. Jump to startBackgroundSpellCheck,
        // it will check if we still have something to do.
        QTimer::singleShot( 100, this, SLOT( startBackgroundSpellCheck() ));
        return;
    }
    // First remove any misspelled format from the paragraph
    // - otherwise we'd never notice words being ok again :)
    KoTextStringChar *ch = m_bgSpell.currentParag->at( 0 );
    KoTextFormat format( *ch->format() );
    format.setMisspelled( false );
    m_bgSpell.currentParag->setFormat( 0, m_bgSpell.currentParag->length()-1, &format, true, KoTextFormat::Misspelled );
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug() << "KoBgSpellCheck::spellCheckNextParagraph spell checking parag " << m_bgSpell.currentParag->paragId() << endl;
#endif
    // Now spell-check that paragraph
    QString text = m_bgSpell.currentParag->string()->toString();
    text.remove( text.length() - 1, 1 ); // trailing space
    m_bgSpell.kspell->check(text);
}

void KoBgSpellCheck::spellCheckerMisspelling(const QString &old, int pos )
{
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug() << "KoBgSpellCheck::spellCheckerMisspelling old=" << old << " pos=" << pos << endl;
#endif
    KoTextObject * fs = m_bgSpell.currentTextObj;
    Q_ASSERT( fs );
    if ( !fs ) return;
    KoTextParag* parag = m_bgSpell.currentParag;
    if ( !parag ) return;
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug() << "KoBgSpellCheck::spellCheckerMisspelling parag=" << parag << " (id=" << parag->paragId() << ", length=" << parag->length() << ") pos=" << pos << " length=" << old.length() << endl;
#endif
    KoTextStringChar *ch = parag->at( pos );
    KoTextFormat format( *ch->format() );
    format.setMisspelled( true );
    parag->setFormat( pos, old.length(), &format, true, KoTextFormat::Misspelled );
    parag->setChanged( true );
    // TODO delay this, so that repaints are 'compressed'
    slotRepaintChanged( m_bgSpell.currentTextObj );
}

void KoBgSpellCheck::spellCheckerDone()
{
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug() << "KoBgSpellCheck::spellCheckerDone" << endl;
#endif
    if(m_bgSpell.currentParag)
        m_bgSpell.currentParag->string()->setNeedsSpellCheck( false );
    if( m_bgSpell.currentTextObj)
        m_bgSpell.currentTextObj->setNeedSpellCheck(false);
    // Done checking the current paragraph, schedule the next one
    QTimer::singleShot( 10, this, SLOT( spellCheckNextParagraph() ) );
}

void KoBgSpellCheck::spellCheckerFinished()
{
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug() << "--- KoBgSpellCheck::spellCheckerFinished ---" << endl;
#endif
    KoSpell::spellStatus status = m_bgSpell.kspell->status();
    delete m_bgSpell.kspell;
    m_bgSpell.kspell = 0;
    m_bgSpell.currentParag = 0;
    m_bgSpell.currentTextObj = 0;
    if (status == KoSpell::Error)
    {
        // KSpell badly configured... what to do?
        kdWarning() << "ISpell/ASpell not configured correctly." << endl;
        return;
    }
    else if (status == KoSpell::Crashed)
    {
        kdWarning() << "ISpell/ASpell seems to have crashed." << endl;
        return;
    }
    // Normal death - nothing to do
}

KSpellConfig* KoBgSpellCheck::spellConfig()
{
  if ( !m_pKSpellConfig )
    m_pKSpellConfig = new KSpellConfig();
  return m_pKSpellConfig;
}

void KoBgSpellCheck::setKSpellConfig(KSpellConfig _kspell)
{
  (void)spellConfig();
  stopSpellChecking();

  m_pKSpellConfig->setNoRootAffix(_kspell.noRootAffix ());
  m_pKSpellConfig->setRunTogether(_kspell.runTogether ());
  m_pKSpellConfig->setDictionary(_kspell.dictionary ());
  m_pKSpellConfig->setDictFromList(_kspell.dictFromList());
  m_pKSpellConfig->setEncoding(_kspell.encoding());
  m_pKSpellConfig->setClient(_kspell.client());
  startBackgroundSpellCheck();
}

void KoBgSpellCheck::stopSpellChecking()
{
  delete m_bgSpell.kspell;
  m_bgSpell.kspell = 0;
  m_bgSpell.currentParag = 0;
  m_bgSpell.currentTextObj = 0;
}
