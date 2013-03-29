/*
 *  kis_main.cpp - part of Krita
 *
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
 */
#ifndef KIS_APPLICATION_H
#define KIS_APPLICATION_H

#include <KoApplicationBase.h>

#include <QApplication>

class KoDocumentFactory;
class KoViewFactory;
class KoDocumentManager;

class KisApplication : public QApplication, public KoApplicationBase
{
    Q_OBJECT
public:
    explicit KisApplication(int &argc, char **argv, QObject *parent = 0);
    
    int start();

    // Overridden to handle exceptions from event handlers.
    virtual bool notify(QObject *receiver, QEvent *event);

private:
    KoDocumentManager *m_documentManager;
    KoDocumentFactory *m_documentFactory;
    KoViewFactory *m_viewFactory;

};

#endif // KIS_APPLICATION_H
