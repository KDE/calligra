/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>
   Copyright (C) 2012 Sven Langkamp <sven.langkamp@gmail.com>
   Copyright (C) 2015-2016 Friedrich W. H. Kossebau <kossebau@kde.org>

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

#include "OkularOdtGenerator.h"

#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QTextDocument>
#include <QMimeDatabase>
#include <QMimeType>

#include <KoDocumentEntry.h>
#include <KoPart.h>
#include <KWDocument.h>
#include <KWPage.h>
#include <KWCanvasItem.h>
#include <frames/KWTextFrameSet.h>
#include <KoShapeManager.h>
#include <KoDocumentInfo.h>
#include <KoGlobal.h>
#include <KoParagraphStyle.h>
#include <KoTextLayoutRootArea.h>
#include <KoCharAreaInfo.h>

#include <okular/core/page.h>


OkularOdtGenerator::OkularOdtGenerator( QObject *parent, const QVariantList &args )
    : Okular::Generator( parent, args )
{
    m_doc = nullptr;
    setFeature( TextExtraction );
}

OkularOdtGenerator::~OkularOdtGenerator()
{
}

static Okular::DocumentViewport calculateViewport( const QTextBlock &block,
                                                   KoTextDocumentLayout* textDocumentLayout )
{
    KoTextLayoutRootArea *rootArea = textDocumentLayout->rootAreaForPosition(block.position());

    QRectF rect = textDocumentLayout->blockBoundingRect( block );
    rect.translate(-(rootArea->referenceRect().topLeft()));

    KoShape *shape = rootArea->associatedShape();
    rect.translate(shape->absolutePosition(KoFlake::TopLeftCorner));

    KWPage* page = static_cast<KWPage *>(rootArea->page());
    rect.translate(static_cast<qreal>(0.0), -(page->offsetInDocument()));

    const qreal pageHeight = page->height();
    const qreal pageWidth = page->width();
    const int pageNumber = page->pageNumber();

    Okular::DocumentViewport viewport( pageNumber-1 );
    viewport.rePos.normalizedX = static_cast<double>(rect.x()) / static_cast<double>(pageWidth);
    viewport.rePos.normalizedY = static_cast<double>(rect.y()) / static_cast<double>(pageHeight);
    viewport.rePos.enabled = true;
    viewport.rePos.pos = Okular::DocumentViewport::TopLeft;

    return viewport;
}

bool OkularOdtGenerator::loadDocument( const QString &fileName, QVector<Okular::Page*> &pages )
{
    const QString mimetype = QMimeDatabase().mimeTypeForFile(fileName).name();

    QString error;
    KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(mimetype);
    KoPart *part = documentEntry.createKoPart(&error);

    if (!error.isEmpty() || !part) {
        qWarning() << "Error creating document" << mimetype << error;
        return 0;
    }

    m_doc = static_cast<KWDocument*>(part->document());
    const QUrl url = QUrl::fromLocalFile(fileName);
    m_doc->setCheckAutoSaveFile(false);
    m_doc->setAutoErrorHandlingEnabled(false); // show error dialogs
    if (!m_doc->openUrl(url)) {
        return false;
    }

    while (!m_doc->layoutFinishedAtleastOnce()) {
        QCoreApplication::processEvents();

        if (!QCoreApplication::eventDispatcher()->hasPendingEvents())
            break;
    }

    KWPageManager *pageManager = m_doc->pageManager();
    int pageCount = pageManager->pages().count();
    for(int i = 1; i <= pageCount; ++i) {

        KWPage kwpage = pageManager->page(i);

        Okular::Page * page = new Okular::Page( i-1, kwpage.width(), kwpage.height(), Okular::Rotation0 );
        pages.append(page);
    }

    // meta data
    const KoDocumentInfo *documentInfo = m_doc->documentInfo();
    m_documentInfo.set( Okular::DocumentInfo::MimeType, mimetype );
    m_documentInfo.set( Okular::DocumentInfo::Producer, documentInfo->originalGenerator() );
    m_documentInfo.set( Okular::DocumentInfo::Title,       documentInfo->aboutInfo("title") );
    m_documentInfo.set( Okular::DocumentInfo::Subject,     documentInfo->aboutInfo("subject") );
    m_documentInfo.set( Okular::DocumentInfo::Keywords,     documentInfo->aboutInfo("keyword") );
    m_documentInfo.set( Okular::DocumentInfo::Description, documentInfo->aboutInfo("description") );
    m_documentInfo.set( "language",    KoGlobal::languageFromTag(documentInfo->aboutInfo("language")),  i18n("Language"));

    const QString creationDate = documentInfo->aboutInfo("creation-date");
    if (!creationDate.isEmpty()) {
        QDateTime t = QDateTime::fromString(creationDate, Qt::ISODate);
        m_documentInfo.set( Okular::DocumentInfo::CreationDate, QLocale().toString(t, QLocale::ShortFormat) );
    }
    m_documentInfo.set( Okular::DocumentInfo::Creator,  documentInfo->aboutInfo("initial-creator") );

    const QString modificationDate = documentInfo->aboutInfo("date");
    if (!modificationDate.isEmpty()) {
        QDateTime t = QDateTime::fromString(modificationDate, Qt::ISODate);
        m_documentInfo.set( Okular::DocumentInfo::ModificationDate, QLocale().toString(t, QLocale::ShortFormat) );
    }
    m_documentInfo.set( Okular::DocumentInfo::Author, documentInfo->aboutInfo("creator") );

    // ToC
    QDomNode parentNode = m_documentSynopsis;

    QStack< QPair<int,QDomNode> > parentNodeStack;
    parentNodeStack.push( qMakePair( 0, parentNode ) );

    QTextDocument* textDocument = m_doc->mainFrameSet()->document();
    KoTextDocumentLayout* textDocumentLayout = static_cast<KoTextDocumentLayout *>(textDocument->documentLayout());

    QTextBlock block = textDocument->begin();
    for (; block.isValid(); block = block.next()) {
        int blockLevel = 0;
        if (block.blockFormat().hasProperty(KoParagraphStyle::OutlineLevel)) {
            blockLevel = block.blockFormat().intProperty(KoParagraphStyle::OutlineLevel);
        }

        // no blockLevel yet?
        if (blockLevel == 0) {
            continue;
        }

        Okular::DocumentViewport viewport = calculateViewport( block, textDocumentLayout );

        QDomElement item = m_documentSynopsis.createElement( block.text() );
        item.setAttribute( "Viewport", viewport.toString() );

        // we need a parent, which has to be at a higher heading level than this heading level
        // so we just work through the stack
        while ( ! parentNodeStack.isEmpty() ) {
            int parentLevel = parentNodeStack.top().first;
            if ( parentLevel < blockLevel ) {
                // this is OK as a parent
                parentNode = parentNodeStack.top().second;
                break;
            } else {
                // we'll need to be further into the stack
                parentNodeStack.pop();
            }
        }
        parentNode.appendChild( item );
        parentNodeStack.push( qMakePair( blockLevel, QDomNode(item) ) );
    }

    return true;
}

