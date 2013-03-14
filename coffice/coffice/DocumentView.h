#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QRunnable>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QDeclarativeItem>

class KoPart;
class KoView;
class KoDocument;
class KWPage;

class Document;
class DocumentItem;

class Page : public QObject
{
    Q_OBJECT
public:
    Page(Document *doc, const KWPage &page);
    virtual ~Page();
    Document* doc() const;
    const KWPage &page() const;
    QRectF rect() const;
    int pageNumber() const;
public Q_SLOTS:
    void updateThumbnail();
Q_SIGNALS:
    void thumbnailFinished(const QImage &image);
private:
    class Private;
    Private *const d;
};

class PageThread : public QObject, public QRunnable
{
    Q_OBJECT
public:
    PageThread(Page *page);
    virtual void run();
Q_SIGNALS:
    void thumbnailFinished(const QImage &image);
private:
    Page *m_page;
};

class PageItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    PageItem(DocumentItem *view, Page *page);
    virtual ~PageItem();
    virtual QRectF boundingRect() const;
protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private Q_SLOTS:
    void slotThumbnailFinished(const QImage &image);
private:
    DocumentItem *m_view;
    Page *m_page;
};

class Document : public QObject
{
    Q_OBJECT
public:
    explicit Document( QObject *parent = 0);
    virtual ~Document();
    KoPart* part() const;
    bool openFile(const QString &file);
Q_SIGNALS:
    void pagesAdded(QList<Page*> page);
    void pagesRemoved();
private Q_SLOTS:
    void slotPageSetupChanged();
    void slotLayoutFinished();
private:
    class Private;
    Private *const d;
};

class DocumentItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit DocumentItem(QObject *parentObject = 0, QGraphicsItem *parentItem = 0);
    virtual ~DocumentItem();
    virtual QRectF boundingRect() const;
    bool openFile(const QString &file);
signals:
    void sizeChanged();
private Q_SLOTS:
    void slotPagesAdded(QList<Page*> pages);
    void slotPagesRemoved();
private:
    Document *m_doc;
    qreal m_width;
    qreal m_height;
    qreal m_margin;
};

class DocumentView : public QDeclarativeItem
{
    Q_OBJECT
public:
    explicit DocumentView(QDeclarativeItem *parent = 0);
    virtual ~DocumentView();
public Q_SLOTS:
    bool openFileWithDialog();
    bool openFile(const QString &file);
private Q_SLOTS:
    void slotSizeChanged();
protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
private:
    DocumentItem *m_doc;
};

#endif // DOCUMENTVIEW_H
