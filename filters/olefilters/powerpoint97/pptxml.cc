/*
    Copyright (C) 2000, S.R.Haque <shaheedhaque@hotmail.com>.
    This file is part of the KDE project

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

DESCRIPTION
*/

#include <pptdoc.h>
#include <pptxml.h>
#include <qobject.h>
#include <koGlobal.h>

PptXml::PptXml(
    const myFile &mainStream,
    const myFile &currentUser):
        PptDoc(
            mainStream,
            currentUser)
{
}

PptXml::~PptXml()
{
}

bool PptXml::convert()
{
    if (!m_isConverted)
    {
        m_y = 0;
        m_success = parse();
    }
    return m_success;
}

void PptXml::encode(QString &text)
{
  // When encoding the stored form of text to its run-time form,
  // be sure to do the conversion for "&amp;" to "&" first to avoid
  // accidentally converting user text into one of the other escape
  // sequences.

  text.replace(QRegExp("&"), "&amp;");
  text.replace(QRegExp("<"), "&lt;");

  // Strictly, there is no need to encode >, but we do so to for safety.

  text.replace(QRegExp(">"), "&gt;");

  // Strictly, there is no need to encode " or ', but we do so to allow
  // them to co-exist!

  text.replace(QRegExp("\""), "&quot;");
  text.replace(QRegExp("'"), "&apos;");
}

const QString PptXml::getXml() const
{
    QString body = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DOC >\n"
        "<DOC author=\"Reginald Stadlbauer\" email=\"reggie@kde.org\" editor=\"Powerpoint import filter\" mime=\"application/x-kpresenter\" syntaxVersion=\"2\">\n"
        "<PAPER format=\"5\" ptWidth=\"680\" ptHeight=\"510\" mmWidth =\"240\" mmHeight=\"180\" inchWidth =\"9.44882\" inchHeight=\"7.08661\" orientation=\"0\" unit=\"0\">\n"
        "<PAPERBORDERS mmLeft=\"0\" mmTop=\"0\" mmRight=\"0\" mmBottom=\"0\" ptLeft=\"0\" ptTop=\"0\" ptRight=\"0\" ptBottom=\"0\" inchLeft=\"0\" inchTop=\"0\" inchRight=\"0\" inchBottom=\"0\"/>\n"
        "</PAPER>\n"
        "<BACKGROUND rastX=\"10\" rastY=\"10\" bred=\"255\" bgreen=\"255\" bblue=\"255\">\n";
    body += m_pages;
    body += "</BACKGROUND>\n"
        "<HEADER show=\"0\">\n"
        "<ORIG x=\"0\" y=\"0\"/>\n"
        "<SIZE width=\"-1\" height=\"-1\"/>\n"
        "<SHADOW distance=\"0\" direction=\"5\" red=\"160\" green=\"160\" blue=\"164\"/>\n"
        "<EFFECTS effect=\"0\" effect2=\"0\"/>\n"
        "<PRESNUM value=\"0\"/>\n"
        "<ANGLE value=\"0\"/>\n"
        "<FILLTYPE value=\"0\"/>\n"
        "<GRADIENT red1=\"255\" green1=\"0\" blue1=\"0\" red2=\"0\" green2=\"255\" blue2=\"0\" type=\"1\" unbalanced=\"0\" xfactor=\"100\" yfactor=\"100\"/>\n"
        "<PEN red=\"0\" green=\"0\" blue=\"0\" width=\"1\" style=\"0\"/>\n"
        "<BRUSH red=\"0\" green=\"0\" blue=\"0\" style=\"0\"/>\n"
        "<DISAPPEAR effect=\"0\" doit=\"0\" num=\"1\"/>\n"
        "<TEXTOBJ lineSpacing=\"0\" paragSpacing=\"0\" margin=\"0\" bulletType1=\"0\" bulletType2=\"1\" bulletType3=\"2\" bulletType4=\"3\" bulletColor1=\"#000000\" bulletColor2=\"#000000\" bulletColor3=\"#000000\" bulletColor4=\"#000000\">\n"
        "<P align=\"1\" type=\"0\" depth=\"0\">\n"
        "<TEXT family=\"utopia\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" color=\"#000000\"> </TEXT>\n"
        "</P>\n"
        "</TEXTOBJ>\n"
        "</HEADER>\n"
        "<FOOTER show=\"0\">\n"
        "<ORIG x=\"0\" y=\"0\"/>\n"
        "<SIZE width=\"-1\" height=\"-1\"/>\n"
        "<SHADOW distance=\"0\" direction=\"5\" red=\"160\" green=\"160\" blue=\"164\"/>\n"
        "<EFFECTS effect=\"0\" effect2=\"0\"/>\n"
        "<PRESNUM value=\"0\"/>\n"
        "<ANGLE value=\"0\"/>\n"
        "<FILLTYPE value=\"0\"/>\n"
        "<GRADIENT red1=\"255\" green1=\"0\" blue1=\"0\" red2=\"0\" green2=\"255\" blue2=\"0\" type=\"1\" unbalanced=\"0\" xfactor=\"100\" yfactor=\"100\"/>\n"
        "<PEN red=\"0\" green=\"0\" blue=\"0\" width=\"1\" style=\"0\"/>\n"
        "<BRUSH red=\"0\" green=\"0\" blue=\"0\" style=\"0\"/>\n"
        "<DISAPPEAR effect=\"0\" doit=\"0\" num=\"1\"/>\n"
        "<TEXTOBJ lineSpacing=\"0\" paragSpacing=\"0\" margin=\"0\" bulletType1=\"0\" bulletType2=\"1\" bulletType3=\"2\" bulletType4=\"3\" bulletColor1=\"#000000\" bulletColor2=\"#000000\" bulletColor3=\"#000000\" bulletColor4=\"#000000\">\n"
        "<P align=\"1\" type=\"0\" depth=\"0\">\n"
        "<TEXT family=\"utopia\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" color=\"#000000\"> </TEXT>\n"
        "</P>\n"
        "</TEXTOBJ>\n"
        "</FOOTER>\n"
        "<OBJECTS>\n";
    body += m_text;
    body += "</OBJECTS>\n"
        "<INFINITLOOP value=\"0\"/>\n"
        "<MANUALSWITCH value=\"1\"/>\n"
        "<PRESSPEED value=\"1\"/>\n"
        "<PRESSLIDES value=\"0\"/>\n"
        "<SELSLIDES>\n"
        "<SLIDE nr=\"0\" show=\"1\"/>\n"
        "</SELSLIDES>\n"
        "<PIXMAPS>\n"
        "</PIXMAPS>\n"
        "<CLIPARTS>\n"
        "</CLIPARTS>\n";
    body += m_embedded;
    body += "</DOC>\n";
    return body;
}

