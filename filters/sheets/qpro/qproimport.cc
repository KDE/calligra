
/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Graham Short <grahshrt@netscape.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <qproimport.h>
#include <qproformula.h>
#include <qpro/stream.h>
#include <qpro/record_factory.h>

#include <KLocalizedString>
#include <KMessageBox>
#include <KPluginFactory>

#include <KoFilterChain.h>

#include <sheets/engine/CellBase.h>
#include <sheets/engine/SheetBase.h>
#include <sheets/engine/Value.h>
#include <sheets/core/DocBase.h>
#include <sheets/core/LoadingInfo.h>
#include <sheets/core/Map.h>

#include <QFile>
#include <QDebug>

using namespace Calligra::Sheets;

Q_LOGGING_CATEGORY(lcQPro, "calligra.filter.qpro")

K_PLUGIN_FACTORY_WITH_JSON(QPROImportFactory, "calligra_filter_qpro2sheets.json",
                           registerPlugin<QpImport>();)

// ---------------------------------------------------------------

QpTableList::QpTableList()
{
    for (int lIdx = 0; lIdx < cNameCnt; ++lIdx) {
        cTable[lIdx] = nullptr;
    }
}

QpTableList::~QpTableList()
{
    // don't delete the list of tables
}


void
QpTableList::table(unsigned pIdx, SheetBase* pTable)
{
    if (pIdx < cNameCnt) {
        cTable[pIdx] = pTable;
    }
}

SheetBase*
QpTableList::table(unsigned pIdx)
{
    return (pIdx < cNameCnt ? cTable[pIdx] : nullptr);
}


// ---------------------------------------------------------------

QpImport::QpImport(QObject* parent, const QVariantList&)
        : KoFilter(parent)
{
}

void
QpImport::InitTableName(int pIdx, QString& pResult)
{
    if (pIdx < 26) {
        pResult = (char)('A' + pIdx);
    } else {
        pResult = (char)('A' - 1 + pIdx / 26);
        pResult += (char)('A' + pIdx % 26);
    }
}

