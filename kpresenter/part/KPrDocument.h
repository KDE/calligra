/* This file is part of the KDE project
   Copyright (C) 2006-2007 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KPRDOCUMENT_H
#define KPRDOCUMENT_H

#include <QObject>

#include <KoPADocument.h>
#include "KPrCustomSlideShows.h"

class KPrShapeAnimation;
class KPrShapeAnimations;

class KPrDocument : public KoPADocument
{
    Q_OBJECT
public:
    explicit KPrDocument( QWidget* parentWidget, QObject* parent, bool singleViewMode = false );
    ~KPrDocument();

    /// reimplemented
    virtual KoPAPage * newPage( KoPAMasterPage * masterPage = 0 );
    /// reimplemented
    virtual KoPAMasterPage * newMasterPage();

    /// reimplemented
    virtual KoOdf::DocumentType documentType() const;

    /**
     * @brief Add animation to shape
     *
     * @param animation animation to add to shape
     */
    void addAnimation( KPrShapeAnimation * animation );

    /**
     * @brief Remove animation from shape
     *
     * @param animation animation to remove from shape
     * @param removeFromApplicationData if true the animation will also be removed from the 
     *        application data
     */
    void removeAnimation( KPrShapeAnimation * animation, bool removeFromApplicationData = true );

    /**
     * @brief get the slideShows defined for this document
     */
    KPrCustomSlideShows* customSlideShows();
    void setCustomSlideShows( KPrCustomSlideShows* replacement );
protected:
    /// reimplemented
    virtual KoView * createViewInstance( QWidget *parent );
    /// reimplemented
    virtual const char *odfTagName( bool withNamespace );

    /// reimplemented
    virtual void postAddShape( KoPAPageBase * page, KoShape * shape );
    /// reimplemented
    virtual void postRemoveShape( KoPAPageBase * page, KoShape * shape );

    /**
     * @brief get the animations of the page
     */
    KPrShapeAnimations & animationsByPage( KoPAPageBase * page );
    
    KPrCustomSlideShows *m_customSlideShows;
};

#endif /* KPRDOCUMENT_H */
