#ifndef COVERPREVIEWTOOL_H
#define COVERPREVIEWTOOL_H

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
    void setCurrentImage(QPair<QString, QByteArray> img);

private slots:
    void open();

private:
    void createActions();
    void createMenus();
    void updateActions();

    QLabel *imageField;

    QAction *openAct;
    QAction *exitAct;

    QMenu *fileMenu;
};

#endif // COVERPREVIEWTOOL_H
