/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextLocator.h"

#include "KoTextPage.h"
#include "KoTextReference.h"
#include "styles/KoListStyle.h"

#include <KoShape.h>

#include "TextDebug.h"
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextInlineObject>
#include <QTextList>

// Include Q_UNSUSED classes, for building on Windows
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>

class Q_DECL_HIDDEN KoTextLocator::Private
{
public:
    Private(KoTextLocator *q)
        : q(q)
        , document(nullptr)
        , dirty(false)
        , cursorPosition(0)
        , chapterPosition(-1)
        , pageNumber(0)
    {
    }
    void update()
    {
        if (dirty == false)
            return;
        dirty = false;
        chapterPosition = -1;

        int pageTmp = pageNumber, chapterTmp = chapterPosition;
        if (document == nullptr)
            return;

        QTextBlock block = document->findBlock(cursorPosition);
        while (block.isValid()) {
            QTextList *list = block.textList();
            if (list) {
                QTextListFormat lf = list->format();
                int level = lf.intProperty(KoListStyle::Level);
                if (level == 1) {
                    chapterPosition = block.position();
                    break;
                }
            }
            block = block.previous();
        }
        /*
                KoShape *shape = shapeForPosition(document, cursorPosition);
                if (shape == 0)
                    pageNumber = -1;
                else {
                    KoTextShapeData *data = static_cast<KoTextShapeData*>(shape->userData());
                    KoTextPage* page = data->page();
                    pageNumber = page->pageNumber();
                }
        */
        if (pageTmp != pageNumber || chapterTmp != chapterPosition) {
            foreach (KoTextReference *reference, listeners)
                reference->variableMoved(nullptr, 0);
        }
    }

    KoTextLocator *q;
    const QTextDocument *document;
    bool dirty;
    int cursorPosition;
    int chapterPosition;
    int pageNumber;

    QList<KoTextReference *> listeners;
};

KoTextLocator::KoTextLocator()
    : KoInlineObject(false)
    , d(new Private(this))
{
}

KoTextLocator::~KoTextLocator()
{
    delete d;
}

void KoTextLocator::updatePosition(const QTextDocument *document, int posInDocument, const QTextCharFormat &format)
{
    Q_UNUSED(format);
    if (d->document != document || d->cursorPosition != posInDocument) {
        d->dirty = true;
        d->document = document;
        d->cursorPosition = posInDocument;
        // debugText <<"KoTextLocator page:" << pageNumber() <<", chapter:" << chapter() <<", '" << word() <<"'";
    }
}

void KoTextLocator::resize(const QTextDocument *document, QTextInlineObject &object, int posInDocument, const QTextCharFormat &format, QPaintDevice *pd)
{
    Q_UNUSED(document);
    Q_UNUSED(posInDocument);
    Q_UNUSED(format);
    Q_UNUSED(pd);
    object.setWidth(0);
    object.setAscent(0);
    object.setDescent(0);
}

void KoTextLocator::paint(QPainter &, QPaintDevice *, const QTextDocument *, const QRectF &, const QTextInlineObject &, int, const QTextCharFormat &)
{
    // nothing to paint.
}

QString KoTextLocator::chapter() const
{
    d->update();
    if (d->chapterPosition < 0)
        return QString();
    QTextBlock block = d->document->findBlock(d->chapterPosition);
    return block.text().remove(QChar::ObjectReplacementCharacter);
}

int KoTextLocator::pageNumber() const
{
    d->update();
    return d->pageNumber;
}

int KoTextLocator::indexPosition() const
{
    return d->cursorPosition;
}

QString KoTextLocator::word() const
{
    if (d->document == nullptr) // layout never started
        return QString();
    QTextCursor cursor(const_cast<QTextDocument *>(d->document));
    cursor.setPosition(d->cursorPosition);
    cursor.movePosition(QTextCursor::NextWord);
    cursor.movePosition(QTextCursor::WordLeft, QTextCursor::KeepAnchor);
    return cursor.selectedText().trimmed().remove(QChar::ObjectReplacementCharacter);
}

void KoTextLocator::addListener(KoTextReference *reference)
{
    d->listeners.append(reference);
}

void KoTextLocator::removeListener(KoTextReference *reference)
{
    d->listeners.removeAll(reference);
}

bool KoTextLocator::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    Q_UNUSED(element);
    Q_UNUSED(context);
    // TODO
    return false;
}

void KoTextLocator::saveOdf(KoShapeSavingContext &context)
{
    Q_UNUSED(context);
    // TODO
}
