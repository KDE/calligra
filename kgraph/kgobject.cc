// kinda dummy file at the moment :)
#include <kgobject.h>
#include <qdom.h>


KGObject::~KGObject() {
}

QDomElement KGObject::save(const QDomDocument &/*doc*/) const {
    return QDomElement();
}

void KGObject::setOrigin(const QPoint &/*origin*/) {    
}

void KGObject::moveX(const int &/*dx*/) {    
}

void KGObject::moveY(const int &/*dy*/) {    
}

void KGObject::move(const QSize &/*d*/) {    
}

void KGObject::setState(const STATE /*state*/) {    
}

void KGObject::setGroup(const KGGroup */*group*/) {
}

void KGObject::setBrush(const QBrush &/*brush*/) {    
}

void KGObject::setPen(const QPen &/*pen*/) {    
}

#include <kgobject.moc>
