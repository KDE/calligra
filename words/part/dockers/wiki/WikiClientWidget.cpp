#include "WikiClientWidget.h"
#include "WikiUserBackend.h"
#include "WikiApiBackend.h"

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

}
