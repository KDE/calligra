#ifndef KARBON_ABOUTDATA_H
#define KARBON_ABOUTDATA_H

#include <kaboutdata.h>
#include <klocale.h>
#include <config.h>

// This is all implemented here so that the executable and the part can share it
// without sharing an object file.
KAboutData * newKarbonAboutData()
{
	KAboutData * aboutData = new KAboutData(
		"karbon",
		I18N_NOOP( "Karbon14" ),
		"0.1",
		I18N_NOOP( "Yet another vector graphics application." ),
		KAboutData::License_GPL,
		I18N_NOOP( "(c) 2001-2004, The Karbon Developers" ),
		I18N_NOOP( "You are invited to participate in any way." ),
		"http://www.xs4all.nl/~rwlbuis/karbon/");
	aboutData->addAuthor(
		"Rob Buis",
		0,
		"buis@kde.org",
		0 );
	aboutData->addAuthor(
		"Tomislav Lukman",
		0,
		"tomislav.lukman@ck.hinet.hr",
		0 );
	aboutData->addAuthor(
		"Benoît Vautrin",
		0,
		"benoit.vautrin@free.fr",
		0 );
	// TODO: add the names of some helpful souls
        return aboutData;
}

#endif /* KARBON_ABOUTDATA_H */

