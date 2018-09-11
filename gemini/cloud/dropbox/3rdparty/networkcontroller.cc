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

#include "networkcontroller.h"
#include "json.h"

#include "controller.h"

#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QSslError>
#include <QMessageBox>

NetworkController::NetworkController(QObject *parent) :
    QObject(parent),
    m_currentDir(""),
    m_currentDir_temp("")
{

    m_transfer_state = NetworkController::FREE;

    m_networkaccessmanager = new QNetworkAccessManager(this);
    m_file_transfer = new QNetworkAccessManager(this);

    m_droprestapi = new DropRestAPI();

    QObject::connect(m_networkaccessmanager, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)), this, SLOT(sslErrors(QNetworkReply*, const QList<QSslError>&)));
    QObject::connect(m_networkaccessmanager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    QObject::connect(m_file_transfer, SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError>&)), this, SLOT(sslErrors(QNetworkReply*, const QList<QSslError>&)));
    QObject::connect(m_file_transfer, SIGNAL(finished(QNetworkReply*)), this, SLOT(file_transfer_finished(QNetworkReply*)));
}

NetworkController::~NetworkController(){
    if (m_networkaccessmanager)
        delete m_networkaccessmanager;
    if (m_file_transfer)
        delete m_file_transfer;
    delete m_droprestapi;
}

void NetworkController::file_transfer_finished(QNetworkReply *networkreply){
    if (networkreply->error() > 0){
        file_transfer_success(false);
        if (m_transfer_state == NetworkController::DOWNLOADING){
            m_file.remove();
            emit file_download_finished();
        }else{
            if (m_multipartform)
                delete m_multipartform;
            emit file_upload_finished();
        }
        emit stop_and_cancel_finished();
    }else{
        file_transfer_success(true);
        if (m_transfer_state == NetworkController::DOWNLOADING){
            m_file.close();
            emit file_download_finished();
        }else{
            if (m_multipartform)
                delete m_multipartform;
            emit file_upload_finished();
        }
    }

    m_transfer_state = NetworkController::FREE;
    networkreply->deleteLater();
}

void NetworkController::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
    QString errorDescription;
    Q_FOREACH(const QSslError& error, errors) {
        if(error.error() != QSslError::NoError) {
            errorDescription.append('\n').append(error.errorString());
        }
    }

    if(errorDescription.length() > 0) {
        QString error = QString("An error occurred when attempting to make a secure connection:%1").arg(errorDescription);
        QMessageBox::StandardButton result = QMessageBox::question(0, "Error establishing secure connection.", QString("%1\n Do you wish to continue?").arg(error), QMessageBox::Yes | QMessageBox::No);

        if(result == QMessageBox::Yes) {
            reply->ignoreSslErrors(errors);
        } else {
            emit network_error(error);
        }
    } else {
        // This may seem weird, but apparently NoError is reported sometimes as
        // an error... so we need to ignore it.
        QSslError noError(QSslError::NoError);
        QList<QSslError> expectedSslErrors;
        expectedSslErrors.append(noError);
        reply->ignoreSslErrors(expectedSslErrors);
    }
}

