/* This file is part of the KDE project
   Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/
#ifndef KPRPAGEAPPLICATIONDATA_H
#define KPRPAGEAPPLICATIONDATA_H

#include "stage_export.h"

#include <KoShapeApplicationData.h>

class KPrPageEffect;

class STAGE_EXPORT KPrPageApplicationData : public KoShapeApplicationData
{
public:
   KPrPageApplicationData();
   ~KPrPageApplicationData();

   /**
    * Get the page effect used to go to this page
    */
   KPrPageEffect * pageEffect();

   /**
    * Set the page effect used for this page
    */
   void setPageEffect( KPrPageEffect * effect );

private:
   KPrPageEffect * m_pageEffect;
};

#endif // KPRPAGEAPPLICATIONDATA_H
