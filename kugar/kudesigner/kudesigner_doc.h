/* This file is part of the KDE project
   Copyright (C) 2002 Alexander Dymo <cloudtemple@mksat.net>

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

#ifndef KUDESIGNER_PART_H
#define KUDESIGNER_PART_H

#include <koDocument.h>
#include <kcommand.h>
#include <kocommandhistory.h>
#include "plugin.h"

class MyCanvas;
class QCanvas;
class QDomNode;
class CanvasReportItem;
class CanvasBand;
class KCommand;

class KudesignerDoc : public KoDocument
{
    Q_OBJECT
public:
    KudesignerDoc( QWidget *parentWidget = 0, const char *widgetName = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
    ~KudesignerDoc();

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE, double zoomX = 1.0, double zoomY = 1.0 );

    virtual bool initDoc();

    virtual bool loadOasis( const QDomDocument&, KoOasisStyles&, const QDomDocument&, KoStore* );
    virtual bool saveOasis(KoStore*, KoXmlWriter*);

    virtual bool loadXML( QIODevice *, const QDomDocument & );
    virtual QDomDocument saveXML();

    MyCanvas *canvas() const;

    void loadPlugin(const QString& name);
    KuDesignerPlugin *plugin();
    Dock propertyPosition();
    void setForcedPropertyEditorPosition(Dock);

    void addCommand(KCommand *cmd);

protected:
    virtual KoView* createViewInstance( QWidget* parent, const char* name );
    virtual bool completeSaving( KoStore* store );
    virtual bool completeLoading( KoStore* store );

protected slots:
    void commandExecuted();
    void documentRestored();

private:
    KoCommandHistory *history;

    MyCanvas *docCanvas;

    KuDesignerPlugin *m_plugin;
    Dock m_propPos;

    void setReportItemAttributes(QDomNode *node, CanvasReportItem *item);
    void addReportItems(QDomNode *node, CanvasBand *section);
    void setReportHeaderAttributes(QDomNode *node);
    void setReportFooterAttributes(QDomNode *node);
    void setPageHeaderAttributes(QDomNode *node);
    void setPageFooterAttributes(QDomNode *node);
    void setDetailHeaderAttributes(QDomNode *node);
    void setDetailAttributes(QDomNode *node);
    void setDetailFooterAttributes(QDomNode *node);
};

#endif
