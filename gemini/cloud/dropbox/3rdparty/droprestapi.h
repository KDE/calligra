/*

    SPDX-FileCopyrightText: 2011 Cuong Le <metacuong@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later

*/

#ifndef DROPRESTAPI_H
#define DROPRESTAPI_H

#include "oauth.h"

#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>

#define REQUEST_TOKEN_URL "https://api.dropbox.com/1/oauth/request_token"
#define REQUEST_ACCESS_TOKEN "https://api.dropbox.com/1/oauth/access_token"

#define FILES_URL "https://api.dropbox.com/1/metadata/dropbox"
#define FILES_TRANSFER_URL "https://api-content.dropbox.com/1/files/dropbox"

#define FILE_DELETE_URL "https://api.dropbox.com/1/fileops/delete"
#define CREATE_FOLDER_URL "https://api.dropbox.com/1/fileops/create_folder"
#define FILE_MOVE_URL "https://api.dropbox.com/1/fileops/move"
#define FILE_COPY_URL "https://api.dropbox.com/1/fileops/copy"
#define SHARES_URL "https://api.dropbox.com/1/shares"

#define METADATA_URL "https://api.dropbox.com/1/metadata/dropbox"
#define ACCOUNT_URL "https://api.dropbox.com/1/account"
#define ACCOUNT_INFO_URL "https://api.dropbox.com/1/account/info"

class QByteArray;

class DropRestAPI
{
public:
    DropRestAPI();
    ~DropRestAPI();

    QNetworkRequest request_token();
    QNetworkRequest request_access_token();

    QNetworkRequest root_dir(const QString &);

    void oauth_request_token_reply_process(QNetworkReply *);

    QNetworkRequest file_transfer(QString filename, QString dropbox_folder, QString boundaryStr);
    QNetworkRequest file_transfer_download(QString dropbox_filepath);

    QNetworkRequest __delete(QString dropbox_filepath);
    QNetworkRequest __create(QString dropbox_filepath);
    QNetworkRequest __move(QString path_source, QString path_destination);
    QNetworkRequest __copy(QString path_source, QString path_destination);
    QNetworkRequest __shares(QString dropbox_filepath);

    QNetworkRequest accountinfo();

    OAuth *oauth;
};

#endif // DROPRESTAPI_H
