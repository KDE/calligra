#ifndef KSCRIPTMENU_H
#define KSCRIPTMENU_H

#include <qaction.h>
#include <qlist.h>
#include <qptrdict.h>
#include <qstring.h>

#include <dcopref.h>

class KInstance;
class KSInterpreter;

class KScriptMenu : public QActionMenu
{
    Q_OBJECT
public:
    KScriptMenu( const DCOPRef& ref, KInstance* instance, const QString& text, QObject* parent = 0, const char* name = 0 );
    ~KScriptMenu();
    
protected slots:
    void slotActivated();

private:
    QList<QAction> m_actions;
    QPtrDict<QString> m_filenames;
    DCOPRef m_ref;
    KInstance* m_instance;
    KSInterpreter* m_interpreter;
};

#endif
