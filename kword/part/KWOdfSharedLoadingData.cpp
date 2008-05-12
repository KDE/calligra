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

#include <QTextCursor>
#include <kdebug.h>

KWOdfSharedLoadingData::KWOdfSharedLoadingData(KWOdfLoader* loader)
    : KoTextSharedLoadingData()
    , m_loader(loader)
{
}

void KWOdfSharedLoadingData::shapeInserted(KoShape* shape)
{
    kDebug()<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";

    /*TODO
        - shape can be anything, not only an image-shape but with the current KWFrame-design
            we need to special case them, or? well, probably it would be wise to refactor
            the KWFrame+KWFrameSet logic here...
                    ==> DONE ? Now, we just use a KWFrame + KWFrameSet, no need for KWImageFrame for instance.
                        ==> imho the best way to go in the long run.
        - we also need to pass the used QTextCursor around to know in what KWFrameSet we are
            atm and where to write to. Or should we just use
            QTextCursor(m_loader->currentFrameset->document())
            each time and assume that we only need to append content anyway?
        - KoTextAnchor should be created+managed by the KoTextLoader but we need access to
            it here to be able to use KWPageManager if "anchor-type"=="page". Maybe we are
            able to utilize KoShapeApplicationData as container for such kind of objects?
                ==> Anchors are actually stored in flake ;)
                    KoTextAnchor adds itself using the KoTextShapeContainerModel::addAnchor() 
                    method. See KoTextAnchor.cpp:70
        - bring back header+footers :)
        - What about parent-styles aka style-inheritance? Does this REALLY work already?
        - table-shape vs. QTextTable vs. the layout-hacks within the textshape is still a huge
            issue of it's own. http://lists.kde.org/?l=koffice-devel&m=120574617208477&w=2 and
            http://lists.kde.org/?l=koffice-devel&m=120582471310900&w=2
        - unittests :)
    */

    //TEMP HACK
    QString anchortype = shape->additionalAttribute("anchor-type");
    KWFrameSet* fs = new KWFrameSet();
    fs->setName("My FrameSet");
    KWFrame *frame = new KWFrame(shape, fs);
    m_loader->document()->addFrameSet(fs);

    KoTextAnchor *anchor = new KoTextAnchor(shape);
    //Q_ASSERT(dynamic_cast<KoTextShapeData*>(shape->userData())); //this asserts cause shapes don't inheritate/share there userdata
    Q_ASSERT(m_loader->currentFrame());
    KWTextFrameSet* docfs = dynamic_cast<KWTextFrameSet*>(m_loader->currentFrame()->frameSet());
    Q_ASSERT(docfs);
    QTextDocument* doc = docfs->document(); //m_loader->document()->mainFrameSet()->document();

    Q_ASSERT(doc);
    QTextCursor cursor(doc);
    KoTextDocumentLayout *layout = dynamic_cast<KoTextDocumentLayout*> ( cursor.block().document()->documentLayout() );
    Q_ASSERT(layout);
    Q_ASSERT(layout->inlineObjectTextManager());
    layout->inlineObjectTextManager()->insertInlineObject(cursor, anchor);
}

