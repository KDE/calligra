#ifndef MSOOXMLDRAWINGREADERTABLEMETHODS_H
#define MSOOXMLDRAWINGREADERTABLEMETHODS_H

protected:
    KoFilter::ConversionStatus read_tbl();
    KoFilter::ConversionStatus read_tblPr();
    KoFilter::ConversionStatus read_tblGrid();
    KoFilter::ConversionStatus read_tr();
    KoFilter::ConversionStatus read_tableStyleId();
    KoFilter::ConversionStatus read_gridCol();
    KoFilter::ConversionStatus read_tc();
    KoFilter::ConversionStatus read_tcPr();

    uint m_currentTableNumber; //!< table counter, from 0
    uint m_currentTableRowNumber; //!< row counter, from 0, initialized in read_tbl()
    uint m_currentTableColumnNumber; //!< column counter, from 0, initialized in read_tr()
    KoGenStyle m_currentTableRowStyle;
    KoGenStyle m_currentTableCellStyle;
    QString m_currentTableName;
    qreal m_currentTableWidth; //!< in cm

    //! Column style info, allows to keep information about repeated columns
    class ColumnStyleInfo
    {
    public:
        ColumnStyleInfo(KoGenStyle *s = 0) : count(0), style(s) {}
        uint count;
        KoGenStyle* style; //!< not owned
    };

    void clearColumnStyles();
    QList<ColumnStyleInfo> columnStyles; //!< for collecting column styles


#endif