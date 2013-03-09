#include "dlg_login.h"
#include "ui_dlg_login.h"

#include <QStringList>

#include "o2deviantart.h"

DlgLogin::DlgLogin(O2DeviantART *deviant, QWidget *parent)
    : QDialog(parent)
    , m_deviant(deviant)
    , ui(new Ui::DlgLogin)
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
    QString str = url.toString();

    if (!url.queryItemValue("code").isEmpty()) {
        m_accessToken = url.queryItemValue("code");
        emit accessTokenObtained();
        QDialog::accept();
    }
    else if (str.contains("access_token")) {
        QStringList query = str.split("#");
        QStringList lst = query[1].split("&");
        for (int i=0; i<lst.count(); i++ ) {
            QStringList pair = lst[i].split("=");
            if (pair[0] == "access_token") {
                m_accessToken = pair[1];
                emit accessTokenObtained();
                QDialog::accept();
            }
        }
    }
}

void DlgLogin::setLoginUrl(const QUrl &url)
{
    ui->webView->setUrl(url);
}
