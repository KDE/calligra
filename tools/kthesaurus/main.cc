/***************************************************************************
                       main.cc - using the thesaurus KDataTool stand alone
                       -------------------
    begin            : 2001-12-22
    copyright        : (C) 2001 by Daniel Naber
    email            : daniel.naber@t-online.de
	$Id$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QString>
#include <QTextDocument>

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdemacros.h>
#include <kcomponentdata.h>

#include <KoTextEditingPlugin.h>
#include <KoTextEditingRegistry.h>
#include <KoTextEditingFactory.h>

extern "C" KDE_EXPORT int kdemain(int argc, char **argv)
{
	KAboutData aboutData("kthesaurus", 0, ki18n("KThesaurus"), "1.0",
		ki18n( "KThesaurus - List synonyms" ), KAboutData::License_GPL,
		ki18n( "(c) 2001 Daniel Naber" ) );

	KCmdLineArgs::init(argc, argv, &aboutData);

	KCmdLineOptions options;
	options.add("+[term]", ki18n("Term to search for when starting up"));
	KCmdLineArgs::addCmdLineOptions(options);
	KApplication a;

    KoTextEditingPlugin *thesaurus = 0;

    foreach(const QString &key, KoTextEditingRegistry::instance()->keys()) {
        KoTextEditingFactory *factory =  KoTextEditingRegistry::instance()->value(key);
        Q_ASSERT(factory);
        if (factory->id() == "thesaurustool") {
            kDebug() <<"Thesaurus plugin found, creating...";
            thesaurus = factory->create();
        }
    }

    if (!thesaurus) // No thesaurus plugin found
        return 1;

/* TODO: get selection(), not only clipboard!
	QClipboard *cb = QApplication::clipboard();
	QString text = cb->text();
	if( text.isNull() || text.length() > 50 ) {
		// long texts are probably not supposed to be searched for
		text = "";
	}
*/
	QString text = "";
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if ( args->count() > 0 ) {
		text = args->arg(0);
	}

    // pas -1 to startPosition and endPosition to set as standalone and will give us different buttons
    thesaurus->checkSection(new QTextDocument(text), -1, -1);

	//kDebug() <<"KThesaurus command=" << command
	//		<< " dataType=" << info->dataType() << endl;

	return a.exec();
}
