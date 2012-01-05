#include "quickpopupmenu.h"
#include "ui_quickpopupmenu.h"

#include<QMenu>
#include<QWidget>
class QMenu;
class QWidget;

QuickPopupMenu::QuickPopupMenu(QToolButton *button, QWidget * parent) :
    QMenu(parent)

{
    QWidget *popup = new QWidget(this);

}


QSize QuickPopupMenu::sizeHint() const
{
    return layout()->sizeHint();
}
