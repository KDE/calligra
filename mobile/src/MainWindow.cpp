/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <jhahoneyk@gmail.com>
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
 */

#include "MainWindow.h"

#include <KoExternalEditorInterface.h>
#include <KoCellTool.h>

MainWindow::MainWindow(QWidget *parent) : KoAbstractApplication(parent)
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::currentPageChanged()
{

}

KoExternalEditorInterface* MainWindow::createExternalCellEditor(KoCellTool* cellTool) const
{
    return 0;
}

bool MainWindow::isVirtualKeyboardVisible() const
{
    return true;
}

void MainWindow::setVirtualKeyboardVisible(bool set)
{

}

QString MainWindow::applicationName() const
{
    return QString("bleh");
}

void MainWindow::updateActions()
{

}

void MainWindow::setWindowTitle(const QString& title)
{

}

void MainWindow::setCentralWidget(QWidget* widget)
{

}

QString MainWindow::showGetSaveFileNameDialog(const QString& caption, const QString& dir, const QString& filter)
{
    return QString("/dev/null");
}

QString MainWindow::showGetOpenFileNameDialog(const QString& caption, const QString& dir, const QString& filter)
{
    return QString("/dev/null");
}

void MainWindow::showUiBeforeDocumentOpening(bool isNewDocument)
{

}

void MainWindow::setProgressIndicatorVisible(bool visible)
{

}

bool MainWindow::startNewInstance(const KoAbstractApplicationOpenDocumentArguments& args)
{
    return true;
}

QMessageBox::StandardButton MainWindow::askQuestion(KoAbstractApplicationController::QuestionType type, const QString& messageText)
{
    return QMessageBox::Ok;
}

void MainWindow::showMessage(KoAbstractApplicationController::MessageType type, const QString& messageText)
{

}

#include "MainWindow.moc"
