/***************************************************************************
                       main.cc - using the language KDataTool 
					   stand alone (for testing purposes only)
                       -------------------
    begin            : 2002-02-19
    copyright        : (C) 2002 by Daniel Naber
    email            : daniel.naber@t-online.de
	$Id: $
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfile.h>
#include <qstring.h>

#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kdatatool.h>
#include <kdebug.h>
#include <kglobal.h>
#include <klocale.h>

static KCmdLineOptions options[] =
{
	{ "+[filename]",   I18N_NOOP("File that should be checked"), 0 },
	{ 0, 0, 0 }
};

int main(int argc, char **argv)
{

	KAboutData aboutData("klanguage", I18N_NOOP("KLanguage"), "1.0",
		I18N_NOOP( "KLanguageTool - Style and Grammar Tool" ), KAboutData::License_GPL,
		I18N_NOOP( "(c) 2002 Daniel Naber" ));

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);
	// "unused var" warning, but we need it because kdatatool needs an instance:
	KApplication *a = new KApplication();

	KService::Ptr service = KService::serviceByDesktopName("languagetool");
	if( ! service ) {
		kdWarning() << "Could not find Service/KDataTool 'languagetool'!" << endl;
		return 1;
	}

	KDataToolInfo *info = new KDataToolInfo(service, 0);
	KDataTool *tool = info->createTool();
	if ( !tool ) {
		kdWarning() << "Could not create tool 'languagetool'!" << endl;
		return 2;
	}

/* TODO: get selection(), not just clipboard! see kthesaurus */

	QString text = "";
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if ( args->count() > 0 ) {
		QString filename = args->arg(0);
		QFile f(filename);
		if( f.open(IO_ReadOnly) ) {
			QTextStream t( &f );
			text = t.read();
		} else {
			kdWarning() << "Could not open file '" << filename << "'" << endl;
		}
		f.close();
	} else {
		kdError() << "No file specified." << endl;
	}
	// fixme: else? can we do anything useful?

	QString command = "language_standalone";	// 'standalone' will give us different buttons
	QString mimetype = "text/plain";
	QString datatype = "QString";
	tool->run(command, &text, datatype, mimetype);
	// TODO??: save the text again if modified and "Ok" was clicked

	delete tool;
	return 0;
}