void NetworkController::finished(QNetworkReply *networkreply){
    if (networkreply->error() > 0){

        qDebug() << "reply url :" <<networkreply->url();
        qDebug() << "error() code = " << networkreply->error();
        qDebug() << "readAll() body = " << networkreply->readAll();


        QString errorstr("Unknow error!");

        if (m_state == NetworkController::FILES_FOLDERS ||  m_state == NetworkController::REQUEST_TOKEN){
            if (networkreply->error() == QNetworkReply::AuthenticationRequiredError)
                errorstr = "The provided user information is not valid";
            else if (networkreply->error() == QNetworkReply::ContentOperationNotPermittedError)
                errorstr = "The operation requested on the server is not permitted";
            emit network_error(errorstr);
        }else{
            if (networkreply->error() == QNetworkReply::TemporaryNetworkFailureError ||
                    networkreply->error() == QNetworkReply::TimeoutError ||
                    networkreply->error() == QNetworkReply::HostNotFoundError ||
                    networkreply->error() == QNetworkReply::ProxyConnectionRefusedError ||
                    networkreply->error() == QNetworkReply::ProxyConnectionClosedError ||
                    networkreply->error() == QNetworkReply::ProxyNotFoundError ||
                    networkreply->error() == QNetworkReply::ProxyTimeoutError ||
                    networkreply->error() == QNetworkReply::ProxyAuthenticationRequiredError ||
                    networkreply->error() == QNetworkReply::ProtocolUnknownError ||
                    networkreply->error() == QNetworkReply::UnknownNetworkError ||
                    networkreply->error() == QNetworkReply::UnknownProxyError
                    ) {
                emit network_error("");
            } else if(networkreply->error() == QNetworkReply::SslHandshakeFailedError) {
                // do nothing... We have an SSL refusal, and need to respect that.
            } else{
                if (m_state == NetworkController::DELETE)
                    emit delete_finished(false);
                else if(m_state == NetworkController::CREATE)
                    emit create_finished(false);
                else if(m_state == NetworkController::RENAME)
                    emit rename_finished(false);
                else if(m_state == NetworkController::MOVE)
                    emit move_finished(false);
                else if(m_state == NetworkController::COPY)
                    emit copy_finished(false);
                else if(m_state == NetworkController::SHARES)
                    emit shares_finished(false);
                else if(m_state == NetworkController::ACCOUNT_INFO)
                    emit accountinfo_finished(false);
            }
        }

    }else{
        QVariantMap jsonmap;
        switch (m_state){
            case NetworkController::REQUEST_TOKEN:
                m_droprestapi->oauth_request_token_reply_process(networkreply);
                emit open_oauth_authorize_page(m_droprestapi->oauth->m_token);
                break;
            case NetworkController::ACCESS_TOKEN :
                m_droprestapi->oauth_request_token_reply_process(networkreply);
                save_token_and_secret();
                emit authenticate_finished();
                break;
            case NetworkController::DELETE:
                emit delete_finished(true);
                break;
            case NetworkController::CREATE: emit create_finished(true); break;
            case NetworkController::RENAME: emit rename_finished(true); break;
            case NetworkController::MOVE: emit move_finished(true);break;
            case NetworkController::COPY:  emit copy_finished(true);break;
            case NetworkController::SHARES:
                jsonmap= Json::parse(networkreply->readAll()).toMap();
                emit shares_metadata(jsonmap["url"].toString(),jsonmap["expires"].toString());
                emit shares_finished(true);
                break;
            case NetworkController::ACCOUNT_INFO:
                jsonmap= Json::parse(networkreply->readAll()).toMap();
                emit accountinfo_metadata(jsonmap);
                emit accountinfo_finished(true);
                break;
            case NetworkController::FILES_FOLDERS:
                jsonmap= Json::parse(networkreply->readAll()).toMap();
                foreach(QVariant item, jsonmap["contents"].toList()){
                    QVariantMap item_map = item.toMap();
                    if (item_map["is_dir"].toString() == "true")
                    emit getfolder_finished(item_map);
                }
                foreach(QVariant item, jsonmap["contents"].toList()){
                    QVariantMap item_map = item.toMap();
                    if (item_map["is_dir"].toString() == "false")
                    emit getfolder_finished(item_map);
                }

                if (m_currentDir != m_currentDir_temp){
                    m_currentDir += m_currentDir_temp;
                }

                emit getfolder_done();
                break;
        }
    }
    networkreply->deleteLater();
}

void NetworkController::file_transfer_success(bool v){
    m_fti->setIn_queue(false);
    m_fti->setIs_finished(true);
    m_fti->setCompleted(v);
    m_fti->setDatetime(QDateTime::currentDateTime().toString("ddd dd MMM, yyyy HH:mm AP"));
}


void NetworkController::authenticate(){
    m_state = NetworkController::REQUEST_TOKEN;
    m_networkreply = m_networkaccessmanager->get(m_droprestapi->request_token());
}


bool NetworkController::need_authenticate() {
    if (recheck_token_and_secret() && (m_droprestapi->oauth->m_token != "") && (m_droprestapi->oauth->m_secret !="")){
        return false;
    }
    return true;
}

bool NetworkController::recheck_token_and_secret(){
    QSettings settings;
    settings.beginGroup("user");
    if( (settings.childKeys().indexOf("token") == -1) ||
            (settings.childKeys().indexOf("secret") == -1) ){
        return false;
    }
    m_droprestapi->oauth->m_token = settings.value("token").toString();
    m_droprestapi->oauth->m_secret = settings.value("secret").toString();
    return true;
}

void NetworkController::save_token_and_secret(){
    QSettings settings;

    settings.setValue("user/token", m_droprestapi->oauth->m_token);
    settings.setValue("user/secret", m_droprestapi->oauth->m_secret);
}

void NetworkController::getfolderlist(QString folder_name){
    m_state = NetworkController::FILES_FOLDERS;
    m_currentDir_temp = folder_name;
    QNetworkRequest __request = m_droprestapi->root_dir(
                QString("%1%2").arg(m_currentDir).arg(folder_name)
                );
    __request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    __request.setPriority(QNetworkRequest::HighPriority);
    m_networkaccessmanager->get(__request);
}

void NetworkController::upload(FileTransferItem *fti){

    emit progressBarChanged(0,0,0);

    m_transfer_state = NetworkController::UPLOADING;

    QStringList path = fti->filename().split("/");
    QString filename = path[path.count()-1];

    QString filepath(fti->filename());

    m_uploadtime.start();

    m_fti = fti;
    m_fti->setIn_queue(false);
    m_fti->setIs_finished(false);

    m_multipartform = new QByteArray();
    QString crlf("\r\n");
    QString boundaryStr(
                    "---------------------------109074266748897678777839994"
                    );
    QString boundary="--"+boundaryStr+crlf;
    m_multipartform->append(boundary.toLatin1());
    m_multipartform->append(
                    QString("Content-Disposition: form-data; name=\"file\"; "
                        "filename=\"" + filename.toUtf8() + "\"" + crlf
                        ).toLatin1()
                    );
    m_multipartform->append(
                    QString("Content-Type: text/plain" + crlf + crlf).toLatin1()
                    );

    m_file.setFileName(filepath);

    if(!m_file.open(QFile::ReadOnly)){
        file_transfer_success(false);
        return;
    }

    m_multipartform->append(m_file.readAll());

    m_file.close();

    m_multipartform->append(
        QString(crlf + "--" + boundaryStr + "--" + crlf).toLatin1()
        );

    m_networkreply = m_file_transfer->post(m_droprestapi->file_transfer(filename, m_currentDir, boundaryStr),*m_multipartform);
    QObject::connect(m_networkreply,SIGNAL(uploadProgress(qint64, qint64)), SLOT(uploadProgress(qint64, qint64)));

}

