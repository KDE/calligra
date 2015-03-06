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

#ifndef DROPRESTAPI_H
#define DROPRESTAPI_H

#include "oauth.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QByteArray>
#include <QUrl>

#define REQUEST_TOKEN_URL "https://api.dropbox.com/1/oauth/request_token"
#define REQUEST_ACCESS_TOKEN "https://api.dropbox.com/1/oauth/access_token"

#define FILES_URL                       "https://api.dropbox.com/1/metadata/dropbox"
#define FILES_TRANSFER_URL              "https://api-content.dropbox.com/1/files/dropbox"

#define FILE_DELETE_URL                 "https://api.dropbox.com/1/fileops/delete"
#define CREATE_FOLDER_URL               "https://api.dropbox.com/1/fileops/create_folder"
#define FILE_MOVE_URL                   "https://api.dropbox.com/1/fileops/move"
#define FILE_COPY_URL                   "https://api.dropbox.com/1/fileops/copy"
#define SHARES_URL                      "https://api.dropbox.com/1/shares"

#define METADATA_URL                    "https://api.dropbox.com/1/metadata/dropbox"
#define ACCOUNT_URL                     "https://api.dropbox.com/1/account"
#define ACCOUNT_INFO_URL                "https://api.dropbox.com/1/account/info"

class QByteArray;

class DropRestAPI
{
public:
    DropRestAPI();
    ~DropRestAPI();

    QNetworkRequest request_token();
    QNetworkRequest request_access_token();

    QNetworkRequest root_dir(const QString&);

    void oauth_request_token_reply_process(QNetworkReply*);

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
