/*
 *
 */

#ifndef LINKLAYER_H
#define LINKLAYER_H

#include <QtDeclarative/qdeclarativeitem.h>

class CQLinkArea : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QVariantList links READ links WRITE setLinks NOTIFY linksChanged)
    Q_PROPERTY(QSizeF sourceSize READ sourceSize WRITE setSourceSize NOTIFY sourceSizeChanged)
    Q_PROPERTY(QColor linkColor READ linkColor WRITE setLinkColor NOTIFY linkColorChanged)

public:
    explicit CQLinkArea(QDeclarativeItem* parent = 0);
    virtual ~CQLinkArea();
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* );

    QVariantList links() const;
    void setLinks(const QVariantList& newLinks);

    QSizeF sourceSize() const;
    void setSourceSize( const QSizeF& size );

    QColor linkColor() const;
    void setLinkColor( const QColor& color );

Q_SIGNALS:
    void linksChanged();
    void clicked();
    void doubleClicked();
    void linkClicked(QUrl linkTarget);
    void sourceSizeChanged();
    void linkColorChanged();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
    class Private;
    Private* d;
};

#endif // LINKLAYER_H
