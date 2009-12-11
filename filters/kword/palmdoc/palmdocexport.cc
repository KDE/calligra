/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>

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

#include <QFileInfo>
#include <QByteArray>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>

#include <KWEFStructures.h>
#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include "palmdoc.h"

#include "palmdocexport.h"

typedef KGenericFactory<PalmDocExport> PalmDocExportFactory;
K_EXPORT_COMPONENT_FACTORY(libpalmdocexport, PalmDocExportFactory("kofficefilters"))

class PalmDocWorker : public KWEFBaseWorker
{
public:
    PalmDocWorker(void)  { }
    virtual ~PalmDocWorker(void) { }
public:
    virtual bool doOpenFile(const QString& filenameOut, const QString& to);
    virtual bool doCloseFile(void);
    virtual bool doOpenDocument(void);
    virtual bool doCloseDocument(void);
    virtual bool doFullDocumentInfo(const KWEFDocumentInfo& docInfo);
    virtual bool doFullParagraph(const QString& paraText, const LayoutData& layout,
                                 const ValueListFormatData& paraFormatDataList);
private:
    QString title;
    QString outfile;
    QString text;
};

bool PalmDocWorker::doOpenFile(const QString& filenameOut, const QString& /*to*/)
{
    outfile = filenameOut;
    return true;
}

bool PalmDocWorker::doCloseFile(void)
{
    if (title.isEmpty()) {
        QFileInfo info(outfile);
        title = info.baseName();
    }

    PalmDoc doc;
    doc.setName(title);
    doc.setText(text);
    doc.save(QFile::encodeName(outfile));

    return true;
}

bool PalmDocWorker::doOpenDocument(void)
{
    text.clear();
    return true;
}

bool PalmDocWorker::doCloseDocument(void)
{
    return true;
}

bool PalmDocWorker::doFullDocumentInfo(const KWEFDocumentInfo& docInfo)
{
    title = docInfo.title;
    return true;
}

bool PalmDocWorker::doFullParagraph(const QString& paraText,
                                    const LayoutData& /*layout*/, const ValueListFormatData& /*paraFormatDataList*/)
{
    kDebug(30525) << "Entering ::doFullParagraph";
    text.append(paraText);
    text.append("\n\n");

    return true;
}

PalmDocExport::PalmDocExport(QObject* parent, const QStringList&):
        KoFilter(parent)
{
}

KoFilter::ConversionStatus PalmDocExport::convert(const QByteArray& from,
        const QByteArray& to)
{
    // check for proper conversion
    if (to != "application/vnd.palm" || from != "application/x-kword")
        return KoFilter::NotImplemented;

    PalmDocWorker* worker = new PalmDocWorker();
    KWEFKWordLeader* leader = new KWEFKWordLeader(worker);

    KoFilter::ConversionStatus result;
    result = leader->convert(m_chain, from, to);

    delete worker;
    delete leader;

    return result;
}

#include "palmdocexport.moc"
