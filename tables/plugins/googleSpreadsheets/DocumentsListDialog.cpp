/* This file is part of the KDE project
   Copyright (C) 2010-2011 Christoph Goerlich <chgoerlich@gmx.de>

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
   Boston, MA 02110-1301, USA.
*/

#include "DocumentsListDialog.h"
#include "DocumentsListTreeModel.h"
#include "DocumentsListService.h"
#include "GoogleDocument.h"

#include <KMenu>
#include <KActionMenu>
#include <KFileDialog>

#include <QSignalMapper>
#include <QMessageBox>

#include <QDebug>


DocumentsListDialog::DocumentsListDialog(QWidget* parent, DocumentsListService* docListSrv/*, Qt::WindowFlags f*/)
    : QDialog(parent/*, f*/),
    m_docListSrv(docListSrv), 
    m_ui_docListDialog(new Ui_DocumentsListDialog),
    m_model(0),
    m_deleteAction(0),
    m_renameAction(0),
    m_exportAsMenuAction(0),
    m_exportAsSignalMapper(new QSignalMapper(this)),
    m_contextMenu(new KMenu(this)),
    m_exportAsMenu(0),
    m_selectedRow(-1)
{   
    m_ui_docListDialog->setupUi(this);
    m_ui_docListDialog->treeView->installEventFilter(this);
    
    m_ui_docListDialog->cbDocType->setCurrentIndex(static_cast<int>(m_docListSrv->docType()));
    
    m_deleteAction = new KAction(i18n("Delete Document"), this);
    connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(slotDeleteDocument()));
    m_contextMenu->addAction(m_deleteAction);

    m_renameAction = new KAction(i18n("Rename Document"), this);
    m_contextMenu->addAction(m_renameAction);
    
    m_exportAsMenuAction = new KActionMenu(i18n("Export As..."), this);
    m_exportAsMenu = m_exportAsMenuAction->menu();
    connect(m_exportAsMenu, SIGNAL(aboutToShow()), this, SLOT(createExportAsMenu()));
    m_contextMenu->addAction(m_exportAsMenuAction); 
    
    connect(m_exportAsSignalMapper, SIGNAL(mapped(const QString&)), this, 
            SLOT(slotDownloadDocument(const QString&)));
    
    connect(m_docListSrv, SIGNAL(updateDocListDialog(QList<GoogleDocument*>&,QString&)), this, 
            SLOT(showDocumentsList(QList<GoogleDocument*>&,QString&)));
    connect(m_docListSrv, SIGNAL(waitForDocList(bool)), this, SLOT(slotWaitForDocList(bool)));
            
    connect(m_ui_docListDialog->uploadBtn, SIGNAL(clicked()), this, SLOT(slotUploadDocument()));
    connect(m_ui_docListDialog->treeView, SIGNAL(doubleClicked(const QModelIndex&)), this, 
            SLOT(accept()));
    connect(m_ui_docListDialog->cbDocType, SIGNAL(currentIndexChanged(int)), m_docListSrv, 
            SLOT(setDocType(int)));
}

DocumentsListDialog::~DocumentsListDialog()
{
    delete m_ui_docListDialog;
    
    if (m_model)
        delete m_model;
}

bool DocumentsListDialog::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::ContextMenu && object == m_ui_docListDialog->treeView) {
        QModelIndex selected = m_ui_docListDialog->treeView->currentIndex();
        m_selectedRow = selected.row();
        m_contextMenu->exec(QCursor::pos());
        return true;
    }
    else {
        //pass event on to parent class
        return QDialog::eventFilter(object, event);
    }
}

void DocumentsListDialog::showDocumentsList(QList<GoogleDocument*>& entryList, QString &title)
{
    if (m_model) {
        delete m_model;
        m_model = 0;
    }
    
    setWindowTitle(title);
    
    m_model = new DocumentsListTreeModel(entryList);
    m_ui_docListDialog->treeView->setModel(m_model);
    m_ui_docListDialog->treeView->show();
    for (int i = 0; i < m_model->columnCount(); ++i)
       m_ui_docListDialog->treeView->resizeColumnToContents(i);
    
    m_ui_docListDialog->treeView->setEnabled(true);
}

void DocumentsListDialog::accept()
{
    QModelIndex selected = m_ui_docListDialog->treeView->currentIndex();

    m_selectedRow = selected.row();
    m_docListSrv->openDocument(m_selectedRow);
    
    QDialog::accept();
}

void DocumentsListDialog::slotDownloadDocument(const QString& format)
{
    m_docListSrv->downloadDocument(m_selectedRow, format);
}

void DocumentsListDialog::slotDeleteDocument()
{
    QModelIndex selected = m_ui_docListDialog->treeView->currentIndex();
    GoogleDocument *doc = m_docListSrv->documentsList().at(selected.row());
    QString str = "Do you really want to delete the document \"" + doc->title() + "\"?";;
    QMessageBox::StandardButton btn = QMessageBox::warning(this, tr("Deleting document..."), str, QMessageBox::Yes | QMessageBox::No);
    if (btn == QMessageBox::Yes)
        m_docListSrv->deleteDocument(selected.row());
}

void DocumentsListDialog::slotRenameDocument()
{
}

void DocumentsListDialog::slotUploadDocument()
{
    QStringList filePathes = KFileDialog::getOpenFileNames(KUrl::fromPath(QDir::homePath()), QString(), this, "Select file(s) to upload...");
    if (filePathes.isEmpty())
        return;

    foreach (const QString &filePath, filePathes)
        m_docListSrv->uploadDocument(filePath);
        //emit uploadDocument(filePath);
}

void DocumentsListDialog::createExportAsMenu()
{
    m_exportAsMenu->clear();
    
    GoogleDocument *doc = m_docListSrv->documentsList().at(m_selectedRow);
    
    if (doc) {
        QStringList formats;
        formats = m_docListSrv->formatsForExport(doc->docType());
        
        //Export-Formats, that aren't listed in the DocList-Metadata-Feed, but are decribed in online-docu
        if (doc->docType() == "document") {
            formats << "jpeg" << "png" << "svg" << "zip";
        }
        else if (doc->docType() == "presentation") {
            formats << "swf" << "txt" << "png";
        }
        else if (doc->docType() == "spreadsheet") {
            formats << "csv" << "tsv" << "html";
        }
        
        foreach (const QString &format, formats) {
            KAction *action = new KAction(format, m_exportAsMenu);
            connect(action, SIGNAL(triggered()), m_exportAsSignalMapper, SLOT(map()));
            m_exportAsSignalMapper->setMapping(action, format);
            m_exportAsMenu->addAction(action);
        }
    }    
}

void DocumentsListDialog::slotWaitForDocList(bool b)
{
    m_ui_docListDialog->treeView->setEnabled(!b);
}

void DocumentsListDialog::resizeColumn(const QModelIndex& index)
{
    int column = index.column();
    m_ui_docListDialog->treeView->resizeColumnToContents(column);
}

#include "DocumentsListDialog.moc"
