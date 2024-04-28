/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Peter Simonsson <peter.simonsson@gmail.com>
 * SPDX-FileCopyrightText: 2007-2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOPALOADINGCONTEXT_H
#define KOPALOADINGCONTEXT_H

#include <KoShapeLoadingContext.h>

#include <QMap>

#include "kopageapp_export.h"

class KoPAMasterPage;
class KoPAPage;
class KoDocumentResourceManager;

/// Context needed for loading the data of a pageapp
class KOPAGEAPP_EXPORT KoPALoadingContext : public KoShapeLoadingContext
{
public:
    /**
     * Constructor.
     *
     * @param context Context for loading oasis docs.
     * @param context The shape controller.
     */
    KoPALoadingContext(KoOdfLoadingContext &context, KoDocumentResourceManager *documentResources);
    ~KoPALoadingContext();

    /**
     * Get the master page with the name @p name.
     *
     * @param name name of the master page.
     */
    KoPAMasterPage *masterPageByName(const QString &name);

    /**
     * Add a master page to the context.
     *
     * @param name name of the master page.
     * @param master master page to add.
     */
    void addMasterPage(const QString &name, KoPAMasterPage *master);

    /**
     * Get the master pages
     */
    const QMap<QString, KoPAMasterPage *> &masterPages();

    /**
     * Get the page with the name @p name.
     *
     * @param name name of the page.
     */
    KoPAPage *pageByName(const QString &name);

    /**
     * Add a master page to the context.
     *
     * @param name name of the page.
     * @param page page to add.
     */
    void addPage(const QString &name, KoPAPage *page);

private:
    class Private;
    Private *const d;
};

#endif /*KOPALOADINGCONTEXT_H*/
