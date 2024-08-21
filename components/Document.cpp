/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "Document.h"

#include <QDebug>
#include <QPoint>
#include <QSizeF>
#include <QUrl>

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoDocument.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoTextEditor.h>

#include "impl/PresentationImpl.h"
#include "impl/SpreadsheetImpl.h"
#include "impl/TextDocumentImpl.h"

using namespace Calligra::Components;

class Document::Private
{
public:
    Private(Document *qq)
        : q{qq}
        , impl{nullptr}
        , status{DocumentStatus::Unloaded}
        , readOnly{false}
    {
    }

    void updateImpl();

    Document *q;

    QUrl source;
    DocumentImpl *impl;
    DocumentStatus::Status status;
    bool readOnly;
};

Document::Document(QObject *parent)
    : QObject{parent}
    , d{new Private{this}}
{
}

Document::~Document()
{
    delete d;
}

QUrl Document::source() const
{
    return d->source;
}

void Document::setSource(const QUrl &value)
{
    if (value != d->source) {
        d->source = value;
        Q_EMIT sourceChanged();

        d->status = DocumentStatus::Loading;
        Q_EMIT statusChanged();

        d->updateImpl();
        Q_EMIT documentTypeChanged();

        if (d->impl) {
            d->impl->setReadOnly(d->readOnly);
            if (d->impl->load(d->source)) {
                d->status = DocumentStatus::Loaded;
                connect(d->impl->canvasController()->canvas()->shapeManager(), &KoShapeManager::selectionChanged, this, &Document::textEditorChanged);
            } else {
                d->status = DocumentStatus::Failed;
            }
        } else {
            d->status = DocumentStatus::Unloaded;
        }

        Q_EMIT indexCountChanged();
        Q_EMIT statusChanged();
    }
}

bool Document::readOnly() const
{
    return d->readOnly;
}

void Document::setReadOnly(bool readOnly)
{
    if (d->readOnly != readOnly) {
        d->readOnly = readOnly;

        Q_EMIT readOnlyChanged();
    }
}

DocumentType::Type Document::documentType() const
{
    if (d->impl) {
        return d->impl->documentType();
    }

    return DocumentType::Unknown;
}

DocumentStatus::Status Document::status() const
{
    return d->status;
}

QSize Document::documentSize() const
{
    if (d->impl) {
        return d->impl->documentSize();
    }

    return QSize{};
}

int Document::currentIndex() const
{
    if (d->impl) {
        return d->impl->currentIndex();
    }

    return -1;
}

void Document::setCurrentIndex(int newValue)
{
    if (d->impl) {
        d->impl->setCurrentIndex(newValue);
    }
}

int Document::indexCount() const
{
    if (d->impl) {
        return d->impl->indexCount();
    }

    return 0;
}

KoFindBase *Document::finder() const
{
    if (d->impl) {
        return d->impl->finder();
    }

    return nullptr;
}

QGraphicsWidget *Document::canvas() const
{
    if (d->impl) {
        return d->impl->canvas();
    }

    return nullptr;
}

KoCanvasController *Document::canvasController() const
{
    if (d->impl) {
        return d->impl->canvasController();
    }

    return nullptr;
}

KoZoomController *Document::zoomController() const
{
    if (d->impl) {
        return d->impl->zoomController();
    }

    return nullptr;
}

QObject *Document::part() const
{
    return d->impl->part();
}

QObject *Document::document() const
{
    return koDocument();
}

KoDocument *Document::koDocument() const
{
    if (d->impl) {
        return d->impl->koDocument();
    }

    return nullptr;
}

QUrl Document::urlAtPoint(const QPoint &point)
{
    if (d->impl)
        return d->impl->urlAtPoint(point);
    return QUrl();
}

QObject *Document::textEditor()
{
    if (d->impl && d->impl->canvasController()) {
        return KoTextEditor::getTextEditorFromCanvas(d->impl->canvasController()->canvas());
    }
    return nullptr;
}

void Document::deselectEverything()
{
    KoTextEditor *editor = KoTextEditor::getTextEditorFromCanvas(d->impl->canvasController()->canvas());
    if (editor) {
        editor->clearSelection();
    }
    d->impl->canvasController()->canvas()->shapeManager()->selection()->deselectAll();
    Q_EMIT requestViewUpdate();
}

void Document::Private::updateImpl()
{
    delete impl;
    impl = nullptr;

    auto type = Global::documentType(source);
    switch (type) {
    case DocumentType::TextDocument:
        impl = new TextDocumentImpl{q};
        break;
    case DocumentType::Spreadsheet:
        impl = new SpreadsheetImpl{q};
        break;
    case DocumentType::Presentation:
        impl = new PresentationImpl{q};
        break;
    default:
        break;
    }

    if (impl) {
        connect(impl, &DocumentImpl::documentSizeChanged, q, &Document::documentSizeChanged);
        connect(impl, &DocumentImpl::currentIndexChanged, q, &Document::currentIndexChanged);
        connect(impl, &DocumentImpl::requestViewUpdate, q, &Document::requestViewUpdate);
    }
    Q_EMIT q->documentChanged();
}

#include "moc_Document.cpp"
