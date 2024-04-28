/* This file is part of the KDE project
SPDX-FileCopyrightText: 2004-2006 David Faure <faure@kde.org>
SPDX-FileCopyrightText: 2007-2008 Thorsten Zachmann <zachmann@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KOTEXTSHAREDSAVINGDATA_H
#define KOTEXTSHAREDSAVINGDATA_H

#include "kotext_export.h"
#include <KoSharedSavingData.h>

#include <QMap>
#include <QSharedPointer>

#define KOTEXT_SHARED_SAVING_ID "KoTextSharedSavingId"

class KoGenChanges;

namespace Soprano
{
class Model;
}

class QString;

class KOTEXT_EXPORT KoTextSharedSavingData : public KoSharedSavingData
{
public:
    KoTextSharedSavingData();
    ~KoTextSharedSavingData() override;

    void setGenChanges(KoGenChanges &changes);

    KoGenChanges &genChanges() const;

    void addRdfIdMapping(const QString &oldid, const QString &newid);
    QMap<QString, QString> getRdfIdMapping() const;

    /**
     * The Rdf Model ownership is not taken, you must still delete it,
     * and you need to ensure that it lives longer than this object
     * unless you reset the model to 0.
     */
#ifdef SHOULD_BUILD_RDF
    void setRdfModel(QSharedPointer<Soprano::Model> m);
    QSharedPointer<Soprano::Model> rdfModel() const;
#endif

    /**
     * Stores the name that written to the file for the style
     *
     * @param styleId the id of the style in KoStyleManger
     * @param savedName the name that is written to the file
     */
    void setStyleName(int styleId, const QString &name);

    /**
     * Style name of the style
     *
     * @param styleId the id of the style in KoStyleManager
     * @return the saved name of the style
     */
    QString styleName(int styleId) const;

    /**
     * @brief styleNames List of all names of the styles that are saved
     * @return All the names of styles that are saved in the style manager
     */
    QList<QString> styleNames() const;

private:
    class Private;
    QScopedPointer<Private> d;
};

#endif // KOTEXTSHAREDSAVINGDATA_H
