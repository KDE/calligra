
#include "CoverPreviewTool.h"
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
}
void CoverPreviewTool::setCurrentImage(QPair<QString, QByteArray> img) {
    imageField->setPixmap(QPixmap::fromImage(QImage::fromData(img.second)));
}

void CoverPreviewTool::open()
{
    QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"),
                                                    QDir::currentPath(),
                                                    tr("Images (*.png *.xpm *.jpg *.jpeg)"));
    if (!fileName.isEmpty()) {
        QImage image(fileName);
        if (image.isNull()) {
            KMessageBox::error(0,
                               tr("Import problem"),
                               tr("Import problem"));
            return;
        }
        imageField->setPixmap(QPixmap::fromImage(image));
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

