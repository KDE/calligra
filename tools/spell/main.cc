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
#include <kmessagebox.h>
#include <klocale.h>
#include <kspell.h>
#include <kdebug.h>
#include <kinstance.h>
#include <kconfig.h>

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

bool SpellChecker::run( const QString& command, void* data, const QString& datatype, const QString& mimetype )
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
	kdDebug(31000) << "SpellChecker only accepts datatype QString" << endl;
	return FALSE;
    }

    if ( mimetype != "text/plain" && mimetype != "application/x-singleword" )
    {
	kdDebug(31000) << "SpellChecker only accepts mimetype text/plain" << endl;
	return FALSE;
    }

    // Get data
    QString buffer = *((QString *)data);
    buffer = buffer.stripWhiteSpace();

    // Read config
    KSpellConfig ksconfig;
    if ( instance() )
    {
        KConfig * config = instance()->config();
        QCString gn( "KSpell " );
        gn += instance()->instanceName(); // for compat reasons, and to avoid finding the group in kdeglobals (hmm...)
        QString groupName = QString::fromLatin1( gn );
        //kdDebug() << "Group: " << groupName << endl;
        if ( config->hasGroup( groupName ) )
        {
            //kdDebug() << "SpellChecker::run - group found -" << endl;
            config->setGroup( groupName );
            ksconfig.setNoRootAffix(config->readNumEntry ("KSpell_NoRootAffix", 0));
            ksconfig.setRunTogether(config->readNumEntry ("KSpell_RunTogether", 0));
            ksconfig.setDictionary(config->readEntry ("KSpell_Dictionary", ""));
            ksconfig.setDictFromList(config->readNumEntry ("KSpell_DictFromList", FALSE));
            ksconfig.setEncoding(config->readNumEntry ("KSpell_Encoding", KS_E_ASCII));
            ksconfig.setClient(config->readNumEntry ("KSpell_Client", KS_CLIENT_ISPELL));
        }
    }

    // Call the spell checker
    KSpell::spellStatus status=(KSpell::spellStatus)KSpell::modalCheck( buffer, &ksconfig );

    if (status == KSpell::Error)
    {
        KMessageBox::sorry(0L, i18n("ISpell could not be started.\n"
                                    "Please make sure you have ISpell properly configured and in your PATH."));
    }
    else if (status == KSpell::Crashed)
    {
        KMessageBox::sorry(0L, i18n("ISpell seems to have crashed."));
    }
    else
    {
        // Set data
        *((QString*)data) = buffer;
    }
    return TRUE;
}

#include "main.moc"
