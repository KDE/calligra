/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2000-2005 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 2002 Phillip Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
   SPDX-FileCopyrightText: 1999-2000 Simon Hausmann <hausmann@kde.org>
   SPDX-FileCopyrightText: 1999 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_PART
#define CALLIGRA_SHEETS_PART

#include <KoPart.h>

#include "sheets_part_export.h"

class QWidget;
class QGraphicsItem;

namespace Calligra
{
namespace Sheets
{
class Doc;

class CALLIGRA_SHEETS_PART_EXPORT Part : public KoPart
{
    Q_OBJECT

public:
    explicit Part(QObject *parent = nullptr);

    /**
     * Destroys the document.
     */
    ~Part() override;

    void setDocument(Doc *document);

    void addView(KoView *_view, KoDocument *document) override; /// reimplemented

    QGraphicsItem *createCanvasItem(KoDocument *document) override; /// reimplemented

    KoMainWindow *createMainWindow() override; /// reimplemented

protected:
    KoView *createViewInstance(KoDocument *document, QWidget *) override; /// reimplemented

    void openTemplate(const QUrl &url) override; /// reimplemented

    Doc *m_document;
};

} // namespace Sheets
} // namespace Calligra

#endif /* CALLIGRA_SHEETS_PART */
