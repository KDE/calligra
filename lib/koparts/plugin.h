#ifndef PLUGIN_H
#define PLUGIN_H

#include <qobject.h>
#include <qaction.h>

/**
 *  A KParts plugin.
 *
 *  @short generic plugin class for KParts.
 */
class Plugin : public QObject
{
    Q_OBJECT

public:

    /**
     *  Initializes a plugin.
     */
    Plugin( QObject* parent = 0, const char* name = 0 );

    /**
     *  Destructor.
     */
    ~Plugin();
    
    /**
     *  Retrieves an action with a given name.
     *
     *  @param name Name of the action.
     *
     *  @return The action, may be NULL.
     *
     *  @see actionCollection
     */
    QAction* action( const char* name );

    /**
     *  Retrieves the action collection of this plugin.
     *
     *  @see action
     */
    QActionCollection* actionCollection();
    
private:

    QActionCollection m_collection;
};

#endif
