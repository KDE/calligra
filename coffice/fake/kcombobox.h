#ifndef FAKE_KCOMBOBOX_H
#define FAKE_KCOMBOBOX_H

#include <QComboBox>
#include <QPointer>
#include <kcompletion.h>

class KComboBox : public QComboBox
{
public:
    KComboBox(QWidget *parent = 0) : QComboBox(parent) {}
    KComboBox(bool, QWidget *parent = 0) : QComboBox(parent) {}
    void setCompletionObject(KCompletion *completionList, bool = false) {}
    void setCompletionMode(KGlobalSettings::Completion c) {}
    void setCurrentItem(int) {}
};

#endif
