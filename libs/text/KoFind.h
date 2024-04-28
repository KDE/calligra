/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOFIND_H
#define KOFIND_H

#include "kotext_export.h"

#include <QObject>

class QTextDocument;
class KoCanvasResourceManager;
class KActionCollection;
class KoFindPrivate;

/**
 * This controller class allows you to get the relevant find actions
 * added to your action collection and make them act similarly for all Calligra apps.
 */
class KOTEXT_EXPORT KoFind : public QObject
{
    Q_OBJECT
public:
    /**
     * Constructor for the KoFind controller.
     * You can create and forget this class in order to gain find features for your application.
     * @param parent the parent widget, used both as an anchor for the find dialog and for
     *   memory management purposes.
     * @param canvasResourceManager the resource provider for the canvas, used to signal the text shape.
     * @param ac the action collection that the find actions can be added to.
     */
    KoFind(QWidget *parent, KoCanvasResourceManager *canvasResourceManager, KActionCollection *ac);
    /// destructor
    ~KoFind() override;

Q_SIGNALS:
    /**
     * @brief This signal is sent when the current document has reached its end
     *
     * Connect to this signal if you want to support find in multiple text shapes.
     * In you code you then should select the next text shape and select the text
     * tool.
     *
     * @param document The currently document where find was used.
     */
    void findDocumentSetNext(QTextDocument *document);

    /**
     * @brief This signal is sent when the current document has reached its beginning
     *
     * Connect to this signal if you want to support find in multiple text shapes.
     * In you code you then should select the next text shape and select the text
     * tool.
     *
     * @param document The currently document where find was used.
     */
    void findDocumentSetPrevious(QTextDocument *document);

private:
    KoFindPrivate *const d;
    friend class KoFindPrivate;

    Q_PRIVATE_SLOT(d, void resourceChanged(int, const QVariant &))
    Q_PRIVATE_SLOT(d, void findActivated())
    Q_PRIVATE_SLOT(d, void findNextActivated())
    Q_PRIVATE_SLOT(d, void findPreviousActivated())
    Q_PRIVATE_SLOT(d, void replaceActivated())
    Q_PRIVATE_SLOT(d, void startFind())
    Q_PRIVATE_SLOT(d, void startReplace())
};

#endif
