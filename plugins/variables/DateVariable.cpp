/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008, 2010 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DateVariable.h"
#include "FixedDateFormat.h"

#include <KoOdfLoadingContext.h>
#include <KoOdfStylesReader.h>
#include <KoOdfWorkaround.h>
#include <KoProperties.h>
#include <KoShapeLoadingContext.h>
#include <KoShapeSavingContext.h>
#include <KoXmlNS.h>
#include <KoXmlReader.h>
#include <KoXmlWriter.h>

DateVariable::DateVariable(DateType type)
    : KoVariable()
    , m_type(type)
    , m_displayType(Date)
    , m_valueType(DateTime)
    , m_daysOffset(0)
    , m_monthsOffset(0)
    , m_yearsOffset(0)
    , m_secsOffset(0)
{
    m_datetime = QDateTime::currentDateTime();
}

DateVariable::~DateVariable() = default;

void DateVariable::saveOdf(KoShapeSavingContext &context)
{
    // TODO support data-style-name
    KoXmlWriter *writer = &context.xmlWriter();
    if (m_displayType == Time) {
        writer->startElement("text:time", false);
    } else {
        writer->startElement("text:date", false);
    }

    if (!m_definition.isEmpty()) {
        QString styleName = KoOdfNumberStyles::saveOdfDateStyle(context.mainStyles(), m_definition, false);
        writer->addAttribute("style:data-style-name", styleName);
    }

    if (m_type == Fixed) {
        writer->addAttribute("text:fixed", "true");
        // only write as much information as we read: just date/time or datetime
        if (m_displayType == Time) {
            const QString timeValue = (m_valueType == DateTime) ? m_datetime.toString(Qt::ISODate) : m_datetime.time().toString(Qt::ISODate);
            writer->addAttribute("text:time-value", timeValue);
        } else {
            const QString dateValue = (m_valueType == DateTime) ? m_datetime.toString(Qt::ISODate) : m_datetime.date().toString(Qt::ISODate);
            writer->addAttribute("text:date-value", dateValue);
        }
    } else {
        writer->addAttribute("text:fixed", "false");
    }
    writer->addTextNode(value());
    writer->endElement();
}

bool DateVariable::loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context)
{
    const QString localName(element.localName());
    QString dateFormat;
    QString dataStyle = element.attributeNS(KoXmlNS::style, "data-style-name");
    if (!dataStyle.isEmpty()) {
        if (context.odfLoadingContext().stylesReader().dataFormats().contains(dataStyle)) {
            KoOdfNumberStyles::NumericStyleFormat dataFormat = context.odfLoadingContext().stylesReader().dataFormats().value(dataStyle).first;
            dateFormat = dataFormat.prefix + dataFormat.formatStr + dataFormat.suffix;
        }
    }

    // dateProperties.setProperty("fixed", QVariant(element.attributeNS(KoXmlNS::text, "fixed") == "true"));
    if (element.attributeNS(KoXmlNS::text, "fixed", "false") == "true") {
        m_type = Fixed;
    } else {
        m_type = AutoUpdate;
    }

    if (localName == "time") {
        m_displayType = Time;
    } else {
        m_displayType = Date;
    }

    // dateProperties.setProperty("time", element.attributeNS(KoXmlNS::text, localName + "-value"));
    QString value(element.attributeNS(KoXmlNS::text, localName + "-value", ""));
    if (!value.isEmpty()) {
#ifndef NWORKAROUND_ODF_BUGS
        KoOdfWorkaround::fixBadDateForTextTime(value);
#endif
        // hopefully this simple detection works in all cases
        const bool isDateTime = (value.indexOf(QLatin1Char('T')) != -1);

        if (isDateTime) {
            m_datetime = QDateTime::fromString(value, Qt::ISODate);
            m_valueType = DateTime;
        } else {
            if (m_displayType == Time) {
                const QTime time = QTime::fromString(value, Qt::ISODate);
                m_datetime = QDateTime(QDate::currentDate(), time);
            } else {
                m_datetime = QDate::fromString(value, Qt::ISODate).startOfDay();
            }
            m_valueType = DateOrTime;
        }
    } else {
        // if value is not set current time is assumed  ODF 19.881 text:time-value
        m_type = AutoUpdate;
    }

    // dateProperties.setProperty("definition", dateFormat);
    m_definition = dateFormat;

    // dateProperties.setProperty("adjust", element.attributeNS(KoXmlNS::text, localName + "-adjust"));
    const QString adjust(element.attributeNS(KoXmlNS::text, localName + "-adjust", ""));
    adjustTime(adjust);
    update();
    return true;
}

