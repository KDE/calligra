#include "krcheckdata.h"
#include <koproperty/Property.h>
#include <koproperty/Set.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <klocalizedstring.h>

KRCheckData::KRCheckData(QDomNode &element)
{
    createProperties();
    QDomNodeList nl = element.childNodes();
    QString n;
    QDomNode node;
    
    m_name->setValue(element.toElement().attribute("report:name"));
    m_controlSource->setValue(element.toElement().attribute("report:control-source"));
    Z = element.toElement().attribute("report:zvalue").toDouble();
    m_foregroundColor->setValue(element.toElement().attribute("report:foreground-color"));
    m_checkStyle->setValue(element.toElement().attribute("report:check-style"));
    
    for (int i = 0; i < nl.count(); i++) {
        node = nl.item(i);
        n = node.nodeName();

        if (n == "report:rect") {
            QRectF r;
            parseReportRect(node.toElement(), r);
            m_pos.setPointPos(r.topLeft());
            m_size.setPointSize(r.size());
        } else if (n == "report:line-style") {
            KRLineStyleData ls;
            if (parseReportLineStyleData(node.toElement(), ls)) {
                m_lineWeight->setValue(ls.weight);
                m_lineColor->setValue(ls.lineColor);
                m_lineStyle->setValue(ls.style);
            }
        } else {
            kDebug() << "while parsing check element encountered unknow element: " << n;
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
    m_checkStyle = new KoProperty::Property("check-style", keys, strings, "Cross", "Check Style");

    m_controlSource = new KoProperty::Property("control-source", QStringList(), QStringList(), "", "Control Source");
    m_controlSource->setOption("extraValueAllowed", "true");

    m_foregroundColor = new KoProperty::Property("foreground-color", Qt::black, "Foreground Color", "Foreground Color");

    m_lineWeight = new KoProperty::Property("line-weight", 1, "Line Weight", "Line Weight");
    m_lineColor = new KoProperty::Property("line-color", Qt::black, "Line Color", "Line Color");
    m_lineStyle = new KoProperty::Property("line-style", Qt::SolidLine, "Line Style", "Line Style", KoProperty::LineStyle);

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

KRLineStyleData KRCheckData::lineStyle()
{
    KRLineStyleData ls;
    ls.weight = m_lineWeight->value().toInt();
    ls.lineColor = m_lineColor->value().value<QColor>();
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
