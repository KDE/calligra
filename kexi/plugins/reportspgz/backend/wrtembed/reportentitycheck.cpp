#include "reportentitycheck.h"
#include "reportentities.h"
#include "reportdesigner.h"

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <klocalizedstring.h>
#include <koproperty/editor.h>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

//
// class ReportEntityCheck
//

void ReportEntityCheck::init(QGraphicsScene * scene)
{
    setFlags(ItemIsSelectable | ItemIsMovable);

    if (scene)
        scene->addItem(this);

    ReportRectEntity::init(&_pos, &_size, _set);

    connect(properties(), SIGNAL(propertyChanged(KoProperty::Set &, KoProperty::Property &)), this, SLOT(propertyChanged(KoProperty::Set &, KoProperty::Property &)));

    setZValue(Z);
}

// methods (constructors)
ReportEntityCheck::ReportEntityCheck(ReportDesigner* d, QGraphicsScene * scene)
        : ReportRectEntity(d)
{
    init(scene);
    setSceneRect(QPointF(0,0),QSizeF(15,15)); //default size

    _name->setValue(_rd->suggestEntityName("Check"));

}

ReportEntityCheck::ReportEntityCheck(QDomNode & element, ReportDesigner * d, QGraphicsScene * s)
        : ReportRectEntity(d), KRCheckData(element)
{
    init(s);
    setSceneRect(_pos.toScene(), _size.toScene());
}

ReportEntityCheck* ReportEntityCheck::clone()
{
    QDomDocument d;
    QDomElement e = d.createElement("clone");;
    QDomNode n;
    buildXML(d, e);
    n = e.firstChild();
    return new ReportEntityCheck(n, designer(), 0);
}

// methods (deconstructor)
ReportEntityCheck::~ReportEntityCheck()
{}

void ReportEntityCheck::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    // store any values we plan on changing so we can restore them
    QFont f = painter->font();
    QPen  p = painter->pen();
    QBrush b = painter->brush();

    painter->setBackgroundMode ( Qt::OpaqueMode );
    painter->setRenderHint(QPainter::Antialiasing);
    QColor bg = _bgColor->value().value<QColor>();
    bg.setAlpha(_bgOpacity->value().toInt());

    painter->setBackground(bg);
    painter->setPen(_fgColor->value().value<QColor>());

    if ((Qt::PenStyle)_lnStyle->value().toInt() == Qt::NoPen || _lnWeight->value().toInt() <= 0) {
        painter->setPen(QPen(QColor(224, 224, 224)));
    } else {
        painter->setPen(QPen(_lnColor->value().value<QColor>(), _lnWeight->value().toInt(), (Qt::PenStyle)_lnStyle->value().toInt()));
    }

    qreal ox = _size.toScene().width()/5;
    qreal oy = _size.toScene().height()/5;

    //Checkbox Style
    if (_checkStyle->value().toString() == "Cross"){
        painter->drawRoundedRect(QGraphicsRectItem::rect(), _size.toScene().width()/10 , _size.toScene().height()/10);

        QPen lp;
        lp.setColor(_fgColor->value().value<QColor>());
        lp.setWidth(ox > oy ? oy : ox);
        painter->setPen(lp);
        painter->drawLine(ox,oy, _size.toScene().width() - ox, _size.toScene().height() - oy);
        painter->drawLine(ox, _size.toScene().height() - oy, _size.toScene().width() - ox, oy);
    }
    else if (_checkStyle->value().toString() == "Dot"){
    //Radio Style
        painter->drawEllipse(QGraphicsRectItem::rect());

        QBrush lb(_fgColor->value().value<QColor>());
        painter->setBrush(lb);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(rect().center(), _size.toScene().width()/2 - ox, _size.toScene().height()/2 - oy);
    }
    else {
    //Tickbox Style
        painter->drawRoundedRect(QGraphicsRectItem::rect(), _size.toScene().width()/10 , _size.toScene().height()/10);

        QPen lp;
        lp.setColor(_fgColor->value().value<QColor>());
        lp.setWidth(ox > oy ? oy : ox);
        painter->setPen(lp);
        painter->drawLine(ox,_size.toScene().height()/2, _size.toScene().width() / 2, _size.toScene().height() - oy);
        painter->drawLine(_size.toScene().width() / 2, _size.toScene().height() - oy, _size.toScene().width() - ox, oy);

    }

    painter->setBackgroundMode(Qt::TransparentMode);
    painter->setPen(_fgColor->value().value<QColor>());

    // restore an values before we started just in case
    painter->setFont(f);
    painter->setPen(p);
    painter->setBrush(b);

    drawHandles(painter);
}

void ReportEntityCheck::buildXML(QDomDocument & doc, QDomElement & parent)
{
    kDebug() << endl;

    QDomElement entity = doc.createElement("check");

    // bounding rect
    buildXMLRect(doc, entity, pointRect());

    // name
    QDomElement n = doc.createElement("name");
    n.appendChild(doc.createTextNode(entityName()));
    entity.appendChild(n);

    // z
    QDomElement z = doc.createElement("zvalue");
    z.appendChild(doc.createTextNode(QString::number(zValue())));
    entity.appendChild(z);

    //Line Style
    buildXMLLineStyle(doc, entity, lineStyle());

    QDomElement cs = doc.createElement("controlsource");
    cs.appendChild(doc.createTextNode(_controlSource->value().toString()));
    entity.appendChild(cs);

    // the check style
    QDomElement sty = doc.createElement("checkstyle");
    sty.appendChild(doc.createTextNode(_checkStyle->value().toString()));
    entity.appendChild(sty);

    parent.appendChild(entity);
}

void ReportEntityCheck::propertyChanged(KoProperty::Set &s, KoProperty::Property &p)
{
    kDebug() << endl;
    //TODO KoProperty needs QPointF and QSizeF and need to sync property with actual size/pos
    if (p.name() == "Position") {
        //_pos.setUnitPos(p.value().value<QPointF>(), false);
    } else if (p.name() == "Size") {
        //_size.setUnitSize(p.value().value<QSizeF>());
    } else if (p.name() == "Name") {
        //For some reason p.oldValue returns an empty string
        if (!_rd->isEntityNameUnique(p.value().toString(), this)) {
            p.setValue(_oldName);
        } else {
            _oldName = p.value().toString();
        }
    }

    //setSceneRect(_pos.toScene(), _size.toScene());

    if (_rd) _rd->setModified(true);
    if (scene()) scene()->update();
}

void ReportEntityCheck::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    _controlSource->setListData(_rd->fieldList(), _rd->fieldList());
    ReportRectEntity::mousePressEvent(event);
}
