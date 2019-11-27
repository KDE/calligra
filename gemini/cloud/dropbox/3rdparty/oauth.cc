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

#include "oauth.h"

#include <QUrl>
#include <QUrlQuery>
#include <QByteArray>
#include <QCryptographicHash>
#include <QDateTime>
#include <QStringList>
#include <QDebug>

#include <algorithm>

OAuth::OAuth():
    m_consumer_key("7y6cr1w19khjkft"),
    m_consumer_secret("jyxb5gu2dp7npz6")
// old dropN9 keys below
//    m_consumer_key("8xf1b5hrsgmempk"),
//    m_consumer_secret("6se47g35sdqsouw")
{
    qsrand(QDateTime::currentDateTime().toTime_t());
    m_token = "";
    m_secret = "";
}

void OAuth::sign(QString method, QNetworkRequest *nr)
{
    QString header = "OAuth ";
    QUrl url(nr->url());

    header += oauth_timestamp() + ",";
    header += oauth_consumer_key() + ",";
    header += oauth_signature_method() + ",";
    header += oauth_token() + ",";
    header += oauth_version() + ",";
    header += oauth_signature(method,&url,header) + ",";

    header.chop(1);

    nr->setRawHeader("Authorization", header.toLatin1());

}

QString OAuth::oauth_timestamp()
{
    int currentTimeStamp = QDateTime::currentDateTime().toUTC().toTime_t();
    QString strtimestamp("oauth_timestamp=\"%1\",oauth_nonce=\"%2\"");
    return strtimestamp.arg(currentTimeStamp).arg(qrand());
}

QString OAuth::oauth_consumer_key()
{
    return QString("oauth_consumer_key=\"%1\"").arg(m_consumer_key);
}

QString OAuth::oauth_signature_method()
{
    return QString("oauth_signature_method=\"%1\"").arg("HMAC-SHA1");
}

QString OAuth::oauth_token()
{
    return QString("oauth_token=\"%1\"").arg(m_token);
}

QString OAuth::oauth_version()
{
    return QString("oauth_version=\"%1\"").arg(OAUTH_VERSION);
}

QString OAuth::oauth_signature(QString method,QUrl *url,QString oAuthHeader)
{
    QString urlSchemeAndHost = url->toString(
        QUrl::RemovePort |
        QUrl::RemovePath |
        QUrl::RemoveQuery |
        QUrl::RemoveFragment
        );
    QString urlPath = url->path();

    QStringList urlPathParts = urlPath.split("/");
    for(int i = 0; i < urlPathParts.length(); ++i)
    {
        urlPathParts[i] = urlPathParts[i].toUtf8().toPercentEncoding();
    }
    urlPath = urlPathParts.join("/");

    QByteArray readyForUseUrl =
        (urlSchemeAndHost+urlPath).toLatin1().toPercentEncoding();

    QList< QPair<QString,QString> > parameters;

    parameters.append(QUrlQuery(*url).queryItems());

    oAuthHeader.remove("OAuth ");
    QStringList oAuthParameters =
        oAuthHeader.split(",", QString::SkipEmptyParts);
    foreach(QString oAuthParameter, oAuthParameters)
    {
        QStringList oAuthParameterParts = oAuthParameter.split("=");
        QString first = oAuthParameterParts.at(0);
        QString second = oAuthParameterParts.at(1);
        second.remove("\"");
        QPair<QString, QString> parameter = qMakePair(first, second);
        parameters.append(parameter);
    }

    for(int i = 0; i < parameters.length(); ++i)
    {
        QPair<QString,QString> parameter = parameters[i];
        parameter.second = parameter.second.toUtf8().toPercentEncoding();
        parameters[i] = parameter;
    }

    std::sort(parameters.begin(), parameters.end());

    QString parametersString;
    QPair<QString,QString> parameter;
    foreach(parameter, parameters)
        parametersString += parameter.first + "=" + parameter.second + "&";

    parametersString.chop(1);

    QString readyForUseParametersString = parametersString.toLatin1().toPercentEncoding();

    QString base = method+"&"+readyForUseUrl+"&"+readyForUseParametersString;

    QString hash = SHA1(base,m_consumer_secret + "&" + m_secret);

    return QString("oauth_signature=\"%1\"").arg(hash);
}

QString OAuth::SHA1(QString base, QString key)
{
    QByteArray ipad;
    ipad.fill(char(0), 64);
    for(int i = 0; i < key.length(); ++i)
        ipad[i] = key[i].toLatin1();

    QByteArray opad;
    opad.fill(char(0), 64);
    for(int i = 0; i < key.length(); ++i)
        opad[i] = key[i].toLatin1();

    for(int i = 0; i < ipad.length(); ++i)
        ipad[i] = ipad[i] ^ 0x36;

    for(int i = 0; i < opad.length(); ++i)
        opad[i] = opad[i] ^ 0x5c;

    QByteArray innerSha1 = QCryptographicHash::hash(
        ipad + base.toLatin1(),
        QCryptographicHash::Sha1
        );

    QByteArray outerSha1 = QCryptographicHash::hash(
        opad + innerSha1,
        QCryptographicHash::Sha1
        );

    return outerSha1.toBase64();
}