KoFilter::ConversionStatus QpImport::convert(const QByteArray& from, const QByteArray& to)
{
    bool bSuccess = true;

    KoDocument* document = m_chain->outputDocument();
    if (!document)
        return KoFilter::StupidError;

    qDebug(lcQPro) << "here we go..." << document->metaObject()->className();

    if (!::qobject_cast<const Calligra::Sheets::DocBase *>(document)) {   // it's safer that way :)
        qWarning(lcQPro) << "document isn't a Calligra::Sheets::DocBase but a " << document->metaObject()->className();
        return KoFilter::NotImplemented;
    }
    if (from != "application/x-quattropro" || to != "application/x-kspread") {
        qWarning(lcQPro) << "Invalid mimetypes " << from << " " << to;
        return KoFilter::NotImplemented;
    }

    qDebug(lcQPro) << "...still here...";
    DocBase *ksdoc = dynamic_cast<DocBase *>(document);

    if (ksdoc->mimeType() != "application/x-ole-storage") {
        qWarning(lcQPro) << "Invalid document mimetype " << ksdoc->mimeType();
        return KoFilter::NotImplemented;
    }

    QpIStream lIn(QFile::encodeName(m_chain->inputFile()));

    if (!lIn) {
        KMessageBox::error(nullptr, i18n("QPRO filter cannot open input file - please report."));
        return KoFilter::FileNotFound;
    }

    SheetBase *table = nullptr;

    QString field;
    int value = 0;
    Q_EMIT sigProgress(value);

    QpRecFactory            lFactory(lIn);
    QpTableList             lTableNames;
    QP_UINT8                lPageIdx = 0;

    QpRec*                  lRec = nullptr;
    QpRecBop*               lRecBop = nullptr;
    QpRecIntegerCell*       lRecInt = nullptr;
    QpRecFloatingPointCell* lRecFloat = nullptr;
    QpRecFormulaCell*       lRecFormula = nullptr;
    QpRecLabelCell*         lRecLabel = nullptr;
    QpRecPageName*          lRecPageName = nullptr;

    do {
        field.clear();
        lRec  = lFactory.nextRecord();

        switch (lRec->type()) {
        case QpBop:
            lRecBop = (QpRecBop*)lRec;
            lPageIdx = lRecBop->pageIndex();

            // find out if we know about this table already, if not create it
            table = lTableNames.table(lPageIdx);

            if (table == nullptr) {
                table = ksdoc->map()->addNewSheet();
                // set up a default name for the table
                table->setSheetName(lTableNames.name(lPageIdx));
                lTableNames.table(lPageIdx, table);
            }
            break;

        case QpIntegerCell:
            lRecInt = (QpRecIntegerCell*)lRec;
            field.setNum(lRecInt->integer());
//cout << "Setting R " << lRecInt->row()+1 << ", C " << ((unsigned)lRecInt->column()) << Qt::endl;
            if (table)
                setText(table, lRecInt->row() + 1, ((unsigned)lRecInt->column()) + 1, field, false);
            break;

        case QpFormulaCell:
            lRecFormula = (QpRecFormulaCell*)lRec;
            {
                QuattroPro::Formula lAnswer(*lRecFormula, lTableNames);

                char*     lFormula = lAnswer.formula();

                field = lFormula;

                delete [] lFormula;
            }

            // check for referenced tables that haven't been created yet
            for (unsigned lIdx = 0; lIdx < lTableNames.cNameCnt; ++lIdx) {
                if (lTableNames.allocated(lIdx) && (lTableNames.table(lIdx) == nullptr)) {
                    // we're about to reference a table that hasn't been created yet.
                    // setText gets upset about this, so create a blank table

                    SheetBase* lNewTable = ksdoc->map()->addNewSheet();

                    // set up a default name for the table
                    lNewTable->setSheetName(lTableNames.name(lIdx));
                    lTableNames.table(lIdx, lNewTable);
                }
            }

            if (table)
                setText(table, lRecFormula->row() + 1, lRecFormula->column() + 1, field, false);
            break;

        case QpFloatingPointCell:
            lRecFloat = (QpRecFloatingPointCell*)lRec;
            field.setNum(lRecFloat->value());
            if (table)
                setText(table, lRecFloat->row() + 1, lRecFloat->column() + 1, field, false);
            break;

        case QpLabelCell:
            lRecLabel = (QpRecLabelCell*)lRec;
            field = "'";
            field += lRecLabel->label();
            if (table)
                setText(table, lRecLabel->row() + 1, lRecLabel->column() + 1, field, false);
            break;

        case QpPageName:
            lRecPageName = (QpRecPageName*)lRec;

            if (lTableNames.allocated(lPageIdx) && lTableNames.table(lPageIdx)) {
                lTableNames.table(lPageIdx)->setSheetName(lRecPageName->pageName());
                lTableNames.name(lPageIdx, lRecPageName->pageName());
            }
            break;

        case QpPassword:
            KMessageBox::error(nullptr, i18n("Unable to open password protected files.\n"
                                        "The password algorithm has not been published")
                              );
            delete lRec;
            return KoFilter::NotImplemented;
        }

        delete lRec;
        lRec = nullptr;
    } while (lIn);

    ksdoc->map()->loadingInfo()->setInitialActiveSheet(table);

    Q_EMIT sigProgress(100);
    if (bSuccess)
        return KoFilter::OK;
    else
        return KoFilter::StupidError;
}

void QpImport::setText(SheetBase* sheet, int _row, int _column, const QString& _text, bool asString)
{
    CellBase cell(sheet, _column, _row);
    if (asString) {
        cell.setUserInput(_text);
        cell.setValue(Value(_text));
    } else {
        cell.parseUserInput(_text);
    }
}

#include <qproimport.moc>
