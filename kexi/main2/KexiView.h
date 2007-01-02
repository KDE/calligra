/* This file is part of the KDE project

   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005-2006 Sebastian Sauer <mail@dipe.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIVIEW_H
#define KEXIVIEW_H

#include <QWidget>
//#include <QList>
//#include <QPoint>
//#include <QString>
//#include <QStringList>

//#include <kprinter.h>
//#include <kdeprint/kprintdialogpage.h>
//#include <QPointF>
#include <KoView.h>
//#include <KoZoomMode.h>

class KexiDocument;

class KDE_EXPORT KexiView : public KoView
{
        Q_OBJECT
    public:

        /**
        * Creates a new view.
        */
        KexiView(QWidget* parent, KexiDocument* document);

        /**
        * Destroys the view.
        */
        ~KexiView();

        /**
        * Return the \a KexiDocument instance.
        */
        KexiDocument* doc() const;

        /**
        * Update the readwrite/readonly state.
        * Implement abstract \a KoView::updateReadWrite method.
        */
        virtual void updateReadWrite(bool readwrite);

    private:
        class Private;
        Private* const d;
};

#endif
