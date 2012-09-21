#ifndef DLG_LOGIN_H
#define DLG_LOGIN_H

#include <QDialog>
#include <QUrl>
#include <QString>

class O2DeviantART;

namespace Ui {
class DlgLogin;
}

class DlgLogin : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgLogin(O2DeviantART *deviant, QWidget *parent = 0);
    ~DlgLogin();

    void setLoginUrl(const QUrl &url);

signals:

    void accessTokenObtained();

private slots:
    void urlChanged(const QUrl &url);

private:
    O2DeviantART *m_deviant;
    Ui::DlgLogin *ui;
    QString m_accessToken;
};

#endif // DLG_LOGIN_H
