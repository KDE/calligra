/*
 * Kexi Report Plugin
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
#include "krkspreadrender.h"
#include <kspread/part/Doc.h>
#include <kspread/Map.h>
#include <kspread/Sheet.h>
#include <KoStore.h>
#include <KoOdfWriteStore.h>
#include <KoEmbeddedDocumentSaver.h>
#include <KoDocument.h>

KRKSpreadRender::KRKSpreadRender()
{
}


KRKSpreadRender::~KRKSpreadRender()
{
}

void KRKSpreadRender::render(ORODocument *document,const QString& sn)
{
	KSpread::Doc *ksdoc = new KSpread::Doc();
	
	KSpread::Sheet *sht = ksdoc->map()->addNewSheet();
	
	sht->setSheetName(document->title());

	bool renderedPageHead = false;
	bool renderedPageFoot = false;
	
	// Render Each Section
	for (long s = 0; s < document->sections(); s++ )
	{
		OROSection *section = document->section(s);
		section->sortPrimatives(OROSection::SortX);

		if (section->type() == KRSectionData::GroupHead || 
			section->type() == KRSectionData::GroupFoot || 
			section->type() == KRSectionData::Detail || 
			section->type() == KRSectionData::ReportHead || 
			section->type() == KRSectionData::ReportFoot || 
			(section->type() == KRSectionData::PageHeadAny && !renderedPageHead) || 
			(section->type() == KRSectionData::PageFootAny && !renderedPageFoot && s > document->sections() - 2 )) //render the page foot right at the end, it will either be the last or second last section if there is a report footer
		{
			if ( section->type() == KRSectionData::PageHeadAny )
			  renderedPageHead = true;

			if ( section->type() == KRSectionData::PageFootAny )
			  renderedPageFoot = true;

			//Render the objects in each section
			for ( int i = 0; i < section->primitives(); i++ )
			{
				OROPrimitive * prim = section->primitive ( i );
				
				if ( prim->type() == OROTextBox::TextBox )
				{
					OROTextBox * tb = ( OROTextBox* ) prim;
					
					sht->setText(s+1,i+1,tb->text());
				}
				/*
				else if (prim->type() == OROImage::Image)
				{
					kDebug() << "Saving an image" << endl;
					OROImage * im = ( OROImage* ) prim;
					tr += "<td>";
					tr += "<img src=\"./" + fi.fileName() + "/object" + QString::number(s) + QString::number(i) + ".png\"></img>";
					tr += "</td>\n";
					im->image().save(saveDir + "/object" + QString::number(s) + QString::number(i) + ".png");
				}
				else if (prim->type() == OROPicture::Picture)
				{
					kDebug() << "Saving a picture" << endl;
					OROPicture * im = ( OROPicture* ) prim;
					
					tr += "<td>";
					tr += "<img src=\"./" + fi.fileName() + "/object" + QString::number(s) + QString::number(i) + ".png\"></img>";
					tr += "</td>\n";
					QImage image(im->size().toSize(), QImage::Format_RGB32);
					QPainter painter(&image);
					im->picture()->play(&painter);
					image.save(saveDir + "/object" + QString::number(s) + QString::number(i) + ".png");
				}*/
				else
				{
					kDebug() << "unhandled primitive type" << endl;
				}
			}
		}
	}
	
	if (ksdoc->exportDocument(KUrl::fromPath(sn)))
	{
		kDebug() << "SAVED OK" << endl;
	}
	else
	{
		kDebug() << "NOT SAVED OK" << endl;
	}

	delete ksdoc;
}
