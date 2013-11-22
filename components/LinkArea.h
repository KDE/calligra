/*
 *
 */

#ifndef LINKLAYER_H
#define LINKLAYER_H

#include <QtQuick/QQuickItem>

#include "Document.h"

namespace Calligra {
namespace Components {
class LinkArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY( Calligra::Components::Document* document READ document WRITE setDocument NOTIFY documentChanged )

public:
    LinkArea(QQuickItem* parent = 0);
    virtual ~LinkArea();

    Calligra::Components::Document* document() const;
    void setDocument( Calligra::Components::Document* newDocument );

Q_SIGNALS:
    void clicked();
    void doubleClicked();
    void linkClicked(QUrl linkTarget);

    void documentChanged();

protected:
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

private:
    class Private;
    Private* d;
};
} // Namespace Components
} // Namespace Calligra
#endif // LINKLAYER_H
