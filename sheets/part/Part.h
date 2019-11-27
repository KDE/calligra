/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2007 Thorsten Zachmann <zachmann@kde.org>
   Copyright 2004 Ariya Hidayat <ariya@kde.org>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2000-2005 Laurent Montel <montel@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Phillip Mueller <philipp.mueller@gmx.de>
   Copyright 2000 Werner Trobin <trobin@kde.org>
   Copyright 1999-2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 David Faure <faure@kde.org>
   Copyright 1998-2000 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_SHEETS_PART
#define CALLIGRA_SHEETS_PART

#include <KoDocument.h>
#include <KoPart.h>

#include "sheets_common_export.h"

class QWidget;
class QGraphicsItem;

namespace Calligra
{
namespace Sheets
{
class Doc;

class CALLIGRA_SHEETS_COMMON_EXPORT Part : public KoPart
{
    Q_OBJECT

public:
    explicit Part(QObject *parent = 0);

    /**
     * Destroys the document.
     */
    ~Part() override;

    void setDocument(Doc *document);

    void addView(KoView *_view, KoDocument *document) override; /// reimplemented

    QGraphicsItem *createCanvasItem(KoDocument *document) override; /// reimplemented

    KoMainWindow *createMainWindow() override; /// reimplemented

protected:
    KoView* createViewInstance(KoDocument *document, QWidget*) override; /// reimplemented

    void openTemplate(const QUrl& url) override; /// reimplemented

    Doc *m_document;

};

} // namespace Sheets
} // namespace Calligra

#endif /* CALLIGRA_SHEETS_PART */
