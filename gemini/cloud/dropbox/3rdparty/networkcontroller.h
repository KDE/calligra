/*

    SPDX-FileCopyrightText: 2011 Cuong Le <metacuong@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <QElapsedTimer>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <qdir.h>

#include "droprestapi.h"

#include "filetransferitem.h"
#include "folderitem.h"

class NetworkController : public QObject
{
    Q_OBJECT
public:
    explicit NetworkController(QObject *parent = nullptr);
    ~NetworkController() override;

    static QString Dropbox_Folder()
    {
        QDir tmpdir = QDir::temp();
        QString folder = tmpdir.canonicalPath().append(QDir::separator()).append("calligra-gemini-dropbox");
        if (!tmpdir.exists(folder)) {
            tmpdir.mkpath(folder);
        }
        return folder;
    }

    enum State { REQUEST_TOKEN, ACCESS_TOKEN, FILES_FOLDERS, DELETE, CREATE, RENAME, MOVE, COPY, SHARES, ACCOUNT_INFO };
    Q_ENUM(State);

    enum TransferState { DOWNLOADING, UPLOADING, FREE, ABORT };

    QString m_user_email;
    QString m_user_password;

    void authenticate();
    void getfolderlist(QString);

    void upload(FileTransferItem *);
    void download(FileTransferItem *);

    void __delete(FolderItem *);
    void __create(QString);
    void __rename(QString, QString);
    void __move(QString);
    void __shares(QString);
    void __copy(QString);

    QString m_currentDir;
    QNetworkReply *m_networkreply;

    bool is_transfer();
    void file_transfer_cancel();
    void request_access_token();
    bool need_authenticate();
    void logout();
    void accountinfo();

private:
    QNetworkAccessManager *m_networkaccessmanager;
    QNetworkAccessManager *m_file_transfer;
    DropRestAPI *m_droprestapi;

    State m_state;
    TransferState m_transfer_state;
    QString m_currentDir_temp;

    FileTransferItem *m_fti;
    FolderItem *m_fi;
    QByteArray *m_multipartform;
    QFile m_file;
    QElapsedTimer m_uploadtime;
    QElapsedTimer m_downloadtime;

    void file_transfer_success(bool);
    bool recheck_token_and_secret();
    void save_token_and_secret();

Q_SIGNALS:
    void authenticate_finished();
    void network_error(QString error);
    void getfolder_finished(const QVariantMap &);
    void getfolder_done();

    void progressBarChanged(const int &percent, const double &speed, const qint64 &sent_received);

    void file_download_finished();
    void file_upload_finished();

    void delete_finished(const bool &);
    void create_finished(const bool &);
    void rename_finished(const bool &);
    void move_finished(const bool &);
    void copy_finished(const bool &);

    void stop_and_cancel_finished();

    void shares_finished(const bool &);
    void shares_metadata(const QString &url, const QString &expire);

    void accountinfo_finished(const bool &);
    void accountinfo_metadata(const QVariant &);

    void open_oauth_authorize_page(const QString &oauth_token);

public Q_SLOTS:
    void uploadProgress(qint64 sent, qint64 total);
    void downloadProgress(qint64 received, qint64 total);
    void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);

private Q_SLOTS:
    void finished(QNetworkReply *);
    void readyRead();
    void file_transfer_finished(QNetworkReply *);
};

#endif // NETWORKCONTROLLER_H
