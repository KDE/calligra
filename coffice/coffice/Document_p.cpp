#include "Document_p.h"

#include <KWPart.h>
#include <KWView.h>
#include <KWDocument.h>
#include <Words.h>
#include <KWCanvasItem.h>
#include <KWPage.h>
#include <KWPageManager.h>

Document::Private::Private(Document *qq)
    : QObject()
    , q(qq)
    , m_kopart(0)
    , m_progressProxy(new DocumentProgressProxy(q))
{
    qRegisterMetaType<PageDef>("PageDef");
    qRegisterMetaType<PageDefList>("PageDefList");
}

Document::Private::~Private()
{
    if (m_kopart) {
        QMetaObject::invokeMethod(m_kopart, "deleteLater", Qt::QueuedConnection);
    }
}

// runs in the AppManager thread
void Document::Private::slotPageSetupChanged()
{
    qDebug() << Q_FUNC_INFO;
}

// runs in the AppManager thread
void Document::Private::slotLayoutFinished()
{
    qDebug() << Q_FUNC_INFO;

    Q_ASSERT(m_kopart);
    KWDocument *kwdoc = dynamic_cast<KWDocument*>(m_kopart->document());
    Q_ASSERT(kwdoc);
    KWPageManager *kwPageManager = kwdoc->pageManager();
    Q_ASSERT(kwPageManager);

    PageDefList pages;
    for(int i = 1; i <= kwPageManager->pageCount(); ++i) {
        KWPage kwpage = kwPageManager->page(i);
        Q_ASSERT(kwpage.isValid());
        pages.append(PageDef(kwpage.pageNumber(), kwpage.rect()));
    }

    QMetaObject::invokeMethod(this, "slotLayoutFinished2", Qt::QueuedConnection, Q_ARG(PageDefList, pages));
}

// runs in the UI-thread (same thread the Document instance itself is attached to)
void Document::Private::slotLayoutFinished2(PageDefList pages)
{
    qDebug() << Q_FUNC_INFO << pages.count();

    m_pages.clear();
    for(PageDefList::ConstIterator it = pages.constBegin(); it != pages.constEnd(); ++it) {
        const PageDef &p = *it;
        QSharedPointer<Page> page(new Page(q, p.first, p.second));
        m_pages.append(page);
    }

    Q_EMIT q->layoutFinished();
}

void Document::Private::slotThumbnailFinished(int pageNumber, const QImage &image)
{
    qDebug() << Q_FUNC_INFO << pageNumber;
    QSharedPointer<Page> page = m_pages.value(pageNumber - 1);
    QMetaObject::invokeMethod(page.data(), "thumbnailFinished", Qt::QueuedConnection, Q_ARG(QImage, image));
}
