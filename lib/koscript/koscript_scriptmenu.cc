/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#include <kstandarddirs.h>
#include <kinstance.h>
#include <klocale.h>

#include "koscript_scriptmenu.h"
#include "kscript.h"

#include <kpopupmenu.h>
#include <kmessagebox.h>

KScriptMenu::KScriptMenu( const DCOPRef& ref, KInstance* instance, const QString& text, QObject* parent, const char* name )
    : KActionMenu( text, parent, name ), m_ref( ref ), m_instance( instance ), m_interpreter( 0 )
{
    m_actions.setAutoDelete( TRUE );
    m_filenames.setAutoDelete( TRUE );

    QStringList scripts = m_instance->dirs()->findAllResources( "scripts", "*", TRUE );
    QStringList::Iterator it = scripts.begin();
    for( ; it != scripts.end(); ++it )
    {
        QString file = *it;
        int pos = file.findRev( '.' );
        if ( pos != -1 )
            file = file.left( pos );

        QString name = file;
        pos = file.findRev( '/' );
        name = file.mid( pos + 1 );
        KAction* action = new KAction( name, 0, (QObject*)0, name.latin1() );
        m_actions.append( action );
        action->plug( popupMenu() );
        connect( action, SIGNAL( activated() ), this, SLOT( slotActivated() ) );

        m_filenames.insert( action, new QString( *it ) );
    }
}

KScriptMenu::~KScriptMenu()
{
    if ( m_interpreter )
        delete m_interpreter;
}

void KScriptMenu::slotActivated()
{
    QString* str = m_filenames[ (void*)sender() ];
    if ( !str )
        return;

    kdDebug() << "Running " << str << endl;

    if ( !m_interpreter )
        m_interpreter = new KSInterpreter();

    QStringList args;
    args.append( m_ref.app() );
    args.append( m_ref.object() );

    QString ex = m_interpreter->runScript( *str, args );
    if ( !ex.isEmpty() )
    {
        KMessageBox::error( 0, ex, i18n("KScript Error"));
    }
}

#include "koscript_scriptmenu.moc"
