/*
   SPDX-FileCopyrightText: 2006 Boudewijn Rempt (boud@valdyas.org)
   SPDX-FileCopyrightText: 2007, 2009, 2010 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KO_DOCUMENTRESOURCEMANAGER_H
#define KO_DOCUMENTRESOURCEMANAGER_H

#include <QObject>

#include "flake_export.h"

class KoShape;
class KUndo2Stack;
class KoImageCollection;
class KoDocumentBase;
class KoShapeController;
class KoColor;
class KoUnit;

class QVariant;
class QSizeF;

/**
 * The KoResourceManager contains a set of per-canvas <i>or</i> per-document
 * properties, like current foreground color, current background
 * color and more. All tools belonging to the current canvas are
 * notified when a Resource changes (is set).
 *
 * The properties come from the KoDocumentResourceManager::DocumentResource
 * See KoShapeController::resourceManager
 *
 * The manager can contain all sorts of variable types and there are accessors
 * for the most common ones.  All variables are always stored inside a QVariant
 * instance internally and you can always just use the resource() method to get
 * that directly.
 * The way to store arbitairy data objects that are stored as pointers you can use
 * the following code snippets;
 * @code
 *  QVariant variant;
 *  variant.setValue<void*>(textShapeData->document());
 *  resourceManager->setResource(KoText::CurrentTextDocument, variant);
 *  // and get it out again.
 *  QVariant var = resourceManager->resource(KoText::CurrentTextDocument);
 *  document = static_cast<QTextDocument*>(var.value<void*>());
 * @endcode
 */
class FLAKE_EXPORT KoDocumentResourceManager : public QObject
{
    Q_OBJECT

public:
    /**
     * This enum holds identifiers to the resources that can be stored in here.
     */
    enum DocumentResource {
        UndoStack, ///< The document-wide undo stack (KUndo2Stack)
        ImageCollection, ///< The KoImageCollection for the document
        OdfDocument, ///< The document this canvas shows (KoDocumentBase)
        PasteOffset, ///< Application wide paste offset
        PasteAtCursor, ///< Application wide paste at cursor setting
        HandleRadius, ///< The handle radius used for drawing handles of any kind
        GrabSensitivity, ///< The grab sensitivity used for grabbing handles of any kind
        MarkerCollection, ///< The collection holding all markers
        ShapeController, ///< The KoShapeController for the document

        KarbonStart = 1000, ///< Base number for Karbon specific values.
        KexiStart = 2000, ///< Base number for Kexi specific values.
        FlowStart = 3000, ///< Base number for Flow specific values.
        PlanStart = 4000, ///< Base number for Plan specific values.
        StageStart = 5000, ///< Base number for Stage specific values.
        SheetsStart = 6000, ///< Base number for Sheets specific values.
        WordsStart = 7000, ///< Base number for Words specific values.
        KoPageAppStart = 8000, ///< Base number for KoPageApp specific values.
        KoTextStart = 9000 ///< Base number for KoText specific values.
    };

    /**
     * Constructor.
     * @param parent the parent QObject, used for memory management.
     */
    explicit KoDocumentResourceManager(QObject *parent = nullptr);
    ~KoDocumentResourceManager() override;

    /**
     * Set a resource of any type.
     * @param key the integer key
     * @param value the new value for the key.
     * @see  KoDocumentResourceManager::DocumentResource
     */
    void setResource(int key, const QVariant &value);

    /**
     * Set a resource of type KoColor.
     * @param key the integer key
     * @param color the new value for the key.
     * @see  KoDocumentResourceManager::DocumentResource
     */
    void setResource(int key, const KoColor &color);

    /**
     * Set a resource of type KoShape*.
     * @param key the integer key
     * @param id the new value for the key.
     * @see  KoDocumentResourceManager::DocumentResource
     */
    void setResource(int key, KoShape *shape);

    /**
     * Set a resource of type KoUnit
     * @param key the integer key
     * @param id the new value for the key.
     * @see  KoDocumentResourceManager::DocumentResource
     */
    void setResource(int key, const KoUnit &unit);

