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

public:
    explicit CQLinkArea(QDeclarativeItem* parent = 0);
    virtual ~CQLinkArea();
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget* );

    QVariantList links() const;
    void setLinks(const QVariantList& newLinks);

Q_SIGNALS:
    void linksChanged();
    void clicked();
    void doubleClicked();
    void linkClicked(QUrl linkTarget);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

private:
    class Private;
    Private* d;
};

#endif // LINKLAYER_H