void NetworkController::downloadProgress(qint64 received, qint64 total){

    m_transfer_state = NetworkController::DOWNLOADING;

    if (received == 0 || total == 0 || m_downloadtime.elapsed() == 0)
        return;

    double speed = received * 1000.0 / m_downloadtime.elapsed();

    m_fti->setProgressing((received*100) / total);
    emit progressBarChanged((received*100) / total, speed, received);
}

void NetworkController::download(FileTransferItem *fti){

    emit progressBarChanged(0,0,0);

    m_transfer_state = NetworkController::DOWNLOADING;

    m_downloadtime.start();
    m_file.setFileName(QString("%1/%2").arg(Dropbox_Folder()).arg(fti->filename()));
    m_file.open(QFile::WriteOnly);

    m_fti = fti;
    m_fti->setIn_queue(false);
    m_fti->setIs_finished(false);

    m_networkreply = m_file_transfer->get(m_droprestapi->file_transfer_download(fti->dropbox_path()));
    QObject::connect(m_networkreply, SIGNAL(readyRead()), this, SLOT(readyRead()));
    QObject::connect(m_networkreply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
}

void NetworkController::readyRead(){
    if (m_file.isOpen())
        m_file.write(m_networkreply->readAll());
}

void NetworkController::uploadProgress(qint64 sent, qint64 total){

    m_transfer_state = NetworkController::UPLOADING;

    if (sent == 0 || total == 0 || m_uploadtime.elapsed() == 0)
        return;

    double speed = sent * 1000.0 / m_uploadtime.elapsed();

    m_fti->setProgressing((sent*100) / total);
    emit progressBarChanged((sent*100) / total, speed, sent);
}

bool NetworkController::is_transfer(){
    if (m_transfer_state == NetworkController::UPLOADING || m_transfer_state == NetworkController::DOWNLOADING)
        return true;
    return false;
}

void NetworkController::file_transfer_cancel(){
    if (m_transfer_state == NetworkController::UPLOADING || m_transfer_state == NetworkController::DOWNLOADING){
        m_networkreply->abort();

        if (m_file.isOpen())
            m_file.close();

        if (m_fti)
            m_fti->setCancelled(true);
    }
    emit stop_and_cancel_finished();
}

void NetworkController::__delete(FolderItem *item){
    m_state = NetworkController::DELETE;
    m_fi = item;
    m_networkaccessmanager->get(m_droprestapi->__delete(item->path()));
}

void NetworkController::__create(QString foldername){
    m_state = NetworkController::CREATE;
    m_networkaccessmanager->get(m_droprestapi->__create(this->m_currentDir+"/"+foldername));
}

void NetworkController::__rename(QString oldname, QString newname){
    m_state = NetworkController::RENAME;
     m_networkaccessmanager->get(m_droprestapi->__move(m_currentDir+"/"+oldname, m_currentDir+"/"+newname));
}

void NetworkController::__move(QString from){
    m_state = NetworkController::MOVE;
    QStringList qName(from.split("/"));
    QString qDir(m_currentDir);
    if (m_currentDir == "/")
        qDir = "";
    m_networkaccessmanager->get(m_droprestapi->__move(from, qDir+"/"+qName.at(qName.length()-1)));
}

void NetworkController::__copy(QString from){
    m_state = NetworkController::COPY;
    QStringList qName(from.split("/"));
    QString qDir(m_currentDir);
    if (m_currentDir == "/")
        qDir = "";
    m_networkaccessmanager->get(m_droprestapi->__copy(from, qDir+"/"+qName.at(qName.length()-1)));
}

void NetworkController::__shares(QString filepath){
    m_state = NetworkController::SHARES;
    m_networkaccessmanager->post(m_droprestapi->__shares(filepath), QByteArray());
}

void NetworkController::request_access_token(){
    m_state = NetworkController::ACCESS_TOKEN;
    m_networkaccessmanager->post(m_droprestapi->request_access_token(), QByteArray());
}

void NetworkController::logout(){
    QSettings settings;
    settings.beginGroup("user");
    settings.clear();
    m_droprestapi->oauth->m_token = "";
    m_droprestapi->oauth->m_secret = "";
}

void NetworkController::accountinfo(){
    m_state = NetworkController::ACCOUNT_INFO;
    m_networkaccessmanager->get(m_droprestapi->accountinfo());
}
