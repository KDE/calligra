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

#ifndef KOBGSPELLCHECK_H
#define KOBGSPELLCHECK_H

/// OK including config.h in public headers is bad practice - to be removed once kspell2 is required
#include <config.h>

#ifdef HAVE_LIBKSPELL2

#include <kspell2/broker.h>
#include <qobject.h>
#include <qstringlist.h>

class KoTextObject;
class KoDocument;
class KoTextParag;
class KoTextIterator;

class KDE_EXPORT KoBgSpellCheck : public QObject
{
    Q_OBJECT
public:
    KoBgSpellCheck( const KSpell2::Broker::Ptr& broker, QObject *parent =0,
                    const char *name =0 );
    virtual ~KoBgSpellCheck();

    void registerNewTextObject( KoTextObject *object );

    virtual KoTextIterator *createWholeDocIterator() const=0;

    bool enabled() const;
public slots:
    void start();
    void stop();
    void setEnabled( bool b );

protected slots:
    void spellCheckerMisspelling(const QString &, int );
    void spellCheckerDone();
    void checkerContinue();

    void slotParagraphCreated( KoTextParag* parag );
    void slotParagraphModified( KoTextParag* parag, int /*ParagModifyType*/, int pos, int length );
    void slotParagraphDeleted( KoTextParag* parag );

    void slotClearPara();
protected:
    void markWord( const QString &old, int pos, bool misspelled );
private:
    class Private;
    Private *d;
};
#endif
#endif
