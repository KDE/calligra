/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include "main.h"

#include <klocale.h>
#include <kspell.h>
#include <kdebug.h>

/***************************************************
 *
 * Factory
 *
 ***************************************************/

extern "C"
{
    void* init_libkspelltool()
    {
	return new SpellCheckerFactory;
    }
};

SpellCheckerFactory::SpellCheckerFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
}

SpellCheckerFactory::~SpellCheckerFactory()
{
}

QObject* SpellCheckerFactory::create( QObject* parent, const char* name, const char* /*classname*/, const QStringList & )
{
    SpellChecker *checker = new SpellChecker( parent, name );
    emit objectCreated(checker);
    return checker;
}

/***************************************************
 *
 * Spellchecker
 *
 ***************************************************/

SpellChecker::SpellChecker( QObject* parent, const char* name )
    : KoDataTool( parent, name )
{
}

bool SpellChecker::run( const QString& command, void* data, const QString& datatype, const QString& mimetype, KSpellConfig *_ksconf )
{
    if ( command != "spellcheck" )
    {
	kdDebug(31000) << "SpellChecker does only accept the command 'spellcheck'" << endl;
	kdDebug(31000) << "   The commands " << command << " is not accepted" << endl;
	return FALSE;
    }

    // Check wether we can accept the data
    if ( datatype != "QString" )
    {
	kdDebug(31000) << "SpellChecker does only accept datatype QString" << endl;
	return FALSE;
    }
    if ( mimetype != "text/plain" )
    {
	kdDebug(31000) << "SpellChecker does only accept mimetype text/plain" << endl;
	return FALSE;
    }

    // Get data
    QString buffer = *((QString*)data);
    buffer = buffer.stripWhiteSpace();

    // #### handle errors
    // Call the spell checker
    KSpell::modalCheck( buffer );

    // Set data
    *((QString*)data) = buffer;

    return TRUE;
}

#include "main.moc"
