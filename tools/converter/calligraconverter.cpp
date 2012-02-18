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

#include <calligraversion.h>

bool convertPdf(const KUrl &uIn, const QString &inputFormat, const KUrl &uOut, const QString &outputFormat, const QString &orientation, const QString &papersize, const QString &margin)
{
    Q_UNUSED(outputFormat);

    QString error;
    KoDocument *doc = KMimeTypeTrader::self()->createPartInstanceFromQuery< KoDocument >(
                    inputFormat, 0, 0, QString(), QVariantList(), &error);
    if (!doc) {
        return false;
    }

    doc->setCheckAutoSaveFile(false);
    doc->setAutoErrorHandlingEnabled(false);
    if (!doc->openUrl(uIn)) {
        qDebug() << "The document" << uIn.path() << "of format" << inputFormat
                 << "failed to open with the error" << error;
        doc->deleteLater();
        return false;
    }

    doc->setReadWrite(false);
    KoView *view = doc->createView();
    KoPrintJob *printJob = view->createPdfPrintJob();

    // We should now have a print job - but check to make sure
    if (!printJob) {
        qDebug() << "The document" << uIn.path() << "of format" << inputFormat
                    << "was unable to provide a print job for PDF export";
        doc->deleteLater();
        return false;
    }

    printJob->printer().setOutputFileName(uOut.path());
    printJob->printer().setOutputFormat(QPrinter::PdfFormat);
    printJob->printer().setColorMode(QPrinter::Color);

    if (orientation == "Portrait") printJob->printer().setOrientation(QPrinter::Portrait);
    else if (orientation == "Landscape") printJob->printer().setOrientation(QPrinter::Landscape);

    if (orientation == "A0") printJob->printer().setPaperSize(QPrinter::A0);
    else if (orientation == "A1") printJob->printer().setPaperSize(QPrinter::A1);
    else if (orientation == "A2") printJob->printer().setPaperSize(QPrinter::A2);
    else if (orientation == "A3") printJob->printer().setPaperSize(QPrinter::A3);
    else if (orientation == "A4") printJob->printer().setPaperSize(QPrinter::A4);
    else if (orientation == "A5") printJob->printer().setPaperSize(QPrinter::A5);
    else if (orientation == "A6") printJob->printer().setPaperSize(QPrinter::A6);
    else if (orientation == "A7") printJob->printer().setPaperSize(QPrinter::A7);
    else if (orientation == "A8") printJob->printer().setPaperSize(QPrinter::A8);
    else if (orientation == "A9") printJob->printer().setPaperSize(QPrinter::A9);
    else if (orientation == "B0") printJob->printer().setPaperSize(QPrinter::B0);
    else if (orientation == "B1") printJob->printer().setPaperSize(QPrinter::B1);
    else if (orientation == "B2") printJob->printer().setPaperSize(QPrinter::B2);
    else if (orientation == "B3") printJob->printer().setPaperSize(QPrinter::B3);
    else if (orientation == "B4") printJob->printer().setPaperSize(QPrinter::B4);
    else if (orientation == "B5") printJob->printer().setPaperSize(QPrinter::B5);
    else if (orientation == "B6") printJob->printer().setPaperSize(QPrinter::B6);
    else if (orientation == "B7") printJob->printer().setPaperSize(QPrinter::B7);
    else if (orientation == "B8") printJob->printer().setPaperSize(QPrinter::B8);
    else if (orientation == "B9") printJob->printer().setPaperSize(QPrinter::B9);
    else if (orientation == "B10") printJob->printer().setPaperSize(QPrinter::B10);
    else if (orientation == "C5E") printJob->printer().setPaperSize(QPrinter::C5E);
    else if (orientation == "Comm10E") printJob->printer().setPaperSize(QPrinter::Comm10E);
    else if (orientation == "DLE") printJob->printer().setPaperSize(QPrinter::DLE);
    else if (orientation == "Executive") printJob->printer().setPaperSize(QPrinter::Executive);
    else if (orientation == "Folio") printJob->printer().setPaperSize(QPrinter::Folio);
    else if (orientation == "Ledger") printJob->printer().setPaperSize(QPrinter::Ledger);
    else if (orientation == "Legal") printJob->printer().setPaperSize(QPrinter::Legal);
    else if (orientation == "Letter") printJob->printer().setPaperSize(QPrinter::Letter);
    else if (orientation == "Tabloid") printJob->printer().setPaperSize(QPrinter::Tabloid);

    bool _marginOk;
    qreal _margin = margin.toDouble(&_marginOk);
    if (!_marginOk)
        _margin = 0.2;
    printJob->printer().setPageMargins(_margin, _margin, _margin, _margin, QPrinter::Point);

    printJob->startPrinting();

    doc->deleteLater();

    return true;
}

