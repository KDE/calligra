/* This file is part of the KDE project
  Copyright (C) 2003-2004 Alexander Dymo <cloudtemple@mksat.net>

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
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#ifndef KUDESIGNER_DOC_H
#define KUDESIGNER_DOC_H

#include <KoDocument.h>
#include <kcommand.h>
#include "plugin.h"

class Q3Canvas;
class QDomNode;
class QIODevice;
class QDomDocument;

namespace Kudesigner
{
class Canvas;
}

class KudesignerDoc: public KoDocument
{
    Q_OBJECT
public:
    KudesignerDoc( QWidget *parentWidget = 0, QObject* parent = 0, bool singleViewMode = false );

    ~KudesignerDoc();

    virtual void paintContent( QPainter& painter, const QRect& rect, bool transparent = FALSE, double zoomX = 1.0, double zoomY = 1.0 );

    virtual bool loadOasis( const QDomDocument&, KoOasisStyles&, const QDomDocument&, KoStore* );
    virtual bool saveOasis( KoStore*, KoXmlWriter* );

    virtual bool loadXML( QIODevice *, const QDomDocument & );
    virtual QDomDocument saveXML();

    virtual int supportedSpecialFormats() const;
    virtual bool saveToStream( QIODevice * dev );

    Kudesigner::Canvas *canvas();

    void loadPlugin( const QString& name );
    KuDesignerPlugin *plugin();
	Qt::DockWidgetArea propertyPosition();
    void setForcedPropertyEditorPosition( Qt::DockWidgetArea );

    void addCommand( KCommand *cmd );

    virtual bool modified() const;

signals:
    void canvasChanged( Kudesigner::Canvas * );
    void modificationMade( bool );

public slots:
    virtual void setModified( const bool val );
    virtual void setModified();
    virtual void initEmpty();

protected:
    virtual KoView* createViewInstance( QWidget* parent );
    virtual bool completeSaving( KoStore* store );
    virtual bool completeLoading( KoStore* store );

protected slots:
    void commandExecuted();
    void documentRestored();

private:
    KCommandHistory *history;

    Kudesigner::Canvas *docCanvas;

    KuDesignerPlugin *m_plugin;
	Qt::DockWidgetArea m_propPos;
    bool m_modified;
};

#endif
