/* This file is part of the KDE project
   Copyright (C) 2001-2006 David Faure <faure@kde.org>
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qfile.h>

#include <kaboutdata.h>
#include <kimageio.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmimetype.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kio/netaccess.h>

#include <KoFilterManager.h>

#include "koconverter.h"


static const KCmdLineOptions options[]=
{
	{"+in", I18N_NOOP("Input file"),0},
	{"+out", I18N_NOOP("Output file"),0},
    {"backup", I18N_NOOP("Make a backup of the destination file"),0},
    {"batch", I18N_NOOP("Batch mode: do not show dialogs"),0},
    {"interactive", I18N_NOOP("Interactive mode: show dialogs (default)"),0},
	{"mimetype <mime>", I18N_NOOP("Mimetype of the output file"),0},
	KCmdLineLastOption
};

bool convert( const KUrl & uIn, const QString & /*inputFormat*/, const KUrl & uOut, const QString & outputFormat, const bool batch )
{
    KoFilterManager* manager = new KoFilterManager( uIn.path() );

    ProgressObject progressObj;
    QObject::connect(manager, SIGNAL(sigProgress(int)), &progressObj, SLOT(slotProgress(int)));

    manager->setBatchMode( batch );

    QByteArray mime( outputFormat.latin1() );
    KoFilter::ConversionStatus status = manager->exp0rt( uOut.path(), mime );
    progressObj.slotProgress(-1);

    delete manager;
    return status == KoFilter::OK;
}

void ProgressObject::slotProgress(int /* progress */ )
{
    // Well, we could have a nifty "=====> " progress bar, but with all the
    // debug output, it would be badly messed up :)
    // kDebug() << "ProgressObject::slotProgress " << progress << endl;
}

int main( int argc, char **argv )
{
    KAboutData aboutData( "koconverter", I18N_NOOP("KOConverter"), "1.4",
                          I18N_NOOP("KOffice Document Converter"),
                          KAboutData::License_GPL,
                          I18N_NOOP("(c) 2001-2004 KOffice developers") );
    aboutData.addAuthor("David Faure",0, "faure@kde.org");
    aboutData.addAuthor("Nicolas Goutte",0, "goutte@kde.org");
    KCmdLineArgs::init( argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication app;

    // Install the libkoffice* translations
    KGlobal::locale()->insertCatalog("koffice");

    

    // Get the command line arguments which we have to parse
    KCmdLineArgs *args= KCmdLineArgs::parsedArgs();
    if ( args->count() == 2 )
    {
        KUrl uIn = args->url( 0 );
        KUrl uOut = args->url( 1 );

        // Are we in batch mode or in interactive mode.
        bool batch = args->isSet("batch");
        if ( args->isSet("interactive") )
            batch = false;

        if ( args->isSet("backup") )
        {
            // Code form koDocument.cc
            KIO::UDSEntry entry;
            if ( KIO::NetAccess::stat( uOut, entry, 0L ) ) // this file exists => backup
            {
                kDebug() << "Making backup..." << endl;;
                KUrl backup( uOut );
                backup.setPath( uOut.path() + '~' );
                KIO::NetAccess::file_copy( uOut, backup, -1, true /*overwrite*/, false /*resume*/, 0L );
            }
        }


        KMimeType::Ptr inputMimetype = KMimeType::findByURL( uIn );
        if ( inputMimetype->name() == KMimeType::defaultMimeType() )
        {
            kError() << i18n("Mimetype for input file %1 not found!").arg(uIn.prettyURL()) << endl;
            return 1;
        }
        KMimeType::Ptr outputMimetype;
        if ( args->isSet("mimetype") )
        {
            QString mime = QString::fromLatin1( args->getOption("mimetype") );
            outputMimetype = KMimeType::mimeType( mime );
            if ( outputMimetype->name() == KMimeType::defaultMimeType() )
            {
                kError() << i18n("Mimetype not found %1").arg(mime) << endl;
                return 1;
            }
        }
        else
        {
            outputMimetype = KMimeType::findByURL( uOut, 0, false, true /* file doesn't exist */ );
            if ( outputMimetype->name() == KMimeType::defaultMimeType() )
            {
                kError() << i18n("Mimetype not found, try using the -mimetype option") << endl;
                return 1;
            }
        }

        QApplication::setOverrideCursor( Qt::WaitCursor );
        bool ok = convert( uIn, inputMimetype->name(), uOut, outputMimetype->name(), batch );
        QApplication::restoreOverrideCursor();
        if ( ok )
        {
            return 0;
        }
        else
        {
            kError() << i18n("*** The conversion failed! ***") << endl;
            return 2;
        }
    }

    KCmdLineArgs::usage(i18n("Two arguments required"));
    return 3;
}

#include "koconverter.moc"
