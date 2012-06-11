/* This file is part of the KDE project
   Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>

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

#ifndef KPRSHAPEANIMATIONDOCKER_H
#define KPRSHAPEANIMATIONDOCKER_H

#include <QWidget>

class QListWidget;
class QToolButton;
class KoPAViewBase;
class KPrView;
class KPrAnimationsTimeLineView;
class KPrAnimationsDataModel;
class QModelIndex;
class KPrViewModePreviewShapeAnimations;

class KPrShapeAnimationDocker : public QWidget
{
    Q_OBJECT
public:
    explicit KPrShapeAnimationDocker(QWidget *parent = 0);
    void setView(KoPAViewBase *view);
    
signals:
    
public slots:
    void slotActivePageChanged();
    void changeSelection(const QModelIndex &index);
    void changeAnimationSelection();
    void slotAnimationPreview();

private:
    KPrView* m_view;
    KPrAnimationsTimeLineView * m_animationsTimeLineView;
    KPrAnimationsDataModel *m_animationsModel;
    QToolButton *m_editAnimation;
    QToolButton *m_buttonAddAnimation;
    QToolButton *m_buttonRemoveAnimation;
    QToolButton *m_buttonAnimationOrderUp;
    QToolButton *m_buttonAnimationOrderDown;
    QToolButton *m_buttonPreviewAnimation;
    KPrViewModePreviewShapeAnimations *m_previewMode;
    
};

#endif // KPRSHAPEANIMATIONDOCKER_H
