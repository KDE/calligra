/*
* This file is part of the KDE project
*
* Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*
* Contact: Amit Aggarwal <amit.5.aggarwal@nokia.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public License
* version 2.1 or (at your option) any later version as published by
* the Free Software Foundation.
*
* This library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301 USA
*
*/

#include <qdatetime.h>
#include <KoXmlWriter.h>
#include <QBuffer>
#include <QString>
#include <iostream>
#include "datetimeformat.h"



DateTimeFormat::DateTimeFormat(Slide *master)
        : master(master)
        , formatId(0)
{
    if(master)
        formatId = master->dateTimeFormatId();
//#ifdef LIBDATE_DEBUG
    std::cout<<"\nDateTimeConstructor formatId :"<<formatId ;
//#endif
}

DateTimeFormat::~DateTimeFormat()
{
}

void DateTimeFormat::addDateStyle(KoGenStyles& styles, bool dayofweek, bool longmonth, bool textualmonth, bool longyear, QString separator)
{
    QBuffer buffer;

    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);

    KoGenStyle dt(KoGenStyle::StyleNumericDate);
    dt.setAutoStyleInStylesDotXml(true);
    if (dayofweek == true) {
        xmlWriter.startElement("number:day-of-week");
        xmlWriter.addAttribute("number:style", "long");
        xmlWriter.endElement();  //number:day-of-week
        xmlWriter.startElement("number:text");
        xmlWriter.addTextNode(",");
        xmlWriter.endElement(); //number:text
    }

    xmlWriter.startElement("number:day");
    xmlWriter.endElement(); // number:day
    xmlWriter.startElement("number:text");
    xmlWriter.addTextNode(separator);
    xmlWriter.endElement(); //number:text

    xmlWriter.startElement("number:month");
    if (longmonth == true) {
        xmlWriter.addAttribute("number:style","long");
        if (textualmonth == true)
           xmlWriter.addAttribute("number:textual","true");
    }
    xmlWriter.endElement();  //number:month
    xmlWriter.startElement("number:text");
    xmlWriter.addTextNode(separator);
    xmlWriter.endElement(); //number:text

    xmlWriter.startElement("number:year");
    if (longyear == true) {
        xmlWriter.addAttribute("number-style","long");
    }
    xmlWriter.endElement(); // number:year

    xmlWriter.startElement("number:text");
    xmlWriter.addTextNode(" ");
    xmlWriter.endElement(); //number:text

    dt.addChildElement("number:date-style",
                       QString::fromUtf8(buffer.buffer(), buffer.buffer().size()));
    styles.lookup(dt, "DT");
    setDateStyleName(styles.lookup(dt));

}


void DateTimeFormat::addTimeStyle(KoGenStyles& styles, bool hr12Format, bool second, QString separator)
{
    QBuffer buffer;

    buffer.open(QIODevice::WriteOnly);
    KoXmlWriter xmlWriter(&buffer);

    KoGenStyle tm(KoGenStyle::StyleNumericTime);
    tm.setAutoStyleInStylesDotXml(true);

    xmlWriter.startElement("number:hours");
    xmlWriter.endElement();  //number:hours

    xmlWriter.startElement("number:text");
    xmlWriter.addTextNode(separator);
    xmlWriter.endElement(); //number:text

    xmlWriter.startElement("number:minutes");
    xmlWriter.endElement(); // number:minutes

    if (second) {
        xmlWriter.startElement("number:text");
        xmlWriter.addTextNode(separator);
        xmlWriter.endElement(); //number:text
        xmlWriter.startElement("number:second"); //TBD in spec
        xmlWriter.endElement();  //number:second
    }

    if (hr12Format) {
        xmlWriter.startElement("number:am-pm");
        xmlWriter.endElement(); // number:am-pm
    }

    tm.addChildElement("number:date-style",

                       QString::fromUtf8(buffer.buffer(), buffer.buffer().size()));
    styles.lookup(tm, "TM");
    setTimeStyleName(styles.lookup(tm));

}


