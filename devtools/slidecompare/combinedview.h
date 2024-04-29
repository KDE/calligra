/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <jos.van.den.oever@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef COMBINEDVIEW_H
#define COMBINEDVIEW_H

#include <QWidget>

class SlideView;
class DirSlideLoader;
class KPresenterSlideLoader;
class OoThread;
class QGridLayout;

class CombinedView : public QWidget
{
private:
    Q_OBJECT
    QList<SlideView *> slideViews;
    DirSlideLoader *const ooodploader;
    KPresenterSlideLoader *const koodploader;
    DirSlideLoader *const oopptloader;
    KPresenterSlideLoader *const kopptloader;
    SlideView *const ooodpview;
    SlideView *const koodpview;
    SlideView *const oopptview;
    SlideView *const kopptview;
    OoThread *const oothread;
    QGridLayout *const layout;
    QString ooodpresult;
    QString nextodpfile;

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void addSlideView(SlideView *slideview);
private Q_SLOTS:
    void slotSetView(qreal zoomFactor, int h, int v);
    void slotHandleOoOdp(const QString &path);
    void slotHandleOoPng(const QString &path);

public:
    explicit CombinedView(QWidget *parent = nullptr);
    ~CombinedView();
    void openFile(const QString &path);
};

#endif
