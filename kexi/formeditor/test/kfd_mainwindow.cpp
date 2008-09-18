/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
#include <kaction.h>
#include <kstandardaction.h>
#include <kurl.h>
#include <kdebug.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kapplication.h>

#include "kfd_mainwindow.h"

KFDMainWindow::KFDMainWindow()
        : KParts::MainWindow()
{
    setXMLFile("kfd_mainwindow.rc");

    setupActions();
    //statusBar()->show();

    KLibFactory *factory = KLibLoader::self()->factory("libkformdesigner_part");
    if (factory) {
        QStringList list;
        list << "shell" << "multipleMode";
        m_part = static_cast<KParts::ReadWritePart *>(factory->create(this, "kformdesigner_part", "KParts::ReadWritePart", list));

        if (m_part) {
            setCentralWidget(m_part->widget());
            createGUI(m_part);
        }
    } else {
        KMessageBox::error(this, i18n("Could not find the KFormDesigner part. Please check your installation."));
        kapp->quit();
        return;
    }

    setAutoSaveSettings();
}

void
KFDMainWindow::loadUIFile(const QString &filename)
{
    loadUIFile(KUrl::fromPathOrURL(filename));
}

void
KFDMainWindow::loadUIFile(const KUrl &url)
{
    m_part->openUrl(url);
}

void
KFDMainWindow::setupActions()
{
    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
}

bool
KFDMainWindow::queryClose()
{
    if (!m_part)
        return true;

    return m_part->closeUrl();
}

#include "kfd_mainwindow.moc"
