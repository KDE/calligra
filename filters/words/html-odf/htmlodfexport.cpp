/* This file is part of the Calligra project
   Copyright (C) 2010 Pramod S G <pramod.xyle@gmail.com>
   Copyright (C) 2010 Srihari Prasad G V <sri-hari@live.com>
   Copyright (C) 2012 Stuart Dickson <stuart@kogmbh.com>

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

#include "htmlodfexport.h"

#include <qdom.h>
#include <qfontinfo.h>
#include <QFile>
#include <QString>
#include <QBuffer>
//Added by qt3to4:
#include <QByteArray>
#include <QFileInfo>
#include <QDir>

#include <kdebug.h>
#include <kpluginfactory.h>

#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoOdfWriteStore.h>
#include <KoStoreDevice.h>
#include <KoXmlWriter.h>

#include <document.h>
#include <exportdialog.h>
#include <QtXmlPatterns>
#include <convert.h>
#include <KoDocument.h>

#include <iostream>

#include "ManifestParser.h"
//#include <kexi/migration/mdb/src/mdbtools/include/mdbtools.h>

K_PLUGIN_FACTORY(HTMLOdfExportFactory, registerPlugin<HTMLOdfExport>();)
K_EXPORT_PLUGIN(HTMLOdfExportFactory("calligrafilters"))


HTMLOdfExport::HTMLOdfExport(QObject* parent, const QVariantList&) :
    KoFilter(parent), m_dialog(new ExportDialog())
{
}

HTMLOdfExport::~HTMLOdfExport()
{
}



KoFilter::ConversionStatus HTMLOdfExport::convert(const QByteArray &from, const QByteArray &to)
{
    // check for proper conversion
    if (to != "text/html"
            || from != "application/vnd.oasis.opendocument.text")
        return KoFilter::NotImplemented;

    kDebug(30503) << "######################## HTMLOdfExport::convert ########################";

    QString inputFile = m_chain->inputFile();
    QString outputFile = m_chain->outputFile();


    if (!m_chain->manager()->getBatchMode() ) {
        if (m_dialog->exec() == QDialog::Rejected) {
            return KoFilter::UserCancelled;
        }
    }

    // Create output files
    QFile out(outputFile);
    if (!out.open(QIODevice::WriteOnly)) {
        kError(30501) << "Unable to open output file!";
        out.close();
        return KoFilter::FileNotFound;
    }
    
    QFileInfo base(outputFile);
    QString filenamewithoutext = outputFile.left(outputFile.lastIndexOf('.'));
    QString directory=base.absolutePath();
    QDir dir(outputFile);
    dir.mkdir(filenamewithoutext);
    
    
    // TODO: Provide feedback for errors occuring during QXmlQuery conversion
    Conversion c1;
    c1.convert(inputFile, &out, filenamewithoutext+"/");
    

    QString stylesheet=filenamewithoutext+"/style.css";
    QFile css(stylesheet);
    if (!css.open(QIODevice::WriteOnly)){
        kError(30501) << "Unable to open stylesheet!";
        css.close();
        return KoFilter::FileNotFound;
    }
            
    QByteArray manifest;
    KoStore* storecont = KoStore::createStore(inputFile, KoStore::Read);
    storecont->extractFile("META-INF/manifest.xml",manifest);
    
    // parse manifest for 
    //  <manifest:file-entry manifest:media-type="image/png" manifest:full-path="Pictures/image1.png"/>
    // This is within the manifest root
    ManifestParser manifestParser;
    QXmlInputSource source;
    source.setData( manifest );
    QXmlSimpleReader reader;
    reader.setContentHandler( &manifestParser );
    reader.parse( source );

    QString sourceImage;
    QString destImage;
    QString outputPath;
    // Extract each file
    QStringListIterator fileListIt = QStringListIterator(manifestParser.fileList());
    while (fileListIt.hasNext()) {
        sourceImage = fileListIt.next();
        destImage = filenamewithoutext + "/" + sourceImage;
        
        // Create the target directory
        outputPath = filenamewithoutext+"/"+sourceImage.left(sourceImage.lastIndexOf('/'));
        dir.mkpath(outputPath);
        
        storecont->extractFile(sourceImage,destImage);
    }
    
    out.close();
    css.close();



    struct Finalizer {
    public:
        Finalizer(KoStore *store) : m_store(store), m_genStyles(0), m_document(0), m_contentWriter(0), m_bodyWriter(0) { }
        ~Finalizer() {
            delete m_store; delete m_genStyles; delete m_document; delete m_contentWriter; delete m_bodyWriter;
        }

        KoStore *m_store;
        KoGenStyles *m_genStyles;
        Document *m_document;
        KoXmlWriter* m_contentWriter;
        KoXmlWriter* m_bodyWriter;
    };


    kDebug(30503) << "######################## HTMLOdfExport::convert done ####################";
    return KoFilter::OK;
}

#include <htmlodfexport.moc>
