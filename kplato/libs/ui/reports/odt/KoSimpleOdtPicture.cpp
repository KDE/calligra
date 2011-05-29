/*
   KoReport Library
   Copyright (C) 2011 by Dag Andersen (danders@get2net.dk)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KoSimpleOdtPicture.h"
#include <KoXmlWriter.h>
#include <KoDpi.h>

#include "renderobjects.h"

#include <kdebug.h>

KoSimpleOdtPicture::KoSimpleOdtPicture(OROPrimitive *primitive)
    : KoSimpleOdtPrimitive(primitive)
{
}

KoSimpleOdtPicture::~KoSimpleOdtPicture()
{
}

OROPicture *KoSimpleOdtPicture::picture() const
{
    return dynamic_cast<OROPicture*>(m_primitive);
}

void KoSimpleOdtPicture::createBody(KoXmlWriter *bodyWriter) const
{
    bodyWriter->startElement("draw:frame");
    bodyWriter->addAttribute("draw:style-name", "picture");
    bodyWriter->addAttribute("draw:name", QString("name_%1x%2").arg(m_primitive->position().x()).arg(m_primitive->position().y()));
    bodyWriter->addAttribute("text:anchor-type", "page");

    commonAttributes(bodyWriter);

    bodyWriter->startElement("draw:image");
    bodyWriter->addAttribute("xlink:href", "Picture/" + pictureName());
    bodyWriter->addAttribute("xlink:type", "simple");
    bodyWriter->addAttribute("xlink:show", "embed");
    bodyWriter->addAttribute("xlink:actuate", "onLoad");
    bodyWriter->endElement();

    bodyWriter->endElement(); // draw:frame
}
