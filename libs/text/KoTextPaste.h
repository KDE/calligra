/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTPASTE_H
#define KOTEXTPASTE_H

#include "kotext_export.h"
#include <KoOdfPaste.h>

class KoTextEditor;
class KoShapeController;
class KUndo2Command;
class KoCanvasBase;

#include <QSharedPointer>
namespace Soprano
{
class Model;
}

class KOTEXT_EXPORT KoTextPaste : public KoOdfPaste
{
public:
    /**
     * Note: RdfModel ownership is not taken. You must ensure that it remains
     * valid for the lifetime of the object.
     *
     * @param editor the KoTextEditor the text will be read into
     * @param shapeController the shapecontroller that gives access to the document's shapes and resourcemanager
     * @param rdfModel the rdfModel we'll insert the tuples into
     */
    KoTextPaste(KoTextEditor *editor, KoShapeController *shapeController, QSharedPointer<Soprano::Model> rdfModel, KoCanvasBase *canvas, KUndo2Command *cmd);
    ~KoTextPaste() override;

protected:
    /// reimplemented
    bool process(const KoXmlElement &body, KoOdfReadStore &odfStore) override;

    class Private;
    Private *const d;
};

#endif /* KOTEXTPASTE_H */
