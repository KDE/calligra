#ifndef __kspread_map_h__
#define __kspread_map_h__

class KSpreadMap;
class KSpreadDoc;

#include <iostream.h>
#include <komlParser.h>

#include <qlist.h>
#include <qstring.h>
#include <qintdict.h>

#include "kspread_table.h"

/**
  A map is a simple container for all tables. Usually a complete map
  is saved in one file.
 */
class KSpreadMap
{
public:
    /**
     * Created an empty map.
     */
    KSpreadMap( KSpreadDoc *_doc );
    /**
     * This deletes all tables contained in this map.
     */
    virtual ~KSpreadMap();

    virtual bool save( ostream& );
    virtual bool load( KOMLParser&, vector<KOMLAttrib>& );
    virtual bool loadChildren( KOStore::Store_ptr _store );
  
    void makeChildList( KOffice::Document_ptr _doc, const char *_path );
    /*
     * @return true if one of the direct children wants to
     *              be saved embedded. If there are no children or if
     *              every direct child saves itself into its own file
     *              then false is returned.
     * 
     */
    bool hasToWriteMultipart();
  
    /**
     * @param _table becomes added to the map.
     */
    void addTable( KSpreadTable *_table );

    /**
     * @param _tables becomes removed from the map. This wont delete the table.
     */
    void removeTable( KSpreadTable *_table );

    KSpreadTable* findTable( const char *_name );
        
    /**
     * Use the @ref #nextTable function to get all the other tables.
     * Attention: Function is not reentrant.
     *
     * @return a pointer to the first table in this map.
     */
    KSpreadTable* firstTable() { return m_lstTables.first();  }

    /**
     * Call @ref #firstTable first. This will set the list pointer to
     * the first table. Attention: Function is not reentrant.
     *
     * @return a pointer to the next table in this map.
     */
    KSpreadTable* nextTable() { return m_lstTables.next();  }

    QList<KSpreadTable>& tableList() { return m_lstTables; }
  
    /**
     * @return amount of tables in this map.
     */
    int count() { return m_lstTables.count(); }
    
    bool movePythonCodeToFile();
    bool getPythonCodeFromFile();
    const char* getPythonCodeFile() { return m_strPythonCodeFile.data(); }
    bool isPythonCodeInFile() { return m_bPythonCodeInFile; }

    void update();
  
    int mapId() { return m_mapId; }

    /**
     * Needed for the printing Extension KOffice::Print
     */
    void draw( QPaintDevice* _dev, CORBA::Long _width, CORBA::Long _height,
	       CORBA::Float _scale );
  
    static KSpreadMap* find( int _map_id );  

protected:
    /**
     * List of all tables in this map. The list has autodelete turned on.
     */
    QList<KSpreadTable> m_lstTables;

    /**
     * Pointer to the part which holds this map.
     */
    KSpreadDoc *m_pDoc;

    QString m_strPythonCode;
    bool m_bPythonCodeInFile;
    QString m_strPythonCodeFile;

    int m_mapId;
  
    static int s_mapId;
    static QIntDict<KSpreadMap>* s_mapMaps;
};

#endif
