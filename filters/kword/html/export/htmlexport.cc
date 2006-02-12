/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002, 2004 Nicolas GOUTTE <goutte@kde.org>

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

/*
   This file is based on the old file:
    /home/kde/koffice/filters/kword/ascii/asciiexport.cc

   The old file was copyrighted by
    Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
    Copyright (c) 2000 ID-PRO Deutschland GmbH. All rights reserved.
                       Contact: Wolf-Michael Bolle <Bolle@ID-PRO.de>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#include <qtextcodec.h>

#include <kdebug.h>
#include <kgenericfactory.h>
#include <KoFilterChain.h>
#include <KoFilterManager.h>

#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include "ExportDialog.h"
#include "ExportFilter.h"
#include "ExportCss.h"
#include "ExportBasic.h"
#include "ExportDocStruct.h"

#include <htmlexport.h>
#include <htmlexport.moc>

typedef KGenericFactory<HTMLExport, KoFilter> HTMLExportFactory;
K_EXPORT_COMPONENT_FACTORY( libhtmlexport, HTMLExportFactory( "kofficefilters" ) )

//
// HTMLExport
//

HTMLExport::HTMLExport(KoFilter *, const char *, const QStringList &) :
                     KoFilter() {
}

KoFilter::ConversionStatus HTMLExport::convert( const QCString& from, const QCString& to )
{
    if ((from != "application/x-kword") || (to != "text/html"))
    {
        return KoFilter::NotImplemented;
    }

    bool batch=false;
    if ( m_chain->manager() )
        batch = m_chain->manager()->getBatchMode();

    HtmlWorker* worker;

    if (batch)
    {
        worker=new HtmlCssWorker();
        worker->setXML(true);
        worker->setCodec(QTextCodec::codecForName("UTF-8"));
    }
    else
    {
        HtmlExportDialog dialog;

        if (!dialog.exec())
        {
            kdDebug(30503) << "Dialog was aborted! Aborting filter!" << endl;
            return KoFilter::UserCancelled;
        }

        const HtmlExportDialog::Mode mode = dialog.getMode();
        switch (mode)
        {
        case HtmlExportDialog::Light:
        worker=new HtmlDocStructWorker();
        break;
        case HtmlExportDialog::Basic:
        worker=new HtmlBasicWorker();
        break;
        case HtmlExportDialog::CustomCSS:
        worker=new HtmlBasicWorker( dialog.cssURL() );
        break;
        default: // Default CSS
        worker=new HtmlCssWorker();
        }

        worker->setXML(dialog.isXHtml());
        worker->setCodec(dialog.getCodec());
    }

    KWEFKWordLeader* leader=new KWEFKWordLeader(worker);

    if (!leader)
    {
        kdError(30503) << "Cannot create Worker! Aborting!" << endl;
        delete worker;
        return KoFilter::StupidError;
    }
    KoFilter::ConversionStatus result=leader->convert(m_chain,from,to);

    delete leader;
    delete worker;

    return result;
}