void PptXml::gotDrawing(
    unsigned id,
    QString type,
    unsigned length,
    const char *data)
{
    QString ourKey;
    QString uid;
    QString mimeType;
    QString filterArgs;

    // Send the picture to the outside world and get back the UID.

    ourKey = "vectorGraphic" + QString::number(id) + "." + type;
    if (type == "msod")
    {
        filterArgs = "shape-id=";
        filterArgs += QString::number(id);
        filterArgs += ";delay-stream=";
        filterArgs += QString::number(0);
    }
    emit signalSavePart(
            ourKey,
            uid,
            mimeType,
            type,
            filterArgs,
            length,
            data);

    // Add an entry to the list of embedded objects too. TBD: fix
    // RECT and FRAME settings.

    m_embedded += "  <EMBEDDED>\n"
                "<OBJECT url=\"" +
                uid +
                "\" mime=\"" +
                mimeType +
                "\">\n<RECT x=\"30\" y=\"190\" w=\"120\" h=\"80\"/>\n"
                "</OBJECT>\n"
                "<SETTINGS>\n"
                "<EFFECTS effect=\"0\" effect2=\"0\"/>\n"
                "<PEN red=\"0\" green=\"0\" blue=\"0\" width=\"1\" style=\"0\"/>\n"
                "<BRUSH red=\"0\" green=\"0\" blue=\"0\" style=\"0\"/>\n"
                "<PRESNUM value=\"0\"/>\n"
                "<ANGLE value=\"0\"/>\n"
                "<FILLTYPE value=\"0\"/>\n"
                "<GRADIENT red1=\"255\" green1=\"0\" blue1=\"0\" red2=\"0\" green2=\"255\" blue2=\"0\" type=\"1\" unbalanced=\"0\" xfactor=\"100\" yfactor=\"100\"/>\n"
                "<DISAPPEAR effect=\"0\" doit=\"0\" num=\"1\"/>\n"
                "</SETTINGS>\n"
                "  </EMBEDDED>\n";
}

