/*
 * This file is part of KWord
 *
 * Copyright (c) 2006 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "Module.h"
#include "Page.h"
#include "PageLayout.h"
#include "FrameSet.h"

#include <QPointer>
#include <QApplication>
#include <kdebug.h>

#include <KoDocumentAdaptor.h>
#include <KoApplicationAdaptor.h>

#include <KWDocument.h>
#include <KWView.h>
#include <KWPage.h>
#include <KWFrameSet.h>
#include <KWFrame.h>

extern "C"
{
    KDE_EXPORT QObject* krossmodule()
    {
        return new Scripting::Module();
    }
}

using namespace Scripting;

namespace Scripting {

    /// \internal d-pointer class.
    class Module::Private
    {
        public:
            QPointer<KWView> view;
            QPointer<KWDocument> doc;
    };
}

Module::Module()
    : QObject()
    , d( new Private() )
{
    setObjectName("KWordScriptingModule");
    d->view = 0;
    d->doc = 0;
}

Module::~Module()
{
    delete d;
}

KWDocument* Module::doc()
{
    if(! d->doc)
        d->doc = d->view ? d->view->kwdocument() : new KWDocument(0, this);
    return d->doc;
}

void Module::setView(KWView* view)
{
    d->view = view;
}

QObject* Module::application()
{
    return qApp->findChild< KoApplicationAdaptor* >();
}

QObject* Module::document()
{
    return doc()->findChild< KoDocumentAdaptor* >();
}

int Module::pageCount()
{
    return doc()->pageCount();
}

QObject* Module::page(int pageNumber)
{
    KWPage* page = doc()->pageManager()->page(pageNumber);
    return page ? new Page(this, page) : 0;
}

QObject* Module::insertPage( int afterPageNum )
{
    //TODO check for doc()->pageManager()->onlyAllowAppend()
    KWPage* page = doc()->insertPage(afterPageNum);
    return page ? new Page(this, page) : 0;
}

void Module::removePage( int pageNumber )
{
    //TODO remove also the wrapper? and what's about pages that are "Spread" (page that represents 2 pagenumbers)?
    doc()->removePage(pageNumber);
}

int Module::startPage()
{
    return doc()->startPage();
}

void Module::setStartPage(int pageNumber)
{
     //TODO this is evil since it changes page(int pageNumber) above... we need a more persistent way to deal with pages!
     doc()->setStartPage(pageNumber);
}

QStringList Module::shapeKeys()
{
    QStringList keys;
    foreach(QString key, KoShapeRegistry::instance()->keys())
        keys.append(key);
    return keys;
}

int Module::frameSetCount()
{
    return doc()->frameSetCount();
}

QObject* Module::frameSet(int frameSetNr)
{
    KWFrameSet* frameset = (frameSetNr>=0 && frameSetNr<doc()->frameSetCount()) ? doc()->frameSets().at(frameSetNr) : 0;
    return frameset ? new FrameSet(this, frameset) : 0;
}

QObject* Module::addTextFrameSet(const QString& framesetname)
{
    KWord::TextFrameSetType type = KWord::OtherTextFrameSet;
    /*
    type = KWord::MainTextFrameSet;
    type = KWord::FirstPageHeaderTextFrameSet;
    type = KWord::FirstPageHeaderTextFrameSet;
    type = KWord::EvenPagesHeaderTextFrameSet;
    type = KWord::OddPagesHeaderTextFrameSet;
    type = KWord::FirstPageFooterTextFrameSet;
    type = KWord::EvenPagesFooterTextFrameSet;
    type = KWord::OddPagesFooterTextFrameSet;
    type = KWord::FootNoteTextFrameSet;
    type = KWord::OtherTextFrameSet;
    */

    KWTextFrameSet* frameset = new KWTextFrameSet(doc(), type);
    frameset->setName(framesetname);
    frameset->setAllowLayout(false);
    doc()->addFrameSet(frameset);
    return new FrameSet(this, frameset);
}

QObject* Module::addFrameSet(const QString& framesetname)
{
    KWFrameSet* frameset = new KWFrameSet();
    frameset->setName(framesetname);
    doc()->addFrameSet(frameset);
    return new FrameSet(this, frameset);
}

int Module::frameCount()
{
    int count = 0;
    foreach(KWFrameSet* set, doc()->frameSets())
        count += set->frames().count();
    return count;
}

QObject* Module::frame(int frameNr)
{
    if(frameNr >= 0) {
        int idx = 0;
        foreach(KWFrameSet* set, doc()->frameSets()) {
            const int c = set->frames().count();
            if(frameNr < idx + c)
                return new Frame(new FrameSet(this, set), set->frames().at(idx));
            idx += c;
        }
    }
    return 0;
}

QObject* Module::addTextFrame(const QString& framesetname)
{
    FrameSet* set = dynamic_cast< FrameSet* >( addTextFrameSet(framesetname) );
    return set ? set->addTextFrame() : 0;
}

QObject* Module::addFrame(const QString& framesetname, const QString& shapeId)
{
    FrameSet* set = dynamic_cast< FrameSet* >( addFrameSet(framesetname) );
    return set ? set->addFrame(shapeId) : 0;
}

QObject* Module::standardPageLayout()
{
    return new PageLayout(this, KoPageLayout::standardLayout());
}

QObject* Module::defaultPageLayout()
{
    const KoPageLayout *pagelayout = doc()->pageManager()->defaultPage();
    return pagelayout ? new PageLayout(this, *pagelayout) : 0;
}

void Module::setDefaultPageLayout(QObject* pagelayout)
{
    PageLayout* l = dynamic_cast<PageLayout*>( pagelayout );
    if( l )
        doc()->setDefaultPageLayout( l->pageLayout() );
}

#include "Module.moc"
