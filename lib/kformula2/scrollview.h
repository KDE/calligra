
#ifndef __SCROLLVIEW_H
#define __SCROLLVIEW_H

#include <qscrollview.h>

class KFormulaWidget;


class ScrollView : public QScrollView {
    Q_OBJECT
public:
    ScrollView();

    virtual void addChild(KFormulaWidget* c, int x=0, int y=0);
    
protected:
    virtual void focusInEvent(QFocusEvent* event);

protected slots:

    void cursorChanged(bool visible, bool selecting);
    
private:
    KFormulaWidget* child;
};

#endif // __SCROLLVIEW_H
