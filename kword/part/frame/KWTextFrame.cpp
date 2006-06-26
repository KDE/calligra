/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#include "KWTextFrame.h"
#include "KWTextFrameSet.h"

#include <KoViewConverter.h>
#include <KoTextShapeData.h>

#include <QTextFrame>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextFragment>
#include <kdebug.h>

KWTextFrame::KWTextFrame(KoShape *shape, KWFrameSet *parent)
    : KWFrame(shape, parent),
    m_minimumFrameHeight( 2 ),
    m_drawFootNoteLine( false ),
    m_documentOffset( 0 )
{
    KoTextShapeData *data = dynamic_cast<KoTextShapeData*> (shape->userData());
    QTextFrame *root = data->document()->rootFrame();
    class Printer {
      public:
        Printer() { }
        QString createBlank(int len) {
            QString blank;
            for(int i=0; i < len; i++)
                blank = blank + " ";
            return blank;
        }
        void printFragment(QTextFragment fragment, int inset=0) {
            if(!fragment.isValid()) return;
            QString blank = createBlank(inset);
            kDebug() << blank << "Fragment" <<endl;
            kDebug() << blank << "  '"  << fragment.text() << "'" << endl;
        }
        void printBlock(QTextBlock block, int inset) {
            if(!block.isValid()) return;
            QString blank = createBlank(inset);
            kDebug() << blank << "Block" << endl;
            //kDebug() << blank << "  '" << block.text() << "'" << endl;
            QTextBlock::Iterator iter = block.begin();
            while(!iter.atEnd()) {
                printFragment(iter.fragment(), inset+1);
                iter++;
            }
        }
        void printFrame(QTextFrame *frame, int inset=0) {
            if(frame==0) return;
            QString blank = createBlank(inset);
           //foreach(QTextFrame *child, frame->childFrames()) {
           //    kDebug() << blank << "Frame" << endl;
           //    printFrame(child, inset+1);
           //}
           //kDebug() << blank << "---------" << endl;
            QTextFrame::Iterator iter = frame->begin();
            while(! iter.atEnd()) {
                printFrame(iter.currentFrame(), inset+1);
                printBlock(iter.currentBlock(), inset+1);
                iter++;
            }
        }
    };

    Printer printer;
    printer.printFrame(root);
}

KWTextFrame::~KWTextFrame() {
}
