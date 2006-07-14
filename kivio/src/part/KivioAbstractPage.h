/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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

#ifndef KIVIOABSTRACTPAGE_H
#define KIVIOABSTRACTPAGE_H

#include <QString>
#include <QList>

#include <KoShapeControllerBase.h>

class KoPageLayout;
class KivioDocument;

/** Base class for KivioPage and KivioMasterPage */
class KivioAbstractPage : public KoShapeControllerBase
{
  public:
    KivioAbstractPage(KivioDocument* document, const QString& title);
    virtual ~KivioAbstractPage();

    KivioDocument* document() const { return m_document; }

    /// Change the page title
    void setTitle(const QString& newTitle);
    /// Get page title
    QString title() const;

    virtual KoPageLayout pageLayout() const = 0;

    virtual void addShape(KoShape* shape);
    virtual void removeShape(KoShape* shape);

  private:
    KivioDocument* m_document;
    QString m_title;
    QList<KoShape*> m_shapeList;
};

#endif
