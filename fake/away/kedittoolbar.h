#ifndef FAKE_KEDITTOOLBAR_H
#define FAKE_KEDITTOOLBAR_H

#include <kdialog.h>
#include <kxmlguifactory.h>

class KEditToolBar : public KDialog
{
public:
    KEditToolBar(KXMLGUIFactory *factory, QWidget *parent) : KDialog(parent) {}
};

#endif
