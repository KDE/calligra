#ifndef COVERPREVIEWTOOL_H
#define COVERPREVIEWTOOL_H

#include "CAuView.h"
#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QLabel>

class CoverPreviewTool : public QMainWindow
{
    Q_OBJECT
public:
    explicit CoverPreviewTool(QWidget *parent = 0);
    void setCAuView(CAuView* au);

private slots:
    void open();

private:
    void refresh();
    void createActions();
    void createMenus();
    void updateActions();

    QLabel *imageField;
    CAuView *view;
    QPair<QString, QByteArray> img;

    QAction *openAct;
    QAction *exitAct;

    QMenu *fileMenu;
};

#endif // COVERPREVIEWTOOL_H
