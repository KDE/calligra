/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2001 Nikolas Zimmermann <wildfox@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <qdom.h>
#include <qfile.h>
#include <qstringlist.h>
#include <kdebug.h>
#include <math.h>

#include "kivio_stencil.h"
#include "kivio_dia_stencil_spawner.h"

KivioDiaStencilSpawner::KivioDiaStencilSpawner(KivioStencilSpawnerSet *p) : KivioStencilSpawner(p)
{
    m_smlStencilSpawner = new KivioSMLStencilSpawner(p);
}

KivioDiaStencilSpawner::~KivioDiaStencilSpawner()
{
}

QDomElement KivioDiaStencilSpawner::saveXML(QDomDocument &d)
{
    return m_smlStencilSpawner->saveXML(d);
}

void KivioDiaStencilSpawner::calculateDimensions(float x, float y)
{
    m_xlist.append(x);
    m_ylist.append(y);
}

float KivioDiaStencilSpawner::diaPointToKivio(float point, bool xpoint)
{
    if(xpoint)
    {
	if(point > 0)
	    return fabs(m_lowestx) + fabs(point);
	else
	    return (fabs(m_highestx) + fabs(m_lowestx)) - (fabs(m_highestx) + fabs(point));
    }
    else
    {
	if(point > 0)
	    return fabs(m_lowesty) + fabs(point);
	else
	    return (fabs(m_highesty) + fabs(m_lowesty)) - (fabs(m_highesty) + fabs(point));
    }
}

