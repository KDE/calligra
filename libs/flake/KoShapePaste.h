/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOSHAPEPASTE_H
#define KOSHAPEPASTE_H

#include <KoOdfPaste.h>
#include "flake_export.h"

#include <QList>

class KoCanvasBase;
class KoShapeLayer;
class KoShape;

/**
 * Class for pasting shapes to the document
 */
class FLAKE_EXPORT KoShapePaste : public KoOdfPaste
{
public:
    /**
     * Constructor
     *
     * @param canvas The canvas on which the paste is done
     * @param parentLayer The layer on which the shapes will be pasted
     */
    KoShapePaste(KoCanvasBase *canvas, KoShapeLayer *parentLayer);
    ~KoShapePaste() override;

    QList<KoShape*> pastedShapes() const;

protected:
    /// reimplemented
    bool process(const KoXmlElement & body, KoOdfReadStore &odfStore) override;

    class Private;
    Private * const d;
};

#endif /* KOSHAPEPASTE_H */
