#include "Document.h"
#include "Document_p.h"
#include "AppManager.h"

/**************************************************************************
 * Page
 */

class Page::Private
{
public:
    Document *m_doc;
    int m_pageNumber;
    QRectF m_rect;
    bool m_isDirty;
    Private(Document *doc, int pageNumber, const QRectF &rect) : m_doc(doc), m_pageNumber(pageNumber), m_rect(rect), m_isDirty(true) {}
};

Page::Page(Document *doc, int pageNumber, const QRectF &rect)
    : QObject(doc)
    , d(new Private(doc, pageNumber, rect))
{
}

Page::~Page()
{
    delete d;
}

Document* Page::doc() const
{
    return d->m_doc;
}

int Page::pageNumber() const
{
    return d->m_pageNumber;
}

QRectF Page::rect() const
{
    return d->m_rect;
}

bool Page::isDirty() const
{
    return d->m_isDirty;
}

void Page::setDirty(bool dirty)
{
    d->m_isDirty = dirty;
}

/**************************************************************************
 * Document
 */

Document::Document(QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
}

Document::~Document()
{
    delete d->m_progressProxy;
    delete d;
}

QList< QSharedPointer<Page> > Document::pages() const
{
    return d->m_pages;
}

QString Document::file() const
{
    return d->m_file;
}

bool Document::openFile(const QString &file)
{
    d->m_file = file;
    bool ok = s_appManager()->openFile(this, file);
    if (!ok)
        d->m_file.clear();
    return ok;
}

void Document::emitProgressUpdated(int percent)
{
    Q_EMIT progressUpdated(percent);
}

void Document::updatePage(const QSharedPointer<Page> &page)
{
    qDebug() << Q_FUNC_INFO << page->pageNumber();
    s_appManager()->update(page);
}