void DateTimeFormat::addDateTimeAutoStyles(KoGenStyles& styles)
{
    int  headerFooterAtomFlags = master->headerFooterFlags();

    if (headerFooterAtomFlags && fHasTodayDate) {
        switch (formatId) {
        case ShortDate:
             break;
        case LongDate:
             addDateStyle(styles, true, true, true, true,".");
         break;
        case LongDateWithoutWeekday:
           addDateStyle(styles, false, true, false, true,".");
         break;
        case ShortDateWithAbbrMonth:
           addDateStyle(styles, false, false, false, true);
         break;
        case ShortDateWithSlashes:
           addDateStyle(styles, false, false, false, false);
         break;
        case DateAnd12HrTime:
           addDateStyle(styles, false, false, false, false);
           addTimeStyle(styles, true, false);
         break;
        case Hr24Time:
           addTimeStyle(styles, false, false);
         break;
        case Hr24TimeWithSec:
           addTimeStyle(styles, false, true);
         break;
        case Hr12Time:
           addTimeStyle(styles, true, false);
         break;
        case Hr12TimeWithSec:
           addTimeStyle(styles, true, true);
         break;
        default:
         break;
        } //switch
    }//if
  else if (headerFooterAtomFlags && fHasUserDate) {
   //Future - Fixed date
  }
}

void DateTimeFormat::addMasterDateTimeSection(KoXmlWriter& xmlWriter,QString tStyle)
{
    QDateTime dt = QDateTime::currentDateTime();
    QString format, result;
    bool hasDate = true;
    bool hasTime = false;

    switch(formatId){
    case ShortDate:
           format = "dd-MM-yy";
         break;
    case LongDate:
           format = "dddd dd MMMM yyyy";
         break;
    case LongDateWithoutWeekday:
           format = "dd MMMM yyyy";
         break;
    case ShortDateWithAbbrMonth:
           format = "dd MMM yyyy";
         break;
    case ShortDateWithSlashes:
           format = "dd/MM/yy";
         break;
    case DateAnd12HrTime:
           hasTime = true;
           format = "dd-MM-yy hh:mm ap";
         break;
    case Hr24Time:
           hasTime = true;
           hasDate = false;
           format = "hh:mm";
         break;
    case Hr24TimeWithSec:
           hasTime = true;
           hasDate = false;
           format = "hh:mm:ss";
         break;
    case Hr12Time:
           hasTime = true;
           hasDate = false;
           format = "hh:mm ap";
         break;
    case Hr12TimeWithSec:
           hasTime = true;
           hasDate = false;
           format = "hh:mm:ss ap";
         break;
    case XMLSchemaDate:
           // http://www.w3.org/TR/2004/REC-xmlschema-2-20041028/#date
           format = "yyyy-MM-dd";
         break;
    case XMLSchemaDateTime:
           // http://www.w3.org/TR/2004/REC-xmlschema-2-20041028/#dateTime
           hasTime = true;
           format = "yyyy-MM-ddTHH:mm:ss.z";
         break;
    case FixedUserDateFormat:
           //Future - Fixed Date
    default:
           // XML Schema time format
           format = "yyyy-MM-ddTHH:mm:ss.z";
         break;
    }

    result = dt.toString(format);
    if ( hasDate == true ) {
        xmlWriter.startElement("text:span");
        xmlWriter.addAttribute("text:style-name", tStyle);
        xmlWriter.startElement("text:date");
        xmlWriter.addAttribute("style:data-style-name", getDateStyleName());
        xmlWriter.addAttribute("text:date-value", result);
        xmlWriter.addTextNode(result);
        xmlWriter.endElement();//text:date
        xmlWriter.endElement(); // text:span
    }
    if ( hasTime == true ) {
        xmlWriter.startElement("text:span");
        xmlWriter.addAttribute("text:style-name", tStyle);
        xmlWriter.startElement("text:time");
        xmlWriter.addAttribute("style:data-style-name", getTimeStyleName());
        xmlWriter.addAttribute("text:time-value", result);
        xmlWriter.addTextNode(result);
        xmlWriter.endElement();//text:time
        xmlWriter.endElement(); // text:span
    }

#ifdef LIBDATE_DEBUG
    std::cout<<"****Date formatted here: "<<result.toLatin1().data();
#endif

}

void DateTimeFormat::setDateStyleName(const QString &name)
{
    dateStyleName = name;
}

QString DateTimeFormat::getDateStyleName() const
{
    std::cout<<"\n**DateStyleName: "<<dateStyleName.toLatin1().data()<<"\n";
    return dateStyleName;
}

void DateTimeFormat::setTimeStyleName(const QString &name)
{
    timeStyleName = name;
    std::cout<<"\n**TimeStyleName:" << timeStyleName.toLatin1().data();
}

QString DateTimeFormat::getTimeStyleName() const
{
    return timeStyleName;
}

