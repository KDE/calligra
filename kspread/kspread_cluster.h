#ifndef kspread_cluster_h
#define kspread_cluster_h

class KSpreadCell;
class ColumnLayout;
class RowLayout;

class QPoint;

#define KSPREAD_CLUSTER_LEVEL1 256
#define KSPREAD_CLUSTER_LEVEL2 128
/* KSPREAD_CLUSTER_MAX is CURRENTLY 2^15 only, so it's 256*256 divided by 2 */
#define KSPREAD_CLUSTER_MAX (256*256/2)

class KSpreadCluster
{
public:
    KSpreadCluster();
    ~KSpreadCluster();

    const KSpreadCell* lookup( int x, int y ) const;
    KSpreadCell* lookup( int x, int y );

    /**
     * Removes all cells from the table and frees memory that
     * was used for the clusters.
     */
    void clear();

    /**
     * Inserts a cell at the requested position. If there is already
     * a cell, then @ref #remove is called on it.
     */
    void insert( KSpreadCell* cell, int x, int y );
    /**
     * Removes the cell at the given position, if there is any.
     */
    void remove( int x, int y );

    void setAutoDelete( bool );
    bool autoDelete() const;

    KSpreadCell* firstCell();

    bool shiftRow( const QPoint& marker );
    /**
     * Moves all cells in the column marker.x() beginning with
     * the one at marker.y() one position downwards.
     *
     * @return FALSE if a cell would drop out of the table because of that.
     *         In this case the shift is not performed.
     */
    bool shiftColumn( const QPoint& marker );

    /**
     * Moves all cells in the column marker.x() beginning with
     * the one at marker.y() + 1 one position upwards.
     */
    void unshiftColumn( const QPoint& marker );
    void unshiftRow( const QPoint& marker );

    /**
     * Moves all columns beginning with @p col one position
     * to the right. If that does not work because a cell would
     * drop out of the table, then FALSE is returned.
     *
     * @see #removeColumn
     */
    bool insertColumn( int col );
    bool insertRow( int row );

    /**
     * Removes all elements from the column and
     * move all columns right of @p col one position
     * to the left.
     *
     * @see #clearColumn
     */
    void removeColumn( int col );
    void removeRow( int row );

    /**
     * Removes all elements from the column.
     */
    void clearColumn( int col );
    void clearRow( int row );

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

    const ColumnLayout* lookup( int col ) const;
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

    const RowLayout* lookup( int col ) const;
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
