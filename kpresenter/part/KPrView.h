/* This file is part of the KDE project
   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KPRVIEW_H
#define KPRVIEW_H

#include <QObject>

#include <KoPAView.h>

class KPrDocument;
class KPrViewModePresentation;

class KPrView : public KoPAView
{
    Q_OBJECT
public:
    explicit KPrView( KPrDocument * document, QWidget * parent = 0 );
    ~KPrView();

protected:    
    void initGUI();
    void initActions();

protected slots:
    // this is just a test method for changing to presentation mode
    // this will be removed again. do not use otherwise
    void changeViewMode( bool mode );

private:
    KToggleAction *m_actionViewMode;
    KPrViewModePresentation *m_presentationMode;
    KoPAViewMode *m_normalMode;
};

#endif /* KPRVIEW_H */
