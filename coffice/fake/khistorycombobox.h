#ifndef FAKE_khistorycombobox_H
#define FAKE_khistorycombobox_H

#include <kcombobox.h>

class KHistoryComboBox : public KComboBox
{
public:
    KHistoryComboBox(QWidget *parent = 0) : KComboBox(parent) {}
    KHistoryComboBox(bool, QWidget *parent = 0) : KComboBox(parent) {}
    void setCompletedItems(QStringList) {}
    void setHistoryItems(QStringList) {}
    void addToHistory(QString) {}
};

#endif
