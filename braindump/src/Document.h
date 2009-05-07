/*
 *  Copyright (c) 2006-2008 Thorsten Zachmann <zachmann@kde.org>
 *  Copyright (c) 2009 Cyrille Berger <cberger@cberger.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _DOCUMENT_H
#define _DOCUMENT_H

#include <QObject>

#include <KoOdf.h>
#include <KoDocument.h>
#include <KoShapeControllerBase.h>
#include "KoPageApp.h"
#include "kopageapp_export.h"
#include <SectionGroup.h>

class Section;

class KoShapeSavingContext;
class KoPAPage;
class KoPALoadingContext;
class KoPASavingContext;
class KoXmlWriter;

/// Document class that stores Sections
class KOPAGEAPP_EXPORT Document : public KoDocument, public KoShapeControllerBase, public SectionGroup
{
    Q_OBJECT
public:

    explicit Document( QWidget* parentWidget, QObject* parent, bool singleViewMode = false );
    virtual ~Document();

    void paintContent( QPainter &painter, const QRect &rect);

    bool loadXML( const KoXmlDocument & doc, KoStore *store );
    bool loadOdf( KoOdfReadStore & odfStore );

    bool saveOdf( SavingContext & documentContext );

    void addShape( KoShape *shape );
    void removeShape( KoShape* shape );

    void setRulersVisible(bool visible);
    bool rulersVisible() const;

    virtual QMap<QString, KoDataCenter *>  dataCenterMap() const;

signals:
    void shapeAdded(KoShape* shape);
    void shapeRemoved(KoShape* shape);
    void sigSectionAdded(Section* section);
    void sigSectionRemoved(Section* section);
  protected:
    virtual void sectionAdded(Section* page);
    virtual void sectionRemoved(Section* page);

protected:
    void insertIntoDataCenterMap(QString key, KoDataCenter *dc);

    /// Load the configuration
    void loadConfig();
    /// Save the configuration
    void saveConfig();
protected:
    /// Creates a BrainDumpView instance and returns it
    virtual KoView* createViewInstance(QWidget* parent);

  private:
    Section* sectionByShape( KoShape * shape ) const;
private:
    class Private;
    Private * const d;
};

#endif /* KOPADOCUMENT_H */
