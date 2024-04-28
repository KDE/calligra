/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPICTURESIMPORT_H
#define KPRPICTURESIMPORT_H

#include <QList>
#include <QObject>

class KoShapeFactoryBase;
class KoPAPageBase;
class KoPAMasterPage;
class KPrDocument;
class KPrView;
class KJob;
class KUndo2Command;
class QUrl;

class KPrPicturesImport : public QObject
{
    Q_OBJECT
public:
    KPrPicturesImport();

    void import(KPrView *view);

private Q_SLOTS:
    // starts the transfer of the next image
    void import();
    void pictureImported(KJob *job);

private:
    KPrDocument *m_doc;
    QList<QUrl> m_urls;
    KoPAPageBase *m_currentPage;
    KoPAMasterPage *m_masterPage;
    KoShapeFactoryBase *m_factory;
    KUndo2Command *m_cmd;
};

#endif /* KPRPICTURESIMPORT_H */
