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
#include <koSconfig.h>
#include <ksconfig.h>
#include <kotextobject.h>
#include <klocale.h>

//#define DEBUG_BGSPELLCHECKING

class KoBgSpellCheck::KoBgSpellCheckPrivate
{
public:
    KSpellConfig * m_pKSpellConfig;
    QTimer * startTimer;
    QTimer * nextParagraphTimer;
};

KoBgSpellCheck::KoBgSpellCheck()
{
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "KoBgSpellCheck::KoBgSpellCheck " << this << endl;
#endif
    d = new KoBgSpellCheck::KoBgSpellCheckPrivate;
    d->m_pKSpellConfig=0L;
    d->startTimer = new QTimer( this );
    connect( d->startTimer, SIGNAL( timeout() ),
             this, SLOT( startBackgroundSpellCheck() ) );
    d->nextParagraphTimer = new QTimer( this );
    connect( d->nextParagraphTimer, SIGNAL( timeout() ),
             this, SLOT( spellCheckNextParagraph() ) );

    m_bgSpell.kspell=0L;
    m_bDontCheckUpperWord=false;
    m_bSpellCheckEnabled=false;
    m_bDontCheckTitleCase=false;
    m_bSpellCheckConfigure=false;
    m_bgSpell.currentTextObj=0L;
    m_bgSpell.needsRepaint=false;
}

KoBgSpellCheck::~KoBgSpellCheck()
{
    delete m_bgSpell.kspell;
    delete d->m_pKSpellConfig;
    delete d;
}

void KoBgSpellCheck::addPersonalDictonary( const QString & word )
{
    if ( m_bgSpell.kspell )
    {
        m_bgSpell.kspell->addPersonal ( word);
    }
}

