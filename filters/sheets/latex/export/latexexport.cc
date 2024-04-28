/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "latexexport.h"

#include "latexexportdialog.h"
#include "LatexDebug.h"
// KF5
#include <KoFilterChain.h>
#include <KPluginFactory>
// Qt
#include <QByteArray>

K_PLUGIN_FACTORY_WITH_JSON(LATEXExportFactory, "calligra_filter_kspread2tex.json",
                           registerPlugin<LATEXExport>();)


LATEXExport::LATEXExport(QObject* parent, const QVariantList&) :
        KoFilter(parent)
{
}

KoFilter::ConversionStatus LATEXExport::convert(const QByteArray& from, const QByteArray& to)
{

    if (to != "text/x-tex" || from != "application/x-kspread")
        return KoFilter::NotImplemented;

    KoStore* in = KoStore::createStore(m_chain->inputFile(), KoStore::Read);
    if (!in || !in->open("root")) {
        errorLatex << "Unable to open input file!" << Qt::endl;
        delete in;
        return KoFilter::FileNotFound;
    }
    debugLatex << "In the kspread latex export filter...";
    /* input file Reading */
    in->close();

    LatexExportDialog* dialog = new LatexExportDialog(in);
    dialog->setOutputFile(m_chain->outputFile());

    dialog->exec();
    delete dialog;
    delete in;

    return KoFilter::OK;
}

#include <latexexport.moc>
