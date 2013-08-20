#ifndef ktreewidgetsearchlinetest_h
#define ktreewidgetsearchlinetest_h

#include <QDialog>

class KTreeWidgetSearchLineTest : public QDialog
{
    Q_OBJECT

public:
    KTreeWidgetSearchLineTest();

    void create2ndLevel(class QTreeWidgetItem* item);
    void create3rdLevel(QTreeWidgetItem* item);

public Q_SLOTS:
    void switchCaseSensitivity(bool cs);

protected:
    virtual void showEvent(QShowEvent* event);

private:
    class KTreeWidgetSearchLine* m_searchLine;
    class QTreeWidget* tw;
};

#endif
