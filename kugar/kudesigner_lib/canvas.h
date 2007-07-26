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

#include <q3canvas.h>
#include <q3valuelist.h>

#include <KoXmlReader.h>

#include "box.h"

class QIODevice;
class KuDesignerPlugin;

namespace Kudesigner
{

class KugarTemplate;
class Band;
class ReportItem;

typedef Q3ValueList<Box*> BoxList;

class Canvas: public Q3Canvas
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

    virtual bool loadXML( const KoXmlElement& report );

public slots:
    void changed();

signals:
    void itemSelected();
    void structureModified();

protected:
    virtual void drawForeground( QPainter &painter, const QRect &clip );

    void setReportItemAttributes( const KoXmlElement& element, ReportItem *item );
    void addReportItems( const KoXmlElement& element, Band *section );
    void setReportHeaderAttributes( const KoXmlElement& element );
    void setReportFooterAttributes( const KoXmlElement& element );
    void setPageHeaderAttributes( const KoXmlElement& element );
    void setPageFooterAttributes( const KoXmlElement& element );
    void setDetailHeaderAttributes( const KoXmlElement& element );
    void setDetailAttributes( const KoXmlElement& element );
    void setDetailFooterAttributes( const KoXmlElement& element );

private:
    //    KudesignerDoc *m_doc;
    void scaleCanvas( int scale );
    KuDesignerPlugin *m_plugin;
    KugarTemplate *m_kugarTemplate;

};

}

#endif
