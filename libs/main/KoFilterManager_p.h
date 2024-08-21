/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Clarence Dang <dang@kde.org>
   SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __koFilterManager_p_h__
#define __koFilterManager_p_h__

#include "KoFilterManager.h"

#include <KoDialog.h>
#include <KoProgressUpdater.h>
#include <QUrl>

#include <QPointer>
#include <QString>
#include <QStringList>

class QListWidget;

class Q_DECL_HIDDEN KoFilterManager::Private
{
public:
    bool batch;
    QByteArray importMimeType;
    QPointer<KoProgressUpdater> progressUpdater;

    Private(KoProgressUpdater *progressUpdater_ = nullptr)
        : progressUpdater(progressUpdater_)
    {
    }
};

class KoFilterChooser : public KoDialog
{
    Q_OBJECT

public:
    KoFilterChooser(QWidget *parent, const QStringList &mimeTypes, const QString &nativeFormat = QString(), const QUrl &url = QUrl());
    ~KoFilterChooser() override;

    QString filterSelected();

private:
    QStringList m_mimeTypes;
    QListWidget *m_filterList;
};

#endif
