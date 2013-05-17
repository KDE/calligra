/* This file is part of the KDE project
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2012 Dimitrios T. Tanis <dimitrios.tanis@kdemail.net>
   Copyright (C) 2013 Yue Liu <yue.liu@mail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXICONNECTIONSELECTORWIDGET_H
#define KEXICONNECTIONSELECTORWIDGET_H

#include <core/kexidbconnectionset.h>
#include <db/driver.h>
#include <kexiutils/KexiContextMessage.h>
#include <widget/KexiServerDriverNotFoundMessage.h>

#include <QPointer>
#include <QTreeWidgetItem>
#include <QLineEdit>
#include <QFileDialog>

//! helper class
class KEXIEXTWIDGETS_EXPORT ConnectionDataLVItem : public QTreeWidgetItem
{
public:
    ConnectionDataLVItem(KexiDB::ConnectionData *data,
                         const KexiDB::Driver::Info& info, QTreeWidget* list);
    ~ConnectionDataLVItem();

    void update(const KexiDB::Driver::Info& info);

    using QTreeWidgetItem::data;
    KexiDB::ConnectionData *data() const {
        return m_data;
    }

protected:
    KexiDB::ConnectionData *m_data;
};

//! @short Widget that allows to select a database connection (file- or server-based)
/*! The widget allows to select database connection without choosing database itself.
*/
class KEXIEXTWIDGETS_EXPORT KexiConnectionSelectorWidget : public QWidget
{
    Q_OBJECT

public:

    /*! Constructs a new widget which contains \a conn_set as connection set.
     \a conn_set can be altered, because Add/Edit/Remove buttons are available
     to users. \a startDirOrVariable can be provided to specify a start dir for file
     browser. */
    KexiConnectionSelectorWidget(KexiDBConnectionSet& conn_set,
                           const QString& startDirOrVariable,
                           QFileDialog::AcceptMode fileAccessType, QWidget* parent = 0);

    virtual ~KexiConnectionSelectorWidget();

    /*! \return data of selected connection, if server-based connection was selected.
     Returns NULL if no selection has been made or file-based connection
     has been selected.
     @see selectedConnectionType()
    */
    KexiDB::ConnectionData* selectedConnectionData() const;

    /*! \return the name of database file, if file-based connection was selected.
     Returns null string if no selection has been made or server-based connection
     has been selected.
     @see selectedConnectionType()
    */
    QString selectedFileName();

    /*! Sets selected filename to \a fileName.
     Only works when selectedConnectionType()==FileBased. */
    void setSelectedFileName(const QString& fileName);

    QTreeWidget* connectionsList() const;

    /*! If true, user will be asked to accept overwriting existing project.
     This is true by default. */
    void setConfirmOverwrites(bool set);

    bool confirmOverwrites() const;

    virtual bool eventFilter(QObject* watched, QEvent* event);
    
    bool hasSelectedConnection() const;

signals:
    void connectionItemExecuted(ConnectionDataLVItem *item);
    void connectionItemHighlighted(ConnectionDataLVItem *item);
    void connectionSelected(bool hasSelected);

public slots:
    void hideConnectonIcon();
    void hideDescription();

protected slots:
    void slotConnectionItemExecuted(QTreeWidgetItem *item);
    void slotConnectionItemExecuted();
    void slotRemoteAddBtnClicked();
    void slotRemoteEditBtnClicked();
    void slotRemoteRemoveBtnClicked();
    void slotConnectionSelectionChanged();
    void slotConnectionSelected();

private:
    ConnectionDataLVItem* addConnectionData(KexiDB::ConnectionData* data);
    ConnectionDataLVItem* selectedConnectionDataItem() const;
    QPointer<KexiServerDriverNotFoundMessage> m_errorMessagePopup;
    QString m_file;
    
    class Private;
    Private * const d;
};

#endif // KEXICONNECTIONSELECTORWIDGET_H
