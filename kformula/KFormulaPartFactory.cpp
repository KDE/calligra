/* This file is part of the KDE project
   Copyright (C) 2006 Martin Pfeiffer <hubipete@gmx.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KFormulaPartFactory.h"

#include "KFormulaPartDocument.h"
#include "KFormulaAboutData.h"

#include <kinstance.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <klocale.h>

#include <QStringList>

K_EXPORT_COMPONENT_FACTORY( libkformulapart, KFormulaPartFactory )

KInstance* KFormulaPartFactory::s_global = 0;
KAboutData* KFormulaPartFactory::s_aboutData = 0;

KFormulaPartFactory::KFormulaPartFactory( QObject* parent ) : KoFactory( parent )
{
    // Create our instance, it becomes KGlobal::instance if the main app is KFormula
    (void)global();
}

KFormulaPartFactory::~KFormulaPartFactory()
{
    delete s_aboutData;
    s_aboutData=0;
    delete s_global;
    s_global = 0L;
}

KParts::Part* KFormulaPartFactory::createPartObject( QWidget *parentWidget,
                                                     QObject* parent,
                                                     const char* classname,
						     const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );
    KFormulaPartDocument *doc = new KFormulaPartDocument( parentWidget,
		                                          parent,
							  !bWantKoDocument );

    if ( !bWantKoDocument )
        doc->setReadWrite( false );

    return doc;
}

KInstance* KFormulaPartFactory::global()
{
    if ( !s_global )
    {
        s_global = new KInstance( aboutData() );
        s_global->dirs()->addResourceType( "toolbar",
                        KStandardDirs::kde_default("data") + "koffice/toolbar/");
        // Tell the iconloader about share/apps/koffice/icons*/
        s_global->iconLoader()->addAppDir("koffice");
    }
    return s_global;
}

KAboutData* KFormulaPartFactory::aboutData()
{
    if( !s_aboutData )
        s_aboutData = newKFormulaAboutData();
    return s_aboutData;
}

#include "KFormulaPartFactory.moc"
