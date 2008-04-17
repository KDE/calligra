//
// C++ Implementation: krkspreadrenderer
//
// Description: 
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
