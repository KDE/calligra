/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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

#include <kinstance.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kiconloader.h>

#include <graphitepart.h>
#include <graphitefactory.h>

extern "C" {
    void* init_libgraphitepart() {
        return new GraphiteFactory;
    }
};

static const char *description=I18N_NOOP("graphite - Scientific Graphs");
static const char *version="0.1";

KInstance *GraphiteFactory::s_global=0;
KAboutData *GraphiteFactory::s_aboutData=0;

GraphiteFactory::GraphiteFactory(QObject *parent, const char *name)
                                : KoFactory(parent, name) {
    global();
}

GraphiteFactory::~GraphiteFactory() {

    delete s_aboutData;
    s_aboutData=0;
    delete s_global;
    s_global=0;
}

KParts::Part* GraphiteFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & ) {

    bool bWantKoDocument=(strcmp(classname, "KoDocument")==0);

    GraphitePart *part = new GraphitePart(parentWidget, widgetName, parent, name, !bWantKoDocument);

    if (!bWantKoDocument)
        part->setReadWrite(false);

    emit objectCreated(part);
    return part;
}

KAboutData* GraphiteFactory::aboutData() {

    if(!s_aboutData) {
        s_aboutData=new KAboutData("graphite", I18N_NOOP("graphite"),
                                   version, description, KAboutData::License_GPL,
                                   "(c) 2000, Werner Trobin");
        s_aboutData->addAuthor("Werner Trobin", 0, "trobin@kde.org");
    }
    return s_aboutData;
}

KInstance *GraphiteFactory::global() {

    if (!s_global) {
        s_global=new KInstance(aboutData());
        // Tell the iconloader about share/apps/koffice/icons
        s_global->iconLoader()->addAppDir(QString::fromLatin1("koffice"));
    }
    return s_global;
}

#include <graphitefactory.moc>
