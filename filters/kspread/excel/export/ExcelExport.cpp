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
#include <XlsRecordOutputStream.h>

#include <CFBWriter.h>

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

    CFBWriter w;
    w.open(d->outputFile);
    QIODevice* a = w.openSubStream("Workbook");
    XlsRecordOutputStream o(a);

    BOFRecord b(0);
    b.setType(BOFRecord::Workbook);
    b.setRecordSize(16);
    o.writeRecord(b);

    o.writeRecord(InterfaceHdrRecord(0));
    o.writeRecord(MmsReservedRecord(0));
    o.writeRecord(InterfaceEndRecord(0));

    LastWriteAccessRecord lwar(0);
    lwar.setUserName("marijn"); // TODO: figure out real username
    lwar.setUnusedBlob(QByteArray(112 - 3 - 2*lwar.userName().length(), ' '));
    o.writeRecord(lwar);

    delete a;
    w.close();

    emit sigProgress(100);

    return KoFilter::OK;
}


