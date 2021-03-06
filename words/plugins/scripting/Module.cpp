/*
 * This file is part of Words
 *
 * SPDX-FileCopyrightText: 2006 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later

#include "Module.h"
#include "Page.h"
#include "PageLayout.h"
#include "FrameSet.h"
#include "Style.h"
#include "Tool.h"

#include <KWDocument.h>
#include <KWView.h>
#include <KWPage.h>

#include <QWeakPointer>

extern "C"
{
    KDE_EXPORT QObject* krossmodule() {
        return new Scripting::Module();
    }
}

using namespace Scripting;

Module::Module(QObject* parent)
    : KoScriptingModule(parent, "Words")
{
}

Module::~Module()
{
}

KWDocument* Module::kwDoc()
{
    if (!m_doc) {
        if (KWView *v = dynamic_cast< KWView* >(view()))
            m_doc = v->kwdocument();
        if (!m_doc)
            m_doc = new KWDocument(0, this);
    }
    return m_doc.data();
}

KoDocument* Module::doc()
{
    return kwDoc();
}

int Module::pageCount()
{
    return kwDoc()->pageCount();
}

QObject* Module::page(int pageNumber)
{
    KWPage page = kwDoc()->pageManager()->page(pageNumber);
    return page.isValid() ? new Page(this, page) : 0;
}

QObject* Module::insertPage(int afterPageNum)
{
    KWPage page = kwDoc()->insertPage(afterPageNum);
    return new Page(this, page);
}

void Module::removePage(int pageNumber)
{
    //TODO remove also the wrapper? and what's about pages that are "Spread" (page that represents 2 pagenumbers)?
    kwDoc()->removePage(pageNumber);
}

QStringList Module::shapeKeys()
{
    QStringList keys;
    foreach(const QString &key, KoShapeRegistry::instance()->keys())
       keys.append(key);
    return keys;
}

int Module::frameSetCount()
{
    return kwDoc()->frameSetCount();
}

QObject* Module::frameSet(int frameSetNr)
{
    KWFrameSet* frameset = (frameSetNr >= 0 && frameSetNr < kwDoc()->frameSetCount()) ? kwDoc()->frameSets().at(frameSetNr) : 0;
    return frameset ? new FrameSet(this, frameset) : 0;
}

QObject* Module::frameSetByName(const QString& name)
{
    KWFrameSet* frameset = kwDoc()->frameSetByName(name);
    return frameset ? new FrameSet(this, frameset) : 0;
}

QObject* Module::addTextFrameSet(const QString& framesetname)
{
    Words::TextFrameSetType type = Words::OtherTextFrameSet;
    /*
    type = Words::MainTextFrameSet;
    type = Words::FirstPageHeaderTextFrameSet;
    type = Words::FirstPageHeaderTextFrameSet;
    type = Words::EvenPagesHeaderTextFrameSet;
    type = Words::OddPagesHeaderTextFrameSet;
    type = Words::FirstPageFooterTextFrameSet;
    type = Words::EvenPagesFooterTextFrameSet;
    type = Words::OddPagesFooterTextFrameSet;
    type = Words::FootNoteTextFrameSet;
    type = Words::OtherTextFrameSet;
    */

    KWTextFrameSet* frameset = new KWTextFrameSet(kwDoc(), type);
    frameset->setName(framesetname);
    frameset->setAllowLayout(false);
    kwDoc()->addFrameSet(frameset);
    return new FrameSet(this, frameset);
}

QObject* Module::addFrameSet(const QString& framesetname)
{
    KWFrameSet* frameset = new KWFrameSet();
    frameset->setName(framesetname);
    kwDoc()->addFrameSet(frameset);
    return new FrameSet(this, frameset);
}

int Module::frameCount()
{
    int count = 0;
    foreach(KWFrameSet* set, kwDoc()->frameSets())
    count += set->frames().count();
    return count;
}

QObject* Module::frame(int frameNr)
{
    if (frameNr >= 0) {
        int idx = 0;
        foreach(KWFrameSet* set, kwDoc()->frameSets()) {
            const int c = set->frames().count();
            if (frameNr < idx + c)
                return new Frame(new FrameSet(this, set), set->frames().at(idx));
            idx += c;
        }
    }
    return 0;
}

QObject* Module::findFrameSet(Words::TextFrameSetType type)
{
    foreach(KWFrameSet* set, kwDoc()->frameSets()) {
        KWTextFrameSet* textframeset = dynamic_cast< KWTextFrameSet* >(set);
        if (textframeset)
            if (textframeset->textFrameSetType() == type)
                return new FrameSet(this, textframeset);
    }
    return 0;
}

QObject* Module::standardPageLayout()
{
    KoPageLayout layout;
    return new PageLayout(this, layout);
}

QObject* Module::defaultParagraphStyle()
{
    KoStyleManager *styleManager = kwDoc()->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();

    Q_ASSERT(styleManager);
    KoParagraphStyle* s = styleManager->defaultParagraphStyle();
    return s ? new ParagraphStyle(this, s) : 0;
}

QObject* Module::paragraphStyle(const QString& name)
{
    KoStyleManager *styleManager = kwDoc()->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
    Q_ASSERT(styleManager);
    KoParagraphStyle* s = styleManager->paragraphStyle(name);
    return s ? new ParagraphStyle(this, s) : 0;
}

QObject* Module::addParagraphStyle(const QString& name)
{
    KoStyleManager *styleManager = kwDoc()->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
    Q_ASSERT(styleManager);
    KoParagraphStyle* s = new KoParagraphStyle();
    s->setName(name);
    styleManager->add(s);
    return new ParagraphStyle(this, s);
}

QObject* Module::characterStyle(const QString& name)
{
    KoStyleManager *styleManager = kwDoc()->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
    Q_ASSERT(styleManager);
    KoCharacterStyle* s = styleManager->characterStyle(name);
    return s ? new CharacterStyle(this, s) : 0;
}

QObject* Module::addCharacterStyle(const QString& name)
{
    KoStyleManager *styleManager = kwDoc()->resourceManager()->resource(KoText::StyleManager).value<KoStyleManager*>();
    Q_ASSERT(styleManager);
    KoCharacterStyle* s = new KoCharacterStyle();
    s->setName(name);
    styleManager->add(s);
    return new CharacterStyle(this, s);
}

QObject* Module::tool()
{
    return new Tool(this);
}
