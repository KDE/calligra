/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>

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

#include "kexirelationmaindlg.h"

#include <klocale.h>
#include <kdebug.h>
#include <kiconloader.h>

#include <QVBoxLayout>

#include <db/connection.h>

#include <KexiMainWindowIface.h>
#include "kexiproject.h"
#include <widget/relations/KexiRelationsView.h>

KexiRelationMainDlg::KexiRelationMainDlg(QWidget *parent)
        : KexiView(parent)
{
    kDebug() << "KexiRelationMainDlg()";
// setIcon(SmallIcon("relation"));
    m_defaultIconName = "relation";
    setWindowTitle(i18n("Relationships"));
// setDocID( win->generatePrivateDocID() );

    m_rel = new KexiRelationsView(this);
    //the view can receive some our actions
    addActionProxyChild(m_rel);
// addActionProxyChild( m_view->relationsView() );

    QVBoxLayout *g = new QVBoxLayout(this);
    g->addWidget(m_rel);

    //show all tables
    KexiDB::Connection *conn = KexiMainWindowIface::global()->project()->dbConnection();
    QStringList tables = conn->tableNames();
    for (QStringList::ConstIterator it = tables.constBegin(); it != tables.constEnd(); ++it) {
        m_rel->addTable(*it);
    }
}

KexiRelationMainDlg::~KexiRelationMainDlg()
{
}

QSize KexiRelationMainDlg::sizeHint() const
{
    return QSize(600, 300);
}

QWidget*
KexiRelationMainDlg::mainWidget()
{
    return m_rel;
}

QString KexiRelationMainDlg::itemIcon()
{
    return "relation";
}

#include "kexirelationmaindlg.moc"
