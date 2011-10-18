/* This file is part of the KDE project
   Copyright (C) 2001-2006 David Faure <faure@kde.org>
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (C) 2011 Dan Leinir Turthra Jensen <admin@leinir.dk>

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

#include <QTimer>

#include <kaboutdata.h>
#include <kimageio.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kglobal.h>
#include <kmimetype.h>
#include <kmimetypetrader.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kio/job.h>

#include <KoDocument.h>
#include <KoFilterManager.h>
#include <KoPrintJob.h>
#include <KoView.h>

#include <tables/part/Doc.h>
#include <tables/part/View.h>
#include <tables/Map.h>
#include <tables/Sheet.h>

#include <calligraversion.h>

bool convert(const KUrl &uIn, const QString &inputFormat, const KUrl &uOut, const QString &outputFormat, bool batch)
{
    KoFilter::ConversionStatus status = KoFilter::OK;
    if (outputFormat == "application/pdf") {
        QString error;
        KoDocument *doc = KMimeTypeTrader::self()->createPartInstanceFromQuery< KoDocument >(
                              inputFormat, 0, 0, QString(), QVariantList(), &error);
        if (doc) {
            doc->setCheckAutoSaveFile(false);
            doc->setAutoErrorHandlingEnabled(false);
            if (doc->openUrl(uIn)) {
                doc->setReadWrite(false);
                KoPrintJob *printJob = 0;
                Calligra::Tables::Doc *tdoc = qobject_cast< Calligra::Tables::Doc * >(doc);
                if (tdoc) {
                    Calligra::Tables::View *tview = new Calligra::Tables::View(0, tdoc);
                    tview->setActiveSheet(tdoc->map()->sheet(0));
                    printJob = tview->createPdfPrintJob();
                } else {
                    KoView *view = doc->createView();
                    printJob = view->createPdfPrintJob();
                }
                // We should now have a print job - but check to make sure
                if (printJob) {
                    printJob->printer().setOutputFileName(uOut.path());
                    printJob->printer().setOutputFormat(QPrinter::PdfFormat);
                    printJob->startPrinting();
                } else {
                    status = KoFilter::StupidError;
                    qDebug() << "The document" << uIn.path()
                             << "of format" << inputFormat
                             << "was unable to provide a print job for PDF export";
                }
            } else {
                status = KoFilter::InvalidFormat;
                qDebug() << "The document" << uIn.path()
                         << "of format" << inputFormat
                         << "failed to open with the error" << error;
            }
            doc->deleteLater();
            doc = 0;
        } else {
            status = KoFilter::WrongFormat;
        }
    } else {
        KoFilterManager *manager = new KoFilterManager(uIn.path());
        manager->setBatchMode(batch);
        QByteArray mime(outputFormat.toLatin1());
        status = manager->exportDocument(uOut.path(), mime);
        delete manager;
    }
    return status == KoFilter::OK;
}

int main(int argc, char **argv)
{
    KAboutData aboutData("calligraconverter", 0, ki18n("CalligraConverter"), CALLIGRA_VERSION_STRING,
                         ki18n("Calligra Document Converter"),
                         KAboutData::License_GPL,
                         ki18n("(c) 2001-2011 Calligra developers"));
    aboutData.addAuthor(ki18n("David Faure"), KLocalizedString(), "faure@kde.org");
    aboutData.addAuthor(ki18n("Nicolas Goutte"), KLocalizedString(), "goutte@kde.org");
    aboutData.addAuthor(ki18n("Dan Leinir Turthra Jensen"), KLocalizedString(), "admin@leinir.dk");
    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("+in", ki18n("Input file"));
    options.add("+out", ki18n("Output file"));
    options.add("backup", ki18n("Make a backup of the destination file"));
    options.add("batch", ki18n("Batch mode: do not show dialogs"));
    options.add("interactive", ki18n("Interactive mode: show dialogs (default)"));
    options.add("mimetype <mime>", ki18n("Mimetype of the output file"));
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    // Install the libcalligra* translations
    KGlobal::locale()->insertCatalog("calligra");

    // Get the command line arguments which we have to parse
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->count() == 2) {
        KUrl uIn = args->url(0);
        KUrl uOut = args->url(1);

        // Are we in batch mode or in interactive mode.
        bool batch = args->isSet("batch");
        if (args->isSet("interactive"))
            batch = false;

        if (args->isSet("backup")) {
            // Code form koDocument.cc
            KIO::UDSEntry entry;
            if (KIO::NetAccess::stat(uOut, entry, 0L)) {   // this file exists => backup
                kDebug() << "Making backup...";;
                KUrl backup(uOut);
                backup.setPath(uOut.path() + '~');
                KIO::FileCopyJob *job = KIO::file_copy(uOut, backup, -1, KIO::Overwrite | KIO::HideProgressInfo);
                job->exec();
            }
        }


        KMimeType::Ptr inputMimetype = KMimeType::findByUrl(uIn);
        if (inputMimetype->name() == KMimeType::defaultMimeType()) {
            kError() << i18n("Mimetype for input file %1 not found!", uIn.prettyUrl()) << endl;
            return 1;
        }
        KMimeType::Ptr outputMimetype;
        if (args->isSet("mimetype")) {
            QString mime = args->getOption("mimetype");
            outputMimetype = KMimeType::mimeType(mime);
            if (! outputMimetype) {
                kError() << i18n("Mimetype not found %1", mime) << endl;
                return 1;
            }
        } else {
            outputMimetype = KMimeType::findByUrl(uOut, 0, false, true /* file doesn't exist */);
            if (outputMimetype->name() == KMimeType::defaultMimeType()) {
                kError() << i18n("Mimetype not found, try using the -mimetype option") << endl;
                return 1;
            }
        }

        QApplication::setOverrideCursor(Qt::WaitCursor);
        bool ok = convert(uIn, inputMimetype->name(), uOut, outputMimetype->name(), batch);
        QTimer::singleShot(0, &app, SLOT(quit()));
        app.exec();
        QApplication::restoreOverrideCursor();
        if (ok) {
            return 0;
        } else {
            kError() << i18n("*** The conversion failed! ***") << endl;
            return 2;
        }
    }

    KCmdLineArgs::usageError(i18n("Two arguments required"));
    return 3;
}
