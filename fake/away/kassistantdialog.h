#ifndef FAKE_kassistantdialog_H
#define FAKE_kassistantdialog_H

#include <kopagedialog.h>

class KAssistantDialog : public KoPageDialog
{
public:
    KAssistantDialog(QWidget *parent=0, Qt::WFlags flags=0) : KoPageDialog(parent) {}
    virtual ~KAssistantDialog() {}
    void setValid(KPageWidgetItem* page, bool enable) {Q_UNUSED(page); Q_UNUSED(enable);}

};

#endif
