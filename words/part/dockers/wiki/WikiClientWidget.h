#ifndef WIKICLIENTWIDGET_H
#define WIKICLIENTWIDGET_H

#include "ui_WikiClientWidget.h"

#include <QWidget>
#include <KWCanvas.h>

class WikiClientWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit WikiClientWidget(QWidget *parent = 0);
    ~WikiClientWidget();

    enum LayoutDirection {
        LayoutVertical,
        LayoutHorizontal
    };

    void setLayoutDirection(LayoutDirection direction);

private slots:
    void clientLogin();

    
private:
    Ui::WikiClientWidget widget;
    class Private;
    Private * const d;
};

#endif // WIKICLIENTWIDGET_H
