/*

    Copyright 2011 Cuong Le <metacuong@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "3rdparty/networkcontroller.h"
#include "3rdparty/listmodel.h"
#include "3rdparty/folderitem.h"
#include "3rdparty/filetransferitem.h"
#include "3rdparty/options.h"

class QAction;

class Controller : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool needAuthenticate READ need_authenticate NOTIFY needAuthenticateChanged)
public:
    explicit Controller(QObject *parent = 0);
    ~Controller() override;

    ListModel *folder_model;
    ListModel *filestransfer_model;
    Options m_options;

    QList<FolderItem *> m_deleted_list;

private:
    QHash<QString, ListModel*> m_cache;
    QList<QString> m_move_cache;

    NetworkController *m_networkcontroller;

    bool m_multi_selection;
    int m_current_filetransferitem;
    int m_current_deleted_item;
    int m_current_move_item;

    void opt_save_user_account();

    int m_current_list_pos;

    void folderlist_have_item_checked();
    void start_delete_items();
    void start_move_items();

    QAction* m_uploadMostRecentAction;

Q_SIGNALS:
    void needAuthenticateChanged();
    void authenticate_finished(QString email, QString password);
    void network_error(QString error);
    void folderfinished();
    void setcheckindexchanged(uint index);
    void progressBarChanged(const int &percent, const double &speed,const qint64 &sent_received);
    void nothingtotransfer();
    void startTransfer();
    void stopTransfer();
    void notification(const QString &notification);
    void delete_selected_items_finished(const QString &result);
    void create_folder_finished(const QString &result);
    void rename_folder_finished(const QString &result);
    void enable_download_and_delete_button(const bool &have_checked);
    void can_not_created_dropbox_folder(const QString &foldername);
    void stop_and_cancel_finished();
    void move_files_folders_finished(const QString &result);
    void shares_finished(const bool &result);
    void shares_metadata(const QString &url, const QString &expire);
    void accountinfo_finished(const bool &result);
    void accountinfo_metadata(const QVariant &result);
    void orientation_changed();
    void open_oauth_authorize_page(const QString &oauth_token);

public Q_SLOTS:
    void authenticate(QString, QString);
    void getlistoffolder(QString folder_name = "");
    void getfolder(const QVariantMap&);
    void folder_model_clear();
    void setFolderRoot(const QString&);
    bool isRootDir() const;
    void backtoRootDir();
    bool is_multi_selection() const;
    void setMultiSelection();
    void setCheck(const int &, const bool&);
    void transfer(const QString&, const bool&);
    void transfer_process();
    void current_transfer_stop_and_cancel();
    void updown_finished();
    void cancel_login();
    void logout();
    void downloadSelectedFiles();
    bool is_transfer() const;
    void authenticate_ok();
    void getfolder_done();
    void refresh_current_folder();
    void delete_selected_items();
    void delete_finished(const bool&);
    void createnewfolder(const QString&);
    void renamefileorfolder(const QString&,const QString&);
    void transfer_clear_log();
    void start_shares(const QString&);
    void start_transfer_process();
    void createnewfolder_finished(const bool&);
    void renamenewfolder_finished(const bool&);
    void add_move_files_folders_to_cache();
    void move_files_folders_to_clear_cache();
    void start_move_files_folders();
    void movefilesfolders_finished(const bool&);
    bool filesize_is_valid(const QString&) const;
    bool need_authenticate() const;
    void accountinfo();
    QString getcurrentdir() const;
    double parseDouble(const QString&) const;
    bool is_duplicated_file_folder(const QString&) const;
    void commit_orientation();
    QString get_file_size(const QString&) const;
    bool is_push_notification() const;

    void add_copy_files_folders_to_cache();
    void copy_files_folders_to_clear_cache();
    void start_copy_files_folders();
    void start_copy_items();
    void copyfilesfolders_finished(const bool&);

    void oauth_v1_request_token();
    void oauth_v1_access_token();

    QString dropboxFolder() { return NetworkController::Dropbox_Folder(); };
    QAction* uploadMostRecentAction();
    void uploadMostRecent();
};

#endif // CONTROLLER_H

