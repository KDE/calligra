#include "Document.h"
#include "Document_p.h"
#include "AppManager.h"

#include <QReadWriteLock>

/**************************************************************************
 * Page
 */

class Page::Private
{
public:
    Document *m_doc;
    int m_pageNumber;
    QRectF m_originalRect;
    QRectF m_rect;
    QReadWriteLock m_rectLock;
    bool m_isDirty;
    Private(Document *doc, int pageNumber, const QRectF &rect) : m_doc(doc), m_pageNumber(pageNumber), m_originalRect(rect), m_rect(rect), m_isDirty(true) {}
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

QRectF Page::originalRect() const
{
    return d->m_originalRect;
}

QRectF Page::rect() const
{
    QRectF r;
    {
        QReadLocker locker(&d->m_rectLock);
        r = d->m_rect;
    }
    return r;
}

void Page::setRect(const QRectF &r)
{
    {
        QWriteLocker locker(&d->m_rectLock);
        d->m_rect = r;
    }
    setDirty(true);
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
    QUrl url(file);
    d->m_file = url.toLocalFile();
    bool ok = s_appManager()->openFile(this, d->m_file);
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
