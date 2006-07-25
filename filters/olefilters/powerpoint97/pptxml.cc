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
#include <KoGlobal.h>

const int PptXml::s_area = 30512;

PptXml::PptXml(
    const myFile &mainStream,
    const myFile &currentUser,
    const myFile &pictures):
        PptDoc(
            mainStream,
            currentUser,
            pictures)
{
    m_isConverted = false;
    m_success = false;
    m_half = false;
    m_y = 0;
}

PptXml::~PptXml()
{
}

bool PptXml::convert()
{
    if (!m_isConverted)
    {
       // m_y = 30;
        m_success = parse();
        m_isConverted = true;
    }
    return m_success;
}

void PptXml::encode(QString &text)
{
  // When encoding the stored form of text to its run-time form,
  // be sure to do the conversion for "&amp;" to "&" first to avoid
  // accidentally converting user text into one of the other escape
  // sequences.

  text.replace('&', "&amp;");
  text.replace('<', "&lt;");
  text.replace('>', "&gt;"); // Needed to avoid ]]>

  // Strictly, there is no need to encode " or ', but we do so to allow
  // them to co-exist!

  text.replace('"', "&quot;");
  text.replace('\'', "&apos;");
}

const QString PptXml::getXml() const
{
    QString body = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><!DOCTYPE DOC>\n"
        "<DOC mime=\"application/x-kpresenter\" email=\"reggie@kde.org\" syntaxVersion=\"2\" author=\"Reginald Stadlbauer\" editor=\"Powerpoint import filter\">\n"
        " <PAPER ptWidth=\"680\" unit=\"0\" format=\"5\" orientation=\"0\" ptHeight=\"510\">\n"
        "  <PAPERBORDERS ptRight=\"0\" ptBottom=\"0\" ptLeft=\"0\" ptTop=\"0\"/>\n"
        " </PAPER>\n"
        " <BACKGROUND rastX=\"10\" rastY=\"10\" color=\"#ffffff\">\n";
    body += m_pages;
    body += " </BACKGROUND>\n"
        " <HEADER show=\"0\">\n"
        "  <ORIG x=\"0\" y=\"0\"/>\n"
        "  <SIZE width=\"680\" height=\"20\"/>\n"
        "  <PEN width=\"1\" style=\"0\" color=\"#000000\"/>\n"
        "  <TEXTOBJ>\n"
        "   <P/>\n"
        "  </TEXTOBJ>\n"
        " </HEADER>\n"
        " <FOOTER show=\"0\">\n"
        "  <ORIG x=\"0\" y=\"430\"/>\n"
        "  <SIZE width=\"680\" height=\"20\"/>\n"
        "  <PEN width=\"1\" style=\"0\" color=\"#000000\"/>\n"
        "  <TEXTOBJ>\n"
        "   <P/>\n"
        "  </TEXTOBJ>\n"
        " </FOOTER>\n"
        " <PAGETITLES>\n";
    body += m_titles;
    body += " </PAGETITLES>\n"
        " <PAGENOTES>\n";
    body += m_notes;
    body += " </PAGENOTES>\n"
        " <OBJECTS>\n";
    body += m_text;
    body += " </OBJECTS>\n"
        " <INFINITLOOP value=\"0\"/>\n"
        " <MANUALSWITCH value=\"1\"/>\n"
        " <PRESSPEED value=\"1\"/>\n"
        " <PRESSLIDES value=\"0\"/>\n"
        " <SELSLIDES>\n"
        " <SLIDE nr=\"0\" show=\"1\"/>\n"
        " </SELSLIDES>\n"
        " <PIXMAPS>\n"
        " </PIXMAPS>\n"
        " <CLIPARTS>\n"
        " </CLIPARTS>\n";
    body += m_embedded;
    body += "</DOC>\n";
    //kdDebug(s_area) << body << endl;
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
            length,
            data);

    // Add an entry to the list of embedded objects too. TBD: fix
    // RECT and FRAME settings.
if (mimeType != "application/x-kontour")
{
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
}

void PptXml::gotSlide(PptSlide &slide)
{
	static const unsigned pageHeight = 510;
	Q_UINT16 numbOfPholders = 0;
    	m_pages += "  <PAGE/>\n";
	numbOfPholders = slide.getNumberOfPholders();
	for(Q_UINT16 i = 0; i < numbOfPholders; i++)
	{
		slide.gotoPlaceholder(i);
		setPlaceholder(slide);
	}
	m_y += pageHeight;
}

