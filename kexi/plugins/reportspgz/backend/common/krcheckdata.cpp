#include "krcheckdata.h"
#include <koproperty/property.h>
#include <koproperty/set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>

KRCheckData::KRCheckData(QDomNode &element)
{
    //ctor
        createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;
    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();
        if (n == "name") {
            _name->setValue(node.firstChild().nodeValue());
        } else if (n == "controlsource") {
            _controlSource->setValue(node.firstChild().nodeValue());
        } else if (n == "checkstyle") {
            _checkStyle->setValue(node.firstChild().nodeValue());
        } else if (n == "zvalue") {
            Z = node.firstChild().nodeValue().toDouble();
        } else if (n == "rect") {
            QRectF r;
            parseReportRect(node.toElement(), r);
            _pos.setPointPos(r.topLeft());
            _size.setPointSize(r.size());
        } else if (n == "linestyle") {
            ORLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), ls)) {
                _lnWeight->setValue(ls.weight);
                _lnColor->setValue(ls.lnColor);
                _lnStyle->setValue(ls.style);
            }
        } else if (n == "fgcolor") {
            _fgColor->setValue(QColor(node.firstChild().nodeValue()));
        } else {
            kDebug() << "while parsing check element encountered unknown element: " << n << endl;
        }
    }
}

KRCheckData::~KRCheckData()
{
    //dtor
}

void KRCheckData::createProperties()
{
    _set = new KoProperty::Set(0, "Check");

    QStringList keys, strings;

    keys << "Cross" << "Tick" << "Dot";
    strings << i18n("Cross") << i18n("Tick") << i18n("Dot");
    _checkStyle = new KoProperty::Property("CheckStyle", keys, strings, "Cross", "Check Style");

    _controlSource = new KoProperty::Property("ControlSource", QStringList(), QStringList(), "", "Control Source");
    _controlSource->setOption("extraValueAllowed", "true");

    _fgColor = new KoProperty::Property("ForegroundColor", Qt::black, "Foreground Color", "Foreground Color");

    _lnWeight = new KoProperty::Property("Weight", 1, "Line Weight", "Line Weight");
    _lnColor = new KoProperty::Property("LineColor", Qt::black, "Line Color", "Line Color");
    _lnStyle = new KoProperty::Property("LineStyle", Qt::SolidLine, "Line Style", "Line Style", KoProperty::LineStyle);

    _set->addProperty(_name);
    _set->addProperty(_controlSource);
    _set->addProperty(_checkStyle);
    _set->addProperty(_pos.property());
    _set->addProperty(_size.property());
    _set->addProperty(_fgColor);
    _set->addProperty(_lnWeight);
    _set->addProperty(_lnColor);
    _set->addProperty(_lnStyle);
}

ORLineStyleData KRCheckData::lineStyle()
{
    ORLineStyleData ls;
    ls.weight = _lnWeight->value().toInt();
    ls.lnColor = _lnColor->value().value<QColor>();
    ls.style = (Qt::PenStyle)_lnStyle->value().toInt();
    return ls;
}

QString KRCheckData::controlSource() const
{
    return _controlSource->value().toString();
}
// RTTI
int KRCheckData::type() const
{
    return RTTI;
}
int KRCheckData::RTTI = KRObjectData::EntityCheck;
KRCheckData * KRCheckData::toCheck()
{
    return this;
}
