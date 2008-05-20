/* This file is part of the KDE project
 * Copyright (C) 2005 David Faure <faure@kde.org>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007-2008 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2007-2008 Pierre Ducroquet <pinaraf@gmail.com>
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWOdfWriter.h"
#include "KWDocument.h"

//#include "frames/KWFrameSet.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWFrame.h"
//#include "frames/KWFrameLayout.h"
//#include "frames/KWTextDocumentLayout.h"
//#include "frames/KWOutlineShape.h"

#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoXmlWriter.h>
#include <KoOdfWriteStore.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoShapeSavingContext.h>
#include <KoTextShapeData.h>

#include <QBuffer>
#include <KTemporaryFile>
#include <kdebug.h>

/// \internal d-pointer class.
class KWOdfWriter::Private
{
    public:
        /// The KWord document.
        QPointer<KWDocument> document;
};

KWOdfWriter::KWOdfWriter(KWDocument *document)
    : QObject()
    , d(new Private())
{
    d->document = document;
}

KWOdfWriter::~KWOdfWriter()
{
    delete d;
}

bool KWOdfWriter::save(KoOdfWriteStore & odfStore, KoEmbeddedDocumentSaver & embeddedSaver)
{
    kDebug()<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";

    KoStore * store = odfStore.store();
    KoXmlWriter * manifestWriter = odfStore.manifestWriter();

    KoGenStyles mainStyles;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Header+Footer

    // Header and footers save their content into master-styles/master-page, and their
    // styles into the page-layout automatic-style.
    QByteArray headerFooterContent;
    {
        QBuffer buffer(&headerFooterContent);
        buffer.open(IO_WriteOnly);
        KoXmlWriter headerFooterTmpWriter(&buffer);

        KoShapeSavingContext ccc(headerFooterTmpWriter, mainStyles, embeddedSaver);

        QMap<KWord::TextFrameSetType, KWTextFrameSet *> textframesets;
        foreach(KWFrameSet *fs, d->document->frameSets()) {
            if (KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*> (fs)) {
                textframesets.insert(tfs->textFrameSetType(), tfs);
            }
        }
        //TODO handle all
        KWTextFrameSet *f = textframesets[ KWord::OddPagesHeaderTextFrameSet ];
        if(f) {
            KoTextShapeData * sd = dynamic_cast<KoTextShapeData *>( f->frames().first()->shape()->userData() );
            Q_ASSERT(sd);
            headerFooterTmpWriter.startElement( "header" ); // e.g. style:header or style:footer-left
            sd->saveOdf(ccc); //TODO how to attach the style-name ?
            headerFooterTmpWriter.endElement();

            //foreach(KoGenStyles::NamedStyle s, mainStyles.styles( KoGenStyle::StyleAuto ))
            //    mainStyles.markStyleForStylesXml( s.name );
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Content

    // add manifest line for content.xml
    manifestWriter->addManifestEntry( "content.xml", "text/xml" );

    if ( !store->open( "content.xml" ) )
        return false;

    KoStoreDevice contentDev( store );
    KoXmlWriter* contentWriter = KoOdfWriteStore::createOasisXmlWriter( &contentDev, "office:document-content" );

    // for office:master-styles
    KTemporaryFile masterStyles;
    masterStyles.open();
    KoXmlWriter masterStylesTmpWriter( &masterStyles, 1 );

    // for office:body
    KTemporaryFile contentTmpFile;
    contentTmpFile.open();
    KoXmlWriter contentTmpWriter( &contentTmpFile, 1 );

    //KoGenStyles mainStyles;
    KoShapeSavingContext context(contentTmpWriter, mainStyles, embeddedSaver);

    contentTmpWriter.startElement( "office:body" );
    contentTmpWriter.startElement( "office:text" );

    KWTextFrameSet *mainTextFrame = 0;

    foreach(KWFrameSet *fs, d->document->frameSets()) {
        // TODO loop over all non-autocreated frames and save them.
        KWTextFrameSet *tfs = dynamic_cast<KWTextFrameSet*> (fs);
        if (tfs) {
            if (tfs->textFrameSetType() == KWord::MainTextFrameSet) {
                mainTextFrame = tfs;
                continue;
            }
        }
#if 0 //sebsauer; don't just save them but we need to handle things more explicit
        foreach(KWFrame *frame, fs->frames()) {
            //FIXME: Each text frame will save the entire document of the frameset.
            frame->saveOdf(context);
        }
#endif
    }

    if (mainTextFrame) {
        if (! mainTextFrame->frames().isEmpty() && mainTextFrame->frames().first() ) {
            KoTextShapeData * shapeData = dynamic_cast<KoTextShapeData *>( mainTextFrame->frames().first()->shape()->userData() );
            if ( shapeData ) {
                shapeData->saveOdf(context);
            }
        }
    }

    /*
    contentTmpWriter.startElement( odfTagName() );
    paContext.setXmlWriter( contentTmpWriter );
    paContext.setOptions( KoPASavingContext::DrawId );
    // save pages
    foreach ( KoPAPageBase *page, m_pages )
    {
        page->saveOdf( paContext );
        paContext.incrementPage();
    }
    */
    contentTmpWriter.endElement(); // office:text
    contentTmpWriter.endElement(); // office:body

    contentTmpFile.close();

    mainStyles.saveOdfAutomaticStyles( contentWriter, false );

    // And now we can copy over the contents from the tempfile to the real one
    contentWriter->addCompleteElement( &contentTmpFile );

    contentWriter->endElement(); // root element
    contentWriter->endDocument();
    delete contentWriter;

    if ( !store->close() ) // done with content.xml
        return false;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Styles

    if( ! headerFooterContent.isNull() ) {
        KoGenStyle headerStyle(KoGenStyle::StyleMaster);
        //headerStyle.setAutoStyleInStylesDotXml(true);
        headerStyle.addChildElement("",headerFooterContent);
        mainStyles.lookup(headerStyle); //inserts it
    }

    // save the styles.xml
    if ( !mainStyles.saveOdfStylesDotXml( store, manifestWriter ) )
        return false;

#if 0 //sebsauer: moved to KWDocument::saveOdf... probably move it back + provide access to the datacenter?!
    bool ok=true;
    foreach(KoDataCenter *dataCenter, m_dataCenterMap)
    {
        ok = ok && dataCenter->completeSaving(store, manifestWriter);
    }
    if(!ok)
        return false;
#endif

#if 0 // tz: remove until data center is used
    if (!context.saveImages(store, manifestWriter))
        return false;
#endif
    return true;
}

