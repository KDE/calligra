/* This file is part of the KDE project
   Copyright (C) 2004 Zack Rusin <zack@kde.org>

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_LIBKSPELL2

#include "kobgspellcheck.h"
#include "kobgspellcheck.moc"
#include "kotextparag.h"

#include "kospell.h"

#include "kotextobject.h"
#include "kotextdocument.h"


#include <kspell2/backgroundchecker.h>
#include <kspell2/broker.h>
#include <kspell2/dictionary.h>
#include <kspell2/settings.h>
#include <kspell2/filter.h>
using namespace KSpell2;

#include <klocale.h>
#include <kdebug.h>
#include <kdeversion.h>
#include <qtimer.h>
#include <qptrdict.h>

//#define DEBUG_BGSPELLCHECKING

class KoBgSpellCheck::Private
{
public:
    bool enabled;
    int marked;
    KoSpell *backSpeller;
    QPtrDict<KoTextParag> paragCache;
    bool startupChecking;
};

static const int delayAfterMarked = 10;

KoBgSpellCheck::KoBgSpellCheck( const Broker::Ptr& broker, QObject *parent,
                                const char *name )
    : QObject( parent, name )
{
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "KoBgSpellCheck::KoBgSpellCheck " << this << endl;
#endif
    d = new Private;
    d->enabled = false;
    d->startupChecking = false;
    d->marked = 0;
    d->backSpeller = new KoSpell( broker, this, "KoSpell" );

    connect( d->backSpeller, SIGNAL(misspelling(const QString&, int)),
             SLOT(spellCheckerMisspelling(const QString &, int )) );
    connect( d->backSpeller, SIGNAL(done()),
             SLOT(spellCheckerDone()) );
    connect( d->backSpeller, SIGNAL(aboutToFeedText()),
             SLOT(slotClearPara()) );
}

KoBgSpellCheck::~KoBgSpellCheck()
{
    delete d; d = 0;
}

void KoBgSpellCheck::registerNewTextObject( KoTextObject *obj )
{
    Q_ASSERT( obj );

    connect( obj, SIGNAL(paragraphCreated(KoTextParag*)),
             SLOT(slotParagraphCreated(KoTextParag*)) );
    connect( obj, SIGNAL(paragraphModified(KoTextParag*, int, int, int)),
             SLOT(slotParagraphModified(KoTextParag*, int, int, int)) );
    connect( obj, SIGNAL(paragraphDeleted(KoTextParag*)),
             SLOT(slotParagraphDeleted(KoTextParag*)) );
}

void KoBgSpellCheck::setEnabled( bool b )
{
    d->enabled = b;
    if ( b )
        start();
    else
        stop();
}

bool KoBgSpellCheck::enabled() const
{
    return d->enabled;
}

void KoBgSpellCheck::start()
{
    if ( !d->enabled )
        return;

    d->startupChecking = true;
    d->marked = 0;
    KoTextIterator *itr = createWholeDocIterator();
    d->backSpeller->check( itr );
    d->backSpeller->start();
}

void KoBgSpellCheck::spellCheckerMisspelling( const QString &old, int pos )
{
    KoTextParag* parag = d->backSpeller->currentParag();
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "KoBgSpellCheck::spellCheckerMisspelling parag=" << parag
                   << " (id=" << parag->paragId() << ", length="
                   << parag->length() << ") pos=" << pos << " length="
                   << old.length() << endl;
#endif
    markWord( parag, old, pos, true );
}

void KoBgSpellCheck::markWord( KoTextParag* parag, const QString &old, int pos, bool misspelled )
{
    if ( pos >= parag->length() ) {
        kdDebug() << k_funcinfo << pos << "is out of parag (length=" << parag->length() << ")" << endl;
        return;
    }

    KoTextStringChar *ch = parag->at( pos );
    KoTextFormat format( *ch->format() );
    format.setMisspelled( misspelled );
    //kdDebug() << k_funcinfo << "changing mark from " << pos << " to " << old.length() << " misspelled=" << misspelled << endl;
    parag->setFormat( pos, old.length(), &format, true, KoTextFormat::Misspelled );

    parag->setChanged( true );
    parag->document()->emitRepaintChanged();

    if ( d->startupChecking && d->marked > delayAfterMarked ) {
        d->marked = 0;
        QTimer::singleShot( 1000, this, SLOT(checkerContinue()) );
    } else {
        if ( d->startupChecking )
            ++d->marked;
        checkerContinue();
    }
}

void KoBgSpellCheck::checkerContinue()
{
    d->backSpeller->continueChecking();
}

void KoBgSpellCheck::spellCheckerDone()
{
    d->startupChecking = false;

    if ( d->paragCache.isEmpty() )
        return;

    QPtrDictIterator<KoTextParag> itr( d->paragCache );
    KoTextParag *parag = d->paragCache.take( itr.currentKey() );
#ifdef DEBUG_BGSPELLCHECKING
    kdDebug(32500) << "spellCheckerDone : " << parag << ", cache = "<< d->paragCache.count() <<endl;
#endif
    d->backSpeller->check( parag );
}

void KoBgSpellCheck::stop()
{
#ifdef DEBUG_BGSPELLCHECKING
  kdDebug(32500) << "KoBgSpellCheck::stopSpellChecking" << endl;
#endif
  d->backSpeller->stop();
}

void KoBgSpellCheck::slotParagraphCreated( KoTextParag* parag )
{
    if ( d->backSpeller->check( parag ) ) {
        d->paragCache.insert( parag, parag );
    }
}

void KoBgSpellCheck::slotParagraphModified( KoTextParag* parag, int /*ParagModifyType*/,
                                            int pos, int length )
{
    //kdDebug()<<"here 1 "<<endl;
    if ( d->backSpeller->checking() ) {
        d->paragCache.insert( parag, parag );
        return;
    }
    //kdDebug()<<"Para modified " << parag << " pos = "<<pos<<", length = "<< length <<endl;
#if KDE_VERSION > KDE_MAKE_VERSION(3,3,0)
    if ( length < 10 ) {
        QString str = parag->string()->stringToSpellCheck();
        /// ##### do we really need to create a Filter every time?
        Filter filter;
        filter.setBuffer( str );
        // pos - 1 wasn't enough for the case a splitting a word into two misspelled halves
        filter.setCurrentPosition( pos - 2 );
        filter.setSettings( d->backSpeller->settings() );

        for ( Word w = filter.nextWord(); !w.end; w = filter.nextWord() ) {
            bool misspelling = !d->backSpeller->checkWord( w.word );
            //kdDebug()<<"Word = \""<< w.word<< "\" , misspelled = "<<misspelling<<endl;
            markWord( parag, w.word, w.start, misspelling );
        }
#else
    if ( length < 3 ) {
        QString word;
        int start;
        bool misspelled = !d->backSpeller->checkWordInParagraph( parag, pos,
                                                                 word, start );
        markWord( parag, word, start, misspelled );
#endif
    } else {
        d->backSpeller->check( parag );
    }
}

void KoBgSpellCheck::slotParagraphDeleted( KoTextParag* parag )
{
    d->paragCache.take( parag );
    if ( parag == d->backSpeller->currentParag() )
        d->backSpeller->slotCurrentParagraphDeleted();
}

void KoBgSpellCheck::slotClearPara()
{
    KoTextParag *parag = d->backSpeller->currentParag();

    // We remove any misspelled format from the paragraph
    // - otherwise we'd never notice words being ok again :)
    KoTextStringChar *ch = parag->at( 0 );
    KoTextFormat format( *ch->format() );
    format.setMisspelled( false );
    parag->setFormat( 0, parag->length()-1, &format, true,
                      KoTextFormat::Misspelled );
}

KSpell2::Settings * KoBgSpellCheck::settings() const
{
    return d->backSpeller->settings();
}

#endif
