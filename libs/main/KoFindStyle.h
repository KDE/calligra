/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOFINDSTYLE_H
#define KOFINDSTYLE_H

#include "KoFindBase.h"

#include <QMetaType>
#include <QTextCursor>
#include <QTextDocument>

#include "komain_export.h"

/**
 * \brief Implementation of KoFindBase that can find style usage.
 *
 * This class will search through a set of QTextDocuments for uses of  a paragraph
 * and character style. It will highlight the character style.
 *
 * The following options are required to be set before it works:
 * <ul>
 *      <li><strong>characterStyle</strong>: Int. Default undefined. The ID of a
 *              character style to search for.</li>
 *      <li><strong>paragraphStyle</strong>: Int. Default undefined. The ID of a
 *              paragraph style to search for.</li>
 * </ul>
 *
 * \note Before you can use this class, be sure to set a list of QTextDocuments
 * using setDocuments().
 *
 * Matches created by this implementation use QTextDocument for the container and
 * QTextCursor for the location.
 */
class KOMAIN_EXPORT KoFindStyle : public KoFindBase
{
public:
    /**
     * Constructor.
     */
    explicit KoFindStyle(QObject *parent = nullptr);
    /**
     * Destructor.
     */
    ~KoFindStyle() override;

    /**
     * Return the list of documents currently being used for searching.
     */
    QList<QTextDocument *> documents() const;
    /**
     * Set the list of documents to use for searching.
     *
     * \param list A list of document to search through.
     */
    void setDocuments(const QList<QTextDocument *> &list);

    /**
     * Reimplemented from KoFindBase::clearMatches()
     */
    void clearMatches() override;

protected:
    /**
     * Reimplemented from KoFindBase::replaceImplementation().
     *
     * \note Replace is currently not supported in this class.
     */
    void replaceImplementation(const KoFindMatch &match, const QVariant &value) override;
    /**
     * Reimplemented from KoFindBase::findImplementation()
     */
    void findImplementation(const QString &pattern, KoFindBase::KoFindMatchList &matchList) override;

private:
    class Private;
    Private *const d;
};

#endif // KOFINDSTYLE_H
