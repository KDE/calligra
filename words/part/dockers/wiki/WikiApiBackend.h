#ifndef WIKIAPIBACKEND_H
#define WIKIAPIBACKEND_H

#include <QObject>

class QNetworkReply;
class KoXmlStreamReader;

class WikiApiBackend : public QObject
{
    Q_OBJECT
public:
    enum ActionType {
        LogIn
    };
    explicit WikiApiBackend();
    
    void login(const QString &username, const QString &password);

private:
    void setActionType(WikiApiBackend::ActionType type);
    WikiApiBackend::ActionType actioType();

private slots:
    void handleNetworkResponse(QNetworkReply *networkReply);
    void handleLoginResponse(KoXmlStreamReader &reader);
    void automaticLogin();

signals:
    void requestFinished(QString message);

private:
    class Private;
    Private *const d;
};

#endif // WIKIAPIBACKEND_H
