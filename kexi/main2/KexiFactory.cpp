/* This file is part of the KDE project

   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005-2006 Sebastian Sauer <mail@dipe.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KexiFactory.h"
#include "KexiDocument.h"
#include "KexiAboutData.h"

#include <kdebug.h>
#include <kcomponentdata.h>
#include <kaboutdata.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

KComponentData* KexiFactory::s_global = 0;
KAboutData* KexiFactory::s_aboutData = 0;
KIconLoader* KexiFactory::s_iconLoader = 0;

KexiFactory::KexiFactory(QObject* parent, const char* name)
    : KoFactory(parent, name)
{
    (void)global();
}

KexiFactory::~KexiFactory()
{
    delete s_aboutData; s_aboutData=0;
    delete s_global; s_global = 0;
    delete s_iconLoader; s_iconLoader = 0;
}

KParts::Part* KexiFactory::createPartObject( QWidget *parentWidget, QObject* parent, const char* classname, const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );
    KexiDocument *doc = new KexiDocument(parentWidget, parent, ! bWantKoDocument);
    if( ! bWantKoDocument )
        doc->setReadWrite( false );
    return doc;
}

KAboutData* KexiFactory::aboutData()
{
    if( !s_aboutData )
        s_aboutData = newAboutData();
    return s_aboutData;
}

const KComponentData &KexiFactory::global()
{
    if ( !s_global ) {
        s_global = new KComponentData( aboutData() );
        //s_global->dirs()->addResourceType( "kspread_template", KStandardDirs::kde_default("data") + "kspread/templates/");
        //s_global->dirs()->addResourceType( "toolbar", KStandardDirs::kde_default("data") + "koffice/toolbar/");
        //s_global->dirs()->addResourceType( "functions", KStandardDirs::kde_default("data") + "kspread/functions/");
        //s_global->dirs()->addResourceType( "sheet-styles", KStandardDirs::kde_default("data") + "kspread/sheetstyles/");
    }
    return *s_global;
}

KIconLoader* KexiFactory::iconLoader()
{
    if( !s_iconLoader ) {
        s_iconLoader = new KIconLoader(global().componentName(), global()->dirs());
        s_iconLoader->addAppDir("koffice");
    }
    return s_iconLoader;
}

#include "KexiFactory.moc"