void PptXml::setPlaceholder(PptSlide &slide)
{
 	QString xml_friendly;
 	unsigned i = 0;
	bool 		bulletedList = false; 	//is this a bulleted list?
	Q_UINT16 	type;				//type of text
	QString 	x;				//x of placeholder
	QString 	align;				//align of text
	QString 	height ;			//height of placeholder
	QString 	width ;			//width of placeholder
	QString 	pointSize;			//font size

 	xml_friendly = *slide.getPlaceholderText().at(i);
  	encode(xml_friendly);

	type = slide.getPlaceholderType();
	Q_UINT16 y = 0;

	switch (type)
	{
	case TITLE_TEXT:
		y = 30;
		height = QString::number(55);
		x = QString::number(30);
		width = QString::number(610);
		align = QString::number(ALIGN_CENTER);
		pointSize = QString::number(44);
		bulletedList = false;
		m_titles += "  <Title title=\"" + xml_friendly + "\" />\n";
		break;
	case CENTER_TITLE_TEXT:
		y = 130;
		x = QString::number(45);
		height = QString::number(55);
		width = QString::number(610);
		align = QString::number(ALIGN_CENTER);
		pointSize = QString::number(44);
		bulletedList = false;
		m_titles += "  <Title title=\"" + xml_friendly + "\" />\n";
		break;
	case CENTER_BODY_TEXT:
		y = 200;
		x = QString::number(35);
		height = QString::number(37);
		width = QString::number(610);
		align = QString::number(ALIGN_CENTER);
		pointSize = QString::number(32);
		bulletedList = false;
		break;
	case NOTES_TEXT:
		m_notes += "  <Note note=\"\" />\n";
		break;
	case BODY_TEXT:
		y = 130;
		x = QString::number(35);
		height = QString::number(268);
		width = QString::number(610);
		align = QString::number(ALIGN_LEFT);
		pointSize = QString::number(28);
		bulletedList = true;
		break;
	case OTHER_TEXT:
	case HALF_BODY_TEXT:
	case QUARTER_BODY_TEXT:
		y = 130;
		if(m_half)
		{
			x = QString::number(335);
			width = QString::number(300);
			m_half = false;
		}
		else
		{
			x = QString::number(30);
			width = QString::number(300);
			m_half = true;
		}
		height = QString::number(268);
		align = QString::number(ALIGN_LEFT);
		pointSize = QString::number(28);
		bulletedList = true;
		break;
	default:
		return;
		break;
	}

    	m_text += "<OBJECT type=\"4\">\n"
            	" <ORIG x=\""+x+"\" y=\""+QString::number(y + m_y)+"\"/>\n"
            	" <SIZE width=\""+width+"\" height=\""+ height +"\"/>\n"
            	" <PEN width=\"1\" style=\"0\" color=\"#000000\"/>\n"
            	" <TEXTOBJ>\n"
            	"  <P align=\""+align+"\">\n";
	if(bulletedList)
 		m_text += "   <COUNTER numberingtype=\"0\" type=\"10\" depth=\"0\" />\n";
	m_text += "   <TEXT family=\"utopia\" pointSize=\""+pointSize+"\" color=\"#000000\">";

    	for (i = 0; i < slide.getPlaceholderText().count(); i++)
    	{
        	xml_friendly = *slide.getPlaceholderText().at(i);
        	encode(xml_friendly);
        	m_text += xml_friendly;
        	if (i < slide.getPlaceholderText().count() - 1)
		{
        		m_text += "</TEXT>\n"
          			"  </P>\n"
            			"  <P align=\"1\">\n";
			if(bulletedList)
            			m_text += "   <COUNTER numberingtype=\"0\" type=\"10\" depth=\"0\" />\n";
           		m_text += "   <TEXT family=\"utopia\" pointSize=\""+pointSize+"\" color=\"#000000\">";
		}
 	}
    	m_text += "</TEXT>\n"
     		"  </P>\n"
            	" </TEXTOBJ>\n"
            	"</OBJECT>";

}

#include "pptxml.moc"
