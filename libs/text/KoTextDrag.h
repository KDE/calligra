/* This file is part of the KDE project
 * Copyright ( C ) 2007 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTDRAG_H
#define KOTEXTDRAG_H

#include "kotext_export.h"

class QMimeData;
class QString;
class QByteArray;
class KoTextOdfSaveHelper;

/**
 * Class for simplifying adding a odf text with tracked changes to the clip board
 *
 * For saving the odf a KoDragOdfSaveHelper class is used.
 * It implements the writing of the body of the document. The
 * setOdf takes care of saving styles and tracked changes and all the other
 * common stuff.
 */
class KOTEXT_EXPORT KoTextDrag
{
public:
    KoTextDrag();
    ~KoTextDrag();

    /**
     * Set odf mime type
     *
     * This calls helper.writeBody();
     *
     * @param mimeType used for creating the odf document
     * @param helper helper for saving the body of the odf document
     */
    bool setOdf(const char *mimeType, KoTextOdfSaveHelper &helper);

    /**
     * Add additional mimeTypes
     */
    void setData(const QString &mimeType, const QByteArray &data);

    /**
     * Get the mime data
     *
     * This transfers the ownership of the mimeData to the caller
     */
    QMimeData *takeMimeData();

private:
    QMimeData *m_mimeData;
};

#endif /* KOTEXTDRAG_H */
