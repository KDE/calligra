/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>
  SPDX-FileCopyrightText: 2008 Inge Wallin <inge@lysator.liu.se>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "EmfViewer.h"

#include <QApplication>
#include <QFileDialog>
#include <QMenuBar>

#include "../EmfOutputPainterStrategy.h"

EmfViewer::EmfViewer(QSize &size)
    : QMainWindow()
{
    m_size = size;

    setWindowTitle("EMF Demo Viewer");

    QMenu *fileMenu = menuBar()->addMenu("&File");

    // The "Open" action
    m_fileOpenAction = fileMenu->addAction("&Open", this, SLOT(slotOpenFile()));
    m_fileOpenAction->setShortcut(Qt::CTRL + Qt::Key_O);

    fileMenu->addSeparator();

    // The "Quit" action
    m_fileQuitAction = fileMenu->addAction("&Quit", qApp, SLOT(closeAllWindows()));
    m_fileQuitAction->setShortcut(Qt::CTRL + Qt::Key_Q);

    // Set a suitably large size.
    resize(m_size + QSize(50, 50));

    // ...and finally create the label that will show everything.
    m_label = new QLabel(this);
    setCentralWidget(m_label);
}

EmfViewer::~EmfViewer()
{
}

void EmfViewer::loadFile(const QString &fileName)
{
    Parser parser;

    // The image that the EMF parser should paint on.
    QImage image(m_size, QImage::Format_ARGB32_Premultiplied);
    QPainter painter(&image);

    OutputPainterStrategy output(painter, m_size);
    parser.setOutput(&output);
    parser.load(QString(fileName));

    QPixmap pixmap = QPixmap::fromImage(image);
    m_label->setPixmap(pixmap.scaled(m_size, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_label->show();
}

// ----------------------------------------------------------------
//                                 Slots

void EmfViewer::slotOpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open EMF document", QDir::homePath(), "EMF Documents (*.emf)");
    if (fileName.isEmpty()) {
        return;
    }

    loadFile(fileName);
}

#include <EmfViewer.moc>
