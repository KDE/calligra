
#include "CoverPreviewTool.h"
#include "CoverImage.h"
#include <QFileDialog>
#include <QDebug>
#include <kmessagebox.h>

#include <KoShapeRegistry.h>
#include <KoShapeManager.h>

CoverPreviewTool::CoverPreviewTool(QWidget *parent) :
    QMainWindow(parent)
{
    imageField = new QLabel;
    imageField->setBackgroundRole(QPalette::Base);
    imageField->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageField->setScaledContents(true);
    setCentralWidget(imageField);

    createActions();
    createMenus();

    setWindowTitle(tr("Cover Viewer"));
    resize(500, 400);
    //refresh();
}
void CoverPreviewTool::setCAuView(CAuView *au){
    view = au;
    refresh();
}

void CoverPreviewTool::refresh(){
    qDebug() << "Refreching";
    img = view->getCurrentCoverImage();
    if(img.second.isEmpty()) {
        imageField->setPixmap(QPixmap::fromImage(QImage::fromData(img.second)));
    }
    else {
        qDebug() << "Uninitialized view";
    }

}

void CoverPreviewTool::open()
{
    CoverImage cover;
    QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"),
                                                    "~",
                                                    tr("Images (*.png *.xpm *.jpg *.jpeg)"));
    if (!fileName.isEmpty()) {
        //QImage image(fileName);
        //image.
        if (cover.readCoverImage(fileName).second.isEmpty()) {
            KMessageBox::error(0,
                               tr("Import problem"),
                               tr("Import problem"));
            return;
        }
        img =cover.readCoverImage(fileName);
        imageField->setPixmap(QPixmap::fromImage(QImage::fromData(img.second)));
        view->setCurrentCoverImage(img);
        qDebug() << "AUTHOR : fichier envoyé -> " << fileName;
        refresh();
    }
}

void CoverPreviewTool::createActions()
{
    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
}

void CoverPreviewTool::createMenus()
{
    fileMenu = new QMenu(tr("&File"), this);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    menuBar()->addMenu(fileMenu);
}

void CoverPreviewTool::updateActions()
{
}

