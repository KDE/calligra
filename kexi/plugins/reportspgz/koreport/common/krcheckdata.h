#ifndef KRCHECKDATA_H
#define KRCHECKDATA_H

#include <krobjectdata.h>
#include <QRect>
#include <QPainter>
#include <qdom.h>
#include "krpos.h"
#include "krsize.h"

namespace Scripting
{
class Check;
}

class KRCheckData : public KRObjectData
{
public:
    KRCheckData() {
        createProperties();
    };
    KRCheckData(QDomNode &element);
    virtual ~KRCheckData();
    virtual int type() const;
    virtual KRCheckData * toCheck();
    bool value();
    void setValue(bool);
    KRLineStyleData lineStyle();

    QString controlSource()const;

protected:
    QRectF _rect();

    KRSize m_size;
    KoProperty::Property * m_controlSource;
    KoProperty::Property* m_checkStyle;
    KoProperty::Property* m_foregroundColor;
    KoProperty::Property* m_lineColor;
    KoProperty::Property* m_lineWeight;
    KoProperty::Property* m_lineStyle;

private:
    virtual void createProperties();
    static int RTTI;

    friend class Scripting::Check;
    friend class ORPreRenderPrivate;
};

#endif // KRCHECKDATA_H
