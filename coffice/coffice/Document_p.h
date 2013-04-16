#ifndef DOCUMENT_P_H
#define DOCUMENT_P_H

#include "Document.h"

#include <QSharedPointer>
#include <QMutex>

#include <KoProgressProxy.h>
#include <KoPart.h>

class DocumentProgressProxy : public KoProgressProxy {
public:
    DocumentProgressProxy(Document *doc) : m_doc(doc), m_subTaskDone(0), m_subTaskCount(1) {}
    ~DocumentProgressProxy() {}
    int maximum() const { return 100; }
    void setValue(int value) {
        if (value < 0) {
            if (m_subTaskDone+1 < m_subTaskCount)
                value = 100;
        }
        if (value >= 0) {
            qreal diff = qreal(100.0) / qreal(m_subTaskCount);
            value /= m_subTaskCount;
            value += m_subTaskDone * diff;
        }
        m_doc->emitProgressUpdated(value);
    }
    void setRange(int minimum, int maximum) {
        qDebug() << minimum << maximum;
        //Q_ASSERT(false);
    }
    void setFormat(const QString &/*format*/) {}
    void beginSubTask() { ++m_subTaskCount; }
    void endSubTask() { ++m_subTaskDone; }
private:
    Document *m_doc;
    int m_subTaskDone;
    int m_subTaskCount;
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

    //QMutex m_mutex;

    QList< QSharedPointer<Page> > m_pages;
    QString m_file;
    DocumentProgressProxy *m_progressProxy;

    Private(Document *qq);
    virtual ~Private();

public Q_SLOTS:
    void slotOpenFileFailed(const QString &error);
    void slotPageSetupChanged();
    void slotLayoutFinished();
    void slotLayoutFinished2(PageDefList pages);
    void slotThumbnailFinished(int pageNumber, const QImage &image);
};

Q_DECLARE_METATYPE(PageDef)
Q_DECLARE_METATYPE(PageDefList)

#endif // DOCUMENT_P_H
