#ifndef QUICKPOPUPMENU_H
#define QUICKPOPUPMENU_H
#include <QWidget>
#include<QToolButton>
#include<QMenu>

class QMenu;
class QToolButton;
class QWidget;


namespace Ui {
    class QuickPopupMenu;
}

class QuickPopupMenu : public QMenu
{
    Q_OBJECT
public:
    QuickPopupMenu(QToolButton *button, QWidget * parent = 0);
    virtual QSize sizeHint() const;

signals:
    void wordsDisplayChange(int);
};

#endif // QUICKPOPUPMENU_H

