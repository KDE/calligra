/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2002 Simon MacMullen <calligra@babysimon.co.uk>
    SPDX-FileCopyrightText: 2012 Friedrich W. H. Kossebau <kossebau@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef _CALLIGRACREATOR_H_
#define _CALLIGRACREATOR_H_

// KF5
#include <KIO/ThumbnailCreator>
// Qt
#include <QEventLoop>

class KoPart;
class KoDocument;

class CalligraCreator : public KIO::ThumbnailCreator
{
    Q_OBJECT
public:
    CalligraCreator(QObject *parent, const QVariantList &args);
    ~CalligraCreator() override;
    KIO::ThumbnailResult create(const KIO::ThumbnailRequest &request) override;

private Q_SLOTS:
    void onLoadingCompleted();

private:
    KoPart *m_part;
    KoDocument *m_doc;

    bool m_loadingCompleted : 1;
    QEventLoop m_eventLoop;
};

#endif
