/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2000 Simon Hausmann <hausmann@kde.org>

   $Id$

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <KoDocumentInfoPropsPage.h>

#include <KPluginFactory>

static QObject *createDocInfoPropsPage(QWidget *w, QObject *parent, const KPluginMetaData &data, const QVariantList &args)
{
    Q_UNUSED(w);
    KPropertiesDialog *props = qobject_cast<KPropertiesDialog *>(parent);
    Q_ASSERT(props);
    return new KoDocumentInfoPropsPage(props, args);
}

K_PLUGIN_FACTORY_WITH_JSON(PropsDlgFactory, "calligradocinfopropspage.json", registerPlugin<KoDocumentInfoPropsPage>(createDocInfoPropsPage);)

#include <KoDocInfoPropsFactory.moc>
