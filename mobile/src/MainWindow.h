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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
typedef QMainWindow KoAbstractApplicationBase;

#include <KoAbstractApplication.h>

class MainWindow : public KoAbstractApplication
{
    Q_OBJECT
protected:
    virtual void currentPageChanged();
    virtual KoExternalEditorInterface* createExternalCellEditor(KoCellTool* cellTool) const;
    virtual bool isVirtualKeyboardVisible() const;
    virtual void setVirtualKeyboardVisible(bool set);
    virtual QString applicationName() const;
    virtual void updateActions();
    virtual void setWindowTitle(const QString& title);
    virtual void setCentralWidget(QWidget* widget);
    virtual QString showGetSaveFileNameDialog(const QString& caption, const QString& dir, const QString& filter);
    virtual QString showGetOpenFileNameDialog(const QString& caption, const QString& dir, const QString& filter);
    virtual void showUiBeforeDocumentOpening(bool isNewDocument);
    virtual void setProgressIndicatorVisible(bool visible);
    virtual bool startNewInstance(const KoAbstractApplicationOpenDocumentArguments& args);
    virtual QMessageBox::StandardButton askQuestion(KoAbstractApplicationController::QuestionType type, const QString& messageText = QString());
    virtual void showMessage(KoAbstractApplicationController::MessageType type, const QString& messageText = QString());

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
