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
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <qdom.h>
#include <QFile>
#include <QRegExp>
#include <qstringlist.h>
#include <kdebug.h>
#include <math.h>

#include "kivio_stencil.h"
#include "kivio_dia_stencil_spawner.h"
#include "diapathparser.h"

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
	float returnPoint = 0.0;
	if(xpoint)
	{
		//if(m_lowestx < 0)
			returnPoint = point - m_lowestx;
		//else
		//	returnPoint =  (fabs(m_highestx) - fabs(m_lowestx)) - (fabs(m_highestx) + fabs(point));
	}
	else
	{
		//if(m_lowesty <  0)
			returnPoint =point - m_lowesty;
		//else
		//	returnPoint =  (fabs(m_highesty) + fabs(m_lowesty)) - (fabs(m_highesty) + fabs(point));
	}
	//kDebug () << "Point " << point << " Return point " << returnPoint << endl;
	return returnPoint;
}

bool KivioDiaStencilSpawner::load(const QString &file)
{
	QDomDocument dia("test");
	QDomDocument kivio("XML");

	m_filename = file;
	QFile f(file);

	if(f.open(QIODevice::ReadOnly) == false)
	{
		kDebug(43000) << "KivioDiaStencilSpawner::load() - Error opening stencil: " << file << endl;
		return false;
	}
	dia.setContent(&f);
	QDomNode diaMain = dia.namedItem("shape");

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

	m_xscale = m_yscale = 20.0f;

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
			else if(svgChild.tagName() == "svg:path")
			{
				if(svgChild.hasAttribute("d"))
				{
					DiaPointFinder *dpf = new DiaPointFinder(&m_xlist, &m_ylist);
					dpf->parseSVG(svgChild.attribute("d"), true);
					delete dpf;
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
		m_highestx = qMax(m_highestx, *itx);
		m_lowestx = qMin(m_lowestx, *itx);
	}

	for( ; ity != m_ylist.end(); ++ity)
	{
		m_highesty = qMax(m_highesty, *ity);
		m_lowesty = qMin(m_lowesty, *ity);
	}

	//if( svgElement.hasAttribute("width") && svgElement.hasAttribute("height"))
	//{
	//	m_yscale = svgElement.attribute("height").toFloat()/(m_highesty - m_lowesty);
	//	m_xscale = svgElement.attribute("width").toFloat()/(m_highestx - m_lowestx);
	//}
	//else
	{
		// scale the shape to be close to 30 by 30
		m_yscale = 30.0/(m_highesty - m_lowesty);
		m_xscale = 30.0/(m_highestx - m_lowestx);
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
					kivioConnectorTarget.setAttribute("x", QString::number(diaPointToKivio(connectionChild.attribute("x").toFloat(),true) * m_xscale));
					kivioConnectorTarget.setAttribute("y", QString::number(diaPointToKivio(connectionChild.attribute("y").toFloat(), false) * m_yscale));

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
					kivioShape.setAttribute("x", QString::number(diaPointToKivio(svgChild.attribute("x").toFloat(),true) * m_xscale));
					kivioShape.setAttribute("y", QString::number(diaPointToKivio(svgChild.attribute("y").toFloat(), false) * m_yscale));
					kivioShape.setAttribute("w", QString::number(svgChild.attribute("width").toFloat() * m_xscale));
					kivioShape.setAttribute("h", QString::number(svgChild.attribute("height").toFloat() * m_yscale));
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
					kivioShape.setAttribute("x", QString::number((diaPointToKivio(svgChild.attribute("cx").toFloat() - svgChild.attribute("r").toFloat(),true) * m_xscale)));
					kivioShape.setAttribute("y", QString::number((diaPointToKivio(svgChild.attribute("cy").toFloat() - svgChild.attribute("r").toFloat(), false) * m_yscale)));
					kivioShape.setAttribute("w", QString::number(svgChild.attribute("r").toFloat() * m_xscale * 2));
					kivioShape.setAttribute("h", QString::number(svgChild.attribute("r").toFloat() * m_yscale * 2));
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
					kivioShape.setAttribute("x", QString::number((diaPointToKivio(svgChild.attribute("cx").toFloat() - svgChild.attribute("rx").toFloat(),true) * m_xscale)));
					kivioShape.setAttribute("y", QString::number((diaPointToKivio(svgChild.attribute("cy").toFloat() - svgChild.attribute("ry").toFloat(), false) * m_yscale)));
					kivioShape.setAttribute("w", QString::number(svgChild.attribute("rx").toFloat() * m_xscale * 2));
					kivioShape.setAttribute("h", QString::number(svgChild.attribute("ry").toFloat() * m_yscale * 2));
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
					lineArrayElement.setAttribute("x1", QString::number(diaPointToKivio(svgChild.attribute("x1").toFloat(),true) * m_xscale));
					lineArrayElement.setAttribute("y1", QString::number(diaPointToKivio(svgChild.attribute("y1").toFloat(), false) * m_yscale));
					lineArrayElement.setAttribute("x2", QString::number(diaPointToKivio(svgChild.attribute("x2").toFloat(),true) * m_xscale));
					lineArrayElement.setAttribute("y2", QString::number(diaPointToKivio(svgChild.attribute("y2").toFloat(), false) * m_yscale));

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
						kivioPointElement.setAttribute("x", QString::number(diaPointToKivio(x.toFloat(),true) * m_xscale));
						kivioPointElement.setAttribute("y", QString::number(diaPointToKivio(y.toFloat(), false) * m_yscale));

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
						kivioPointElement.setAttribute("x", QString::number(diaPointToKivio(x.toFloat(),true) * m_xscale));
						kivioPointElement.setAttribute("y", QString::number(diaPointToKivio(y.toFloat(), false) * m_yscale));

						kivioShape.appendChild(kivioPointElement);
					}
					kivio.documentElement().appendChild(kivioShape);
				}
			}
			else if(svgChild.tagName() == "svg:path")
			{
				runs++;
				bool isClosed;
				QDomElement kivioShape = kivio.createElement("KivioShape");
				if(svgChild.hasAttribute("d"))
				{

					if(svgChild.attribute("d").contains('z') || svgChild.attribute("d").contains('Z'))
					{
						isClosed = true;
						kivioShape.setAttribute("type", "ClosedPath");
					}
					else
					{
						isClosed = false;
						kivioShape.setAttribute("type", "OpenPath");
					}

					kivioShape.setAttribute("name", QString::fromLatin1("Element") + QString::number(runs));

					DiaPathParser *dpp = new DiaPathParser(&kivio,
						&kivioShape, m_xscale, m_yscale,
						m_lowestx, m_lowesty);
					dpp->parseSVG(svgChild.attribute("d"), true);
					delete dpp;


				}

				if( svgChild.hasAttribute("style"))
				{
					// style="stroke: background; stroke-width: 0.8; stroke-miterlimit: 1; stroke-linecap: round; stroke-linejoin: round"
					// Supported:
					// stroke-width:
					// stroke-linejoin: milter, bevel, round
					// stroke-linecap: round, square, flat
					// fill: ?
					QStringList styles = QStringList::split(";", svgChild.attribute("style"));
					for( uint idx = 0; idx < styles.count(); idx++)
					{
						//kDebug(43000) << "Style: " << styles[idx] << endl;
						if( isClosed && styles[idx].contains("fill:"))
						{
							QDomElement fillStyle = kivio.createElement("KivioFillStyle");
							if( styles[idx].contains("forground"))
								fillStyle.setAttribute("color", "#0000");
							else if (styles[idx].contains("background"))
								fillStyle.setAttribute("color", "#ffff");

							fillStyle.setAttribute("colorStyle", "1");
							kivioShape.appendChild(fillStyle);
						}
					}
				}
				kivio.documentElement().appendChild(kivioShape);
			}
		}
		svgNode = svgNode.nextSibling();
	}

	// Apply width and height

	dimensionsElement.setAttribute("w", QString::number((fabs(m_highestx - m_lowestx)) * m_xscale));
	dimensionsElement.setAttribute("h", QString::number((fabs(m_highesty - m_lowesty)) * m_yscale));

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
