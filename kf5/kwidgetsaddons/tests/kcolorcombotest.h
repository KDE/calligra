#ifndef _KCOLORCOMBOTEST_H
#define _KCOLORCOMBOTEST_H

#include <QWidget>

class QPushButton;
class KColorCombo;

class KColorComboTest : public QWidget
{
    Q_OBJECT

public:
    KColorComboTest(QWidget *parent = 0);
    ~KColorComboTest();

private Q_SLOTS:
    void quitApp();

protected:
    KColorCombo* mStandard;
    KColorCombo* mCustom;
    QPushButton* mExit;
};

#endif
