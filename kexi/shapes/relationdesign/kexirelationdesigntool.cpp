/* This file is part of the KDE project
   Copyright (C) 2009-2010 Adam Pigg <adam@piggz.co.uk>
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kexirelationdesigntool.h"
#include "kexirelationdesignshape.h"

#include "kexi/main/startup/KexiStartupDialog.h"
#include "kexi/kexidb/utils.h"
#include <KoSelection.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <KUrl>
#include <KFileDialog>
#include <KIO/Job>
#include <kdebug.h>
#include <KComboBox>

KexiRelationDesignTool::KexiRelationDesignTool ( KoCanvasBase* canvas ) : KoToolBase ( canvas )
{
    m_dbDialog = 0;
}

void KexiRelationDesignTool::mouseReleaseEvent ( KoPointerEvent* event )
{
    Q_UNUSED(event);
}

void KexiRelationDesignTool::mouseMoveEvent ( KoPointerEvent* event )
{
    Q_UNUSED(event);
}

void KexiRelationDesignTool::mousePressEvent ( KoPointerEvent* event )
{
    Q_UNUSED(event);
}

void KexiRelationDesignTool::paint ( QPainter& painter, const KoViewConverter& converter )
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

void KexiRelationDesignTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(toolActivation);
    foreach (KoShape *shape, shapes) {
        m_relationDesign = dynamic_cast<KexiRelationDesignShape*>( shape );
        if ( m_relationDesign )
            break;
    }
    if ( !m_relationDesign )
    {
        emit done();
        return;
    }
    useCursor(Qt::ArrowCursor);
}

void KexiRelationDesignTool::deactivate()
{
}

QWidget* KexiRelationDesignTool::createOptionWidget()
{
    QWidget *optionWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(optionWidget);
    
    m_fileLabel = new QLabel(optionWidget);
    QToolButton *fileButton = 0;

    m_relationCombo = new KComboBox();
    
    fileButton = new QToolButton(optionWidget);
    fileButton->setIcon(SmallIcon("document-open"));
    fileButton->setToolTip(i18n( "Open Database"));
    
    connect(fileButton, SIGNAL(clicked(bool)), this, SLOT(changeUrlPressed()));
    connect(m_relationCombo, SIGNAL(activated(const QString&)), this, SLOT(relationSelected(const QString&)));
    
    layout->addWidget(fileButton);
    layout->addWidget(m_fileLabel);
    layout->addWidget(m_relationCombo);
    layout->addStretch();
    optionWidget->setLayout(layout);

    return optionWidget;
}

void KexiRelationDesignTool::changeUrlPressed()
{
    if (m_relationDesign == 0)
        return;

    KexiProjectSet kps;
    KexiDBConnectionSet kcs;

    if (!m_dbDialog) {
        m_dbDialog = new KexiStartupDialog(KexiStartupDialog::OpenExisting, 0, kcs, kps);
        //connect( m_dbDialog, SIGNAL( okClicked() ), this, SLOT( openDatabase() ) );
    }

    int res = m_dbDialog->exec();
    kDebug() << res;
    KexiDB::ConnectionData *_cd = m_dbDialog->selectedExistingConnection();
    
    if (_cd) {
        m_fileLabel->setText(_cd->description);
        kDebug() << _cd->description << _cd->caption;
    }
    else {
        kDebug() << "No connectiondata!";
        kDebug() << m_dbDialog->selectedFileName();
        m_fileLabel->setText(m_dbDialog->selectedFileName());
        
        _cd = new KexiDB::ConnectionData();
        _cd->setFileName(m_dbDialog->selectedFileName());
        _cd->driverName = KexiDB::defaultFileBasedDriverName().toLocal8Bit();
    }

    m_relationDesign->setConnectionData(_cd);

    updateCombo();
}

void KexiRelationDesignTool::updateCombo()
{
    m_relationCombo->clear();
    m_relationCombo->addItems(queryList());
}

QStringList KexiRelationDesignTool::queryList()
{
    KexiDB::Connection* conn = m_relationDesign->connection();
    
    //Get the list of queries in the database
    QStringList qs;
    if (conn && conn->isConnected()) {
        QList<int> tids = conn->tableIds();
        qs << "";
        for (int i = 0; i < tids.size(); ++i) {
            KexiDB::TableSchema* tsc = conn->tableSchema(tids[i]);
            if (tsc)
                qs << tsc->name();
        }
        
        QList<int> qids = conn->queryIds();
        qs << "";
        for (int i = 0; i < qids.size(); ++i) {
            KexiDB::QuerySchema* qsc = conn->querySchema(qids[i]);
            if (qsc)
                qs << qsc->name();
        }
    }
    
    return qs;
}

void KexiRelationDesignTool::relationSelected(const QString& rel)
{
    if (m_relationDesign == 0)
        return;

    m_relationDesign->setRelation(rel);
}
