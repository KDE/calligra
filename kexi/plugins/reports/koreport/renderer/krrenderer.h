/*
   KOffice Reporting Library
   Copyright (C) 2010 by Adam Pigg (adam@piggz.co.uk)
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KRRENDERER_H
#define KRRENDERER_H
#include "koreport_export.h"
#include <KUrl>

class QPainter;
class QPrinter;
class ORODocument;

class KOREPORT_EXPORT KoReportRendererContext
{
    public:
        KoReportRendererContext();
        KUrl saveFileName;
        QPainter *painter;
        QPrinter *printer;
};

class KOREPORT_EXPORT KRRenderer
{
    public:
        KRRenderer();
        //!Render the page of the given document within the given context.
        //!If page = 0, render the entire document
        virtual bool render(KoReportRendererContext, ORODocument *, int page = -1) = 0;

};

class KOREPORT_EXPORT KoReportRendererFactory
{
    public:
        static KRRenderer* createInstance(const QString &className);
};



#endif // KRRENDERER_H
