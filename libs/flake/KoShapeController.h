/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006-2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPECONTROLLER_H
#define KOSHAPECONTROLLER_H

#include "flake_export.h"

#include <QList>
#include <QMetaType>

class KoCanvasBase;
class KoShape;
class KoShapeBasedDocumentBase;
class KUndo2Command;
class KoDocumentResourceManager;

/**
 * Class used by tools to maintain the list of shapes.
 * All applications have some sort of list of all shapes that belong to the document.
 * The applications implement the KoShapeBasedDocumentBase interface (all pure virtuals)
 * to add and remove shapes from the document. To ensure that an application can expect
 * a certain protocol to be adhered to when adding/removing shapes, all tools use the API
 * from this class for maintaining the list of shapes in the document. So no tool gets
 * to access the application directly.
 */
class FLAKE_EXPORT KoShapeController
{
public:
    /**
     * Create a new Controller; typically not called by applications, only
     * by the KonCanvasBase constructor.
     * @param canvas the canvas this controller works for. The canvas can be 0
     * @param shapeBasedDocument the application provided shapeBasedDocument that we can call.
     */
    KoShapeController(KoCanvasBase *canvas, KoShapeBasedDocumentBase *shapeBasedDocument);
    /// destructor
    ~KoShapeController();

    /**
     * @brief Add a shape to the document.
     * If the shape has no parent, the active layer will become its parent.
     *
     * @param shape to add to the document
     * @param parent the parent command if the resulting command is a compound undo command.
     *
     * @return command which will insert the shape into the document or 0 if the
     *         insertion was cancelled. The command is not yet executed.
     */
    KUndo2Command *addShape(KoShape *shape, KUndo2Command *parent = nullptr);

    /**
     * @brief Add a shape to the document, skipping any dialogs or other user interaction.
     *
     * @param shape to add to the document
     * @param parent the parent command if the resulting command is a compound undo command.
     *
     * @return command which will insert the shape into the document. The command is not yet executed.
     */
    KUndo2Command *addShapeDirect(KoShape *shape, KUndo2Command *parent = nullptr);

    /**
     * @brief Remove a shape from the document.
     *
     * @param shape to remove from the document
     * @param parent the parent command if the resulting command is a compound undo command.
     *
     * @return command which will remove the shape from the document.
     *         The command is not yet executed.
     */
    KUndo2Command *removeShape(KoShape *shape, KUndo2Command *parent = nullptr);

    /**
     * Remove a shape from the document.
     *
     * @param shapes the set of shapes to remove from the document
     * @param parent the parent command if the resulting command is a compound undo command.
     *
     * @return command which will remove the shape from the document.
     *         The command is not yet executed.
     */
    KUndo2Command *removeShapes(const QList<KoShape *> &shapes, KUndo2Command *parent = nullptr);

    /**
     * @brief Set the KoShapeBasedDocumentBase used to add/remove shapes.
     *
     * NOTE: only Sheets uses this method. Do not use it in your application. Sheets
     * has to also call:
     * <code>KoToolManager::instance()->updateShapeControllerBase(shapeBasedDocument, canvas->canvasController());</code>
     *
     * @param shapeBasedDocument the new shapeBasedDocument.
     */
    void setShapeControllerBase(KoShapeBasedDocumentBase *shapeBasedDocument);

    /**
     * Return a pointer to the resource manager associated with the
     * shape-set (typically a document). The resource manager contains
     * document wide resources * such as variable managers, the image
     * collection and others.
     */
    KoDocumentResourceManager *resourceManager() const;

    /**
     * @brief Returns the KoShapeBasedDocumentBase used to add/remove shapes.
     *
     * @return the KoShapeBasedDocumentBase
     */
    KoShapeBasedDocumentBase *documentBase() const;

private:
    class Private;
    Private *const d;
};

Q_DECLARE_METATYPE(KoShapeController *)

#endif
