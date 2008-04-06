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
#include "renderobjects.h"


//
// ORODocument
//
ORODocument::ORODocument(const QString & pTitle)
  : _title(pTitle)
{
}

ORODocument::~ORODocument()
{
  while(!_pages.isEmpty())
  {
    OROPage * p = _pages.takeFirst();
    p->_document = 0;
    delete p;
  }
}

void ORODocument::setTitle(const QString & pTitle)
{
  _title = pTitle;
}

OROPage* ORODocument::page(int pnum)
{
  return _pages.at(pnum);
}

void ORODocument::addPage(OROPage* p)
{
  if(p == 0)
    return;

  // check that this page is not already in another document

  p->_document = this;
  _pages.append(p);
}

void ORODocument::setPageOptions(const ReportPageOptions & options)
{
  _pageOptions = options;
}

//
// OROPage
//
OROPage::OROPage(ORODocument * pDocument)
  : _document(pDocument)
{

}

OROPage::~OROPage()
{
  if(_document != 0)
  {
    _document->_pages.removeAt(page());
    _document = 0;
  }

  while(!_primitives.isEmpty())
  {
    OROPrimitive* p = _primitives.takeFirst();
    p->_page = 0;
    delete p;
  }
}

int OROPage::page() const
{
  if(_document != 0)
  {
    for(int i = 0; i < _document->_pages.size(); i++)
    {
      if(_document->_pages.at(i) == this)
        return i;
    }
  }
  return -1;
}

OROPrimitive* OROPage::primitive(int idx)
{
  return _primitives.at(idx);
}

void OROPage::addPrimitive(OROPrimitive* p, bool atBeginning)
{
  if(p == 0)
    return;

  // check that this primitve is not already in another page

  p->_page = this;
  if (atBeginning)
  {
	  _primitives.prepend(p);
  }
  else
  {
  	_primitives.append(p);
  }
}


//
// OROPrimitive
//
OROPrimitive::OROPrimitive(int pType)
  : _type(pType)
{
  _page = 0;
}

OROPrimitive::~OROPrimitive()
{
  if(_page != 0)
  {
    _page->_primitives.removeAt(_page->_primitives.indexOf(this));
    _page = 0;
  }
}

void OROPrimitive::setPosition(const QPointF & p)
{
  _position = p;
}

//
// OROTextBox
//
const int OROTextBox::TextBox = 1;
OROTextBox::OROTextBox()
  : OROPrimitive(OROTextBox::TextBox)
{
  _flags = 0;
  
   _lineStyle.lnColor=Qt::black;
   _lineStyle.weight = 0;
   _lineStyle.style = Qt::NoPen;
}

OROTextBox::~OROTextBox()
{
}

void OROTextBox::setSize(const QSizeF & s)
{
  _size = s;
}

void OROTextBox::setText(const QString & s)
{
  _text = s;
}

void OROTextBox::setTextStyle(const ORTextStyleData & ts)
{
	_textStyle = ts;
}

void OROTextBox::setLineStyle(const ORLineStyleData & ls)
{
	_lineStyle = ls;
}

void OROTextBox::setFont(const QFont & f)
{
  _textStyle.font = f;
}

void OROTextBox::setFlags(int f)
{
  _flags = f;
}

//
// OROLine
//
const int OROLine::Line = 2;

OROLine::OROLine()
  : OROPrimitive(OROLine::Line)
{

} 

OROLine::~OROLine()
{
}

void OROLine::setStartPoint(const QPointF & p)
{
  setPosition(p);
}

void OROLine::setEndPoint(const QPointF & p)
{
  _endPoint = p;
}

void OROLine::setLineStyle(const ORLineStyleData& ls)
{
  _ls = ls;
}

//
// OROImage
//
const int OROImage::Image = 3;

OROImage::OROImage()
  : OROPrimitive(OROImage::Image)
{
  _scaled = false;
  _transformFlags = Qt::FastTransformation;
  _aspectFlags = Qt::IgnoreAspectRatio;
}

OROImage::~OROImage()
{
}

void OROImage::setImage(const QImage & img)
{
  _image = img;
}

void OROImage::setSize(const QSizeF & sz)
{
  _size = sz;
}

void OROImage::setScaled(bool b)
{
  _scaled = b;
}

void OROImage::setTransformationMode(int tm)
{
  _transformFlags = tm;
}

void OROImage::setAspectRatioMode(int arm)
{
  _aspectFlags = arm;
}

//
// ORORect
//
const int ORORect::Rect = 4;

ORORect::ORORect()
  : OROPrimitive(ORORect::Rect)
{
}

ORORect::~ORORect()
{
}

void ORORect::setSize(const QSizeF & s)
{
  _size = s;
}

void ORORect::setRect(const QRectF & r)
{
  setPosition(r.topLeft());
  setSize(r.size());
}

void ORORect::setPen(const QPen & p)
{
  _pen = p;
}

void ORORect::setBrush(const QBrush & b)
{
  _brush = b;
}

//
// OROEllipse
//
const int OROEllipse::Ellipse = 5;

OROEllipse::OROEllipse()
	: OROPrimitive(OROEllipse::Ellipse)
{
}

OROEllipse::~OROEllipse()
{
}

void OROEllipse::setSize(const QSizeF & s)
{
	_size = s;
}

void OROEllipse::setRect(const QRectF & r)
{
	setPosition(r.topLeft());
	setSize(r.size());
}

void OROEllipse::setPen(const QPen & p)
{
	_pen = p;
}

void OROEllipse::setBrush(const QBrush & b)
{
	_brush = b;
}