#include "quickpopupmenu.h"
#include "ui_quickpopupmenu.h"

#include <QMenu>

QuickPopupMenu::QuickPopupMenu(QToolButton *button, QWidget *parent)
    : QMenu(parent)
{
    Ui::QuickPopupMenu *w = new Ui::QuickPopupMenu();

    w->setupUi(this);
    connect(w->checkBox, SIGNAL(stateChanged(int)), this, SIGNAL(wordsDisplayChange(int)));
}


QSize QuickPopupMenu::sizeHint() const
{
    return layout()->sizeHint();
}
