/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPLATO_PART_H
#define KPLATO_PART_H

#include <koDocument.h>

class KoView;
class KPTView;
class KoCommandHistory;
class KCommand;
class KPTProject;
class KPTProjectDialog;
class KPTResourceGroup;


class KPTPart : public KoDocument {
    Q_OBJECT

public:
    KPTPart(QWidget *parentWidget = 0, const char *widgetName = 0,
	    QObject* parent = 0, const char* name = 0,
	    bool singleViewMode = false);
    ~KPTPart();

    virtual void paintContent(QPainter& painter, const QRect& rect,
			      bool transparent = FALSE,
			      double zoomX = 1.0, double zoomY = 1.0);

    virtual bool initDoc();

    /**
     * Edit the settings of the project
     */
    void editProject();

    KPTProject &getProject() { return *m_project; }
    const KPTProject &getProject() const { return *m_project; }

    // The load and save functions. Look in the file kplato.dtd for info
    virtual bool loadXML(QIODevice *, const QDomDocument &document);
    virtual QDomDocument saveXML();

    bool loadOasis(const QDomDocument &, KoOasisStyles &, KoStore *) { return false; }

    void addCommand(KCommand * cmd);
    
protected:
    virtual KoView* createViewInstance(QWidget* parent, const char* name);

protected slots:
    void slotDocumentRestored();
    void slotCommandExecuted();

private:
    KPTProject *m_project;
    KPTProjectDialog *m_projectDialog;
    KPTView *m_view;
    KoCommandHistory *m_commandHistory;
};

#endif
