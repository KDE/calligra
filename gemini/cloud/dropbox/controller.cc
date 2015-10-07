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

#include "controller.h"
#include <QStringList>
#include <QSettings>
#include <QDir>

#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <QTimer>
#include <QAction>
#include <QApplication>

#include <KoIcon.h>

//#include <MNotification>

Controller::Controller(QObject *parent) :
    QObject(parent),
    m_networkcontroller(new NetworkController(parent)),
    m_multi_selection(false),
    m_current_filetransferitem(0),
    m_uploadMostRecentAction(0)
{
    folder_model = new ListModel(new FolderItem, parent);
    filestransfer_model = new ListModel(new FileTransferItem, parent);

    m_current_list_pos = 0;

    connect(m_networkcontroller, SIGNAL(authenticate_finished()), SLOT(authenticate_ok()));
    connect(m_networkcontroller, SIGNAL(network_error(QString)), SIGNAL(network_error(QString)));
    connect(m_networkcontroller, SIGNAL(getfolder_finished(const QVariantMap&)), SLOT(getfolder(const QVariantMap&)));
    connect(m_networkcontroller, SIGNAL(getfolder_done()), SLOT(getfolder_done()));

    connect(m_networkcontroller, SIGNAL(progressBarChanged(int,double,qint64)), SIGNAL(progressBarChanged(int,double,qint64)));
    connect(m_networkcontroller, SIGNAL(file_download_finished()), SLOT(updown_finished()));
    connect(m_networkcontroller, SIGNAL(file_upload_finished()), SLOT(updown_finished()));

    connect(m_networkcontroller, SIGNAL(delete_finished(bool)), SLOT(delete_finished(bool)));
    connect(m_networkcontroller, SIGNAL(stop_and_cancel_finished()), SIGNAL(stop_and_cancel_finished()));

    connect(m_networkcontroller, SIGNAL(create_finished(bool)), SLOT(createnewfolder_finished(bool)));
    connect(m_networkcontroller, SIGNAL(rename_finished(bool)), SLOT(renamenewfolder_finished(bool)));
    connect(m_networkcontroller, SIGNAL(move_finished(bool)), SLOT(movefilesfolders_finished(bool)));
    connect(m_networkcontroller, SIGNAL(copy_finished(bool)), SLOT(copyfilesfolders_finished(bool)));


    connect(m_networkcontroller, SIGNAL(shares_finished(bool)),SIGNAL(shares_finished(bool)));
    connect(m_networkcontroller, SIGNAL(shares_metadata(QString,QString)),SIGNAL(shares_metadata(QString,QString)));

    connect(m_networkcontroller, SIGNAL(accountinfo_metadata(QVariant)), SIGNAL(accountinfo_metadata(QVariant)));
    connect(m_networkcontroller, SIGNAL(accountinfo_finished(bool)),SIGNAL(accountinfo_finished(bool)));

    connect(m_networkcontroller, SIGNAL(open_oauth_authorize_page(QString)), SIGNAL(open_oauth_authorize_page(QString)));
}

Controller::~Controller(){
    delete m_networkcontroller;
    delete folder_model;
    delete filestransfer_model;
}

void Controller::authenticate(QString email, QString password){

    m_networkcontroller->m_user_email = email;
    m_networkcontroller->m_user_password = password;
    m_networkcontroller->authenticate();
}

void Controller::authenticate_ok(){
    emit authenticate_finished(m_networkcontroller->m_user_email,
                               m_networkcontroller->m_user_password);
    QTimer::singleShot(250, this, SIGNAL(needAuthenticateChanged()));
}

void Controller::getlistoffolder(QString folder_name){

    QString folder_name1(m_networkcontroller->m_currentDir);
    if (folder_name1.isEmpty())
        folder_name1 = "root";

    folder_model->clear();


    if (m_cache[folder_name1]){
        ListModel *t = m_cache[folder_name1];

        int f;
        for (f=0;f<t->getCount();f++){
            FolderItem *fi=(FolderItem*)t->getRow(f);
            folder_model->appendRow(new FolderItem(
                                             fi->revision(), //revision
                                             fi->thumb_exists(), //thumb_exists
                                             fi->bytes(), //bytes
                                             fi->modified(), //modified
                                             fi->path(), //path
                                             fi->is_dir(), // is_dir
                                             fi->icon(), //icon
                                             fi->mime_type(), //mime_type
                                             fi->size() //size
                                             , folder_model));
        }

        emit folderfinished();
    }else{
        m_networkcontroller->getfolderlist(folder_name);
    }
}

