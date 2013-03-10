/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2000 Michael Johnson <mikej@xnet.com>
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (C) 2010-2011 Thorsten Zachmann <zachmann@kde.org>
   Copyright (C) 2010 Christoph Cullmann <cullmann@kde.org> 
   Copyright (C) 2013 Inge Wallin <inge@lysator.liu.se>

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

#include "AsciiExport.h"

#include <QTextCodec>
#include <QFile>
#include <QTextDocument>
#include <QTextCursor>

#include <kdebug.h>
#include <kpluginfactory.h>
#include <kencodingprober.h>

#include <calligraversion.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoStore.h>
#include <KoOdfWriteStore.h>
#include <KoGenStyles.h>
#include <KoXmlWriter.h>
#include <KoStyleManager.h>
#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>
#include <KoOdfStylesReader.h>
#include <KoOdfLoadingContext.h>
#include <KoShapeLoadingContext.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoShapeSavingContext.h>
#include <KoTextWriter.h>
#include <KoProgressUpdater.h>
#include <KoUpdater.h>
#include <KoTextDocumentLayout.h>

#include "OdtTraverser.h"

#include "OdtTraverserAsciiBackend.h"

//#include <KWDocument.h>
//#include <KWPage.h>
//#include <frames/KWTextFrameSet.h>


K_PLUGIN_FACTORY(AsciiExportFactory, registerPlugin<AsciiExport>();)
K_EXPORT_PLUGIN(AsciiExportFactory("wordsasciiexportng", "calligrafilters"))

#if 0
bool checkEncoding(QTextCodec *codec, QByteArray &data)
{
    QTextCodec::ConverterState state(QTextCodec::ConvertInvalidToNull);
    QString unicode = codec->toUnicode(data.constData(), data.size(), &state);
    for (int i = 0; i < unicode.size(); ++i) {
        if (unicode[i] == 0) {
            return false;
        }
    }
    return true;
}
#endif

AsciiExport::AsciiExport(QObject *parent, const QVariantList &)
: KoFilter(parent)
{
}

AsciiExport::~AsciiExport()
{
}

KoFilter::ConversionStatus AsciiExport::convert(const QByteArray& from, const QByteArray& to)
{
    // Check for types
    if (from != "application/vnd.oasis.opendocument.text" || to != "text/plain") {
        return KoFilter::NotImplemented;
    }

    // Open the infile and return an error if it fails.
    KoStore *odfStore = KoStore::createStore(m_chain->inputFile(), KoStore::Read,
                                             "", KoStore::Auto);
    // If we don't call disallowNameExpansion(), then filenames that
    // begin with numbers will not be opened. Embedded images often
    // have names like this.
    odfStore->disallowNameExpansion();
    if (!odfStore->open("mimetype")) {
        kError(30503) << "Unable to open input file!" << endl;
        delete odfStore;
        return KoFilter::FileNotFound;
    }
    odfStore->close();

    // Start the conversion

    // Create output file.
    QFile outfile(m_chain->outputFile());
    if (!outfile.open(QIODevice::WriteOnly)) {
        kError(30501) << "Unable to open output file!" << endl;
        outfile.close();
        return KoFilter::FileNotFound;
    }

    OdtTraverserAsciiContext  asciiBackendContext(odfStore, outfile);
    OdtTraverserAsciiBackend  asciiBackend(&asciiBackendContext);
    OdtTraverser              odtTraverser;

    odtTraverser.traverseContent(&asciiBackendContext, &asciiBackend);
    outfile.close();

    return KoFilter::OK;
}
