/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001-2006 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2004 Nicolas GOUTTE <goutte@kde.org>
   SPDX-FileCopyrightText: 2011 Dan Leinir Turthra Jensen <admin@leinir.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QMimeDatabase>
#include <QMimeType>
#include <QTimer>

#include <KAboutData>
#include <KIO/FileCopyJob>
#include <KLocalizedString>
#include <KoNetAccess.h>

#include <KoDocument.h>
#include <KoDocumentEntry.h>
#include <KoFilterManager.h>
#include <KoPart.h>
#include <KoPrintJob.h>
#include <KoView.h>
#include <calligra-version.h>

bool convertPdf(const QUrl &uIn,
                const QString &inputFormat,
                const QUrl &uOut,
                const QString &outputFormat,
                const QString &orientation,
                const QString &papersize,
                const QString &margin)
{
    Q_UNUSED(outputFormat);

    QString error;

    KoDocumentEntry documentEntry = KoDocumentEntry::queryByMimeType(inputFormat);
    KoPart *part = documentEntry.createKoPart();
    if (!part) {
        return false;
    }

    KoDocument *doc = part->document();

    doc->setCheckAutoSaveFile(false);
    doc->setAutoErrorHandlingEnabled(false);
    if (!doc->openUrl(uIn)) {
        qDebug() << "The document" << uIn.path() << "of format" << inputFormat << "failed to open with the error" << error;
        doc->deleteLater();
        return false;
    }

    doc->setReadWrite(false);
    KoView *view = part->createView(doc);
    KoPrintJob *printJob = view->createPdfPrintJob();

    // We should now have a print job - but check to make sure
    if (!printJob) {
        qDebug() << "The document" << uIn.path() << "of format" << inputFormat << "was unable to provide a print job for PDF export";
        doc->deleteLater();
        return false;
    }

    printJob->printer().setOutputFileName(uOut.path());
    printJob->printer().setOutputFormat(QPrinter::PdfFormat);
    printJob->printer().setColorMode(QPrinter::Color);

    if (orientation == "Portrait")
        printJob->printer().pageLayout().setOrientation(QPageLayout::Portrait);
    else if (orientation == "Landscape")
        printJob->printer().pageLayout().setOrientation(QPageLayout::Landscape);

    if (papersize == "A0")
        printJob->printer().setPageSize(QPageSize::A0);
    else if (papersize == "A1")
        printJob->printer().setPageSize(QPageSize::A1);
    else if (papersize == "A2")
        printJob->printer().setPageSize(QPageSize::A2);
    else if (papersize == "A3")
        printJob->printer().setPageSize(QPageSize::A3);
    else if (papersize == "A4")
        printJob->printer().setPageSize(QPageSize::A4);
    else if (papersize == "A5")
        printJob->printer().setPageSize(QPageSize::A5);
    else if (papersize == "A6")
        printJob->printer().setPageSize(QPageSize::A6);
    else if (papersize == "A7")
        printJob->printer().setPageSize(QPageSize::A7);
    else if (papersize == "A8")
        printJob->printer().setPageSize(QPageSize::A8);
    else if (papersize == "A9")
        printJob->printer().setPageSize(QPageSize::A9);
    else if (papersize == "B0")
        printJob->printer().setPageSize(QPageSize::B0);
    else if (papersize == "B1")
        printJob->printer().setPageSize(QPageSize::B1);
    else if (papersize == "B2")
        printJob->printer().setPageSize(QPageSize::B2);
    else if (papersize == "B3")
        printJob->printer().setPageSize(QPageSize::B3);
    else if (papersize == "B4")
        printJob->printer().setPageSize(QPageSize::B4);
    else if (papersize == "B5")
        printJob->printer().setPageSize(QPageSize::B5);
    else if (papersize == "B6")
        printJob->printer().setPageSize(QPageSize::B6);
    else if (papersize == "B7")
        printJob->printer().setPageSize(QPageSize::B7);
    else if (papersize == "B8")
        printJob->printer().setPageSize(QPageSize::B8);
    else if (papersize == "B9")
        printJob->printer().setPageSize(QPageSize::B9);
    else if (papersize == "B10")
        printJob->printer().setPageSize(QPageSize::B10);
    else if (papersize == "C5E")
        printJob->printer().setPageSize(QPageSize::C5E);
    else if (papersize == "Comm10E")
        printJob->printer().setPageSize(QPageSize::Comm10E);
    else if (papersize == "DLE")
        printJob->printer().setPageSize(QPageSize::DLE);
    else if (papersize == "Executive")
        printJob->printer().setPageSize(QPageSize::Executive);
    else if (papersize == "Folio")
        printJob->printer().setPageSize(QPageSize::Folio);
    else if (papersize == "Ledger")
        printJob->printer().setPageSize(QPageSize::Ledger);
    else if (papersize == "Legal")
        printJob->printer().setPageSize(QPageSize::Legal);
    else if (papersize == "Letter")
        printJob->printer().setPageSize(QPageSize::Letter);
    else if (papersize == "Tabloid")
        printJob->printer().setPageSize(QPageSize::Tabloid);

    bool _marginOk;
    qreal _margin = margin.toDouble(&_marginOk);
    if (!_marginOk)
        _margin = 0.2;
    printJob->printer().setPageMargins({_margin, _margin, _margin, _margin}, QPageLayout::Point);

    printJob->startPrinting();

    doc->deleteLater();

    return true;
}