void Controller::getfolder(const QVariantMap &item){
    folder_model->appendRow(new FolderItem(
                                item["thumb_exists"].toReal(), //revision
                                item["thumb_exists"].toBool(), //thumb_exists
                                item["bytes"].toReal(), //bytes
                                item["modified"].toString(), //modified
                                item["path"].toString(), //path
                                item["is_dir"].toBool(), // is_dir
                                item["icon"].toString(), //icon
                                item["mime_type"].toString(), //mime_type
                                item["size"].toString() //size
                                , folder_model));
}

void Controller::folder_model_clear(){
    folder_model->clear();
}

void Controller::setFolderRoot(const QString &folder_root){
    m_networkcontroller->m_currentDir = folder_root;
}

bool Controller::isRootDir() const{
    if (m_networkcontroller->m_currentDir.isEmpty() || m_networkcontroller->m_currentDir == "/")
        return true;
    else
        return false;
}

void Controller::backtoRootDir() {
    QStringList backtodir = m_networkcontroller->m_currentDir.split("/");

    QString finaldir("");
    int i;
    for (i=0;i<backtodir.length()-1;i++)
        finaldir+=backtodir[i]+"/";

    finaldir.chop(1);

    m_networkcontroller->m_currentDir = finaldir;
}

bool Controller::is_multi_selection() const{
    return m_multi_selection;
}

void Controller::setMultiSelection(){
    m_multi_selection = !m_multi_selection;
    if (!m_multi_selection){
        int i;
        for (i=0;i<folder_model->getCount();i++){
            FolderItem *fi=(FolderItem*)folder_model->getRow(i);
            if (fi->checked()){
                fi->setChecked(false);
                emit setcheckindexchanged(i);
            }
        }
    }
}

void Controller::setCheck(const int &index, const bool &v){
    FolderItem *fi=(FolderItem*)folder_model->getRow(index);
    fi->setChecked(v);
    folderlist_have_item_checked();
}

void Controller::folderlist_have_item_checked(){
    int i;
    bool have_checked = false;
    for (i=0;i<folder_model->getCount();i++){
        FolderItem *fi=(FolderItem*)folder_model->getRow(i);
        if (fi->checked()){
            have_checked = true;
            break;
        }
    }
    emit enable_download_and_delete_button(have_checked);
}

void Controller::transfer(const QString &filename, const bool &is_download){
    if (!filestransfer_model->find(filename)){
        QString sizestr=get_file_size("file://"+filename);
        filestransfer_model->appendRow(new FileTransferItem(
                                           filename,
                                           sizestr,
                                           m_networkcontroller->m_currentDir,
                                           is_download
                                           ));
        if (m_options.is_transfers_auto() && !is_transfer())
            start_transfer_process();
    }
}

void Controller::start_transfer_process(){
    bool _continue = true;
    QDir _fi(NetworkController::Dropbox_Folder());
    if (!_fi.exists())
        if (!_fi.mkdir(NetworkController::Dropbox_Folder())){
            _continue = false;
            emit can_not_created_dropbox_folder(NetworkController::Dropbox_Folder());
        }

    if (_continue)
        transfer_process();
}

void Controller::transfer_process(){
    if (filestransfer_model->getCount() && m_current_filetransferitem < filestransfer_model->getCount()){
        emit startTransfer();
        FileTransferItem *fti = (FileTransferItem*)filestransfer_model->getRow(m_current_filetransferitem);
        if (!fti->is_finished()){
            if (fti->is_download())
                m_networkcontroller->download(fti);
            else
                m_networkcontroller->upload(fti);
        }
    }else
        emit nothingtotransfer();
}

void Controller::current_transfer_stop_and_cancel(){
    m_networkcontroller->file_transfer_cancel();
}

void Controller::updown_finished(){
  m_current_filetransferitem++;
  if (m_current_filetransferitem < filestransfer_model->getCount())
    transfer_process();
  else
    emit stopTransfer();
}

void Controller::cancel_login(){
    if (m_networkcontroller->m_networkreply)
        m_networkcontroller->m_networkreply->abort();
}

void Controller::downloadSelectedFiles(){
    int i;
    bool is_have_files = false;
    for (i=0;i<folder_model->getCount();i++){
        FolderItem *fi=(FolderItem*)folder_model->getRow(i);
        if (fi->checked()){
            if (!fi->is_dir()){
                QStringList ____t = fi->path().split("/");
                QString filename = ____t.at(____t.length()-1);
                if (!filestransfer_model->find(filename)){
                    is_have_files = true;
                    filestransfer_model->appendRow(new FileTransferItem(
                                                       filename,
                                                       fi->size(),
                                                       fi->path(),
                                                       true
                                                       ));
                    if (m_options.is_transfers_auto() && !is_transfer())
                        start_transfer_process();
                }
            }
            fi->setChecked(false);
            emit setcheckindexchanged(i);
        }
    }
    if (!is_have_files)
        emit notification("Please select file(s) to download");
    else
        emit notification("Added file(s) to transfer box");

    emit enable_download_and_delete_button(false);
}

