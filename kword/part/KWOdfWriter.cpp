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

        QByteArray serializeHeaderFooter(KoEmbeddedDocumentSaver& embeddedSaver, KoGenStyles& mainStyles, KWTextFrameSet* fs)
        {
            QByteArray tag;
            switch (fs->textFrameSetType()) {
                case KWord::OddPagesHeaderTextFrameSet:  tag = "style:header";       break;
                case KWord::EvenPagesHeaderTextFrameSet: tag = "style:header-left";  break;
                case KWord::OddPagesFooterTextFrameSet:  tag = "style:footer";       break;
                case KWord::EvenPagesFooterTextFrameSet: tag = "style:footer-left";  break;
                default: return QByteArray();
            }

            QByteArray content;
            QBuffer buffer(&content);
            buffer.open(IO_WriteOnly);
            KoXmlWriter writer(&buffer);
            KoShapeSavingContext context(writer, mainStyles, embeddedSaver);

            Q_ASSERT(fs->frames().count() > 0);
            KoTextShapeData *shapedata = dynamic_cast<KoTextShapeData *>( fs->frames().first()->shape()->userData() );
            Q_ASSERT(shapedata);

            writer.startElement(tag);
            shapedata->saveOdf(context, 0, -1, false);
            writer.endElement();

            return content;
        }

        void saveHeaderFooter(KoEmbeddedDocumentSaver& embeddedSaver, KoGenStyles& mainStyles)
        {
kDebug()<<"START saveHeaderFooter ############################################";

            // We need to flush them out ordered as defined in the specs.
            QList<KWord::TextFrameSetType> order;
            order << KWord::OddPagesHeaderTextFrameSet << KWord::EvenPagesHeaderTextFrameSet
                  << KWord::OddPagesFooterTextFrameSet << KWord::EvenPagesFooterTextFrameSet;

            // Iterate over all pagestyles and write them out.
            QHash<QString, KWPageStyle *> pagestyles = document->pageManager()->pageStyles();
            QHashIterator<QString, KWPageStyle *> it(pagestyles);
            
            while (it.hasNext()) {
                it.next();

                KoGenStyle masterStyle(KoGenStyle::StyleMaster);
                //masterStyle.setAutoStyleInStylesDotXml(true);
                KoGenStyle layoutStyle = it.value()->pageLayout().saveOasis();
                layoutStyle.setAutoStyleInStylesDotXml(true);
                masterStyle.addProperty("style:page-layout-name", mainStyles.lookup(layoutStyle, "pm"));

                for(int i = 0; i < order.count(); ++i) {
                    KWTextFrameSet *fs = it.value()->getFrameSet(order[i]);
                    if (! fs)
                        continue;

                    if (fs->frames().count() < 1) { //FIXME should not happen but it does :-/
                        kWarning()<<"Frameset with ZERO frames what should not happen!!!";
                        continue;
                    }

                    QByteArray content = serializeHeaderFooter(embeddedSaver, mainStyles, fs);
                    if (content.isNull())
                        continue;

                    masterStyle.addChildElement(QString::number(i), content);
                }

                // append the headerfooter-style to the main-style
                if(! masterStyle.isEmpty())
                    mainStyles.lookup(masterStyle, it.key(), KoGenStyles::DontForceNumbering);
            }

            //foreach(KoGenStyles::NamedStyle s, mainStyles.styles(KoGenStyle::StyleAuto))
            //    mainStyles.markStyleForStylesXml( s.name );

kDebug()<<"END saveHeaderFooter ############################################";
        }
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

// 1.6: KWDocument::saveOasisHelper()
bool KWOdfWriter::save(KoOdfWriteStore & odfStore, KoEmbeddedDocumentSaver & embeddedSaver)
{
    kDebug()<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";

    KoStore * store = odfStore.store();
    KoXmlWriter * manifestWriter = odfStore.manifestWriter();

    KoXmlWriter* contentWriter = odfStore.contentWriter();
    if (  !contentWriter )
        return false;

    KoGenStyles mainStyles;
    KoXmlWriter * bodyWriter = odfStore.bodyWriter();

    // Header and footers save their content into master-styles/master-page, and their
    // styles into the page-layout automatic-style.
    d->saveHeaderFooter(embeddedSaver, mainStyles);

    KoShapeSavingContext context( *bodyWriter, mainStyles, embeddedSaver);

    bodyWriter->startElement( "office:body" );
    bodyWriter->startElement( "office:text" );

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

    bodyWriter->endElement(); // office:text
    bodyWriter->endElement(); // office:body

    mainStyles.saveOdfAutomaticStyles( contentWriter, false );

    odfStore.closeContentWriter();

    // add manifest line for content.xml
    manifestWriter->addManifestEntry( "content.xml", "text/xml" );

    // save the styles.xml
    if ( !mainStyles.saveOdfStylesDotXml( store, manifestWriter ) )
        return false;

    if ( !context.saveDataCenter( store, manifestWriter ) ) {
        return false;
    }

    return true;
}

