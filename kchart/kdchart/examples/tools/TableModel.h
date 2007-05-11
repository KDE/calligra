#ifndef TABLEMODEL_H
#define TABLEMODEL_H

#include <QVariant>
#include <QVector>
#include <QAbstractTableModel>
#include "tools_export.h"
#include <QStringList>

/** TableModel uses a simple rectangular vector of vectors to represent a data
    table that can be displayed in regular Qt Interview views.
    Additionally, it provides a method to load CSV files exported by
    OpenOffice Calc in the default configuration. This allows to prepare test
    data using spreadsheet software.

    It expects the CSV files in the subfolder ./modeldata. If the application
    is started from another location, it will ask for the location of the
    model data files.
*/

class TESTTOOLS_EXPORT TableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    TableModel ( QObject * parent = 0 );
    ~TableModel ();

    /** Return header data from the model.
        The model will use the first data row and the first data column of the
        physical data as source of column and row header data. This data isnot
        exposed as model data, that means, the first model row and column will
        start at index (1,1).
    */
    QVariant headerData ( int section, Qt::Orientation orientation,
                          int role = Qt::DisplayRole ) const ;

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

    bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

    /** Load the table from a comma separated file.
     *
     * The files are supposed to be Unicode (UTF8), have commas (',') as
     * delimiters, and quotes ('"') as text delimiters. All lines are expected
     * to provide the same number of fields. HINT: This is the default way
     * OO.o-Calc exports CSV files.
     * The cell data is expected to be floating point values, except for the
     * first row and the first column, where string values are exected (those
     * will be used as axis descriptors). If values cannot be converted to
     * doubles, their string representation will be used.
     * 
     * @returns true if successful, false otherwise
     *
     * @sa titleText
     */
    bool loadFromCSV ( const QString& filename );

    /**
     * If both DataHasHorizontalHeaders and DataHasVerticalHeaders is
     * set true (that's the default setting) then loadFromCSV will interpret
     * the first field in the first row as title-text.
     * If no such field is found the loadFromCSV will set the title text to
     * an empty string.
     *
     * The text is stored and can be retrieved via titleText(), but the model
     * itself does nothing else with it: The calling application may use this
     * method and e.g. display the text as header or as title of the Legend
     * or as caption of the window ...
     *
     * @sa loadFromCSV
     */
    const QString titleText()const {
        return m_titleText;
    }

    /**
     * Setting the title text has no effect except that the text
     * can then be retrieved via titleText.
     * 
     * TableModel is just storing this data but it does nothing
     * else with it, nor does Qt's IndeView model make use of it.
     */
    void setTitleText( const QString& txt ) {
        m_titleText = txt;
    }

    /** Make the model invalid, that is, provide no data. */
    void clear();

    /**
     * Set to false if the data has no horizontal header
     */
    void setDataHasHorizontalHeaders( bool value ) {
        m_dataHasHorizontalHeaders = value;
    }
    /**
     * Set to false if the data has no vertical header
     */
    void setDataHasVerticalHeaders( bool value ) {
        m_dataHasVerticalHeaders = value;
    }
    /**
     * setSupplyHeaderData(false) allows to prevent the model from supplying header data,
     * even if parsing found any
     */
    void setSupplyHeaderData( bool value ) {
        m_supplyHeaderData = value;
    }

protected:
    // the vector of rows:
    QVector< QVector<QVariant> > m_rows;

private:

    // the header data:
    QStringList m_horizontalHeaderData;
    QStringList m_verticalHeaderData;
    QString m_titleText;
    bool m_dataHasHorizontalHeaders;
    bool m_dataHasVerticalHeaders;
    bool m_supplyHeaderData;
};


#endif
