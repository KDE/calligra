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
        m_shape->setPosition(QPointF(0,0));
        m_shape->resize(m_resourceProvider->sizeResource(KoCanvasResource::PageSize));
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
