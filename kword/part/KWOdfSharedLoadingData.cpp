/* This file is part of the KDE project
 * Copyright (C) 2005 David Faure <faure@kde.org>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2007 Pierre Ducroquet <pinaraf@gmail.com>
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

#include "KWOdfSharedLoadingData.h"
#include "KWOdfLoader.h"
#include "KWDocument.h"
#include "frames/KWTextFrameSet.h"
#include "frames/KWTextFrame.h"
#include "frames/KWFrame.h"

#include <KoShape.h>
#include <KoTextAnchor.h>
#include <KoTextDocumentLayout.h>
#include <KoInlineTextObjectManager.h>
#include <KoShapeLoadingContext.h>
#include <KoXmlNS.h>

#include <QTextCursor>
#include <kdebug.h>

KWOdfSharedLoadingData::KWOdfSharedLoadingData(KWOdfLoader* loader)
    : KoTextSharedLoadingData()
    , m_loader(loader)
{
    KoShapeLoadingContext::addAdditionalAttributeData(
        KoShapeLoadingContext::AdditionalAttributeData(
            KoXmlNS::text, "anchor-type", "text:anchor-type" ) );
}

void KWOdfSharedLoadingData::shapeInserted(KoShape* shape)
{
    kDebug()<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! ext:anchor-type="<<shape->additionalAttribute("text:anchor-type");

    KWFrameSet* fs = new KWFrameSet();
    fs->setName("My FrameSet");
    KWFrame *frame = new KWFrame(shape, fs);
    m_loader->document()->addFrameSet(fs);

//TODO anchor->updatePosition()
//shape->setSize(QSizeF(100,100));

    //KoTextAnchor *anchor = new KoTextAnchor(shape);
    //Q_ASSERT(dynamic_cast<KoTextShapeData*>(shape->userData())); //this asserts cause shapes don't inheritate/share there userdata
    /*Q_ASSERT(m_loader->currentFrame());
    KWTextFrameSet* docfs = dynamic_cast<KWTextFrameSet*>(m_loader->currentFrame()->frameSet());
    Q_ASSERT(docfs);
    QTextDocument* doc = docfs->document(); //m_loader->document()->mainFrameSet()->document();
    Q_ASSERT(doc);
    QTextCursor cursor(doc);
    KoTextDocumentLayout *layout = dynamic_cast<KoTextDocumentLayout*> ( cursor.block().document()->documentLayout() );
    Q_ASSERT(layout);
    Q_ASSERT(layout->inlineObjectTextManager());
    layout->inlineObjectTextManager()->insertInlineObject(cursor, anchor);*/
}

#if 0
/**
* The KWOpenDocumentFrameLoader class implements a KoTextFrameLoader to
* handle loading of frames (e.g. images) within KWord.
*/
class KWOpenDocumentFrameLoader //TODO no frame loading for now : public KoTextFrameLoader
{
    public:
        explicit KWOpenDocumentFrameLoader(KWOdfLoader* loader) /*: KoTextFrameLoader(loader), m_loader(loader)*/ {}
        virtual ~KWOpenDocumentFrameLoader() {}

