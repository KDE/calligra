/* This file is part of the KDE project
  Copyright (C) 2002 Alexander Dymo <cloudtemple@mksat.net>

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

#include <kudesigner_factory.h>
#include <kudesigner_doc.h>
#include <kudesigner_aboutdata.h>
#include <kinstance.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdebug.h>
#include <kstandarddirs.h>

KInstance* KudesignerFactory::s_global = 0L;
KAboutData* KudesignerFactory::s_aboutData = 0L;

KudesignerFactory::KudesignerFactory( QObject* parent )
        : KoFactory( parent )
{
    global();
}

KudesignerFactory::~KudesignerFactory()
{
    delete s_aboutData;
    s_aboutData = 0L;
    delete s_global;
    s_global = 0L;
}

KParts::Part* KudesignerFactory::createPartObject( QWidget *parentWidget, QObject* parent, const char* classname, const QStringList & data )
{
    // If classname is "KoDocument", our host is a koffice application
    // otherwise, the host wants us as a simple part, so switch to readonly and single view.
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );
    // parentWidget is used by KoDocument for the "readonly+singleView" case.
    KudesignerDoc *part = new KudesignerDoc( parentWidget, parent, !bWantKoDocument );

    if ( !bWantKoDocument )
        part->setReadWrite( false );

    if ( bWantKoDocument && ( data.count() > 0 ) )
    {
        for ( QStringList::const_iterator it = data.begin();it != data.end();++it )
        {
            if ( ( *it ).startsWith( "plugin=" ) )
            {
                part->loadPlugin( ( *it ).right( ( *it ).length() - 7 ) );
            }
            else if ( ( *it ).startsWith( "forcePropertyEditorPosition=" ) )
            {
                QString tmp = ( *it ).right( ( *it ).length() - 28 ).upper();
                kDebug() << "forced property editor position: " << tmp << endl;
                part->setForcedPropertyEditorPosition( tmp == "LEFT" ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea );
            }
        }
    }

    return part;
}

KAboutData* KudesignerFactory::aboutData()
{
    if ( !s_aboutData )
        s_aboutData = newKudesignerAboutData();
    return s_aboutData;
}

KInstance* KudesignerFactory::global()
{
    if ( !s_global )
    {
        s_global = new KInstance( aboutData() );
        // Add any application-specific resource directories here
        s_global->dirs() ->addResourceType( "kudesigner_template",
                                            KStandardDirs::kde_default( "data" ) + "kudesigner/templates/" );

        // Tell the iconloader about share/apps/koffice/icons
        s_global->iconLoader() ->addAppDir( "koffice" );
    }
    return s_global;
}

#include <kudesigner_factory.moc>
