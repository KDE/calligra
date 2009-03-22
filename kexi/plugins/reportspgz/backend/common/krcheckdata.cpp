#include "krcheckdata.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
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
            m_name->setValue(node.firstChild().nodeValue());
        } else if (n == "controlsource") {
            m_controlSource->setValue(node.firstChild().nodeValue());
        } else if (n == "checkstyle") {
            m_checkStyle->setValue(node.firstChild().nodeValue());
        } else if (n == "zvalue") {
            Z = node.firstChild().nodeValue().toDouble();
        } else if (n == "rect") {
            QRectF r;
            parseReportRect(node.toElement(), r);
            m_pos.setPointPos(r.topLeft());
            m_size.setPointSize(r.size());
        } else if (n == "linestyle") {
            ORLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), ls)) {
                m_lineWeight->setValue(ls.weight);
                m_lineColor->setValue(ls.lnColor);
                m_lineStyle->setValue(ls.style);
            }
        } else if (n == "fgcolor") {
            m_foregroundColor->setValue(QColor(node.firstChild().nodeValue()));
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
    m_set = new KoProperty::Set(0, "Check");

    QStringList keys, strings;

    keys << "Cross" << "Tick" << "Dot";
    strings << i18n("Cross") << i18n("Tick") << i18n("Dot");
    m_checkStyle = new KoProperty::Property("CheckStyle", keys, strings, "Cross", "Check Style");

    m_controlSource = new KoProperty::Property("ControlSource", QStringList(), QStringList(), "", "Control Source");
    m_controlSource->setOption("extraValueAllowed", "true");

    m_foregroundColor = new KoProperty::Property("ForegroundColor", Qt::black, "Foreground Color", "Foreground Color");

    m_lineWeight = new KoProperty::Property("Weight", 1, "Line Weight", "Line Weight");
    m_lineColor = new KoProperty::Property("LineColor", Qt::black, "Line Color", "Line Color");
    m_lineStyle = new KoProperty::Property("LineStyle", Qt::SolidLine, "Line Style", "Line Style", KoProperty::LineStyle);

    m_set->addProperty(m_name);
    m_set->addProperty(m_controlSource);
    m_set->addProperty(m_checkStyle);
    m_set->addProperty(m_pos.property());
    m_set->addProperty(m_size.property());
    m_set->addProperty(m_foregroundColor);
    m_set->addProperty(m_lineWeight);
    m_set->addProperty(m_lineColor);
    m_set->addProperty(m_lineStyle);
}

ORLineStyleData KRCheckData::lineStyle()
{
    ORLineStyleData ls;
    ls.weight = m_lineWeight->value().toInt();
    ls.lnColor = m_lineColor->value().value<QColor>();
    ls.style = (Qt::PenStyle)m_lineStyle->value().toInt();
    return ls;
}

QString KRCheckData::controlSource() const
{
    return m_controlSource->value().toString();
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
