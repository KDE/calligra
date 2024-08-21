/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-FileCopyrightText: 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
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
    Q_PROPERTY(QObject *desktopKoView READ desktopKoView NOTIFY desktopKoViewChanged)

public:
    explicit MainWindow(QStringList fileNames, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~MainWindow() override;

    bool allowClose() const;
    void setAllowClose(bool allow);

    bool slateMode() const;

    QString applicationName() const
    {
        return QLatin1String("CALLIGRA GEMINI");
    }

    QString currentTouchPage() const;
    void setCurrentTouchPage(QString newPage);

    bool temporaryFile() const;
    void setTemporaryFile(bool newValue);

    bool fullScreen() const;
    void setFullScreen(bool newValue);

    QObject *desktopKoView() const;
    Q_INVOKABLE int lastScreen() const;

    Q_INVOKABLE void setAlternativeSaveAction(QAction *altAction);

    void closeEvent(QCloseEvent *event) override;

    Q_INVOKABLE void setDocAndPart(QObject *document, QObject *part);

public Q_SLOTS:
    void minimize();
    void closeWindow();

    void switchToTouch();
    void switchToDesktop();
    void documentChanged();
    void resetWindowTitle();
    void resourceChanged(int key, const QVariant &v);
    void resourceChangedTouch(int key, const QVariant &v);
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
    Private *const d;

#ifdef Q_OS_WIN
    bool winEvent(MSG *message, long *result);
#endif
};

#endif // MAINWINDOW_H
