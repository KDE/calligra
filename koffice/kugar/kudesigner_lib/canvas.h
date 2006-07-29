/* This file is part of the KDE project
 Copyright (C) 2002-2004 Alexander Dymo <adymo@mksat.net>

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
#ifndef MYCANVAS_H
#define MYCANVAS_H

#include <qcanvas.h>
#include <qvaluelist.h>

#include "box.h"

class QDomNode;
class QIODevice;
class KuDesignerPlugin;

namespace Kudesigner
{

class KugarTemplate;
class Band;
class ReportItem;

typedef QValueList<Box*> BoxList;

class Canvas: public QCanvas
{
    Q_OBJECT
public:
    Canvas( int w, int h );
    ~Canvas();

    KugarTemplate *kugarTemplate();
    void setKugarTemplate( KugarTemplate *kugarTemplate );

    //    KudesignerDoc *document(){return m_doc;}

    BoxList selected;

    KuDesignerPlugin *plugin();
    void setPlugin( KuDesignerPlugin *plugin );

    void unselectAll();
    void selectAll();
    void selectItem( Box *it, bool addToSelection = true );
    void unselectItem( Box *it );
    void setStructureModified() { emit structureModified(); }
    //    void deleteSelected();

    virtual bool loadXML( const QDomNode &report );

public slots:
    void changed();

signals:
    void itemSelected();
    void structureModified();

protected:
    virtual void drawForeground( QPainter &painter, const QRect &clip );

    void setReportItemAttributes( QDomNode *node, ReportItem *item );
    void addReportItems( QDomNode *node, Band *section );
    void setReportHeaderAttributes( QDomNode *node );
    void setReportFooterAttributes( QDomNode *node );
    void setPageHeaderAttributes( QDomNode *node );
    void setPageFooterAttributes( QDomNode *node );
    void setDetailHeaderAttributes( QDomNode *node );
    void setDetailAttributes( QDomNode *node );
    void setDetailFooterAttributes( QDomNode *node );

private:
    //    KudesignerDoc *m_doc;
    void scaleCanvas( int scale );
    KuDesignerPlugin *m_plugin;
    KugarTemplate *m_kugarTemplate;

};

}

#endif
