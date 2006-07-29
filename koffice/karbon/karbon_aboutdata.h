#ifndef KARBON_ABOUTDATA_H
#define KARBON_ABOUTDATA_H

#include <kaboutdata.h>
#include <klocale.h>
#include <config.h>

static const char* description=I18N_NOOP( "A Vector Graphics Drawing Application." );
static const char* version=VERSION;

// This is all implemented here so that the executable and the part can share it
// without sharing an object file.
KAboutData * newKarbonAboutData()
{
	KAboutData * aboutData = new KAboutData(
		"karbon",
		I18N_NOOP( "Karbon14" ),
		version,
		description,
		KAboutData::License_GPL,
		I18N_NOOP( "(c) 2001-2006, The Karbon Developers" ),
		I18N_NOOP( "You are invited to participate in any way." ),
		"http://www.koffice.org/karbon/");
	aboutData->addAuthor(
		"Rob Buis",
		0,
		"buis@kde.org",
		0 );
	aboutData->addAuthor(
		"Tomislav Lukman",
		0,
		"tomislav.lukman@ck.t-com.hr",
		0 );
	aboutData->addAuthor(
		"Benoît Vautrin",
		0,
		"benoit.vautrin@free.fr",
		0 );
	aboutData->addCredit(
		"Jan Hambrecht",
		I18N_NOOP( "Bug fixes and improvements" ),
		"jaham@gmx.net",
		0 );
	aboutData->addCredit(
		"Peter Simonsson",
		I18N_NOOP( "Bug fixes and improvements" ),
		"psn@linux.se",
		0 );
	aboutData->addCredit(
		"Tim Beaulen",
		I18N_NOOP( "Bug fixes and improvements" ),
		"tbscope@gmail.com",
		0 );
	aboutData->addCredit(
		"Boudewijn Rempt",
		I18N_NOOP( "Bug fixes and improvements" ),
		"boud@valdyas.org",
		0 );
	aboutData->addCredit(
		"Pierre Stirnweiss",
		I18N_NOOP( "Bug fixes and improvements" ),
		"pierre.stirnweiss_kde@gadz.org",
		0 );
	aboutData->addCredit(
		"Inge Wallin",
		I18N_NOOP( "Bug fixes" ),
		"inge@lysator.liu.se",
		0 );
	aboutData->addCredit(
		"Alan Horkan",
		I18N_NOOP( "Helpfull patches and advice" ),
		0,
		0 );
	// TODO: add the names of some helpful souls
        return aboutData;
}

#endif /* KARBON_ABOUTDATA_H */

