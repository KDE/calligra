/*
 * <one line to give the library's name and an idea of what it does.>
 * SPDX-FileCopyrightText: 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 *
 */

#ifndef DESKTOPVIEWPROXY_H
#define DESKTOPVIEWPROXY_H

#include <QObject>

class QUrl;
class KoMainWindow;
class MainWindow;
class DesktopViewProxy : public QObject
{
    Q_OBJECT
public:
    explicit DesktopViewProxy(MainWindow *mainWindow, KoMainWindow *parent = nullptr);
    ~DesktopViewProxy() override;

public Q_SLOTS:
    void fileNew();
    void fileOpen();
    void fileSave();
    bool fileSaveAs();
    void reload();
    void loadExistingAsNew();
    void slotFileOpenRecent(const QUrl &url);

Q_SIGNALS:
    void documentSaved();

private:
    class Private;
    Private *d;
};

#endif // DESKTOPVIEWPROXY_H
