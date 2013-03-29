/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIS_MDI_MAINWINDOW_H
#define KIS_MDI_MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>

#include <KoMainWindowBase.h>
#include <KoMainWindowFactory.h>

class KoDocumentManager;

class KisMdiMainWindow : public QMainWindow, public KoMainWindowBase
{
    Q_OBJECT
public:
    explicit KisMdiMainWindow(KoDocumentManager *documentManager, QWidget *parent = 0);
    virtual ~KisMdiMainWindow();

protected:

    void closeEvent(QCloseEvent *event);

private:
    QMdiArea *m_mdiArea;
    KoDocumentManager *m_documentManager;
};

class KisMdiMainWindowFactory : public KoMainWindowFactory {
public:
    KoMainWindowBase *create(KoDocumentManager *documentManager) {
        return new KisMdiMainWindow(documentManager);
    }
};


#endif // KIS_MDI_MAINWINDOW_H
