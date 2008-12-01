#ifndef REPORTENTITYCHECK_H
#define REPORTENTITYCHECK_H

#include "reportrectentity.h"
#include <QGraphicsRectItem>
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <krcheckdata.h>

class ReportEntityCheck : public QObject, public ReportRectEntity, public KRCheckData
{
    Q_OBJECT
public:
    ReportEntityCheck(ReportDesigner *, QGraphicsScene * scene);
    ReportEntityCheck(QDomNode & element, ReportDesigner *, QGraphicsScene * scene);

    virtual ~ReportEntityCheck();

    virtual void buildXML(QDomDocument & doc, QDomElement & parent);

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

    virtual ReportEntityCheck* clone();
private:
    void init(QGraphicsScene*);

private slots:
    void propertyChanged(KoProperty::Set &, KoProperty::Property &);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
};

#endif // REPORTENTITYCHECK_H
