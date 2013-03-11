#include "CoverSelectionDialog.h"
#include "ui_CoverSelectionDialog.h"
#include "CoverImage.h"

#include <kmessagebox.h>

#include <QFileDialog>
#include <QPushButton>
#include <QDebug>

CoverSelectionDialog::CoverSelectionDialog(CAuView *au, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CoverSelectionDialog)
{
    ui->setupUi(this);
    createActions();

    view = au;
    img = view->getCurrentCoverImage();
    refresh();
}

CoverSelectionDialog::~CoverSelectionDialog()
{
    delete ui;
}

void CoverSelectionDialog::createActions()
{
    //need to add that 'open' button manualy, the standard one close the dialog
    QPushButton * b = ui->coverSelectionButtonBox->addButton(tr("Open"),QDialogButtonBox::ActionRole);
    b->setIcon(QIcon::fromTheme("document-open"));
    connect(b, SIGNAL(clicked()), this, SLOT(open()));

    connect(ui->coverSelectionButtonBox->button(QDialogButtonBox::Reset), SIGNAL(clicked()), this, SLOT(reset()));
    connect(ui->coverSelectionButtonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), this, SLOT(ok()));
}

void CoverSelectionDialog::open()
{
    //qDebug() << "Opening file selector";
    QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"),
                                                    "~",
                                                    tr("Images (*.png *.xpm *.jpg *.jpeg)"));
    if (!fileName.isEmpty()) {
        CoverImage cover;
        QPair<QString, QByteArray> tmp_img = cover.readCoverImage(fileName);
        if (tmp_img.second.isEmpty()) {
            KMessageBox::error(0,
                               tr("Import problem"),
                               tr("Import problem"));
            return;
        }
        img = tmp_img;
        refresh();
    }

}

void CoverSelectionDialog::refresh(){
    //qDebug() << "Refreching cover preview";
    if(!img.second.isNull()) {
        ui->coverSelectLabel->setPixmap(QPixmap::fromImage(QImage::fromData(img.second)));
    }
    else {
        qDebug() << "AUTHOR : nothing to display in cover preview";
    }
}

void CoverSelectionDialog::reset()
{
    //qDebug() << "reset image";
    ui->coverSelectLabel->setText("No cover selected yet");
    img = QPair<QString, QByteArray>();
}

void CoverSelectionDialog::ok()
{
    //qDebug() << "Valid image";
    view->setCurrentCoverImage(img);
    //qDebug() << "AUTHOR : cover image saved";
}

