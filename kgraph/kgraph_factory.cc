/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <wtrobin@carinthia.com>

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

#include <kgraph_part.h>
#include <kgraph_factory.h>
#include <kdebug.h>

extern "C" {
    void* init_libkgraph() {
        return new KGraphFactory;
    }
};

static const char *description=I18N_NOOP("KGraph - Graphs for Engineers");
static const char *version="0.1";

KInstance *KGraphFactory::s_global=0;

KGraphFactory::KGraphFactory(QObject *parent, const char *name)
                             : KLibFactory(parent, name) {
}

KGraphFactory::~KGraphFactory() {
    if (s_global)
        delete s_global;
}

QObject *KGraphFactory::create(QObject *parent, const char *name, const char *classname, const QStringList &) {

/*
    if (parent && !parent->inherits("KoDocument")) {
        kdDebug(37001) << "KGraphFactory: parent does not inherit KoDocument" << endl;
        return 0;
    }
*/

    bool bWantKoDocument=(strcmp(classname, "KoDocument")==0);

    KGraphPart *part = new KGraphPart(parent, name, !bWantKoDocument);

    if (!bWantKoDocument)
        part->setReadWrite(false);

    emit objectCreated(part);
    return part;
}

KAboutData* KGraphFactory::aboutData() {
        KAboutData *aboutData=new KAboutData("kgraph", I18N_NOOP("KGraph"),
                                             version, description, KAboutData::License_GPL,
                                             "(c) 2000, Werner Trobin");
        aboutData->addAuthor("Werner Trobin", 0, "wtrobin@carinthia.com");
        return aboutData;
}

KInstance *KGraphFactory::global() {
    if (!s_global) {
        s_global=new KInstance(aboutData());
    }
    return s_global;
}
#include <kgraph_factory.moc>