void KoBgSpellCheck::spellCheckParagraphDeleted( KoTextParag *_parag,  KoTextObject *obj)
{
    if ( m_bgSpell.currentTextObj == obj && m_bgSpell.currentParag == _parag)
    {
        stopSpellChecking();
        startBackgroundSpellCheck();
    }
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
    d->startTimer->stop(); // In case we were called directly, while the timer was running.
    if ( !m_bSpellCheckEnabled )
        return;
    //re-test text obj
    if ( !m_bgSpell.currentTextObj )
    {
        m_bgSpell.currentTextObj = nextTextObject(m_bgSpell.currentTextObj );
    }
    if ( !m_bgSpell.currentTextObj )
    {
#ifdef DEBUG_BGSPELLCHECKING
        //kdDebug(32500) << "KoBgSpellCheck::startBackgroundSpellCheck no currentTextObj to check this time." << endl;
#endif
        d->startTimer->start( 1000, true );
        return;
    }
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "KoBgSpellCheck::startBackgroundSpellCheck" << endl;
#endif

    m_bgSpell.currentParag = m_bgSpell.currentTextObj->textDocument()->firstParag();
    nextParagraphNeedingCheck();

#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "fs=" << m_bgSpell.currentTextObj << " parag=" << m_bgSpell.currentParag << endl;
#endif

    if ( !m_bgSpell.currentTextObj || !m_bgSpell.currentParag ) {
        if ( m_bgSpell.currentTextObj )
        {
            if ( (m_bgSpell.currentTextObj->textDocument()->firstParag() == m_bgSpell.currentTextObj->textDocument()->lastParag()) && m_bgSpell.currentTextObj->textDocument()->firstParag()->length() <= 1)
                m_bgSpell.currentTextObj->setNeedSpellCheck(false);
        }
        // Might be better to launch again upon document modification (key, pasting, etc.) instead of right now
#ifdef DEBUG_BGSPELLCHECKING
        kdDebug(32500) << "KoBgSpellCheck::startBackgroundSpellCheck nothing to check this time." << endl;
#endif
        d->startTimer->start( 1000, true );
        return;
    }

    bool needsWait = false;
    if ( !m_bgSpell.kspell ) // reuse if existing
    {
        m_bgSpell.kspell = new KoSpell(0L, this, SLOT( spellCheckerReady() ), d->m_pKSpellConfig );

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
    if ( !needsWait )
        spellCheckerReady();
}

void KoBgSpellCheck::spellCheckerReady()
{
    //necessary to restart to beginning otherwise we don't check
    //other parag
    if (m_bgSpell.currentTextObj)
        m_bgSpell.currentParag = m_bgSpell.currentTextObj->textDocument()->firstParag();

#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "KoBgSpellCheck::spellCheckerReady textobj=" << m_bgSpell.currentTextObj << endl;
#endif
    d->nextParagraphTimer->start( 10, true );
}

// Input: currentTextObj non-null, and currentParag set to the last parag checked
// Output: currentTextObj+currentParag set to next parag to check. Both 0 if end.
void KoBgSpellCheck::nextParagraphNeedingCheck()
{
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "KoBgSpellCheck::nextParagraphNeedingCheck textobj=" <<m_bgSpell.currentTextObj <<endl;
#endif
    if ( !m_bgSpell.currentTextObj ) {
        m_bgSpell.currentParag = 0L;
        return;
    }

    // repaint the textObject here if it requires it
    // (perhaps there should be a way to repaint just a paragraph.... - JJ)
    // Well repaintChanged looks for the changed flag in parags and repaints only those - DF
    if(m_bgSpell.needsRepaint)
    {
         slotRepaintChanged( m_bgSpell.currentTextObj );
         m_bgSpell.needsRepaint=false;
    }

    KoTextParag* parag = m_bgSpell.currentParag;
    if ( parag && parag->string() && parag->string()->needsSpellCheck() )
    {
#ifdef DEBUG_BGSPELLCHECKING
        kdDebug(32500) << "current parag " << parag << " needs checking again." <<endl;
#endif
        return;
    }

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
    // Still not found? Check from the start up to where we started from
    if ( !parag ) {
        parag = m_bgSpell.currentTextObj->textDocument()->firstParag();
        while ( parag != m_bgSpell.currentParag && !parag->string()->needsSpellCheck() )
            parag = parag->next();
        while ( parag != m_bgSpell.currentParag && parag->length() <= 1 ) // empty parag
        {
            parag->string()->setNeedsSpellCheck( false ); // nothing to check
            while ( parag != m_bgSpell.currentParag && !parag->string()->needsSpellCheck() ) // keep looking
                parag = parag->next();
        }
       if ( parag == m_bgSpell.currentParag && !parag->string()->needsSpellCheck() )
           parag = 0; // wrapped around and found nothing to check
    }

    if ( parag )
        m_bgSpell.currentParag = parag;
    else
    {
        KoTextObject *obj = m_bgSpell.currentTextObj;
        // OK, nothing more to do in this textobj
        obj->setNeedSpellCheck(false);

        m_bgSpell.currentTextObj = nextTextObject( m_bgSpell.currentTextObj );
        //kdDebug(32500)<<" m_bgSpell.currentTextObj="<<m_bgSpell.currentTextObj<<endl;
        if ( m_bgSpell.currentTextObj && m_bgSpell.currentTextObj != obj)
        {
            m_bgSpell.currentParag = m_bgSpell.currentTextObj->textDocument()->firstParag();
        }
        else
        {
            if ( m_bgSpell.currentTextObj )
                m_bgSpell.currentTextObj->setNeedSpellCheck( false );
            m_bgSpell.currentParag = 0L;
        }
    }
    //kdDebug(32500)<<" KoBgSpellCheck::nextParagraphNeedingCheck() : m_bgSpell.currentParag :"<<m_bgSpell.currentParag<<endl;

}

void KoBgSpellCheck::spellCheckNextParagraph()
{
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "KoBgSpellCheck::spellCheckNextParagraph" << endl;
#endif

    nextParagraphNeedingCheck();
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "textobj=" << m_bgSpell.currentTextObj << " parag=" << m_bgSpell.currentParag << endl;
#endif
    if ( !m_bgSpell.currentTextObj || !m_bgSpell.currentParag )
    {
#ifdef DEBUG_BGSPELLCHECKING
        kdDebug(32500) << "KoBgSpellCheck::spellCheckNextParagraph scheduling restart" << endl;
#endif
        // We arrived to the end of the paragraphs. Jump to startBackgroundSpellCheck,
        // it will check if we still have something to do.
        d->startTimer->start( 100, true );
        return;
    }
    // First remove any misspelled format from the paragraph
    // - otherwise we'd never notice words being ok again :)
    KoTextStringChar *ch = m_bgSpell.currentParag->at( 0 );
    KoTextFormat format( *ch->format() );
    format.setMisspelled( false );
    m_bgSpell.currentParag->setFormat( 0, m_bgSpell.currentParag->length()-1, &format, true, KoTextFormat::Misspelled );
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "KoBgSpellCheck::spellCheckNextParagraph spell checking parag " << m_bgSpell.currentParag->paragId() << endl;
#endif

    // Get the text to spell-check
    QString text = m_bgSpell.currentParag->string()->toString();
    text.remove( text.length() - 1, 1 ); // trailing space

    // Mark it as "we've read the text to be spell-checked", *before* doing it.
    // This prevents race conditions: if the user modifies the text during
    // the spellchecking, the new text _will_ be checked, since the bool will
    // be set to true.
    m_bgSpell.currentParag->string()->setNeedsSpellCheck( false );

    // Now spell-check that paragraph
    m_bgSpell.kspell->check(text);
}

void KoBgSpellCheck::spellCheckerMisspelling(const QString &old, int pos )
{
    KoTextObject * textobj = m_bgSpell.currentTextObj;
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "KoBgSpellCheck::spellCheckerMisspelling textobj=" << textobj << " old=" << old << " pos=" << pos << endl;
#endif
    Q_ASSERT( textobj );
    if ( !textobj ) return;
    KoTextParag* parag = m_bgSpell.currentParag;
    if ( !parag ) return;
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "KoBgSpellCheck::spellCheckerMisspelling parag=" << parag << " (id=" << parag->paragId() << ", length=" << parag->length() << ") pos=" << pos << " length=" << old.length() << endl;
#endif
    KoTextStringChar *ch = parag->at( pos );
    KoTextFormat format( *ch->format() );
    format.setMisspelled( true );
    parag->setFormat( pos, old.length(), &format, true, KoTextFormat::Misspelled );
}

void KoBgSpellCheck::spellCheckerDone()
{
    // Set the repaint flags. This needs to be done even if no misspelling was found,
    // so that words added to the dictionary become ok again (#56506,#57357)
    m_bgSpell.currentParag->setChanged( true );
    m_bgSpell.needsRepaint=true;

#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "KoBgSpellCheck::spellCheckerDone" << endl;
#endif
    // Done checking the current paragraph, schedule the next one
    d->nextParagraphTimer->start( 10, true );
}

void KoBgSpellCheck::spellCheckerFinished()
{
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "--- KoBgSpellCheck::spellCheckerFinished ---" << endl;
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
        if ( !m_bSpellCheckConfigure )
        {
            m_bSpellCheckConfigure=true;
            configurateSpellChecker();
        }
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
  if ( !d->m_pKSpellConfig )
    d->m_pKSpellConfig = new KSpellConfig();
  return d->m_pKSpellConfig;
}

void KoBgSpellCheck::setKSpellConfig(const KOSpellConfig &_kspell)
{
  (void)spellConfig();
  stopSpellChecking();

  d->m_pKSpellConfig->setNoRootAffix(_kspell.noRootAffix ());
  d->m_pKSpellConfig->setRunTogether(_kspell.runTogether ());
  d->m_pKSpellConfig->setDictionary(_kspell.dictionary ());
  d->m_pKSpellConfig->setDictFromList(_kspell.dictFromList());
  d->m_pKSpellConfig->setEncoding(_kspell.encoding());
  d->m_pKSpellConfig->setClient(_kspell.client());
  m_bSpellCheckConfigure = false;
  startBackgroundSpellCheck();
}

void KoBgSpellCheck::stopSpellChecking()
{
#ifdef DEBUG_BGSPELLCHECKING
  kdDebug(32500) << "KoBgSpellCheck::stopSpellChecking" << endl;
#endif
  delete m_bgSpell.kspell;
  m_bgSpell.kspell = 0;
  m_bgSpell.currentParag = 0;
  m_bgSpell.currentTextObj = 0;
  d->startTimer->stop();
  d->nextParagraphTimer->stop();
}
