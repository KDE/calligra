#ifndef KSCRIPTMENU_H
#define KSCRIPTMENU_H

#include <kaction.h>
#include <qlist.h>
#include <qptrdict.h>
#include <qstring.h>

#include <dcopref.h>

class KInstance;
class KSInterpreter;

class KScriptMenu : public KActionMenu
{
    Q_OBJECT
public:
    KScriptMenu( const DCOPRef& ref, KInstance* instance, const QString& text, QObject* parent = 0, const char* name = 0 );
    ~KScriptMenu();
    
protected slots:
    void slotActivated();

private:
    QList<KAction> m_actions;
    QPtrDict<QString> m_filenames;
    DCOPRef m_ref;
    KInstance* m_instance;
    KSInterpreter* m_interpreter;
};

#endif
