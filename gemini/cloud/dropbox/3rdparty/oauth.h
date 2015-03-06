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

#ifndef OAUTH_H
#define OAUTH_H

#include <QPair>
#include <QString>

#include <QNetworkRequest>

class QUrl;

#define OAUTH_VERSION "1.0"

class OAuth
{
public:
    OAuth();

    QString m_consumer_key;
    QString m_consumer_secret;
    QString m_token;
    QString m_secret;

    void sign(QString method,QNetworkRequest *networkRequest);

private:
    QString oauth_timestamp();
    QString oauth_consumer_key();
    QString oauth_signature_method();
    QString oauth_token();
    QString oauth_version();
    QString oauth_signature(QString method,QUrl *url,QString oAuthHeader);
    QString SHA1(QString base, QString key);
};

#endif // OAUTH_H
