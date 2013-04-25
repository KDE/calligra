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
    PageItem(DocumentItem *view, const QSharedPointer<Page> &page);
    virtual ~PageItem();
    QSharedPointer<Page> page() const;
    virtual QRectF boundingRect() const;
    //void prepareGeometryChange() { QGraphicsPixmapItem::prepareGeometryChange(); }

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private Q_SLOTS:
    void slotThumbnailFinished(const QImage &image);

private:
    DocumentItem *m_view;
    QSharedPointer<Page> m_page;
};

class DocumentItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
public:
    explicit DocumentItem(QObject *parentObject = 0, QGraphicsItem *parentItem = 0);
    virtual ~DocumentItem();

    Document* document() const { return m_doc; }

    qreal pageMargin() const { return m_margin; }
    void setPageMargin(qreal margin) { m_margin = margin; }

    qreal pageSpacing() const { return m_spacing; }
    void setPageSpacing(qreal spacing) { m_spacing = spacing; }

    QColor pageColor() const { return m_color; }
    void setPageColor(const QColor &color) { m_color = color; }

    qreal zoom() const;
    void setZoom(qreal factor);

    void setZoomBegin();
    void setZoomEnd();

    virtual QRectF boundingRect() const;
    bool openFile(const QString &file);

signals:
    void sizeChanged();

private Q_SLOTS:
    void slotLayoutFinished();

private:
    QColor m_color;
    Document *m_doc;
    qreal m_originalWidth, m_originalHeight;
    qreal m_width, m_height;
    qreal m_margin, m_spacing;
    qreal m_zoom;
    qreal m_zoomTemp;
    int m_zoomTempBegin;

    void updateSize();
};

class DocumentView : public QDeclarativeItem
{
    Q_OBJECT
    //Q_PROPERTY(bool pinchEnabled READ pinchEnabled NOTIFY pinchEnabledChanged)

    Q_PROPERTY(qreal pageMargin READ pageMargin WRITE setPageMargin NOTIFY pageMarginChanged)
    Q_PROPERTY(qreal pageSpacing READ pageSpacing WRITE setPageSpacing NOTIFY pageSpacingChanged)
    Q_PROPERTY(QColor pageColor READ pageColor WRITE setPageColor NOTIFY pageColorChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

public:
    explicit DocumentView(QDeclarativeItem *parent = 0);
    virtual ~DocumentView();

    virtual QRectF boundingRect() const;

    qreal pageMargin() const;
    void setPageMargin(qreal margin);

    qreal pageSpacing() const;
    void setPageSpacing(qreal spacing);

    QColor pageColor() const;
    void setPageColor(const QColor &color);

    qreal zoom() const;
    void setZoom(qreal factor);

Q_SIGNALS:
    void pageMarginChanged();
    void pageSpacingChanged();
    void pageColorChanged();
    void zoomChanged();

    void openFileFailed(const QString &file, const QString &error);
    void progressUpdated(int percent);

public Q_SLOTS:
    QPointF pos() const;
    void setPos(const QPointF &position);

    void setZoomBegin();
    void setZoomEnd();
    //bool isZoomToFit() const;
    //void zoomToCenter(qreal factor = 1.0);
    //void zoomToFit();

    QString file() const;
    bool openFile(const QString &file);

private Q_SLOTS:
    void slotSizeChanged();
protected:
    virtual bool sceneEvent(QEvent *event);
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);
private:
    DocumentItem *m_doc;
    //bool m_pinchEnabled;
};

#endif // DOCUMENTVIEW_H
