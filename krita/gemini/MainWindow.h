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

public:
    explicit MainWindow(QStringList fileNames, QWidget* parent = 0, Qt::WindowFlags flags = 0);
    virtual ~MainWindow();

    bool allowClose() const;
    void setAllowClose(bool allow);

	bool slateMode() const;

    virtual void closeEvent(QCloseEvent* event);

public Q_SLOTS:
    void minimize();
    void closeWindow();

    void switchToSketch();
    void switchToDesktop();
    void documentChanged();
Q_SIGNALS:
    void closeRequested();
    void switchedToSketch();
	void slateModeChanged();

private:
    class Private;
    Private * const d;

    bool winEvent ( MSG * message, long * result );
};

#endif // MAINWINDOW_H
