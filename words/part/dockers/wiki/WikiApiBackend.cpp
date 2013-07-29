#include "WikiApiBackend.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <QBuffer>
#include <QTimer>

#include <KoXmlStreamReader.h>

#include <kdebug.h>


class WikiApiBackend::Private
{
public:
    Private() {}

    QNetworkAccessManager networkManager;
    WikiApiBackend::ActionType actionType;
    QString username;
    QString password;
    QString token;
    QString userId;
    QString seesionId;
};

WikiApiBackend::WikiApiBackend()
    : d(new Private())
{
    connect(&d->networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(handleNetworkResponse(QNetworkReply*))); 
}

void WikiApiBackend::login(const QString &username, const QString &password)
{
    d->username = username;
    d->password = password;

    QByteArray data;
    data.append(QString("action=login&format=xml&lgname=" + username + "&lgpassword=" + password).toUtf8());
    if (!d->token.isEmpty()) {
        data.append(QString("&lgtoken=" + d->token).toUtf8());
    }
    kDebug() << "Post data " << data;
    QNetworkRequest req(QUrl("https://www.mediawiki.org/w/api.php"));
    req.setRawHeader("Host", "www.mediawiki.org");
    //req.setRawHeader("GData-Version", "3.0");
    req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");

    setActionType(WikiApiBackend::LogIn);

    d->networkManager.post(req, data);
}

void WikiApiBackend::handleNetworkResponse(QNetworkReply *networkReply)
{
    QByteArray reply = QString(networkReply->readAll()).toUtf8();

    kDebug() << "Network reply:" << reply;

    QBuffer buffer(&reply);
    buffer.open(QIODevice::ReadOnly);
    KoXmlStreamReader reader;
    reader.setDevice(&buffer);

    if (d->actionType == WikiApiBackend::LogIn) {
        handleLoginResponse(reader);
    }
    buffer.close();
}

void WikiApiBackend::handleLoginResponse(KoXmlStreamReader &reader)
{
    while(!reader.atEnd()) {
        reader.readNextStartElement();
        if (reader.isStartElement() && reader.qualifiedName() == "login") {

            QString message;
            QString result = reader.attributes().value("result").toString();
            if (result == "NeedToken") {
                // We need token, get token from reply and login after 5 seconds again with given token.
                /** Wiki page comment:
                  "you will not be able to attempt another log-in through this method for 5 seconds.
                  This is to prevent password guessing by automated password crackers "
                  */
                //FIXME: But i try again after 6 seconds to make sure, will not see the error.
                kDebug() << "Need Token";
                d->token = reader.attributes().value("token").toString();
                QTimer::singleShot(6000, this, SLOT(automaticLogin()));
            }
            else if (result == "Success") {
                kDebug() << "Success";
                d->userId = reader.attributes().value("lguserid").toString();
                d->seesionId = reader.attributes().value("sessionid").toString();
                message = "Success";

            }
            else if (result == "WrongPass") {
                message = "WrongPass";
            }
            else if  (result == "NotExists") {
                message = "NotExists";
            }
            emit requestFinished(message);

            break;
        }
    }
}

void WikiApiBackend::automaticLogin()
{
    login(d->username, d->password);
}

void WikiApiBackend::setActionType(WikiApiBackend::ActionType type)
{
    d->actionType = type;
}

WikiApiBackend::ActionType WikiApiBackend::actioType()
{
    return d->actionType;
}
