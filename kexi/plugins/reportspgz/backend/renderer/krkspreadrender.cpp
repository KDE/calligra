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
#include <kspread/Doc.h>
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

void KRKSpreadRender::render(ORODocument *doc)
{
	KSpread::Doc *ksdoc = new KSpread::Doc();
	
	KSpread::Sheet *sht = ksdoc->map()->createSheet();
	
	sht->setSheetName(doc->title());
	sht->setText(0,0,"Hello");
	sht->setText(1,1,"Hello");
	
	///=============================
	
	KoStore* store = KoStore::createStore( "/home/piggz/kexitest.ods", KoStore::Write);
	if ( store->bad() )
	{
		delete store;
		return;
	}

	
	kDebug() <<"Saving to OASIS format";
    // Tell KoStore not to touch the file names
	store->disallowNameExpansion();
	KoOdfWriteStore odfStore( store );

	KoXmlWriter* manifestWriter = odfStore.manifestWriter( ksdoc->mimeType() );

	KoEmbeddedDocumentSaver embeddedSaver;
	KoDocument::SavingContext documentContext( odfStore, embeddedSaver );

	if ( !ksdoc->saveOdf( documentContext ) )
	{
		kDebug() <<"saveOdf failed";
		delete store;
		return;
	}
	
	// Save embedded objects
	if ( !embeddedSaver.saveEmbeddedDocuments( documentContext ) )
	{
		kDebug() <<"save embedded documents failed";
		delete store;
		return ;
	}

    // Write out manifest file
	if ( !odfStore.closeManifestWriter() )
	{
		kDebug() << "Error while trying to write 'META-INF/manifest.xml'. Partition full?";
		delete store;
		return;
	}

	if ( !store->finalize() )
	{
		delete store;
		return;
	}
	
	kDebug() << "SAVED OK" << endl;
	delete store;
	
	delete doc;
}
