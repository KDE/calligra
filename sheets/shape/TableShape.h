/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_SHEETS_TABLE_SHAPE_H
#define CALLIGRA_SHEETS_TABLE_SHAPE_H

#include <QObject>

#include <KoShape.h>
#include <KoFrameShape.h>

#define TableShapeId "TableShape"

class KoStore;
class KoDocumentBase;
class KoPart;
class KoDocument;
class KoDocumentResourceManager;

namespace Calligra
{
namespace Sheets
{
class Damage;
class Map;
class Sheet;
class SheetView;
class Doc;

class TableShape : public QObject, public KoShape, public KoFrameShape
{
    Q_OBJECT

public:
    explicit TableShape(KoDocumentResourceManager *resourceManager, KoDocumentBase *parentDoc, int firtColumn = 1, int firstRow = 1, int columns = 2, int rows = 8);
    ~TableShape() override;

    // KoShape interface
    void paint(QPainter& painter, const KoViewConverter& converter, KoShapePaintingContext &paintcontext) override;
    bool loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context) override;
    bool loadOdfFrameElement(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext & context) const override;
    void setSize(const QSizeF &size) override;
    /**
     * \return the map containing the data for this shape
     */
    Map* map() const;
    void setMap();

    void clear();

    /**
     * \return the sheet containing the data for this shape
     */
    Sheet* sheet() const;

    SheetView* sheetView() const;

    /**
     * Set the current sheet to the sheet with name \p sheetName .
     */
    void setSheet(const QString& sheetName);

    void setTopLeftOffset(const QPointF &point);
    QPointF topLeftOffset() const;

    QRect visibleCells() const;
    void updateVisibleCellRange();

    Doc *document() const;

    KoDocumentResourceManager *resourceManager() const;
    void paintCells(QPainter& painter);

    /// Used by tool because the tool paints the shape itself
    void setPaintingDisabled(bool disable);

Q_SIGNALS:
    void dataChanged();

protected:
    void shapeChanged(ChangeType type, KoShape *shape = nullptr) override;
    bool loadEmbeddedDocument(KoStore *store, const KoXmlElement &objectElement, KoShapeLoadingContext &context);

private Q_SLOTS:
    void handleDamages(const QList<Damage*>& damages);

private:
    Q_DISABLE_COPY(TableShape)

    class Private;
    Private * const d;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_TABLE_SHAPE