bool convert(const KUrl &uIn, const QString &inputFormat, const KUrl &uOut, const QString &outputFormat, bool batch)
{
    KoFilter::ConversionStatus status = KoFilter::OK;
    KoFilterManager *manager = new KoFilterManager(uIn.path());
    manager->setBatchMode(batch);
    QByteArray mime(outputFormat.toLatin1());
    status = manager->exportDocument(uOut.path(), mime);
    delete manager;

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

    // PDF related options.
    options.add("print-orientation <name>", ki18n("The print orientation. This could be either Portrait or Landscape."));
    options.add("print-papersize <name>", ki18n("The paper size. A4, Legal, Letter, ..."));
    options.add("print-margin <size>", ki18n("The size of the paper margin. By default this is 0.2."));

    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    // Install the libcalligra* translations
    KGlobal::locale()->insertCatalog("calligra");

    // Get the command line arguments which we have to parse
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->count() != 2) {
        KCmdLineArgs::usageError(i18n("Two arguments required"));
        return 3;
    }

    KUrl urlIn = args->url(0);
    KUrl urlOut = args->url(1);

    // Are we in batch mode or in interactive mode.
    bool batch = args->isSet("batch");
    if (args->isSet("interactive")) {
        batch = false;
    }

    if (args->isSet("backup")) {
        // Code form koDocument.cc
        KIO::UDSEntry entry;
        if (KIO::NetAccess::stat(urlOut, entry, 0L)) {   // this file exists => backup
            kDebug() << "Making backup...";;
            KUrl backup(urlOut);
            backup.setPath(urlOut.path() + '~');
            KIO::FileCopyJob *job = KIO::file_copy(urlOut, backup, -1, KIO::Overwrite | KIO::HideProgressInfo);
            job->exec();
        }
    }


    KMimeType::Ptr inputMimetype = KMimeType::findByUrl(urlIn);
    if (inputMimetype->name() == KMimeType::defaultMimeType()) {
        kError() << i18n("Mimetype for input file %1 not found!", urlIn.prettyUrl()) << endl;
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
        outputMimetype = KMimeType::findByUrl(urlOut, 0, false, true /* file doesn't exist */);
        if (outputMimetype->name() == KMimeType::defaultMimeType()) {
            kError() << i18n("Mimetype not found, try using the -mimetype option") << endl;
            return 1;
        }
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QString outputFormat = outputMimetype->name();
    bool ok = false;
    if (outputFormat == "application/pdf") {
        QString orientation = args->getOption("print-orientation");
        QString papersize = args->getOption("print-papersize");
        QString margin = args->getOption("print-margin");
        ok = convertPdf(urlIn, inputMimetype->name(), urlOut, outputFormat, orientation, papersize, margin);
    } else {
        ok = convert(urlIn, inputMimetype->name(), urlOut, outputFormat, batch);
    }

    QTimer::singleShot(0, &app, SLOT(quit()));
    app.exec();

    QApplication::restoreOverrideCursor();

    if (!ok) {
        kError() << i18n("*** The conversion failed! ***") << endl;
        return 2;
    }

    return 0;
}