void Controller::getfolder_done(){

    ListModel *newCacheListModel = new ListModel(new FolderItem, this);

    int f;
    for (f=0;f<folder_model->getCount();f++){
        FolderItem *fi=(FolderItem*)folder_model->getRow(f);
        newCacheListModel->appendRow(new FolderItem(
                                         fi->revision(), //revision
                                         fi->thumb_exists(), //thumb_exists
                                         fi->bytes(), //bytes
                                         fi->modified(), //modified
                                         fi->path(), //path
                                         fi->is_dir(), // is_dir
                                         fi->icon(), //icon
                                         fi->mime_type(), //mime_type
                                         fi->size() //size
                                         , newCacheListModel));
    }

    if (m_networkcontroller->m_currentDir.isEmpty())
        m_cache.insert("root", newCacheListModel);
    else
        m_cache.insert(m_networkcontroller->m_currentDir, newCacheListModel);

    emit folderfinished();
}

void Controller::logout(){
    m_networkcontroller->logout();
    folder_model->clear();
    transfer_clear_log();
    m_cache.clear();
    QTimer::singleShot(250, this, SIGNAL(needAuthenticateChanged()));
}

void Controller::refresh_current_folder(){
    folder_model->clear();
    if (m_cache[m_networkcontroller->m_currentDir])
        m_cache.remove(m_networkcontroller->m_currentDir);
    m_networkcontroller->getfolderlist("");
}

bool Controller::is_transfer() const {
    return m_networkcontroller->is_transfer();
}

void Controller::delete_selected_items(){
    int f;
    bool have_item_checked = false;
    m_deleted_list.clear();
    for (f=0;f<folder_model->getCount();f++){
        FolderItem *fi=(FolderItem*)folder_model->getRow(f);
        if (fi->checked()){
            have_item_checked = true;
            fi->setChecked(false);
            emit setcheckindexchanged(f);
            m_deleted_list.append(fi);
        }
    }

    m_current_deleted_item = 0;
    if (have_item_checked)
        start_delete_items();
}

void Controller::start_delete_items(){
    if (m_deleted_list.length() && m_current_deleted_item < m_deleted_list.length())
        m_networkcontroller->__delete(m_deleted_list.at(m_current_deleted_item));
    else{
         m_current_list_pos = 0;
         m_cache.clear();
         refresh_current_folder();
         emit delete_selected_items_finished("Delete file(s)/folder(s) completed");
         emit enable_download_and_delete_button(false);
    }
}

void Controller::delete_finished(const bool&){
    m_current_deleted_item++;
    start_delete_items();
}

void Controller::createnewfolder(const QString &foldername){
    m_networkcontroller->__create(foldername);
}

void Controller::createnewfolder_finished(const bool &result){
    if (result)
        refresh_current_folder();
    emit create_folder_finished(result?"The folder was created successfully":"Error: Duplicate folder name");
}


void Controller::renamefileorfolder(const QString &oldname, const QString &newname){
    m_networkcontroller->__rename(oldname, newname);
}

void Controller::renamenewfolder_finished(const bool &result){
    if (result){
        m_cache.clear();
        refresh_current_folder();
    }
    emit rename_folder_finished(result?"The folder/file was renamed successfully":"Error: Duplicate folder/file name");
}

void Controller::transfer_clear_log(){
    filestransfer_model->clear();
    m_current_filetransferitem=0;
}

void Controller::add_move_files_folders_to_cache() {
    int f;
    m_move_cache.clear();
    for (f=0;f<folder_model->getCount();f++){
        FolderItem *fi=(FolderItem*)folder_model->getRow(f);
        if (fi->checked()){
            fi->setChecked(false);
            m_move_cache.append(fi->path());
            emit setcheckindexchanged(f);
        }
    }
}

void Controller::move_files_folders_to_clear_cache() {
    m_move_cache.clear();
    emit enable_download_and_delete_button(false);
}

void Controller::start_move_files_folders(){
    m_current_move_item = 0;
    start_move_items();
}

void Controller::start_move_items() {
    if (m_move_cache.length() && m_current_move_item < m_move_cache.length())
        m_networkcontroller->__move(m_move_cache.at(m_current_move_item));
    else{
         m_current_move_item = 0;
         m_cache.clear();
         refresh_current_folder();
         move_files_folders_to_clear_cache();
         emit move_files_folders_finished("All file(s)/folder(s) has been moved to "+m_networkcontroller->m_currentDir);
    }
}

