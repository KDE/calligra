// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
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

#include <kdebug.h>
#include <kgenericfactory.h>

#include <KWEFBaseWorker.h>
#include <KWEFKWordLeader.h>

#include "ExportDialog.h"
#include "ExportFilter.h"

#include <htmlexport.h>
#include <htmlexport.moc>

typedef KGenericFactory<HTMLExport, KoFilter> HTMLExportFactory;
K_EXPORT_COMPONENT_FACTORY( libhtmlexport, HTMLExportFactory( "kwordhtmlexportfilter" ) );

//
// HTMLExport
//

HTMLExport::HTMLExport(KoFilter *parent, const char *name, const QStringList &) :
                     KoFilter(parent, name) {
}

bool HTMLExport::filter(const QString  &filenameIn,
                        const QString  &filenameOut,
                        const QString  &from,
                        const QString  &to,
                        const QString  &param )
{
    if ((from != "application/x-kword") || (to != "text/html"))
    {
        return false;
    }

    HtmlExportDialog* dialog = new HtmlExportDialog();

    if (!dialog)
    {
        kdError(30503) << "Dialog has not been created! Aborting!" << endl;
        return false;
    }

    if (!dialog->exec())
    {
        kdError(30503) << "Dialog was aborted! Aborting filter!" << endl;
        return false;
    }

    HtmlWorker* worker=new HtmlWorker();

    if (!worker)
    {
        delete dialog;
        kdError(30503) << "Cannot create Worker! Aborting!" << endl;
        return false;
    }

    worker->setXML(dialog->isXHtml());
    worker->setCodec(dialog->getCodec());

    delete dialog;

    KWEFKWordLeader* leader=new KWEFKWordLeader(worker);

    if (!leader)
    {
        kdError(30503) << "Cannot create Worker! Aborting!" << endl;
        delete worker;
        return false;
    }

    bool flag=leader->filter(filenameIn,filenameOut,from,to,param);

    delete leader;
    delete worker;

    return flag;
}
