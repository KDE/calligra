/* This file is part of the KDE project
   Copyright (C) 2003 Alexander Dymo <cloudtemple@mksat.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include <qfile.h>
#include <qmessagebox.h>
#include <qfileinfo.h>

#include "mreportviewer.h"

#include "kugarmain.h"

KugarMain::KugarMain(QString dataFile, QWidget* parent, const char* name, WFlags fl)
        : KugarMainBase(parent,name,fl)
{
    rptviewer = new MReportViewer(this);
    connect(rptviewer,SIGNAL(preferedTemplate(const QString &)),
        SLOT(slotPreferredTemplate(const QString &)));

    setCentralWidget(rptviewer);

    gen_ok = true;

    QFile f(dataFile);
    if (f.open(IO_ReadOnly))
    {
        QFileInfo fi(dataFile);
        dataPath = fi.dirPath(true);
        if (!rptviewer->setReportData(&f))
        {
            QMessageBox::critical(this,"Kugar",tr("Invalid data file: %1").arg(dataFile));
            gen_ok = false;
        }
        f.close();
    }
    else
    {
        QMessageBox::critical(this,"Kugar",tr("Unable to open data file: %1").arg(dataFile));
        gen_ok = false;
    }

    rptviewer->renderReport();
    rptviewer->show();
}

KugarMain::~KugarMain()
{
    delete rptviewer;
}

/*$SPECIALIZATION$*/
void KugarMain::viewLastPage()
{
    rptviewer->slotLastPage();
}

void KugarMain::viewFirstPage()
{
    rptviewer->slotFirstPage();
}

void KugarMain::viewPrevPage()
{
    rptviewer->slotPrevPage();
}

void KugarMain::viewNextPage()
{
    rptviewer->slotNextPage();
}

void KugarMain::helpAbout()
{
    QMessageBox::information(this, "Kugar", tr("Kugar report viewer\n(C) 2003 by Alexander Dymo.\nLicensed after GPL."));
}

void KugarMain::fileExit()
{
    qApp->quit();
}

void KugarMain::filePrint()
{
    rptviewer->printReport();
}

void KugarMain::slotPreferredTemplate( const QString & tpl )
{
    QString realtpl = tpl;
    if (!dataPath.isEmpty())
        realtpl.prepend(dataPath + QString("/"));
    QFile ft(realtpl);
    if (ft.open(IO_ReadOnly))
    {
        if (!rptviewer->setReportTemplate(&ft))
        {
            QMessageBox::critical(this, "Kugar", tr("Invalid template file: %1").arg(tpl));
            gen_ok = false;
        }
        ft.close();
    }
    else
    {
        QMessageBox::critical(this,"Kugar",tr("Unable to open template file: %1").arg(tpl));
        gen_ok = false;
    }
}

