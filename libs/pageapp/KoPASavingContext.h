/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOPASAVINGCONTEXT_H
#define KOPASAVINGCONTEXT_H

#include <QMap>
#include <QSet>
#include <QString>

#include <KoShapeSavingContext.h>

#include "kopageapp_export.h"

class KoPAMasterPage;
class KoPAPage;

/**
 * Context needed for saving the data of a kopageapp.
 */
class KOPAGEAPP_EXPORT KoPASavingContext : public KoShapeSavingContext
{
public:
    /**
     * @brief Constructor
     *
     * @param xmlWriter used for writing the data to
     * @param mainStyles the saving styles
     * @param embeddedSaver for saving embedded documents
     * @param page the starting page number
     */
    KoPASavingContext(KoXmlWriter &xmlWriter, KoGenStyles &mainStyles, KoEmbeddedDocumentSaver &embeddedSaver, int page); // TODO default

    /**
     * @brief Destructor
     */
    ~KoPASavingContext() override;

    /**
     * @brief Add style name of master to lookup table
     *
     * @param masterPage the master page
     * @param name the style name of the master page
     */
    void addMasterPage(const KoPAMasterPage *masterPage, const QString &name);

    /**
     * @brief Get the name of the masterpage
     *
     * @param masterPage for which the style name should be returned
     *
     * @return the style name of the masterPage
     */
    QString masterPageName(const KoPAMasterPage *masterPage) const;

    /**
     * @brief Get the element name use in saving
     *
     * The ement name is used for saving the content of the master page style.
     * This makes it possible to save 2 master pages with the same content when
     * the element name is different.
     *
     * If KoShapeSavingContext::UniqueMasterPages is set duplicate master pages
     * will be merged into one.
     *
     * @return element name
     */
    QString masterPageElementName();

    /**
     * @brief Increment the page
     */
    void incrementPage();

    /**
     * @brief Get the current page number
     *
     * The page number starts at 1
     */
    int page();

    /**
     * @brief Set the clearDrawIds flag
     *
     * @see KoPAPastePage::process
     */
    void setClearDrawIds(bool clear);

    /**
     * @brief Get the clearDrawIds flag
     *
     * @see KoPAPastePage::process
     */
    bool isSetClearDrawIds();

    /**
     * @brief get the draw:name of the page to use
     */
    QString pageName(const KoPAPage *page);

private:
    QMap<const KoPAMasterPage *, QString> m_masterPageNames;
    // TODO use a boost::multi_index_container
    QMap<const KoPAPage *, QString> m_pageToNames;
    QSet<QString> m_pageNames;
    int m_page;
    int m_masterPageIndex;
    bool m_clearDrawIds;
};

#endif /* KOPASAVINGCONTEXT_H */
