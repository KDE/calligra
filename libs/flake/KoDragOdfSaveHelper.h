/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KODRAGODFSAVEHELPER_H
#define KODRAGODFSAVEHELPER_H

#include "flake_export.h"

#include <QtGlobal>

class KoShapeSavingContext;
class KoXmlWriter;
class KoGenStyles;
class KoEmbeddedDocumentSaver;
class KoDragOdfSaveHelperPrivate;

class FLAKE_EXPORT KoDragOdfSaveHelper
{
public:
    KoDragOdfSaveHelper();
    virtual ~KoDragOdfSaveHelper();

    /**
     * Create and return the context used for saving
     *
     * If you need a special context for saving you can reimplent this function.
     * The default implementation return a KoShapeSavingContext.
     *
     * The returned context is valid as long as the KoDragOdfSaveHelper is existing
     */
    virtual KoShapeSavingContext *context(KoXmlWriter *bodyWriter, KoGenStyles &mainStyles, KoEmbeddedDocumentSaver &embeddedSaver);

    /**
     * This method is called for writing the body of odf document.
     *
     * You need to have created a context before calling this function
     */
    virtual bool writeBody() = 0;

protected:
    /// constructor
    KoDragOdfSaveHelper(KoDragOdfSaveHelperPrivate &);

    KoDragOdfSaveHelperPrivate *d_ptr;

private:
    Q_DECLARE_PRIVATE(KoDragOdfSaveHelper)
};

#endif /* KODRAGODFSAVEHELPER_H */
