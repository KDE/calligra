#include <kstddirs.h>
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