void Controller::movefilesfolders_finished(const bool&){
    m_current_move_item++;
    start_move_items();
}

bool Controller::filesize_is_valid(const QString &filepath) const{
    QFileInfo qfi(filepath);
    qreal fileSize = (qfi.size()/1024.00)/1024.00;
    if (fileSize < 300.00)
        return true;
    return false;
}

bool Controller::need_authenticate() const {
    return m_networkcontroller->need_authenticate();
}

void Controller::start_shares(const QString &filepath){
    m_networkcontroller->__shares(filepath);
}

void Controller::accountinfo() {
    m_networkcontroller->accountinfo();
}

double Controller::parseDouble(const QString &val) const {
    return val.toDouble();
}

QString Controller::getcurrentdir() const {
    return m_networkcontroller->m_currentDir;
}

bool Controller::is_duplicated_file_folder(const QString &name) const {
    for (int f=0;f<folder_model->getCount();f++){
        FolderItem *fi=(FolderItem*)folder_model->getRow(f);
        QStringList ____t = fi->path().split("/");
        QString ___name = ____t.at(____t.length()-1);
        if (___name.toLower() == name.toLower())
            return true;
    }
    return false;
}

void Controller::commit_orientation() {
    emit orientation_changed();
}

QString Controller::get_file_size(const QString &filepath) const {
    QFileInfo f(filepath.split("file://")[1]);
    qint64 B = f.size();
    float KB = B / 1024.00;
    float MB = KB / 1024.00;
    float GB = MB / 1024.00;

    if (B <1024)
        return QString("%1 bytes").arg(B);
    else if (KB <1024)
        return QString("%1").number(KB,'f',2) + " KB";
    else if (MB <1024)
        return QString("%1").number(MB,'f',2) + " MB";
    else
        return QString("%1").number(GB,'f',2) + " GB";
}

bool Controller::is_push_notification() const {
    if (m_options.is_push_notification()){
//         MNotification *notification = new MNotification("transfer.complete", "Sailbox - Files Transfer", QString("Completed"));
//         notification->setImage("imgs/sailbox.png");
//         notification->setObjectName("notification");
//         notification->publish();
//         delete notification;
        return true;
    }
    return false;
}

void Controller::add_copy_files_folders_to_cache() {
    add_move_files_folders_to_cache();
}

void Controller::copy_files_folders_to_clear_cache() {
    move_files_folders_to_clear_cache();
}

void Controller::start_copy_files_folders() {
    m_current_move_item = 0;
    start_copy_items();
}

void Controller::start_copy_items(){
    if (m_move_cache.length() && m_current_move_item < m_move_cache.length())
        m_networkcontroller->__copy(m_move_cache.at(m_current_move_item));
    else{
         m_current_move_item = 0;
         m_cache.clear();
         refresh_current_folder();
         copy_files_folders_to_clear_cache();
         emit move_files_folders_finished("All file(s)/folder(s) has been copied to "+m_networkcontroller->m_currentDir);
    }
}

void Controller::copyfilesfolders_finished(const bool&){
    m_current_move_item++;
    start_copy_items();
}

void Controller::oauth_v1_request_token() {
    m_networkcontroller->authenticate();
}

void Controller::oauth_v1_access_token() {
    m_networkcontroller->request_access_token();
}

QAction* Controller::uploadMostRecentAction()
{
    if(!m_uploadMostRecentAction)
    {
        m_uploadMostRecentAction = new QAction(koIcon("folder-remote"), "Update DropBox Copy", this);
        connect(m_uploadMostRecentAction, SIGNAL(triggered(bool)), SLOT(uploadMostRecent()));
    }
    return m_uploadMostRecentAction;
}

void Controller::uploadMostRecent()
{
    // don't trigger any new work until the current activities are done
    if(m_networkcontroller->is_transfer())
        return;
    FileTransferItem* fti = qobject_cast<FileTransferItem*>(filestransfer_model->getRow(m_current_filetransferitem - 1));
    if(fti) {
        // upload, needs FULL local path, operates on current dir remotely
        QString filename = QString("%1%2%3").arg(dropboxFolder()).arg(QDir::separator()).arg(fti->filename());
        QString sizestr=get_file_size("file://"+filename);
        FileTransferItem* newFti = new FileTransferItem(
                                           filename,
                                           sizestr,
                                           m_networkcontroller->m_currentDir,
                                           false
                                           );
        filestransfer_model->appendRow(newFti);
        m_networkcontroller->upload(newFti);
        while(m_networkcontroller->is_transfer()) {
            qApp->processEvents();
        }
    }
    // We don't want to do the uploading too often, so we disable the action and expect the
    // application using the action to enable it again when appropriate
    m_uploadMostRecentAction->setEnabled(false);
}
