/* This file is part of the KDE project
   Copyright (C) 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

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
#ifndef COMBINEDVIEW_H
#define COMBINEDVIEW_H

#include <QWidget>

class SlideView;
class DirSlideLoader;
class KPresenterSlideLoader;
class OoThread;
class QGridLayout;

class CombinedView : public QWidget {
private:
Q_OBJECT
    QList<SlideView*> slideViews;
    DirSlideLoader * const ooodploader;
    KPresenterSlideLoader * const koodploader;
    DirSlideLoader * const oopptloader;
    KPresenterSlideLoader * const kopptloader;
    SlideView * const ooodpview;
    SlideView * const koodpview;
    SlideView * const oopptview;
    SlideView * const kopptview;
    OoThread * const oothread;
    QGridLayout * const layout;
    QString ooodpresult;
    QString nextodpfile;

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void addSlideView(SlideView* slideview);
private Q_SLOTS:
    void slotSetView(qreal zoomFactor, int h, int v);
    void slotHandleOoOdp(const QString& path);
    void slotHandleOoPng(const QString& path);
public:
    explicit CombinedView(QWidget* parent=0);
    ~CombinedView();
    void openFile(const QString& path);
};

#endif
