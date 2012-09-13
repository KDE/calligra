#ifndef DLG_LOGIN_H
#define DLG_LOGIN_H

#include <QDialog>
#include <QUrl>
#include <QString>

namespace Ui {
class DlgLogin;
}

class DlgLogin : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgLogin(QWidget *parent = 0);
    ~DlgLogin();

    void setLoginUrl(const QUrl &url);
    QString accessToken();
signals:

    void accessTokenObtained();

private slots:
    void urlChanged(const QUrl &url);

private:
    Ui::DlgLogin *ui;
    QString m_accessToken;
};

#endif // DLG_LOGIN_H
