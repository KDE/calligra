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

#include "krhtmlrender.h"
#include "renderobjects.h"
#include "orutils.h"
#include "barcodes.h"
#include <kdebug.h>

//
// KRHtmlRender
//

KRHtmlRender::KRHtmlRender ()
{

}

KRHtmlRender::~KRHtmlRender()
{
}

QString KRHtmlRender::render(ORODocument *document, bool css)
{
	kDebug() << endl;
	QString html;
	
	html = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">\n<html>\n<head>\n";
	html += "</head><body>";
				 
	// Render Each Section
	for (long s = 0; s < document->sections(); s++ )
	{

	  OROSection *section = document->section(s);
	  html += "<div style=\"position: relative; top: 0pt; left: 0pt; background-color: " + section->backgroundColor().name() + "; height: " + QString::number(section->height()) + "pt;\">\n";
	  //Render the objects in each section
	  for ( int i = 0; i < section->primitives(); i++ )
	  {
		  
		  OROPrimitive * prim = section->primitive ( i );
		  
		  if ( prim->type() == OROTextBox::TextBox )
		  {
			  OROTextBox * tb = ( OROTextBox* ) prim;
			  html += "<div style=\"position: absolute; ";
			  html += "background-color: " + tb->textStyle().bgColor.name() + "; ";
			  html += "top: " + QString::number(tb->position().y()) + "pt; ";
			  html += "left: " + QString::number(tb->position().x()) + "pt; ";
			  html += "font-size: " + QString::number(tb->textStyle().font.pointSize()) + "; ";
			  html += "color: " + tb->textStyle().fgColor.name() + "; ";
			  html += "filter:alpha(opacity=" + QString::number((tb->textStyle().bgOpacity / 255) * 100) + ");"; //ie opacity
			  html += "opactiy: " + QString::number(tb->textStyle().bgOpacity / 255.0) + "; \">";
			  html += tb->text();
			  html += "</div>\n";
		  }
		  else
		  {
			  kDebug() << "unrecognized primitive type" << endl;
		  }
	  }
	  html += "</div>\n";
	}

	html += "</body></html>";

	return html;
}
