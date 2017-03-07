// kate: space-indent on; indent-width 4; replace-tabs off;
/*
   This file is part of the KDE project
   Copyright (C) 2001 Ewald Snel <ewald@rambo.its.tudelft.nl>
   Copyright (C) 2001 Tomasz Grobelny <grotk@poczta.onet.pl>
   Copyright (C) 2003, 2004 Nicolas GOUTTE <goutte@kde.org>
   Copyright (C) 2011 Boudewijn Rempt <boud@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
*/
#include "rtfimport.h"

#include <kpluginfactory.h>
#include <kmessagebox.h>
#include <klocalizedstring.h>

#include <KoFilterChain.h>
#include <KoFilterManager.h>

#include "rtfreader.h"
#include "TextDocumentRtfOutput.h"
#include <QTextDocument>
#include <QTextDocumentWriter>

K_PLUGIN_FACTORY_WITH_JSON(RTFImportFactory, "calligra_filter_rtf2odt.json",
                           registerPlugin<RTFImport>();)

RTFImport::RTFImport(QObject* parent, const QVariantList&)
    : KoFilter(parent)
{
}

KoFilter::ConversionStatus RTFImport::convert(const QByteArray& from, const QByteArray& to)
{
    // This filter only supports RTF to Words conversion
    if ((from != "application/rtf") || (to != "application/vnd.oasis.opendocument.text")) {
        return KoFilter::NotImplemented;
    }

    // Are we in batch mode, i.e. non-interactive
    bool batch = false;
    if (m_chain->manager())
        batch = m_chain->manager()->getBatchMode();

    // Open input file
    QString inFileName = m_chain->inputFile();

    RtfReader::Reader reader;
    if (!reader.open(inFileName)) {
        if (!batch) {
            KMessageBox::error(0,
                               i18n("The file cannot be loaded, as it seems not to be an RTF document."),
                               i18n("Words's RTF Import Filter"), 0);
        }
        return KoFilter::WrongFormat;
    }

    QTextDocument doc;
    RtfReader::TextDocumentRtfOutput *output = new RtfReader::TextDocumentRtfOutput( &doc );
    reader.parseTo(output);

    QFile saveFile(m_chain->outputFile());
    if (!saveFile.open(QIODevice::WriteOnly)) {
        if (!batch) {
            KMessageBox::error(0,
                               i18n("The file cannot be loaded."),
                               i18n("Words's RTF Import Filter"), 0);
        }
        return KoFilter::CreationError;
    }

    QTextDocumentWriter writer(&saveFile, "odf");
    if (!writer.write(&doc)) {
        if (!batch) {
            KMessageBox::error(0,
                               i18n("The file cannot be loaded."),
                               i18n("Words's RTF Import Filter"), 0);

        }
        return KoFilter::CreationError;
    }

    saveFile.close();

    return KoFilter::OK;
}

#include "rtfimport.moc"