        virtual KoShape* loadImageShape(KWLoadingContext& context, const KoXmlElement& frameElem, const KoXmlElement& imageElem, QTextCursor& cursor)
        {
            const QString href = imageElem.attribute("href");
            const QString anchortype = frameElem.attribute("anchor-type");

            KWFrameSet* fs = new KWFrameSet();
            fs->setName(href);

            KWImageFrame *imageFrame = new KWImageFrame(fs);

            m_loader->document()->addFrameSet(fs);
            KoShape* shape = imageFrame->shape();
            if( ! shape ) {
                kWarning(32001)<<"KWOdfLoader::loadImage No image shape"<<endl;
                return 0;
            }

            KoShapeLoadingContext shapecontext(context, m_loader->document() );
            if( ! shape->loadOdf(frameElem, shapecontext) ) {
                kWarning(32500) << "KoTextFrameLoader::loadImage Failed to load odf for picture shape" << endl;
            }

            KoTextAnchor* anchor = 0;
            
            if( anchortype == "paragraph" ) {
                // Anchor position is the paragraph that the current drawing shape element is contained in.
                // The shape appears at the start of the paragraph element.
                anchor = new KoTextAnchor(shape);
                anchor->setOffset( shape->position() );
                anchor->setAlignment(KoTextAnchor::HorizontalOffset);
                anchor->setAlignment(KoTextAnchor::VerticalOffset);
                KoTextDocumentLayout *layout = dynamic_cast<KoTextDocumentLayout*> ( cursor.block().document()->documentLayout() );
                Q_ASSERT(layout && layout->inlineObjectTextManager());
                layout->inlineObjectTextManager()->insertInlineObject(cursor, anchor);
            }
            else if( anchortype == "char" ) {
                // Anchor position is the character after the drawing shape element.
                // The shape appears just before the character.
                anchor = new KoTextAnchor(shape);
                //anchor->setOffset( shape->position() );
                //anchor->setAlignment(KoTextAnchor::HorizontalOffset);
                //anchor->setAlignment(KoTextAnchor::VerticalOffset);
                KoTextDocumentLayout *layout = dynamic_cast<KoTextDocumentLayout*> ( cursor.block().document()->documentLayout() );
                Q_ASSERT(layout && layout->inlineObjectTextManager());
                layout->inlineObjectTextManager()->insertInlineObject(cursor, anchor);
            }
            else if( anchortype == "page" ) {
                // Anchor position is the the page that has the same physical page number as the value of
                // the text:anchor-page-number attribute that is attached to the drawing shape element.
                // If no text:anchor-page-number attribute is given, the anchor position is the page at
                // which the character behind the drawing object element appears.
                // The shape appears either at the start of the document body, outside any paragraph or
                // frame, or inside any paragraph element that is not contained in a header, footer,
                // footnote, or text box.
                int pagenum = QVariant( frameElem.attribute("anchor-page-number") ).toInt();
                if( pagenum < 1 ) //FIXME If no given, anchor position is the page at which the character behind the drawing object element appears.
                    pagenum = 1;

                KWPageManager* pagemanager = m_loader->pageManager();
                for(int i = pagemanager->pageCount(); i < pagenum; ++i) //FIXME prevent to large numbers that would freez KWord
                    pagemanager->appendPage();
                KWPage* page = pagemanager->page(pagenum);
                Q_ASSERT(page);

                anchor = new KoTextAnchor(shape);
                anchor->setAlignment(KoTextAnchor::HorizontalOffset);
                anchor->setAlignment(KoTextAnchor::VerticalOffset);

                QPointF pt = shape->position();
                const double x = pt.x() - page->leftMargin();
                const double y = pt.y() - page->topMargin();
                const double offset = page->offsetInDocument();
                anchor->setOffset( QPointF(x, offset+y) );

                imageFrame->setTextRunAround(KWord::RunThrough);//dirty hack for now

                //QTextDocument* doc = m_loader->document()->mainFrameSet()->document();
                KoTextDocumentLayout *layout = dynamic_cast<KoTextDocumentLayout*> ( cursor.block().document()->documentLayout() );
                Q_ASSERT(layout && layout->inlineObjectTextManager());
                layout->inlineObjectTextManager()->insertInlineObject(cursor, anchor);
            }
            else if( anchortype == "frame" ) {
                // Anchor position is the parent text box that the current drawing shape element is
                // contained in. The shape appears in the element representing the text box to which
                // the drawing object is bound.
                anchor = new KoTextAnchor(shape);
                anchor->setOffset( shape->position() );
                anchor->setAlignment(KoTextAnchor::TopOfFrame);
                anchor->setAlignment(KoTextAnchor::Left);
                KoTextDocumentLayout *layout = dynamic_cast<KoTextDocumentLayout*> ( cursor.block().document()->documentLayout() );
                Q_ASSERT(layout && layout->inlineObjectTextManager());
                layout->inlineObjectTextManager()->insertInlineObject(cursor, anchor);
            }
            else if( anchortype == "as-char" ) {
                // There is no anchor position. The drawing shape behaves like a character.
                // The shape appears at the position where the character appears in the document.
                anchor = new KoTextAnchor(shape);
                KoTextDocumentLayout *layout = dynamic_cast<KoTextDocumentLayout*> ( cursor.block().document()->documentLayout() );
                Q_ASSERT(layout && layout->inlineObjectTextManager());
                layout->inlineObjectTextManager()->insertInlineObject(cursor, anchor);
            }
            else
                kWarning(32001)<<"KWOdfLoader::loadImage Unknown anchor-type: "<<anchortype<<endl;
            if (anchor)
                imageFrame->attachAnchor(anchor);

            return shape;
        }
        
        virtual KoShape* loadTextShape(KWLoadingContext& context, const KoXmlElement& frameElem, const KoXmlElement& textElem, QTextCursor& cursor) {
#if 0 // TODO do differently not frame loading for now
            const QString anchortype = frameElem.attribute("anchor-type");
            KoShapeFactory *factory = KoShapeRegistry::instance()->value(TextShape_SHAPEID);
            Q_ASSERT(factory);
            KoProperties props;
            KoShape *shape = factory->createShape(&props);
            if (!shape) {
                kDebug(32001) << "No text shape";
                return 0;
            }
            KWTextFrameSet* fs = new KWTextFrameSet(m_loader->document());
            // fs->setName is needed ? I hope it isn't...
            fs->setName("test-textbox");
            
            QTextCursor frameCursor( fs->document() );
            m_loader->loadBody(context, textElem, frameCursor);
            
            KWTextFrame *textFrame = new KWTextFrame(shape, fs);
            
            if( anchortype == "paragraph" ) {
                // Anchor position is the paragraph that the current drawing shape element is contained in.
                // The shape appears at the start of the paragraph element.
                KoTextAnchor* anchor = new KoTextAnchor(shape);
                anchor->setOffset( shape->position() );
                anchor->setAlignment(KoTextAnchor::HorizontalOffset);
                anchor->setAlignment(KoTextAnchor::VerticalOffset);
                KoTextDocumentLayout *layout = dynamic_cast<KoTextDocumentLayout*> ( cursor.block().document()->documentLayout() );
                Q_ASSERT(layout && layout->inlineObjectTextManager());
                layout->inlineObjectTextManager()->insertInlineObject(cursor, anchor);
            } else {
                kDebug(32001) << "Support for '" << anchortype << "' is going to wait.";
            }
            return shape;
#else
            return 0;
#endif
        }

        virtual KoShape * loadTableShape(KWLoadingContext& context, const KoXmlElement& frameElem, const KoXmlElement& textElem, QTextCursor& cursor) {
            KoShapeFactory * factory = KoShapeRegistry::instance()->value(TableShape_SHAPEID);
            if (!factory) {
                kDebug(32001) << "No table shape";
                return 0;
            }
            
            KoProperties props;
            // this would need to pass a shape controller but as this needs to be reworked do nothing now
            KoShape *shape = factory->createShape( &props, 0 );
            
            if (!shape) {
                kDebug(32001) << "No table shape";
                return 0;
            }
kDebug(32001) << "HOOOO Look at me, I've got a table shape !!! Incredible." << shape;
            return shape;
        }
        

    private:
        KWOdfLoader* m_loader;
};
#endif
