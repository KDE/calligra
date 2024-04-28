/* This file is part of the KDE project
 *  SPDX-FileCopyrightText: 2009 Nokia Corporation and /or its subsidiary(-ies).
 *  Contact: Amit Aggarwal <amitcs06@gmail.com>
 *            <amit.5.aggarwal@nokia.com>
 *  SPDX-FileCopyrightText: 2010 Thorsten Zachmann <t.zachmann@zagge.de>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KPRDECLARATIONS_H
#define KPRDECLARATIONS_H

#include <QHash>
#include <QString>
#include <QVariant>

class KoXmlElement;
class KoPALoadingContext;
class KoPASavingContext;

class KPrDeclarations
{
public:
    /**
     * Presentation declaration type
     */
    enum Type { Footer, Header, DateTime };

    /**
     * Constructor
     */
    KPrDeclarations();

    /**
     * Destructor
     */
    ~KPrDeclarations();

    /**
     * loadOdfDeclaration
     * @param body KoXmlElement & body
     * @param context KoPALoadingContext
     * @return bool value
     */
    bool loadOdf(const KoXmlElement &body, KoPALoadingContext &context);

    /**
     * Save presentation:declaration entries
     */
    bool saveOdf(KoPASavingContext &paContext) const;

    /**
     * Similar to findStyle but for decls only.
     * \note Searches in content.xml added declaration!
     */
    const QString declaration(Type type, const QString &key);

private:
    QHash<Type, QHash<QString /*key*/, QVariant /*data*/>> m_declarations;
};

#endif /* KPRDECLARATIONS_H */
