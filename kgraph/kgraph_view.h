#ifndef KGRAPH_VIEW_H
#define KGRAPH_VIEW_H

#include <koView.h>

class QPaintEvent;

//class KAction;
class KGraphPart;


class KGraphView : public KoView {

    Q_OBJECT

public:
    KGraphView(KGraphPart *part, QWidget *parent=0, const char *name=0);

//protected slots:
    //void a_editcut();

protected:
    void paintEvent(QPaintEvent *ev);

    virtual void updateReadWrite(bool readwrite);

private:
    //KAction *editcut;
};
#endif
