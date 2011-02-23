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

#ifndef DOCUMENTSLISTDIALOG_H
#define DOCUMENTSLISTDIALOG_H


#include "ui_DocumentsListDialog.h"

#include <QDialog>


class DocumentsListService;
class GoogleDocument;
class QModelIndex;
class DocumentsListTreeModel;
class QEvent;
class KMenu;
class KAction;
class KActionMenu;
class QSignalMapper;

class DocumentsListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DocumentsListDialog(QWidget* parent, DocumentsListService *docListSrv/*, Qt::WindowFlags f = 0*/);
    virtual ~DocumentsListDialog();

    int selectedDocument() const { return m_selectedRow; }

public Q_SLOTS:
    void showDocumentsList(QList<GoogleDocument*> &entryList, QString &title);
    void slotWaitForDocList(bool b);
                           
private Q_SLOTS:
    void accept();
    void slotDownloadDocument(const QString &format);
    void slotDeleteDocument();
    void slotRenameDocument();
    void slotUploadDocument();
    void createExportAsMenu();
    
private:
    ///reimplemented from superclass
    bool eventFilter(QObject *object, QEvent *event);
    
    void resizeColumn(const QModelIndex& index);
    
    DocumentsListService *m_docListSrv;
    Ui_DocumentsListDialog *m_ui_docListDialog;
    DocumentsListTreeModel *m_model;
    KAction *m_deleteAction;
    KAction *m_renameAction;
    KActionMenu *m_exportAsMenuAction;
    QSignalMapper *m_exportAsSignalMapper;
    KMenu *m_contextMenu;
    KMenu *m_exportAsMenu;
    int m_selectedRow;
};

#endif //DOCUMENTSLISTDIALOG_H
