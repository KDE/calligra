/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXISIMPLEPRINTINGPART_H
#define KEXISIMPLEPRINTINGPART_H

#include <KexiWindow.h>
#include <kexistaticpart.h>
#include <kexipartitem.h>


//! @short Internal Kexi Simple Printing Plugin.
class KexiSimplePrintingPart : public KexiPart::StaticPart
{
    Q_OBJECT

public:
    KexiSimplePrintingPart();
    virtual ~KexiSimplePrintingPart();

protected:
    virtual KexiView* createView(QWidget *parent, KexiWindow* window,
                                 KexiPart::Item &item, Kexi::ViewMode viewMode, QMap<QString, QVariant>* staticObjectArgs);
};

#endif

