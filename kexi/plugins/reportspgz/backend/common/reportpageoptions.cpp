/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

#include "reportpageoptions.h"
#include <KoPageFormat.h>
#include <KoUnit.h>
#include <KoGlobal.h>

ReportPageOptions::ReportPageOptions()
  : QObject(), _pagesize("Letter"), _labelType(QString::null)
{
  _marginTop = _marginBottom = 1.0;
  _marginLeft = _marginRight = 1.0;

  _orientation = Portrait;

  _customWidth = 8.5;
  _customHeight = 11.0;
}

ReportPageOptions::ReportPageOptions(const ReportPageOptions & rpo)
  : QObject()
{
  _marginTop = rpo._marginTop;
  _marginBottom = rpo._marginBottom;
  _marginLeft = rpo._marginLeft;
  _marginRight = rpo._marginRight;

  _pagesize = rpo._pagesize;
  _customWidth = rpo._customWidth;
  _customHeight = rpo._customHeight;

  _orientation = rpo._orientation;

  _labelType = rpo._labelType;
}

ReportPageOptions & ReportPageOptions::operator=(const ReportPageOptions & rpo)
{
  _marginTop = rpo._marginTop;
  _marginBottom = rpo._marginBottom;
  _marginLeft = rpo._marginLeft;
  _marginRight = rpo._marginRight;

  _pagesize = rpo._pagesize;
  _customWidth = rpo._customWidth;
  _customHeight = rpo._customHeight;

  _orientation = rpo._orientation;

  _labelType = rpo._labelType;

  return *this;
}

qreal ReportPageOptions::getMarginTop()
{
  return _marginTop;
}

void ReportPageOptions::setMarginTop(qreal v)
{
  if (_marginTop == v)
    return;

  _marginTop = v;
  emit pageOptionsChanged();
}

qreal ReportPageOptions::getMarginBottom()
{
  return _marginBottom;
}

void ReportPageOptions::setMarginBottom(qreal v)
{
  if (_marginBottom == v)
    return;

  _marginBottom = v;
  emit pageOptionsChanged();
}

qreal ReportPageOptions::getMarginLeft()
{
  return _marginLeft;
}

void ReportPageOptions::setMarginLeft(qreal v)
{
  if (_marginLeft == v)
    return;

  _marginLeft = v;
  emit pageOptionsChanged();
}

qreal ReportPageOptions::getMarginRight()
{
  return _marginRight;
}

void ReportPageOptions::setMarginRight(qreal v)
{
  if (_marginRight == v)
    return;

  _marginRight = v;
  emit pageOptionsChanged();
}

const QString & ReportPageOptions::getPageSize()
{
  return _pagesize;
}
void ReportPageOptions::setPageSize(const QString & s)
{
  if (_pagesize == s)
    return;

  _pagesize = s;
  emit pageOptionsChanged();
}
qreal ReportPageOptions::getCustomWidth()
{
  return _customWidth;
}
void ReportPageOptions::setCustomWidth(qreal v)
{
  if (_customWidth == v)
    return;

  _customWidth = v;
  emit pageOptionsChanged();
}
qreal ReportPageOptions::getCustomHeight()
{
  return _customHeight;
}
void ReportPageOptions::setCustomHeight(qreal v)
{
  if (_customHeight == v)
    return;

  _customHeight = v;
  emit pageOptionsChanged();
}

ReportPageOptions::PageOrientation ReportPageOptions::getOrientation()
{
  return _orientation;
}

bool ReportPageOptions::isPortrait()
{
  return (_orientation == Portrait);
}

void ReportPageOptions::setOrientation(PageOrientation o)
{
  if (_orientation == o)
    return;

  _orientation = o;
  emit pageOptionsChanged();
}
void ReportPageOptions::setPortrait(bool yes)
{
  setOrientation((yes ? Portrait : Landscape));
}

const QString & ReportPageOptions::getLabelType()
{
  return _labelType;
}
void ReportPageOptions::setLabelType(const QString & type)
{
  if (_labelType == type)
    return;

  _labelType = type;
  emit pageOptionsChanged();
}

//Convenience functions that return the page width/height in pixels based on the DPI
qreal ReportPageOptions::widthPx()
{
	int pageWidth;
	
	if (isPortrait())
	{
		pageWidth = KoPageFormat::width(KoPageFormat::formatFromString(getPageSize()), KoPageFormat::Portrait);
	}
	else
	{
		pageWidth = KoPageFormat::width(KoPageFormat::formatFromString(getPageSize()), KoPageFormat::Landscape);
	}
	
	KoUnit pageUnit(KoUnit::Millimeter);
	pageWidth = KoUnit::toInch(pageUnit.fromUserValue(pageWidth)) * KoGlobal::dpiX();
	
	return pageWidth;
}

qreal ReportPageOptions::heightPx()
{
	int pageHeight;
	
	if (isPortrait())
	{
		pageHeight = KoPageFormat::height(KoPageFormat::formatFromString(getPageSize()), KoPageFormat::Portrait);		
	}
	else
	{
		pageHeight = KoPageFormat::height(KoPageFormat::formatFromString(getPageSize()), KoPageFormat::Landscape);
	}
	
	KoUnit pageUnit(KoUnit::Millimeter);
	pageHeight = KoUnit::toInch(pageUnit.fromUserValue(pageHeight)) * KoGlobal::dpiY();
	
	return pageHeight;
}