#include "WikiClientWidget.h"
#include "WikiUserBackend.h"
#include "WikiApiBackend.h"

#include <QTimer>

#include <kdebug.h>

class WikiClientWidget::Private
{
 public:
    Private() :
        api(0),
        user(0)
    {}

    WikiApiBackend *api;
    WikiUserBackend *user;
};

WikiClientWidget::WikiClientWidget(QWidget *parent) :
    QWidget(parent),
    d(new Private())
{
    widget.setupUi(this);
    widget.loginStatus->setVisible(false);
    connect(widget.loginButton, SIGNAL(clicked()), this, SLOT(clientLogin()));

    d->api = new WikiApiBackend();
    connect(d->api, SIGNAL(requestFinished(QString)), this, SLOT(handleRequestResult(QString)));
}

WikiClientWidget::~WikiClientWidget()
{
    delete d;
}

void WikiClientWidget::setLayoutDirection(LayoutDirection direction)
{
}


void WikiClientWidget::clientLogin()
{
    widget.loginButton->setEnabled(false);
    widget.loginForm->setEnabled(false);
    widget.loginStatus->setText("Connecting to server ...");
    widget.loginStatus->setVisible(true);
    d->api->login(widget.username->text(), widget.password->text());
}

void WikiClientWidget::handleRequestResult(QString message)
{
    if (message == "Success") {
        d->user = new WikiUserBackend();
        //FIXME: Save username and password in username information here.

        widget.loginForm->setVisible(false);
        widget.loginButton->setVisible(false);

        widget.loginStatus->setText(QString("<b>Hi</b>, You have logined successfully"));
        widget.loginStatus->setVisible("true");
    }
    else if (message == "WrongPass") {
        widget.loginForm->setEnabled(true);
        widget.loginStatus->setText("<font color=red>Wrong Password, you can try again after 5 seconds. </font>");
        QTimer::singleShot(5000, this, SLOT(updateLoginStatus()));
    }
    else if (message == "NotExists") {
        widget.loginForm->setEnabled(true);
        widget.loginStatus->setText("<font color=red>Username is incorrect, you can try again after 5 seconds. </font>");
        QTimer::singleShot(5000, this, SLOT(updateLoginStatus()));
    }
}

void WikiClientWidget::updateLoginStatus()
{
    widget.loginStatus->setVisible(false);
    widget.loginButton->setEnabled(true);
}