void DateVariable::readProperties(const KoProperties *props)
{
    m_definition = props->stringProperty("definition");
    if (!props->stringProperty("time").isEmpty())
        m_datetime = QDateTime::fromString(props->stringProperty("time"), Qt::ISODate);
    if (props->intProperty("id") == Fixed)
        m_type = Fixed;
    else
        m_type = AutoUpdate;
    QString displayTypeProp = props->stringProperty("displayType", "date");
    if (displayTypeProp == "time") {
        m_displayType = Time;
    } else {
        m_displayType = Date;
    }
    m_valueType = DateTime;
    adjustTime(props->stringProperty("adjust"));
    update();
}

QWidget *DateVariable::createOptionsWidget()
{
    switch (m_type) {
    case Fixed:
        return new FixedDateFormat(this);
    default:;
    }
    return nullptr;
}

void DateVariable::setDefinition(const QString &definition)
{
    m_definition = definition;
    update();
}

void DateVariable::setSecsOffset(int offset)
{
    m_secsOffset = offset;
    update();
}

void DateVariable::setDaysOffset(int offset)
{
    m_daysOffset = offset;
    update();
}

void DateVariable::setMonthsOffset(int offset)
{
    m_monthsOffset = offset;
    update();
}

void DateVariable::setYearsOffset(int offset)
{
    m_yearsOffset = offset;
    update();
}

void DateVariable::update()
{
    QDateTime target;
    switch (m_type) {
    case Fixed:
        target = m_datetime;
        break;
    case AutoUpdate:
        target = QDateTime::currentDateTime();
        break;
    }
    target = target.addSecs(m_secsOffset);
    target = target.addDays(m_daysOffset);
    target = target.addMonths(m_monthsOffset);
    target = target.addYears(m_yearsOffset);
    switch (m_displayType) {
    case Time:
        if (m_definition.isEmpty()) {
            setValue(QLocale().toString(target.time()));
        } else {
            setValue(target.time().toString(m_definition));
        }
        break;
    case Date:
        if (m_definition.isEmpty()) {
            setValue(QLocale().toString(target.date()));
        } else {
            setValue(target.toString(m_definition));
        }
        break;
    }
}

void DateVariable::adjustTime(const QString &value)
{
    if (!value.isEmpty()) {
        m_daysOffset = 0;
        m_monthsOffset = 0;
        m_yearsOffset = 0;
        m_secsOffset = 0;
        int multiplier = 1;
        if (value.contains("-")) {
            multiplier = -1;
        }
        QString timePart;
        QString datePart;
        QStringList parts = value.mid(value.indexOf('P') + 1).split('T');
        datePart = parts[0];
        if (parts.size() > 1) {
            timePart = parts[1];
        }
        QRegularExpression rx("([0-9]+)([DHMSY])");
        QRegularExpressionMatch match;
        int value;
        bool valueOk;
        if (!timePart.isEmpty()) {
            int pos = 0;
            while ((pos = timePart.indexOf(rx, pos, &match)) != -1) {
                value = match.captured(1).toInt(&valueOk);
                if (valueOk) {
                    if (match.captured(2) == "H") {
                        m_secsOffset += multiplier * 3600 * value;
                    } else if (match.captured(2) == "M") {
                        m_secsOffset += multiplier * 60 * value;
                    } else if (match.captured(2) == "S") {
                        m_secsOffset += multiplier * value;
                    }
                }
                pos += match.capturedLength();
            }
        }
        if (!datePart.isEmpty()) {
            int pos = 0;
            while ((pos = datePart.indexOf(rx, pos, &match)) != -1) {
                value = match.captured(1).toInt(&valueOk);
                if (valueOk) {
                    if (match.captured(2) == "Y") {
                        m_yearsOffset += multiplier * value;
                    } else if (match.captured(2) == "M") {
                        m_monthsOffset += multiplier * value;
                    } else if (match.captured(2) == "D") {
                        m_daysOffset += multiplier * value;
                    }
                }
                pos += match.capturedLength();
            }
        }
    }
}
