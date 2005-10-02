/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2002 Laurent Montel <lmontel@mandrakesoft.com>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "main.h"
#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kinstance.h>
#include <kconfig.h>
#include <kgenericfactory.h>
#include <klibloader.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/***************************************************
 *
 * Factory
 *
 ***************************************************/

K_EXPORT_COMPONENT_FACTORY( libkspelltool, KGenericFactory<SpellChecker> )

/***************************************************
 *
 * Spellchecker
 *
 ***************************************************/

SpellChecker::SpellChecker( QObject* parent, const char* name, const QStringList & )
    : KDataTool( parent, name )
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
	kdDebug(31000) << "SpellChecker only accepts mimetype text/plain and application/x-singleword" << endl;
	return FALSE;
    }

    // Get data
    QString buffer = *((QString *)data);
    buffer = buffer.stripWhiteSpace();

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
#if 0
            kosconfig.setNoRootAffix(config->readNumEntry ("KSpell_NoRootAffix", 0));
            kosconfig.setRunTogether(config->readNumEntry ("KSpell_RunTogether", 0));
            kosconfig.setDictionary(config->readEntry ("KSpell_Dictionary", ""));
            kosconfig.setDictFromList(config->readNumEntry ("KSpell_DictFromList", FALSE));
            kosconfig.setEncoding(config->readNumEntry ("KSpell_Encoding", KOS_E_ASCII));
            kosconfig.setClient(config->readNumEntry ("KSpell_Client", KOS_CLIENT_ISPELL));
            kosconfig.setNoRootAffix(config->readNumEntry ("KSpell_NoRootAffix", 0));
            kosconfig.setRunTogether(config->readNumEntry ("KSpell_RunTogether", 0));
            kosconfig.setDictionary(config->readEntry ("KSpell_Dictionary", ""));
            kosconfig.setDictFromList(config->readNumEntry ("KSpell_DictFromList", FALSE));

            kosconfig.setIgnoreCase( config->readNumEntry( "KSpell_IgnoreCase", 0));
            kosconfig.setIgnoreAccent( config->readNumEntry( "KSpell_IgnoreAccent", 0));
            kosconfig.setDontCheckUpperWord(config->readBoolEntry("KSpell_dont_check_upper_word",false));
            kosconfig.setDontCheckTitleCase(config->readBoolEntry("KSpell_dont_check_title_case",false));
            kosconfig.setSpellWordWithNumber( config->readNumEntry("KSpell_SpellWordWithNumber", false));
#endif
        }
    }
#if 0 //PORT to kspell2
    // Call the spell checker
    KOSpell::modalCheck( buffer, &kosconfig );
    *((QString*)data) = buffer;
#endif
#if 0 //fixme
    // Call the spell checker
    KSpell::spellStatus status=(KSpell::spellStatus)KSpell::modalCheck( buffer, &ksconfig );

    if (status == KSpell::Error)
    {
        KMessageBox::sorry(0L, i18n("KSpell could not be started.\n"
                                    "Please make sure you have ISpell or ASpell properly configured and in your PATH."));
    }
    else if (status == KSpell::Crashed)
    {
        KMessageBox::sorry(0L, i18n("KSpell seems to have crashed."));
    }
    else
    {
        // Set data
        *((QString*)data) = buffer;
    }
#endif
    return TRUE;
}

#include "main.moc"
