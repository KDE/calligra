#ifndef PLUGIN_H
#define PLUGIN_H

#include <qobject.h>
#include <qaction.h>

class Plugin : public QObject
{
    Q_OBJECT
public:
    Plugin( QObject* parent = 0, const char* name = 0 );
    ~Plugin();
    
    QAction* action( const char* name );
    QActionCollection* actionCollection();
    
private:
    QActionCollection m_collection;
};

#endif
