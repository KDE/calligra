/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 * Copyright (C) 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(bool allowClose READ allowClose WRITE setAllowClose)
    Q_PROPERTY(bool slateMode READ slateMode NOTIFY slateModeChanged)
    Q_PROPERTY(QString applicationName READ applicationName CONSTANT)
    Q_PROPERTY(QString currentTouchPage READ currentTouchPage WRITE setCurrentTouchPage NOTIFY currentTouchPageChanged)
    Q_PROPERTY(bool temporaryFile READ temporaryFile WRITE setTemporaryFile NOTIFY temporaryFileChanged)
    Q_PROPERTY(bool fullScreen READ fullScreen WRITE setFullScreen NOTIFY fullScreenChanged)
    Q_PROPERTY(QObject* desktopKoView READ desktopKoView NOTIFY desktopKoViewChanged)

public:
    explicit MainWindow(QStringList fileNames, QWidget* parent = 0, Qt::WindowFlags flags = 0);
    ~MainWindow() override;

    bool allowClose() const;
    void setAllowClose(bool allow);

    bool slateMode() const;

    QString applicationName() const {
        return QLatin1String("CALLIGRA GEMINI");
    }

    QString currentTouchPage() const;
    void setCurrentTouchPage(QString newPage);

    bool temporaryFile() const;
    void setTemporaryFile(bool newValue);

    bool fullScreen() const;
    void setFullScreen(bool newValue);

    QObject* desktopKoView() const;
    Q_INVOKABLE int lastScreen() const;

    Q_INVOKABLE void setAlternativeSaveAction(QAction* altAction);

    void closeEvent(QCloseEvent* event) override;

    Q_INVOKABLE void setDocAndPart(QObject* document, QObject* part);

public Q_SLOTS:
    void minimize();
    void closeWindow();

    void switchToTouch();
    void switchToDesktop();
    void documentChanged();
    void resetWindowTitle();
    void resourceChanged(int key, const QVariant& v);
    void resourceChangedTouch(int key, const QVariant& v);
    /**
     * Used to open an arbitrary file from the welcome screen,
     * not from the normal UI (desktopproxy does that)
     */
    void openFile();
Q_SIGNALS:
    void closeRequested();
    void switchedToTouch();
    void slateModeChanged();
    void currentTouchPageChanged();
    void temporaryFileChanged();
    void fullScreenChanged();
    void desktopKoViewChanged();
    void documentSaved();

private Q_SLOTS:
    void switchDesktopForced();
    void switchTouchForced();
    void touchChange();
    void enableAltSaveAction();

private:
    class Private;
    Private * const d;

#ifdef Q_OS_WIN
    bool winEvent(MSG * message, long * result);
#endif
};

#endif // MAINWINDOW_H
