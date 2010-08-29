/* This file is part of the KDE project
   Copyright (C) 2010 Marijn Kruisselbrink <m.kruisselbrink@student.tue.nl>

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

#include <ExcelExport.h>
#include <ExcelExport.moc>

#include <kdebug.h>
#include <kgenericfactory.h>

#include <KoFilterChain.h>

#include <part/Doc.h>

#include <swinder.h>

typedef KGenericFactory<ExcelExport> ExcelExportFactory;
K_EXPORT_COMPONENT_FACTORY(libexcelexport, ExcelExportFactory("kofficefilters"))

using namespace Swinder;

class ExcelExport::Private
{
public:
    const KSpread::Doc* inputDoc;
    QString outputFile;

};

ExcelExport::ExcelExport(QObject* parent, const QStringList&)
        : KoFilter(parent)
{
    d = new Private;
}

ExcelExport::~ExcelExport()
{
    delete d;
}

KoFilter::ConversionStatus ExcelExport::convert(const QByteArray& from, const QByteArray& to)
{
    if (to != "application/vnd.ms-excel")
        return KoFilter::NotImplemented;

    if (from != "application/vnd.oasis.opendocument.spreadsheet")
        return KoFilter::NotImplemented;

    d->outputFile = m_chain->outputFile();

    KoDocument* document = m_chain->inputDocument();
    if (!document)
        return KoFilter::StupidError;

    d->inputDoc = qobject_cast<const KSpread::Doc*>(document);
    if (!d->inputDoc) {
        kWarning() << "document isn't a KSpread::Doc but a " << document->metaObject()->className();
    }

    emit sigProgress(100);

    return KoFilter::OK;
}


