/*
 * This file is part of Maemo 5 Office UI for Calligra
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>
 *
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "MainWindow.h"
#include "ApplicationController.h"
#include "ui_MainWindow.h"
#include "Common.h"
#include "ZoomDialog.h"
#include "HildonMenu.h"
#include "NotifyDialog.h"
#include "AboutDialog.h"
#include "PresentationTool.h"
#include "MainWindowAdaptor.h"
#include "FoExternalEditor.h"
#include "FoImageSelectionWidget.h"
#ifdef HAVE_OPENGL
#include "GlPresenter.h"
#endif
#include <KoCellTool.h>
#include "VirtualKeyBoard.h"
#include "ConfirmationDialog.h"
#include "PreviewDialog.h"
#include "FileChooserDialog.h"

#include <QFileDialog>
#include <QUrl>
#include <QDebug>
#include <QLineEdit>
#include <QCheckBox>
#include <QScrollBar>
#include <QTimer>
#include <QIcon>
#include <QPushButton>
#include <QSize>
#include <QTextDocument>
#include <QTextCursor>
#include <QPair>
#include <QMenuBar>
#include <QX11Info>
#include <QShortcut>
#include <QProcess>
#include <QAction>
#include <QLabel>
#include <QTextBlock>
#include <QTextList>
#include <QGridLayout>
#include <QDialog>
#include <QToolButton>
#include <QMessageBox>
#include <QFontComboBox>
#include <QColor>
#include <QColorDialog>
#include <QFrame>
#include <QPalette>
#include <QListWidget>

#ifdef Q_WS_MAEMO_5
#include <QtMaemo5/QMaemo5InformationBox>
#include "Accelerator.h"
#endif

#include <kfileitem.h>

#include <KoView.h>
#include <KoCanvasBase.h>
#include <kdemacros.h>
#include <KoCanvasControllerWidget.h>
#include <KoZoomMode.h>
#include <KoZoomController.h>
#include <KoToolProxy.h>
#include <KoToolBase.h>
#include <KoToolManager.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoShapeUserData.h>
#include <KoSelection.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoPACanvasBase.h>
#include <KoTextEditor.h>
#include <KoTextDocument.h>
#include <KoTextShapeData.h>
#include <KoPAView.h>
#include <KoStore.h>
#include <KoCanvasBase.h>
#include <KoToolRegistry.h>
#include <KoShapeCreateCommand.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeRegistry.h>
#include <KWDocument.h>
#include <KWPage.h>
#include <KWView.h>
#include <KoShapeLayer.h>
#include <styles/KoParagraphStyle.h>
#include <styles/KoListLevelProperties.h>
#include <KoList.h>
#include <kundo2stack.h>
#include <tables/Map.h>
#include <tables/DocBase.h>
#include <tables/part/View.h>
#include <tables/Sheet.h>

MainWindow::MainWindow(Splash *aSplash, QWidget *parent)
        : QMainWindow(parent),
          KoAbstractApplicationWindow((m_controller = new ApplicationController(aSplash, this)))
{
}

MainWindow::~MainWindow()
{
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    return m_controller->handleMainWindowEventFilter(watched, event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    m_controller->handleMainWindowMousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    m_controller->handleMainWindowMouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_controller->handleMainWindowMouseReleaseEvent(event);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    m_controller->handleMainWindowPaintEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_controller->handleMainWindowResizeEvent(event);
}
