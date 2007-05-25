#include "Intro.h"

#include "DivineProportionShape.h"

Intro::Intro() {
    widget.setupUi(this);
}

void Intro::open(KoShape *shape) {
    m_shape = dynamic_cast<DivineProportionShape*> (shape);
    widget.topRight->setChecked(true);
}

void Intro::save() {
    if(m_shape == 0)
        return;
    Q_ASSERT(m_resourceProvider);
    if(m_resourceProvider->hasResource(KoCanvasResource::PageSize)) {
        QSizeF size = m_resourceProvider->sizeResource(KoCanvasResource::PageSize);
        if(size.height() > size.width()) {
            m_shape->resize(QSizeF(size.height(), size.width()));
            m_shape->rotate(-90);
            m_shape->setAbsolutePosition(QPointF(size.width() / 2, size.height() / 2));
        }
        else {
            m_shape->setPosition(QPointF(0, 0));
            m_shape->resize(size);
        }
    }

    DivineProportionShape::Orientation orientation;
    if(widget.topLeft->isChecked())
        orientation = DivineProportionShape::TopLeft;
    else if(widget.topRight->isChecked())
        orientation = DivineProportionShape::TopRight;
    else if(widget.bottomLeft->isChecked())
        orientation = DivineProportionShape::BottomLeft;
    else
        orientation = DivineProportionShape::BottomRight;
    m_shape->setOrientation(orientation);
    m_shape->setPrintable(widget.printable->isChecked());
}

KAction *Intro::createAction() {
    return 0;
}
