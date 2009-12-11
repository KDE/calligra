/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers

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

#include "oopptimport.h"

#include <QString>
#include <QFile>

#include <KoFilter.h>
#include <KoFilterChain.h>

#include <KGenericFactory>
#include <KRun>
#include <KProcess>
#include <KShell>
#include <KDebug>

typedef KGenericFactory<OOPPTImport> OOPPTImportFactory;
K_EXPORT_COMPONENT_FACTORY(liboopptimport, OOPPTImportFactory("kofficefilters"))

OOPPTImport::OOPPTImport(QObject*parent, const QStringList&)
        : KoFilter(parent)
{
}

OOPPTImport::~OOPPTImport()
{
}

KoFilter::ConversionStatus OOPPTImport::convert(const QByteArray& from, const QByteArray& to)
{
    if (from != "application/vnd.ms-powerpoint") {
        return KoFilter::NotImplemented;
    }

    if (to == "application/vnd.oasis.opendocument.presentation") {
        // Copy filenames
        QString input = m_chain->inputFile();
        QString output = m_chain->outputFile();

        QString command("python KOfficeOODocumentConverter.py ");
        command += KShell::quoteArg(input);
        command += ' ';
        command += KShell::quoteArg(output);

        kDebug() << "command to execute is (%s)" << QFile::encodeName(command).data() ;

        // Execute it:
        if (! system(QFile::encodeName(command)))
            return KoFilter::OK;
        else
            return KoFilter::StupidError;
    }

    return KoFilter::NotImplemented;
}

#include "oopptimport.moc"

