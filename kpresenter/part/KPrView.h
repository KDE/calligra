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

#ifndef KPRVIEW_H
#define KPRVIEW_H

#include <QObject>

#include <KoPAView.h>

class KPrDocument;
class KPrViewModeNotes;
class KPrViewModePresentation;
class KPrViewModePresenterView;
class KActionMenu;
class KoPAPageBase;

class KPrView : public KoPAView
{
    Q_OBJECT
public:
    explicit KPrView( KPrDocument * document, QWidget * parent = 0 );
    ~KPrView();

    virtual KoViewConverter * viewConverter();

public slots:
    void updateActivePage(KoPAPageBase *page);

protected:
    void initGUI();
    void initActions();

protected slots:
    void startPresentation();
    void startPresentationFromBeginning();
    void activatePresenterView( KPrView *mainView );
    void createAnimation();
    void showNormal();
    void showNotes();
    void dialogCustomSlideShows();
    void configureSlideShow();

private:
    KActionMenu *m_actionStartPresentation;
    KAction *m_actionCreateAnimation;
    KAction *m_actionViewModeNormal;
    KAction *m_actionViewModeNotes;
    KAction *m_actionCreateCustomSlideShowsDialog;

    KPrViewModePresentation *m_presentationMode;
    KPrViewModePresenterView *m_presenterViewMode;
    KoPAViewMode *m_normalMode;
    KPrViewModeNotes *m_notesMode;
};

#endif /* KPRVIEW_H */
