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

void Page::markDirty()
{
    d->m_isDirty = true;
}

void Page::maybeUpdateThumbnail()
{
    if (d->m_isDirty) {
        d->m_isDirty = false;
        forceUpdateThumbnail();
    }
}

void Page::forceUpdateThumbnail()
{
    struct DebugHelper {
        DebugHelper() { qDebug() << Q_FUNC_INFO << "START"; }
        ~DebugHelper() { qDebug() << Q_FUNC_INFO << "DONE"; }
    };
    DebugHelper debugHelper;
    s_appManager()->update(this);
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

bool Document::openFile(const QString &file)
{
    bool ok = s_appManager()->openFile(this, file);
    return ok;
}

void Document::emitProgressUpdated(int percent)
{
    Q_EMIT progressUpdated(percent);
}
