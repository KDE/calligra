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

#include "droprestapi.h"
#include "json.h"

#include <QUrlQuery>
#include <QDebug>


DropRestAPI::DropRestAPI()
{
    oauth = new OAuth();
}

DropRestAPI::~DropRestAPI(){
    delete oauth;
}

QNetworkRequest DropRestAPI::request_token() {
    QUrl url(REQUEST_TOKEN_URL);
    QNetworkRequest rt;

    rt.setUrl(url);

    oauth->sign("GET", &rt);

    return rt;
}

QNetworkRequest DropRestAPI::request_access_token() {
    QUrl url(REQUEST_ACCESS_TOKEN);
    QNetworkRequest rt;

    rt.setUrl(url);

    oauth->sign("POST", &rt);

    return rt;
}

QNetworkRequest DropRestAPI::root_dir(const QString &folder_name){
    QUrl url(QString("%1%2").arg(FILES_URL).arg(folder_name));
    QNetworkRequest rt;

    rt.setUrl(url);

    oauth->sign("GET", &rt);

    return rt;
}

void DropRestAPI::oauth_request_token_reply_process(QNetworkReply *networkreply){
    QList<QByteArray> oauth_content = networkreply->readAll().split('&');

    oauth->m_secret = oauth_content.at(0).split('=').at(1);
    oauth->m_token = oauth_content.at(1).split('=').at(1);
}

QNetworkRequest DropRestAPI::file_transfer(QString filename, QString dropbox_folder, QString boundaryStr){
   QUrl url;
   QNetworkRequest rt;

   url.setUrl(QString("%1%2").arg(FILES_TRANSFER_URL).arg(dropbox_folder));
   QUrlQuery query;
   query.addQueryItem("file", filename);
   url.setQuery(query);

   rt.setUrl(url);
   rt.setHeader(QNetworkRequest::ContentTypeHeader, QString("multipart/form-data; boundary=").append(boundaryStr));

   oauth->sign("POST", &rt);

   return rt;
}

QNetworkRequest DropRestAPI::file_transfer_download(QString dropbox_filepath){
    QUrl url;
    url.setUrl(QString("%1%2").arg(FILES_TRANSFER_URL).arg(dropbox_filepath));
    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::__delete(QString dropbox_filepath){
    QUrl url;
    url.setUrl(QString("%1").arg(FILE_DELETE_URL));
    QUrlQuery query;
    query.addQueryItem("root", "dropbox");
    query.addQueryItem("path", dropbox_filepath);
    url.setQuery(query);
    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::__create(QString dropbox_filepath){
    QUrl url;
    url.setUrl(QString("%1").arg(CREATE_FOLDER_URL));
    QUrlQuery query;
    query.addQueryItem("root", "dropbox");
    query.addQueryItem("path", dropbox_filepath);
    url.setQuery(query);
    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::__move(QString path_source, QString path_destination){
    QUrl url;
    url.setUrl(QString("%1").arg(FILE_MOVE_URL));
    QUrlQuery query;
    query.addQueryItem("root", "dropbox");
    query.addQueryItem("from_path", path_source);
    query.addQueryItem("to_path", path_destination);
    url.setQuery(query);

    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::__copy(QString path_source, QString path_destination){
    QUrl url;
    url.setUrl(QString("%1").arg(FILE_COPY_URL));
    QUrlQuery query;
    query.addQueryItem("root", "dropbox");
    query.addQueryItem("from_path", path_source);
    query.addQueryItem("to_path", path_destination);
    url.setQuery(query);

    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::__shares(QString dropbox_filepath){
    QUrl url;
    url.setUrl(QString("%1").arg(SHARES_URL));
    QUrlQuery query;
    query.addQueryItem("root", "dropbox");
    query.addQueryItem("path", dropbox_filepath);
    url.setQuery(query);

    QNetworkRequest rt(url);
    oauth->sign("POST", &rt);
    return rt;
}

QNetworkRequest DropRestAPI::accountinfo() {
    QUrl url;
    url.setUrl(QString("%1").arg(ACCOUNT_INFO_URL));
    QNetworkRequest rt(url);
    oauth->sign("GET", &rt);
    return rt;
}
