#ifndef DOCUMENT_P_H
#define DOCUMENT_P_H

#include "Document.h"

#include <QMutex>

#include <KoProgressProxy.h>
#include <KoPart.h>

class DocumentProgressProxy : public KoProgressProxy {
public:
    DocumentProgressProxy(Document *doc) : m_doc(doc) {}
    ~DocumentProgressProxy() {}
    int maximum() const { return 100; }
    void setValue(int value) { m_doc->emitProgressUpdated(value); }
    void setRange(int /*minimum*/, int /*maximum*/) {}
    void setFormat(const QString &/*format*/) {}
private:
    Document *m_doc;
};

typedef QPair<int,QRectF> PageDef;
typedef QVector< PageDef > PageDefList;

class Document::Private : public QObject
{
    Q_OBJECT
public:
    Document *q;

    // the KoPart lifes in the AppManager thread
    KoPart *m_kopart;

    QMutex m_mutex;

    QList<Page*> m_pages;
    DocumentProgressProxy *m_progressProxy;

    Private(Document *qq);
    virtual ~Private();

public Q_SLOTS:
    void slotPageSetupChanged();
    void slotLayoutFinished();
    void slotLayoutFinished2(PageDefList pages);
    void slotThumbnailFinished(int pageNumber, const QImage &image);
};

Q_DECLARE_METATYPE(PageDef)
Q_DECLARE_METATYPE(PageDefList)

#endif // DOCUMENT_P_H
