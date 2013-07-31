#ifndef FAKE_kassistantdialog_H
#define FAKE_kassistantdialog_H

#include <kpagedialog.h>

class KAssistantDialog : public KPageDialog
{
public:
    KAssistantDialog(QWidget *parent=0, Qt::WFlags flags=0) : KPageDialog(parent) {}
    virtual ~KAssistantDialog() {}
};

#endif
