/* This file is part of the KDE project
   Copyright (C) 2009, 2011, 2012 Dag Andersen <danders@get2net.dk>

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
   Boston, MA 02110-1301, USA.
*/

#include "kplatoimport.h"

#include <kptmaindocument.h>
#include <kpttask.h>
#include <kptnode.h>
#include <kptresource.h>
#include <kptdocuments.h>
#include "kptdebug.h"

#include <QTextCodec>
#include <QByteArray>
#include <QString>
#include <QTextStream>
#include <QFile>

#include <kpluginfactory.h>
#include <kurl.h>

#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoDocument.h>


using namespace KPlato;

K_PLUGIN_FACTORY_WITH_JSON(KPlatoImportFactory, "plan_kplato_import.json", registerPlugin<KPlatoImport>();)
//K_EXPORT_PLUGIN(KPlatoImportFactory("calligrafilters"))

KPlatoImport::KPlatoImport(QObject* parent, const QVariantList &)
    : KoFilter(parent)
{
}

KoFilter::ConversionStatus KPlatoImport::convert(const QByteArray& from, const QByteArray& to)
{
    kDebug(planDbg()) << from << to;
    if ( ( from != "application/x-vnd.kde.kplato" ) || ( to != "application/x-vnd.kde.plan" ) ) {
        return KoFilter::NotImplemented;
    }
    KoDocument *part = 0;
    bool batch = false;
    if ( m_chain->manager() ) {
        batch = m_chain->manager()->getBatchMode();
    }
    if (batch) {
        //TODO
        kDebug(planDbg()) << "batch";
    } else {
        //kDebug(planDbg())<<"online";
        part = m_chain->outputDocument();
    }
    if (part == 0) {
        kError() << "Cannot open document";
        return KoFilter::InternalError;
    }
    if ( ! part->loadNativeFormat( m_chain->inputFile() ) ) {
        return KoFilter::ParsingError;
    }

    return KoFilter::OK;
}


#include "kplatoimport.moc"
