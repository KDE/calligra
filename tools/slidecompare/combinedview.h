#ifndef COMBINEDVIEW_H
#define COMBINEDVIEW_H

#include <QtGui/QWidget>

class SlideView;
class DirSlideLoader;
class KPresenterSlideLoader;
class QGridLayout;
class OoThread;

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

    void addSlideView(SlideView* slideview);
private slots:
    void slotSetView(qreal zoomFactor, int h, int v);
    void slotHandleOoOdp(const QString& path);
    void slotHandleOoPng(const QString& path);
public:
    CombinedView(QWidget* parent=0);
    ~CombinedView();
    void openFile(const QString& path);
};

#endif