bool convert(const QUrl &uIn, const QString &inputFormat, const QUrl &uOut, const QString &outputFormat, bool batch)
{
    Q_UNUSED(inputFormat);

    KoFilter::ConversionStatus status = KoFilter::OK;
    KoFilterManager *manager = new KoFilterManager(uIn.path());
    manager->setBatchMode(batch);
    QByteArray mime(outputFormat.toLatin1());
    status = manager->exportDocument(uOut.path(), mime);
    delete manager;

    return status == KoFilter::OK;
}

QUrl urlFromFileArg(const QString &file)
{
    const QRegularExpression withProtocolChecker(QStringLiteral("^[a-zA-Z]+:"));
    // convert to an url
    const bool startsWithProtocol = (file.indexOf(withProtocolChecker) == 0);
    return startsWithProtocol ? QUrl::fromUserInput(file) : QUrl::fromLocalFile(file);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("koconverter");

    KAboutData aboutData("calligraconverter",
                         i18n("CalligraConverter"),
                         QStringLiteral(CALLIGRA_VERSION_STRING),
                         i18n("Calligra Document Converter"),
                         KAboutLicense::GPL,
                         i18n("Copyright 2001-%1 Calligra developers", QString::number(CALLIGRA_YEAR)));
    aboutData.addAuthor(i18n("David Faure"), QString(), "faure@kde.org");
    aboutData.addAuthor(i18n("Nicolas Goutte"), QString(), "goutte@kde.org");
    aboutData.addAuthor(i18n("Dan Leinir Turthra Jensen"), QString(), "admin@leinir.dk");

    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    parser.addPositionalArgument(QStringLiteral("in"), i18n("Input file"));
    parser.addPositionalArgument(QStringLiteral("out"), i18n("Output file"));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("backup"), i18n("Make a backup of the destination file")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("batch"), i18n("Batch mode: do not show dialogs")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("interactive"), i18n("Interactive mode: show dialogs (default)")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("mimetype"), i18n("Mimetype of the output file"), QStringLiteral("mime")));

    // PDF related options.
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("print-orientation"),
                                        i18n("The print orientation. This could be either Portrait or Landscape."),
                                        QStringLiteral("name")));
    parser.addOption(
        QCommandLineOption(QStringList() << QStringLiteral("print-papersize"), i18n("The paper size. A4, Legal, Letter, ..."), QStringLiteral("name")));
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("print-margin"),
                                        i18n("The size of the paper margin. By default this is 0.2."),
                                        QStringLiteral("size")));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    const QStringList files = parser.positionalArguments();
    if (files.count() != 2) {
        qCritical() << i18n("Two arguments required");
        return 3;
    }

    const QUrl urlIn = urlFromFileArg(files.at(0));
    const QUrl urlOut = urlFromFileArg(files.at(1));

    // Are we in batch mode or in interactive mode.
    bool batch = parser.isSet("batch");
    if (parser.isSet("interactive")) {
        batch = false;
    }

    if (parser.isSet("backup")) {
        // Code form koDocument.cc
        KIO::UDSEntry entry;
        if (KIO::NetAccess::stat(urlOut, entry, nullptr)) { // this file exists => backup
            qDebug() << "Making backup...";
            QUrl backup(urlOut);
            backup.setPath(urlOut.path() + '~');
            KIO::FileCopyJob *job = KIO::file_copy(urlOut, backup, -1, KIO::Overwrite | KIO::HideProgressInfo);
            job->exec();
        }
    }

    QMimeDatabase db;
    QMimeType inputMimetype = db.mimeTypeForUrl(urlIn);
    if (!inputMimetype.isValid() || inputMimetype.isDefault()) {
        qCritical() << i18n("Mimetype for input file %1 not found!", urlIn.toDisplayString());
        return 1;
    }

    QMimeType outputMimetype;
    if (parser.isSet("mimetype")) {
        QString mime = parser.value("mimetype");
        outputMimetype = db.mimeTypeForName(mime);
        if (!outputMimetype.isValid()) {
            qCritical() << i18n("Mimetype not found %1", mime);
            return 1;
        }
    } else {
        outputMimetype = db.mimeTypeForUrl(urlOut);
        if (!outputMimetype.isValid() || outputMimetype.isDefault()) {
            qCritical() << i18n("Mimetype not found, try using the -mimetype option");
            return 1;
        }
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    QString outputFormat = outputMimetype.name();
    bool ok = false;
    if (outputFormat == "application/pdf") {
        QString orientation = parser.value("print-orientation");
        QString papersize = parser.value("print-papersize");
        QString margin = parser.value("print-margin");
        ok = convertPdf(urlIn, inputMimetype.name(), urlOut, outputFormat, orientation, papersize, margin);
    } else {
        ok = convert(urlIn, inputMimetype.name(), urlOut, outputFormat, batch);
    }

    QTimer::singleShot(0, &app, &QApplication::quit);
    app.exec();

    QApplication::restoreOverrideCursor();

    if (!ok) {
        qCritical() << i18n("*** The conversion failed! ***");
        return 2;
    }

    return 0;
}
