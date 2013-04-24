#ifndef SIMPLEPAGESTYLEWIDGET_H
#define SIMPLEPAGESTYLEWIDGET_H

#include <QWidget>

class KWView;
class KWPageTool;
namespace Ui {
class SimplePageStyleWidget;
}

class SimplePageStyleWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SimplePageStyleWidget(KWView* view, KWPageTool* pageTool, QWidget *parent = 0);
    ~SimplePageStyleWidget();
    
private slots:
    void refreshInformations();

    void on_spinTo_valueChanged(int arg1);

    void on_spinFrom_valueChanged(int arg1);

    void on_buttonApply_clicked();

    void on_list_clicked(const int &index);

private:
    KWPageTool* m_pageTool;
    KWView* m_view;
    Ui::SimplePageStyleWidget *ui;
};

#endif // SIMPLEPAGESTYLEWIDGET_H