void PptXml::gotSlide(
    const Slide &slide)
{
    static const unsigned pageHeight = 510;
    QString xml_friendly;
    unsigned i;

    m_pages += "<PAGE>\n"
            "<BACKTYPE value=\"0\"/>\n"
            "<BACKVIEW value=\"1\"/>\n"
            "<BACKCOLOR1 red=\"255\" green=\"255\" blue=\"255\"/>\n"
            "<BACKCOLOR2 red=\"255\" green=\"255\" blue=\"255\"/>\n"
            "<BCTYPE value=\"0\"/>\n"
            "<BGRADIENT unbalanced=\"0\" xfactor=\"100\" yfactor=\"100\"/>\n"
            "<PGEFFECT value=\"0\"/>\n"
            "</PAGE>\n";
    m_text += "<OBJECT type=\"4\">\n"
            "<ORIG x=\"30\" y=\"" +
            QString::number(30 + m_y) +
            "\"/>\n"
            "<SIZE width=\"610\" height=\"43\"/>\n"
            "<SHADOW distance=\"0\" direction=\"5\" red=\"160\" green=\"160\" blue=\"164\"/>\n"
            "<EFFECTS effect=\"0\" effect2=\"0\"/>\n"
            "<PRESNUM value=\"0\"/>\n"
            "<ANGLE value=\"0\"/>\n"
            "<FILLTYPE value=\"0\"/>\n"
            "<GRADIENT red1=\"255\" green1=\"0\" blue1=\"0\" red2=\"0\" green2=\"255\" blue2=\"0\" type=\"1\" unbalanced=\"0\" xfactor=\"100\" yfactor=\"100\"/>\n"
            "<PEN red=\"0\" green=\"0\" blue=\"0\" width=\"1\" style=\"0\"/>\n"
            "<BRUSH red=\"0\" green=\"0\" blue=\"0\" style=\"0\"/>\n"
            "<DISAPPEAR effect=\"0\" doit=\"0\" num=\"1\"/>\n"
            "<TEXTOBJ lineSpacing=\"0\" paragSpacing=\"0\" margin=\"0\" bulletType1=\"0\" bulletType2=\"0\" bulletType3=\"0\" bulletType4=\"0\" bulletColor1=\"#ff0000\" bulletColor2=\"#ff0000\" bulletColor3=\"#ff0000\" bulletColor4=\"#ff0000\">\n"
            "<P align=\"4\" type=\"0\" depth=\"0\">\n"
            "<TEXT family=\"utopia\" pointSize=\"36\" bold=\"0\" italic=\"0\" underline=\"0\" color=\"#000000\">\n";
    xml_friendly = slide.title;
    encode(xml_friendly);
    m_text += xml_friendly;
    m_text += "</TEXT>\n"
            "</P>\n"
            "</TEXTOBJ>\n"
            "</OBJECT>\n"
            "<OBJECT type=\"4\">\n"
            "<ORIG x=\"30\" y=\"" +
            QString::number(130 + m_y) +
            "\"/>\n"
            "<SIZE width=\"610\" height=\"24\"/>\n"
            "<SHADOW distance=\"0\" direction=\"5\" red=\"160\" green=\"160\" blue=\"164\"/>\n"
            "<EFFECTS effect=\"0\" effect2=\"0\"/>\n"
            "<PRESNUM value=\"0\"/>\n"
            "<ANGLE value=\"0\"/>\n"
            "<FILLTYPE value=\"0\"/>\n"
            "<GRADIENT red1=\"255\" green1=\"0\" blue1=\"0\" red2=\"0\" green2=\"255\" blue2=\"0\" type=\"1\" unbalanced=\"0\" xfactor=\"100\" yfactor=\"100\"/>\n"
            "<PEN red=\"0\" green=\"0\" blue=\"0\" width=\"1\" style=\"0\"/>\n"
            "<BRUSH red=\"0\" green=\"0\" blue=\"0\" style=\"0\"/>\n"
            "<DISAPPEAR effect=\"0\" doit=\"0\" num=\"1\"/>\n"
            "<TEXTOBJ lineSpacing=\"0\" paragSpacing=\"0\" margin=\"0\" bulletType1=\"0\" bulletType2=\"0\" bulletType3=\"0\" bulletType4=\"0\" bulletColor1=\"#000000\" bulletColor2=\"#ff0000\" bulletColor3=\"#ff0000\" bulletColor4=\"#ff0000\">\n"
            "<P align=\"0\" type=\"1\" depth=\"0\">\n"
            "<TEXT family=\"utopia\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" color=\"#000000\">\n";
    for (i = 0; i < slide.body.count(); i++)
    {
        xml_friendly = slide.body[i];
        encode(xml_friendly);
        m_text += xml_friendly;
        if (i < slide.body.count() - 1)
            m_text += "</TEXT>\n"
                "</P>\n"
                "<P align=\"0\" type=\"1\" depth=\"0\">\n"
                "<TEXT family=\"utopia\" pointSize=\"20\" bold=\"0\" italic=\"0\" underline=\"0\" color=\"#000000\">\n";
    }
    m_text += "</TEXT>\n"
            "</P>\n"
            "</TEXTOBJ>\n"
            "</OBJECT>";
    m_y += pageHeight;
}
#include "pptxml.moc"
