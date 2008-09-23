#ifndef KRCHECKDATA_H
#define KRCHECKDATA_H

#include <krobjectdata.h>
#include "parsexmlutils.h"
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
        KRCheckData(){createProperties();};
        KRCheckData(QDomNode &element);
        virtual ~KRCheckData();
        virtual int type() const;
        virtual KRCheckData * toCheck();
        bool value();
        void setValue(bool);
        ORLineStyleData lineStyle();

        ORDataData data() {
        return ORDataData("Data Source", _controlSource->value().toString());
        }
        QString controlSource()const;

    protected:
    QRectF _rect();

    KRSize _size;
    KoProperty::Property * _controlSource;
    KoProperty::Property* _checkStyle;
    KoProperty::Property* _fgColor;
    KoProperty::Property* _lnColor;
    KoProperty::Property* _lnWeight;
    KoProperty::Property* _lnStyle;

private:
    virtual void createProperties();
    static int RTTI;

    friend class Scripting::Check;
    friend class ORPreRenderPrivate;
};

#endif // KRCHECKDATA_H
