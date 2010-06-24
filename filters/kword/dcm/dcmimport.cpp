/* This file is part of the KOffice project
   Copyright (C) 2010 Arjun Asthana <arjun.kde@iiitd.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the Library GNU General Public
   version 2 of the License, or (at your option) version 3 or,
   at the discretion of KDE e.V (which shall act as a proxy as in
   section 14 of the GPLv3), any later version..

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#include <sstream>

#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtGui/QApplication>
#include <QtGui/QTextDocument>
#include <QtGui/QTextDocumentWriter>

#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <KoFilterManager.h>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmsr/dsrdoc.h>
#include <dcmtk/ofstd/ofstream.h>
#include <dcmtk/dcmsr/dsrtypes.h>

#include "dcmimport.h"
#include "DCMImportDialog.h"

typedef KGenericFactory<DCMImport> DCMImportFactory;
K_EXPORT_COMPONENT_FACTORY(libdcmimport, DCMImportFactory("kofficefilters"))

KoFilter::ConversionStatus DCMImport::convert(QByteArray const& from, QByteArray const& to)
{
    if (to != "application/x-kword" || from != "application/dicom") {
        return KoFilter::NotImplemented;
    }

    OFCondition result;

    DcmFileFormat ff;
    if ((result = ff.loadFile(m_chain->inputFile().toLocal8Bit().constData())).bad()) {
        return KoFilter::WrongFormat;
    }

    DCMImportDialog* dialog = 0;
    if (!m_chain->manager()->getBatchMode()) {
        dialog = new DCMImportDialog(QApplication::activeWindow());
        if (!dialog) {
            return KoFilter::OutOfMemory;
        }
        if (!dialog->exec()) {
            return KoFilter::UserCancelled;
        }
    }

    size_t readFlags = 0;

    if (dialog->getDigitalSignature()) {
        readFlags |= DSRTypes::RF_readDigitalSignatures;
    }

    if (dialog->getIgnoreContentItemErrors()) {
        readFlags |= DSRTypes::RF_ignoreContentItemErrors;
    }

    if (dialog->getIgnoreRelationshipConstraints()) {
        readFlags |= DSRTypes::RF_ignoreRelationshipConstraints;
    }

    if (dialog->getSkipInvalidSubtree()) {
        readFlags |= DSRTypes::RF_skipInvalidContentItems;
    }

    delete dialog;

    DSRDocument doc;
    if ((result = doc.read(*ff.getDataset(), readFlags)).bad()) {
        return KoFilter::ParsingError;
    }

    std::ostringstream htmlStream;
    if ((result = doc.renderHTML(htmlStream)).bad()) {
        return KoFilter::StupidError;
    }

    QTextDocument qdoc;
    qdoc.setHtml(QString(htmlStream.str().c_str()));

    QTextDocumentWriter qdocWriter(m_chain->outputFile(), "odf");
    if (qdocWriter.write(&qdoc)) {
        return KoFilter::OK;
    }
    else
        return KoFilter::CreationError;
}
