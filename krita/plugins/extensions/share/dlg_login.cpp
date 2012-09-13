#include "dlg_login.h"
#include "ui_dlg_login.h"

#include <QDebug>
#include <QStringList>

DlgLogin::DlgLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgLogin)
{
    ui->setupUi(this);
    connect(ui->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(urlChanged(QUrl)));

}

DlgLogin::~DlgLogin()
{
    delete ui;
}

void DlgLogin::urlChanged(const QUrl &url)
{
    qDebug() << "URL =" << url;
    QString str = url.toString();

    if (str.contains("access_token")) {
        QStringList query = str.split("#");
        QStringList lst = query[1].split("&");
        for (int i=0; i<lst.count(); i++ ) {
            QStringList pair = lst[i].split("=");
            if (pair[0] == "access_token") {
                qDebug() << "AZccess token" << m_accessToken;
                m_accessToken = pair[1];
                emit accessTokenObtained();
                QDialog::accept();
            }
        }
    }
    else if (str.contains("blank")) {
        QDialog::close();
    }
}


QString DlgLogin::accessToken()
{
    return m_accessToken;
}


void DlgLogin::setLoginUrl(const QUrl &url)
{
   ui->webView->setUrl(url);
}
