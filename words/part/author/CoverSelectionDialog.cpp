#include "CoverSelectionDialog.h"
#include "ui_CoverSelectionDialog.h"
#include "CoverImage.h"

#include <kmessagebox.h>

#include <QFileDialog>
#include <QPushButton>
#include <QDebug>

CoverSelectionDialog::CoverSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CoverSelectionDialog)
{
    ui->setupUi(this);
    createActions();
}

CoverSelectionDialog::~CoverSelectionDialog()
{
    delete ui;
}

void CoverSelectionDialog::setCAuView(CAuView *au){
    view = au;
    img = view->getCurrentCoverImage();
    refresh();
}

void CoverSelectionDialog::createActions()
{
    qDebug() << "creation des actions";

    //need to add that 'open' button manualy, the standard one close the dialog
    QPushButton * b = ui->coverSelectionButtonBox->addButton(tr("Open"),QDialogButtonBox::ActionRole);
    b->setIcon(QIcon::fromTheme("document-open"));
    connect(b, SIGNAL(clicked()), this, SLOT(open()));



    connect(ui->coverSelectionButtonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));
    connect(ui->coverSelectionButtonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(ok()));

}

void CoverSelectionDialog::open()
{
    CoverImage cover;
    qDebug() << "open file";
    QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"),
                                                    "~",
                                                    tr("Images (*.png *.xpm *.jpg *.jpeg)"));
    if (!fileName.isEmpty()) {
        if (cover.readCoverImage(fileName).second.isEmpty()) {
            KMessageBox::error(0,
                               tr("Import problem"),
                               tr("Import problem"));
            return;
        }
        img = cover.readCoverImage(fileName);
        //ui->coverSelectLabel->setPixmap(QPixmap::fromImage(QImage::fromData(img.second)));
        refresh();
    }
    //ui->coverSelectFilepreview->showPreview(fileName);
    //ui->coverSelectLabel->setText("");

}

void CoverSelectionDialog::reset()
{
    qDebug() << "reset image";
    ui->coverSelectLabel->setText("No cover selected yet");
    img = QPair<QString, QByteArray>();
}

void CoverSelectionDialog::ok()
{
    qDebug() << "Valid image";
    view->setCurrentCoverImage(img);
    qDebug() << "AUTHOR : cover image sended";
}

void CoverSelectionDialog::refresh(){
    qDebug() << "Refreching";
    //img = view->getCurrentCoverImage();
    if(!img.second.isNull()) {
        ui->coverSelectLabel->setPixmap(QPixmap::fromImage(QImage::fromData(img.second)));
    }
    else {
        qDebug() << "Nothing to display";
    }

}