bool OkularOdtGenerator::doCloseDocument()
{
    delete m_doc;
    m_doc = 0;

    m_documentInfo = Okular::DocumentInfo();
    m_documentSynopsis = Okular::DocumentSynopsis();

    return true;
}

bool OkularOdtGenerator::canGeneratePixmap() const
{
    return true;
}

void OkularOdtGenerator::generatePixmap( Okular::PixmapRequest *request )
{
    QPixmap* pix;
    if (!m_doc) {
        pix = new QPixmap(request->width(), request->height());
        QPainter painter(pix);
        painter.fillRect(0 ,0 , request->width(), request->height(), Qt::white);
    } else {

        // use shape manager from canvasItem even for QWidget environments
        // if using the shape manager from one of the views there is no guarantee
        // that the view, its canvas and the shapemanager is not destroyed in between
        KoShapeManager* shapeManager = static_cast<KWCanvasItem*>(m_doc->documentPart()->canvasItem(m_doc))->shapeManager();

        KWPageManager *pageManager = m_doc->pageManager();

        KWPage page = pageManager->page(request->pageNumber()+1);

        QSize rSize(request->width(), request->height());

        pix = new QPixmap();
        pix->convertFromImage(page.thumbnail(rSize, shapeManager, true));
    }

    request->page()->setPixmap( request->observer(), pix );

    signalPixmapRequestDone( request );
}

bool OkularOdtGenerator::canGenerateTextPage() const
{
    return true;
}

Okular::TextPage* OkularOdtGenerator::textPage( Okular::Page *page )
{
    QTextDocument* textDocument = m_doc->mainFrameSet()->document();
    KoTextDocumentLayout* textDocumentLayout = static_cast<KoTextDocumentLayout *>(textDocument->documentLayout());

    KoTextLayoutRootArea *rootArea = 0;
    foreach(KoTextLayoutRootArea *area, textDocumentLayout->rootAreas()) {
        if (area->page()->pageNumber() == page->number()+1) {
            rootArea = area;
            break;
        }
    }

    if (!rootArea) {
        return 0;
    }

    const QVector<KoCharAreaInfo> charAreaInfos = rootArea->generateCharAreaInfos();

    // TODO: text from master pages (headers/footers), text in floating shapes
    if (charAreaInfos.isEmpty()) {
        return 0;
    }

    KWPage* wpage = static_cast<KWPage *>(rootArea->page());
    KoShape *shape = rootArea->associatedShape();

    const QPointF offset = shape->absolutePosition(KoFlake::TopLeftCorner)
                           + QPointF(static_cast<qreal>(0.0), -(wpage->offsetInDocument()))
                           - rootArea->referenceRect().topLeft();

    const double pageHeight = wpage->height();
    const double pageWidth = wpage->width();

    Okular::TextPage *textPage = new Okular::TextPage;
    foreach(const KoCharAreaInfo &charAreaInfo, charAreaInfos) {
        const QRectF rect = charAreaInfo.rect.translated(offset);
        const double left = static_cast<double>(rect.left()) / pageWidth;
        const double top = static_cast<double>(rect.top()) / pageHeight;
        const double right = static_cast<double>(rect.right()) / pageWidth;
        const double bottom = static_cast<double>(rect.bottom()) / pageHeight;
        textPage->append( charAreaInfo.character,
                          new Okular::NormalizedRect( left, top, right, bottom ) );
    }
    return textPage;
}


Okular::DocumentInfo OkularOdtGenerator::generateDocumentInfo( const QSet<Okular::DocumentInfo::Key> &keys ) const
{
    Q_UNUSED(keys);

    return m_documentInfo;
}

const Okular::DocumentSynopsis* OkularOdtGenerator::generateDocumentSynopsis()
{
    return m_documentSynopsis.hasChildNodes() ? &m_documentSynopsis : 0;
}
