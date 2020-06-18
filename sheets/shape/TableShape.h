/* This file is part of the KDE project
 * Copyright 2020 Dag Andersen <dag.andersen@kdemail.net>
 * Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
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

#ifndef CALLIGRA_SHEETS_TABLE_SHAPE_H
#define CALLIGRA_SHEETS_TABLE_SHAPE_H

#include <QObject>

#include <KoShape.h>
#include <KoFrameShape.h>
#include <KoPart.h>

#include <Doc.h>

#define TableShapeId "TableShape"

class KoStore;
class KoDocumentBase;
class KoPart;
class KoDocument;
class KoDocumentResourceManager;
class KoMainWindow;
class KoView;

namespace Calligra
{
namespace Sheets
{
class Damage;
class Map;
class Sheet;
class SheetView;
class Doc;

class TableShapePart : public KoPart
{
    Q_OBJECT
public:
    TableShapePart(const KoComponentData &data, QObject *parent = nullptr)  : KoPart(data, parent) {}
    ~TableShapePart() {}

    KoMainWindow *createMainWindow() override {
        return nullptr;//new KoMainWindow("application/vnd.oasis.opendocument.spreadsheet", componentData());
    }
protected:
    KoView *createViewInstance(KoDocument */*document*/, QWidget */*parent*/) override { return nullptr; }
};

class TableShapeDoc : public Doc
{
    Q_OBJECT
public:
    TableShapeDoc(KoPart *part) : Doc(part) {}
    ~TableShapeDoc() {}
    void initConfig() override {}
    void saveConfig() override {}

public Q_SLOTS:
    void initEmpty() override {}
};


class TableShape : public QObject, public KoShape, public KoFrameShape
{
    Q_OBJECT

public:
    explicit TableShape(KoPart *part, KoDocumentResourceManager *resourceManager);
    ~TableShape() override;

    void updateUrl();

    // KoShape interface
    void paint(QPainter& painter, const KoViewConverter& converter, KoShapePaintingContext &paintcontext) override;
    bool loadOdf(const KoXmlElement & element, KoShapeLoadingContext &context) override;
    bool loadOdfFrameElement(const KoXmlElement &element, KoShapeLoadingContext &context) override;
    void saveOdf(KoShapeSavingContext & context) const override;
    void setSize(const QSizeF &size) override;
    void resize(const QSizeF &size); // resize without adjusting columns/rows
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

    TableShapeDoc *document() const;

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