    /**
     * Returns a qvariant containing the specified resource or a standard one if the
     * specified resource does not exist.
     * @param key the key
     * @see  KoDocumentResourceManager::DocumentResource
     */
    QVariant resource(int key) const;

    /**
     * Return the resource determined by param key as a boolean.
     * @param key the identifying key for the resource
     * @see  KoDocumentResourceManager::DocumentResource
     */
    bool boolResource(int key) const;

    /**
     * Return the resource determined by param key as an integer.
     * @param key the identifying key for the resource
     * @see  KoDocumentResourceManager::DocumentResource
     */
    int intResource(int key) const;

    /**
     * Return the resource determined by param key as a KoColor.
     * @param key the identifying key for the resource
     * @see  KoDocumentResourceManager::DocumentResource
     */
    KoColor koColorResource(int key) const;

    /**
     * Return the resource determined by param key as a pointer to a KoShape.
     * @param key the identifying key for the resource
     * @see  KoDocumentResourceManager::DocumentResource
     */
    KoShape *koShapeResource(int key) const;

    /**
     * Return the resource determined by param key as a QString .
     * @param key the identifying key for the resource
     * @see  KoDocumentResourceManager::DocumentResource
     */
    QString stringResource(int key) const;

    /**
     * Return the resource determined by param key as a QSizeF.
     * @param key the identifying key for the resource
     * @see  KoDocumentResourceManager::DocumentResource
     */
    QSizeF sizeResource(int key) const;

    /**
     * Return the resource determined by param key as a KoUnit.
     * @param key the identifying key for the resource
     * @see  KoDocumentResourceManager::DocumentResource
     */
    KoUnit unitResource(int key) const;

    /**
     * Returns true if there is a resource set with the requested key.
     * @param key the identifying key for the resource
     * @see  KoDocumentResourceManager::DocumentResource
     */
    bool hasResource(int key) const;

    /**
     * Remove the resource with @p key from the provider.
     * @param key the key that will be used to remove the resource
     * There will be a signal emitted with a variable that will return true on QVariable::isNull();
     * @see  KoDocumentResourceManager::DocumentResource
     */
    void clearResource(int key);

    /**
     * Tools that provide a handle for controlling the content that the tool can edit can
     * use this property to alter the radius that a circular handle should have on screen.
     * @param handleSize the radius in pixels.
     */
    void setHandleRadius(int handleSize);
    /// Returns the actual handle radius
    int handleRadius() const;

    /**
     * Tools that are used to grab handles or similar with the mouse
     * should use this value to determine if the mouse is near enough
     * @param grabSensitivity the grab sensitivity in pixels
     */
    void setGrabSensitivity(int grabSensitivity);
    /// Returns the actual grab sensitivity
    int grabSensitivity() const;

    /**
     * Offset used for pasting shapes to a document.
     */
    void setPasteOffset(qreal pasteOffset);
    /// Returns the current paste offset
    qreal pasteOffset() const;

    /**
     * Enables/disables pasting shape at cursor position
     */
    void enablePasteAtCursor(bool enable);
    /// Returns current state of paste at cursor setting
    bool pasteAtCursor() const;

    KUndo2Stack *undoStack() const;
    void setUndoStack(KUndo2Stack *undoStack);

    KoImageCollection *imageCollection() const;
    void setImageCollection(KoImageCollection *ic);

    KoDocumentBase *odfDocument() const;
    void setOdfDocument(KoDocumentBase *currentDocument);

    KoShapeController *shapeController() const;
    void setShapeController(KoShapeController *shapeController);

Q_SIGNALS:
    /**
     * This signal is emitted every time a resource is set that is either
     * new or different from the previous set value.
     * @param key the identifying key for the resource
     * @param value the variants new value.
     * @see KoDocumentResourceManager::DocumentResource
     */
    void resourceChanged(int key, const QVariant &value);

private:
    KoDocumentResourceManager(const KoDocumentResourceManager &);
    KoDocumentResourceManager &operator=(const KoDocumentResourceManager &);

    class Private;
    Private *const d;
};

#endif