bool KivioDiaStencilSpawner::load(const QString &file)
{
    QDomDocument dia("test");
    QDomDocument kivio("XML");

    m_filename = file;
    QFile f(file);

    if(f.open(IO_ReadOnly) == false)
    {
	kdDebug() << "KivioDiaStencilSpawner::load() - Error opening stencil: " << file << endl;
	return false;
    }
    dia.setContent(&f);
    QDomNode diaMain = dia.firstChild().nextSibling();

    // Set "creator" attribute
    QDomElement firstElement = kivio.createElement("KivioShapeStencil");
    firstElement.setAttribute("creator", "kiviodiafilter");

    kivio.appendChild(firstElement);

    // Add KivioSMLStencilSpawnerInfo
    QDomElement spawnerInfoElement = kivio.createElement("KivioSMLStencilSpawnerInfo");
    QDomElement authorInfoElement = kivio.createElement("Author");
    authorInfoElement.setAttribute("data", "n/a");
    QDomElement titleInfoElement = kivio.createElement("Title");
    titleInfoElement.setAttribute("data", diaMain.namedItem("name").toElement().text());
    QDomElement idInfoElement = kivio.createElement("Id");
    idInfoElement.setAttribute("data", diaMain.namedItem("name").toElement().text());
    QDomElement descriptionInfoElement = kivio.createElement("Description");
    descriptionInfoElement.setAttribute("data", diaMain.namedItem("description").toElement().text());
    QDomElement versionInfoElement = kivio.createElement("Version");
    versionInfoElement.setAttribute("data", "1.0");
    QDomElement webInfoElement = kivio.createElement("Web");
    webInfoElement.setAttribute("data", "http://");
    QDomElement emailInfoElement = kivio.createElement("Email");
    emailInfoElement.setAttribute("data", "n/a");
    QDomElement copyrightInfoElement = kivio.createElement("Copyright");
    copyrightInfoElement.setAttribute("data", "n/a");
    QDomElement autoUpdateInfoElement = kivio.createElement("AutoUpdate");
    autoUpdateInfoElement.setAttribute("data", "off");

    spawnerInfoElement.appendChild(authorInfoElement);
    spawnerInfoElement.appendChild(titleInfoElement);
    spawnerInfoElement.appendChild(idInfoElement);
    spawnerInfoElement.appendChild(descriptionInfoElement);
    spawnerInfoElement.appendChild(versionInfoElement);
    spawnerInfoElement.appendChild(webInfoElement);
    spawnerInfoElement.appendChild(emailInfoElement);
    spawnerInfoElement.appendChild(copyrightInfoElement);
    spawnerInfoElement.appendChild(autoUpdateInfoElement);

    kivio.documentElement().appendChild(spawnerInfoElement);

    float scale = 72.0f;

    // Add Dimensions
    QDomElement dimensionsElement = kivio.createElement("Dimensions");
    kivio.documentElement().appendChild(dimensionsElement);

    // Calculate Dimensions
    QDomElement svgElement = diaMain.namedItem("svg:svg").toElement();
    QDomNode svgNode = svgElement.firstChild();
    while(!svgNode.isNull())
    {
	QDomElement svgChild = svgNode.toElement();
	if(!svgChild.isNull())
	{
	    if(svgChild.tagName() == "svg:rect")
	    {
		// TODO: rx and ry -> rounded rects
		if(svgChild.hasAttribute("x") && svgChild.hasAttribute("y") && svgChild.hasAttribute("width") && svgChild.hasAttribute("height"))
		{
		    calculateDimensions(svgChild.attribute("x").toFloat(), svgChild.attribute("y").toFloat());
		    calculateDimensions(svgChild.attribute("x").toFloat() + svgChild.attribute("width").toFloat(), svgChild.attribute("y").toFloat() + svgChild.attribute("height").toFloat());
		}
	    }
	    else if(svgChild.tagName() == "svg:circle")
	    {
		if(svgChild.hasAttribute("cx") && svgChild.hasAttribute("cy") && svgChild.hasAttribute("r"))
		{
		    calculateDimensions((svgChild.attribute("cx").toFloat()) - (svgChild.attribute("r").toFloat()), (svgChild.attribute("cy").toFloat()) - (svgChild.attribute("r").toFloat()));
		    calculateDimensions((svgChild.attribute("cx").toFloat()) + (svgChild.attribute("r").toFloat()), (svgChild.attribute("cy").toFloat()) + (svgChild.attribute("r").toFloat()));
		}
	    }
	    else if(svgChild.tagName() == "svg:ellipse")
	    {
		if(svgChild.hasAttribute("cx") && svgChild.hasAttribute("cy") && svgChild.hasAttribute("rx") && svgChild.hasAttribute("ry"))
		{
		    calculateDimensions((svgChild.attribute("cx").toFloat()) - (svgChild.attribute("rx").toFloat()), (svgChild.attribute("cy").toFloat()) - (svgChild.attribute("ry").toFloat()));
		    calculateDimensions((svgChild.attribute("cx").toFloat()) + (svgChild.attribute("rx").toFloat()), (svgChild.attribute("cy").toFloat()) + (svgChild.attribute("ry").toFloat()));
		}
	    }
	    else if(svgChild.tagName() == "svg:line")
	    {
		if(svgChild.hasAttribute("x1") && svgChild.hasAttribute("y1") && svgChild.hasAttribute("x2") && svgChild.hasAttribute("y2"))
		{
		    calculateDimensions(svgChild.attribute("x1").toFloat(), svgChild.attribute("y1").toFloat());
		    calculateDimensions(svgChild.attribute("x2").toFloat(), svgChild.attribute("y2").toFloat());
		}
	    }
	    else if(svgChild.tagName() == "svg:polyline")
	    {
		if(svgChild.hasAttribute("points"))
		{
		    QStringList points = QStringList::split(" ", svgChild.attribute("points"));
		    for(QStringList::Iterator it = points.begin(); it != points.end(); ++it)
		    {
			QString x, y;

			QStringList parsed = QStringList::split(",", (*it));
			QStringList::Iterator itp = parsed.begin();
			x = (*itp);
			++itp;
			y = (*itp);

			calculateDimensions(x.toFloat(), y.toFloat());
		    }
		}
	    }
	    else if(svgChild.tagName() == "svg:polygon")
	    {
		if(svgChild.hasAttribute("points"))
		{
		    QStringList points = QStringList::split(" ", svgChild.attribute("points"));
		    for(QStringList::Iterator it = points.begin(); it != points.end(); ++it)
		    {
			QString x, y;

			QStringList parsed = QStringList::split(",", (*it));
			QStringList::Iterator itp = parsed.begin();
			x = (*itp);
			++itp;
			y = (*itp);

			calculateDimensions(x.toFloat(), y.toFloat());
		    }
		}
	    }

	}
	svgNode = svgNode.nextSibling();
    }

    QValueList<float>::Iterator itx = m_xlist.begin();
    QValueList<float>::Iterator ity = m_ylist.begin();
    m_highestx = *itx;
    m_lowestx = *itx;
    m_highesty = *ity;
    m_lowesty = *ity;
    ++itx;
    ++ity;

    for( ; itx != m_xlist.end(); ++itx)
    {
	m_highestx = QMAX(m_highestx, *itx);
	m_lowestx = QMIN(m_lowestx, *itx);
    }

    for( ; ity != m_ylist.end(); ++ity)
    {
	m_highesty = QMAX(m_highesty, *ity);
	m_lowesty = QMIN(m_lowesty, *ity);
    }

    // Add KivioConnectorTarget's
    QDomElement connectionsElement = diaMain.namedItem("connections").toElement();
    QDomNode connectionsNode = connectionsElement.firstChild();
    while(!connectionsNode.isNull())
    {
	QDomElement connectionChild = connectionsNode.toElement();
	if(!connectionChild.isNull())
	{
	    if(connectionChild.tagName() == "point")
	    {
		if(connectionChild.hasAttribute("x") && connectionChild.hasAttribute("y"))
		{
		    QDomElement kivioConnectorTarget = kivio.createElement("KivioConnectorTarget");
		    kivioConnectorTarget.setAttribute("x", QString::number(diaPointToKivio(connectionChild.attribute("x").toFloat(), true) * scale));
		    kivioConnectorTarget.setAttribute("y", QString::number(diaPointToKivio(connectionChild.attribute("y").toFloat(), false) * scale));

		    kivio.documentElement().appendChild(kivioConnectorTarget);
		}
	    }
	}
	connectionsNode = connectionsNode.nextSibling();
    }

    // Add KivioShape's and convert to Kivio's Coordinate System
    svgNode = svgElement.firstChild();
    int runs = 0;
    while(!svgNode.isNull())
    {
	QDomElement svgChild = svgNode.toElement();
	if(!svgChild.isNull())
	{
	    if(svgChild.tagName() == "svg:rect")
	    {
		runs++;
		// TODO: rx and ry -> rounded rects
		if(svgChild.hasAttribute("x") && svgChild.hasAttribute("y") && svgChild.hasAttribute("width") && svgChild.hasAttribute("height"))
		{
		    QDomElement kivioShape = kivio.createElement("KivioShape");
		    kivioShape.setAttribute("type", "Rectangle");
		    kivioShape.setAttribute("name", QString::fromLatin1("Element") + QString::number(runs));
		    kivioShape.setAttribute("x", QString::number(diaPointToKivio(svgChild.attribute("x").toFloat(), true) * scale));
		    kivioShape.setAttribute("y", QString::number(diaPointToKivio(svgChild.attribute("y").toFloat(), false) * scale));
		    kivioShape.setAttribute("w", QString::number(svgChild.attribute("width").toFloat() * scale));
		    kivioShape.setAttribute("h", QString::number(svgChild.attribute("height").toFloat() * scale));
		    kivio.documentElement().appendChild(kivioShape);
		}
	    }
	    else if(svgChild.tagName() == "svg:circle")
	    {
	        runs++;
		if(svgChild.hasAttribute("cx") && svgChild.hasAttribute("cy") && svgChild.hasAttribute("r"))
		{
		    QDomElement kivioShape = kivio.createElement("KivioShape");
		    kivioShape.setAttribute("type", "Ellipse");
		    kivioShape.setAttribute("name", QString::fromLatin1("Element") + QString::number(runs));
		    kivioShape.setAttribute("x", QString::number((diaPointToKivio(svgChild.attribute("cx").toFloat() - svgChild.attribute("r").toFloat(), true) * scale)));
		    kivioShape.setAttribute("y", QString::number((diaPointToKivio(svgChild.attribute("cy").toFloat() - svgChild.attribute("r").toFloat(), false) * scale)));
		    kivioShape.setAttribute("w", QString::number(svgChild.attribute("r").toFloat() * scale * 2));
		    kivioShape.setAttribute("h", QString::number(svgChild.attribute("r").toFloat() * scale * 2));
		    kivio.documentElement().appendChild(kivioShape);
		}
	    }
	    else if(svgChild.tagName() == "svg:ellipse")
	    {
		runs++;
		if(svgChild.hasAttribute("cx") && svgChild.hasAttribute("cy") && svgChild.hasAttribute("rx") && svgChild.hasAttribute("ry"))
		{
		    QDomElement kivioShape = kivio.createElement("KivioShape");
		    kivioShape.setAttribute("type", "Ellipse");
		    kivioShape.setAttribute("name", QString::fromLatin1("Element") + QString::number(runs));
		    kivioShape.setAttribute("x", QString::number((diaPointToKivio(svgChild.attribute("cx").toFloat() - svgChild.attribute("rx").toFloat(), true) * scale)));
		    kivioShape.setAttribute("y", QString::number((diaPointToKivio(svgChild.attribute("cy").toFloat() - svgChild.attribute("ry").toFloat(), false) * scale)));
		    kivioShape.setAttribute("w", QString::number(svgChild.attribute("rx").toFloat() * scale * 2));
		    kivioShape.setAttribute("h", QString::number(svgChild.attribute("ry").toFloat() * scale * 2));
	    	    kivio.documentElement().appendChild(kivioShape);
		}
	    }
	    else if(svgChild.tagName() == "svg:line")
	    {
	        runs++;
		if(svgChild.hasAttribute("x1") && svgChild.hasAttribute("y1") && svgChild.hasAttribute("x2") && svgChild.hasAttribute("y2"))
		{
		    QDomElement kivioShape = kivio.createElement("KivioShape");
		    kivioShape.setAttribute("type", "LineArray");
		    kivioShape.setAttribute("name", QString::fromLatin1("Element") + QString::number(runs));

		    QDomElement lineArrayElement = kivio.createElement("Line");
		    lineArrayElement.setAttribute("x1", QString::number(diaPointToKivio(svgChild.attribute("x1").toFloat(), true) * scale));
		    lineArrayElement.setAttribute("y1", QString::number(diaPointToKivio(svgChild.attribute("y1").toFloat(), false) * scale));
		    lineArrayElement.setAttribute("x2", QString::number(diaPointToKivio(svgChild.attribute("x2").toFloat(), true) * scale));
    		    lineArrayElement.setAttribute("y2", QString::number(diaPointToKivio(svgChild.attribute("y2").toFloat(), false) * scale));

		    kivioShape.appendChild(lineArrayElement);
		    kivio.documentElement().appendChild(kivioShape);
		}
	    }
	    else if(svgChild.tagName() == "svg:polyline")
	    {
		runs++;
		if(svgChild.hasAttribute("points"))
		{
		    QDomElement kivioShape = kivio.createElement("KivioShape");
		    kivioShape.setAttribute("type", "Polyline");
		    kivioShape.setAttribute("name", QString::fromLatin1("Element") + QString::number(runs));

		    QStringList points = QStringList::split(" ", svgChild.attribute("points"));
		    for(QStringList::Iterator it = points.begin(); it != points.end(); ++it)
		    {
			QString x, y;

			QStringList parsed = QStringList::split(",", (*it));
			QStringList::Iterator itp = parsed.begin();
			x = (*itp);
			++itp;
			y = (*itp);

			QDomElement kivioPointElement = kivio.createElement("KivioPoint");
			kivioPointElement.setAttribute("x", QString::number(diaPointToKivio(x.toFloat(), true) * scale));
			kivioPointElement.setAttribute("y", QString::number(diaPointToKivio(y.toFloat(), false) * scale));

			kivioShape.appendChild(kivioPointElement);
		    }
		    kivio.documentElement().appendChild(kivioShape);
		}
	    }
	    else if(svgChild.tagName() == "svg:polygon")
	    {
		runs++;
		if(svgChild.hasAttribute("points"))
		{
		    QDomElement kivioShape = kivio.createElement("KivioShape");
		    kivioShape.setAttribute("type", "Polygon");
		    kivioShape.setAttribute("name", QString::fromLatin1("Element") + QString::number(runs));

		    QStringList points = QStringList::split(" ", svgChild.attribute("points"));
		    for(QStringList::Iterator it = points.begin(); it != points.end(); ++it)
		    {
			QString x, y;

			QStringList parsed = QStringList::split(",", (*it));
			QStringList::Iterator itp = parsed.begin();
			x = (*itp);
			++itp;
			y = (*itp);

			QDomElement kivioPointElement = kivio.createElement("KivioPoint");
			kivioPointElement.setAttribute("x", QString::number(diaPointToKivio(x.toFloat(), true) * scale));
			kivioPointElement.setAttribute("y", QString::number(diaPointToKivio(y.toFloat(), false) * scale));

			kivioShape.appendChild(kivioPointElement);
		    }
		    kivio.documentElement().appendChild(kivioShape);
		}
	    }
	}
	svgNode = svgNode.nextSibling();
    }

    // Apply width and height

    dimensionsElement.setAttribute("w", QString::number((fabs(m_highestx - m_lowestx)) * scale));
    dimensionsElement.setAttribute("h", QString::number((fabs(m_highesty - m_lowesty)) * scale));

    m_xlist.clear();
    m_ylist.clear();

    return loadXML(file, kivio);
}

bool KivioDiaStencilSpawner::loadXML(const QString &file, QDomDocument &d)
{
    bool ret = m_smlStencilSpawner->loadXML(file, d);

    m_icon = *m_smlStencilSpawner->icon();
    m_pSet = m_smlStencilSpawner->set();
    m_pInfo = m_smlStencilSpawner->info();
    m_defWidth = m_smlStencilSpawner->defWidth();
    m_defHeight = m_smlStencilSpawner->defHeight();

    return ret;
}

KivioStencil *KivioDiaStencilSpawner::newStencil()
{
    KivioStencil *newStencil = m_smlStencilSpawner->newStencil();
    newStencil->setSpawner(this);

    return newStencil;
}
