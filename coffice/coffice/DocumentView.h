#ifndef DOCUMENTVIEW_H
#define DOCUMENTVIEW_H

#include <QThread>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QDeclarativeItem>

class KoPart;
class KoView;
class KoDocument;
class KWPage;

class Page;
class Document;
class DocumentItem;
class OpenFileCommand;
class AppManager;

class PageItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    PageItem(DocumentItem *view, Page *page);
    virtual ~PageItem();
    Page* page() const;
    virtual QRectF boundingRect() const;
protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
private Q_SLOTS:
    void slotThumbnailFinished(const QImage &image);
private:
    DocumentItem *m_view;
    Page *m_page;
};

class DocumentItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit DocumentItem(QObject *parentObject = 0, QGraphicsItem *parentItem = 0);
    virtual ~DocumentItem();
    Document* document() const { return m_doc; }
    virtual QRectF boundingRect() const;
    bool openFile(const QString &file);
signals:
    void sizeChanged();
private Q_SLOTS:
    void slotLayoutFinished();
private:
    Document *m_doc;
    qreal m_width, m_height;
    qreal m_margin, m_spacing;
};

class DocumentView : public QDeclarativeItem
{
    Q_OBJECT
    //Q_PROPERTY(bool pinchEnabled READ pinchEnabled NOTIFY pinchEnabledChanged)
public:
    explicit DocumentView(QDeclarativeItem *parent = 0);
    virtual ~DocumentView();
    virtual QRectF boundingRect() const;
Q_SIGNALS:
    void progressUpdated(int percent);
public Q_SLOTS:
    QPointF pos() const;
    void setPos(const QPointF &position);
    qreal zoom() const;
    //bool isZoomToFit() const;
    void setZoom(qreal factor);
    //void zoomToCenter(qreal factor = 1.0);
    //void zoomToFit();
    bool openFile(const QString &file);
private Q_SLOTS:
    void slotSizeChanged();
protected:
    virtual bool sceneEvent(QEvent *event);
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    DocumentItem *m_doc;
    //qreal m_totalScaleFactor;
    //bool m_pinchEnabled;
};

#endif // DOCUMENTVIEW_H
