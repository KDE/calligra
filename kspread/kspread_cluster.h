#ifndef kspread_cluster_h
#define kspread_cluster_h

class KSpreadCell;
class ColumnLayout;
class RowLayout;

class QPoint;

#define KSPREAD_CLUSTER_LEVEL1 100
#define KSPREAD_CLUSTER_LEVEL2 100
#define KSPREAD_CLUSTER_MAX (100*100)

class KSpreadCluster
{
public:
    KSpreadCluster();
    ~KSpreadCluster();

    KSpreadCell* lookup( int x, int y );

    void clear();

    void insert( KSpreadCell* cell, int x, int y );
    void remove( int x, int y );

    void setAutoDelete( bool );
    bool autoDelete() const;

    KSpreadCell* firstCell();

    bool shiftRow( const QPoint& marker );
    bool shiftColumn( const QPoint& marker );

    void unshiftColumn( const QPoint& marker );
    void unshiftRow( const QPoint& marker );

    bool insertColumn( int col );
    bool insertRow( int row );

    void removeColumn( int col );
    void removeRow( int row );

private:
    /**
     * @param work is set to TRUE if the method found some clusters
     *        which belong to the shifted row.
     */
    bool shiftRow( const QPoint& marker, bool& work );
    bool shiftColumn( const QPoint& marker, bool& work );

    void unshiftColumn( const QPoint& marker, bool& work );
    void unshiftRow( const QPoint& marker, bool& work );

    KSpreadCell*** m_cluster;
    KSpreadCell* m_first;
    bool m_autoDelete;
};

class KSpreadColumnCluster
{
public:
    KSpreadColumnCluster();
    ~KSpreadColumnCluster();

    ColumnLayout* lookup( int col );

    void clear();

    void insertElement( ColumnLayout*, int col );
    void removeElement( int col );

    bool insertColumn( int col );
    bool removeColumn( int col );

    void setAutoDelete( bool );
    bool autoDelete() const;

    ColumnLayout* first() { return m_first; }

private:
    ColumnLayout*** m_cluster;
    ColumnLayout* m_first;
    bool m_autoDelete;
};

class KSpreadRowCluster
{
public:
    KSpreadRowCluster();
    ~KSpreadRowCluster();

    RowLayout* lookup( int col );

    void clear();

    void insertElement( RowLayout*, int row );
    void removeElement( int row );

    bool insertRow( int row );
    bool removeRow( int row );

    void setAutoDelete( bool );
    bool autoDelete() const;

    RowLayout* first() { return m_first; }

private:
    RowLayout*** m_cluster;
    RowLayout* m_first;
    bool m_autoDelete;
};

#endif
