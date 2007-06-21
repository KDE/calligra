/*
 * This file is part of KSpread
 *
 * Copyright (c) 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTINGREADER_H
#define SCRIPTINGREADER_H

#include <QString>
#include <QStringList>
#include <QObject>

#include "ScriptingModule.h"

#include <Doc.h>
#include <Sheet.h>
#include <Map.h>
#include <Region.h>
#include <Cell.h>
#include <Value.h>

/**
* The ScriptingReader class provides abstract high-level functionality to read
* content from KSpread sheets.
*/
class ScriptingReader : public QObject
{
        Q_OBJECT
    public:

        /**
        * Constructor.
        * \param module The \a ScriptingModule instance that should be used
        * as datasource for this reader.
        */
        explicit ScriptingReader(ScriptingModule* module) : QObject(module), m_module(module) { clear(); }

        /**
        * Destructor.
        */
        virtual ~ScriptingReader() {}

    public slots:

        /**
        * Start the reading. This let's the reader run over the defined sheets and there ranges
        * and emits the signals this reader provides while walking over the data.
        */
        bool start() {
            if( m_state != Stopped ) return false;
            m_state = Running;
            QStringList sheets = m_sheetnames.count() > 0 ? m_sheetnames : m_module->sheetNames();
            foreach(QString sheetname, sheets) {
                readSheet(sheetname);
                if( m_state != Running ) break;
            }
            clear();
            return true;
        }

        /**
        * Stop the reading. This method does request to shutdown a reading process what means,
        * that reading will stop as soon as possible. This does make it save to call this
        * method also from within scripting functions that connected with the signals this
        * reader provides.
        */
        void stop() {
            if( m_state == Running )
                m_state = Shutdown;
        }

        /**
        * \returns true if the reader is still running what includes also shutdown. If the
        * reader is stopped and can be started again by using the \a start() method false
        * got returned.
        */
        bool isRunning() {
            return m_state != Stopped;
        }

        /**
        * \return the names of the sheets that should be readed.
        */
        QStringList sheetNames() {
            return m_sheetnames;
        }

        /**
        * \return the defined ranges for the \p sheetname .
        */
        QVariantList range(const QString& sheetname) {
            return m_ranges.contains(sheetname) ? m_ranges[sheetname] : QVariantList();
        }

        /**
        * Set the defined ranges for the \p sheetname to \p range .
        */
        void setRange(const QString& sheetname, const QVariantList& range) {
            m_ranges[sheetname] = range;
        }

        /**
        * Set the names of the sheets that should be readed to \p sheetnames .
        */
        void setSheetNames(const QStringList& sheetnames) {
            m_sheetnames = sheetnames;
            m_ranges.clear();
        }

        /**
        * Set the sheets and there ranges.
        *
        * For example in python following structure is a valid definition for 3
        * 3 sheets where Sheet1 and Sheet2 are selected. Sheet1 also does
        * define the range A1:B2.
        * [['Sheet1', 1, [1, 1, 2, 2]], ['Sheet2', 1], ['Sheet3', 0]]
        *
        * Each sheet contains a tuple of
        * \li sheetname
        * \li 1=enabled or 0=disabled
        * \li optional range tuple [from column, from row, to column, to row]
        */
        void setSheets(const QVariantList& sheets) {
            m_sheetnames.clear();
            m_ranges.clear();
            foreach(QVariant item, sheets) {
                QList<QVariant> args = item.toList();
                if( args.count() < 2 ) continue;
                if( ! args[1].toBool() ) continue;
                const QString sheetname = args[0].toString();
                if( sheetname.isEmpty() ) continue;
                args.removeFirst();
                args.removeFirst();
                m_sheetnames.append(sheetname);
                if( args.count() > 0 ) m_ranges[sheetname] = args;
            }
        }

        /**
        * Set a sheet that should be readed. The sheet with name \p sheetname and
        * the optional defined range \p range will be appended to all already
        * defined sheets if no such sheet with \p sheetname was defined before else
        * we just replace the previous definition including the range.
        * The range should looks like e.g. "A1:C3" while the defined sheetname
        * should be an existing one if a range got defined.
        */
        void setSheet(const QString& sheetname, const QString& range = QString()) {
            if( ! m_sheetnames.contains(sheetname) )
                m_sheetnames.append(sheetname);
            if( range.isNull() ) {
                m_ranges.remove(sheetname);
                return;
            }
            KSpread::Sheet* sheet = m_module->kspreadDoc()->map()->findSheet(sheetname);
            if( ! sheet ) return;
            QVariantList ranges;
            KSpread::Region region(m_module->kspreadDoc()->map(), range, sheet);
            if( ! region.isValid() ) return;
            for(KSpread::Region::ConstIterator it = region.constBegin(); it != region.constEnd(); ++it) {
                const QRect rect = (*it)->rect();
                if( ! rect.isNull() ) ranges.append(rect);
            }
            m_ranges[sheetname] = ranges;
        }

        /**
        * \return the sheetname the reader currently is on. An empty/null string will be
        * returned if there is no current sheet (e.g. if the reader just doesn't run currently).
        */
        QString currentSheetName() {
            return m_currentSheet ? m_currentSheet->sheetName() : QString();
        }

        /**
        * \return the current row number the reader is on. This will be -1 if the reader
        * isn't running.
        */
        int currentRow() {
            return m_currentRow;
        }

        /**
        * \return a list of values for the current row. This will be an empty list of the
        * reader isn't running.
        */
        QVariantList currentValues() {
            QVariantList values;
            if( m_currentSheet && m_currentRow >= 0 ) {
                for(int col = m_currentLeft; col <= m_currentRight; ++col) {
                    KSpread::Cell cell(m_currentSheet, col, m_currentRow);
                    KSpread::Value value = cell.value();

                    //TODO add toVariant() method to KSpread::Value and use it here and in SheetAdaptor::valueToVariant
                    //values << value.toVariant();

                    QVariant v;
                    switch( value.type() ) {
                        case KSpread::Value::Empty: break;
                        case KSpread::Value::Boolean: v = value.asBoolean(); break;
                        case KSpread::Value::Integer: v = value.asInteger(); break;
                        case KSpread::Value::Float: v = value.asFloat(); break;
                        case KSpread::Value::String: //fall through
                        default: v = value.asString(); break; 
                    }
                    values << v;
                }
            }
            return values;
        }


    signals:

        /**
        * This signal is emitted once the reading started with the \a start() method
        * changed to the sheet with name \p sheetname cause reading the probably
        * previous sheet was done.
        */
        void changedSheet(const QString& sheetname);

        /**
        * This signal is emitted once the reading started with the \a start() method
        * changed to the row \p rownumber where rownumber is >=0.
        */
        void changedRow(int rownumber);

    private:
        Q_DISABLE_COPY( ScriptingReader )

        ScriptingModule* const m_module;
        QStringList m_sheetnames;
        QHash<QString, QVariantList> m_ranges;

        enum State { Stopped, Running, Shutdown };
        State m_state;
        KSpread::Sheet* m_currentSheet;
        int m_currentRow, m_currentLeft, m_currentRight;

        void clear() {
            m_state = Stopped;
            m_currentSheet = 0;
            m_currentRow = -1;
        }

        void readSheet(const QString& sheetname) {
            m_currentSheet = m_module->kspreadDoc()->map()->findSheet(sheetname);
            if( ! m_currentSheet ) return;
            emit changedSheet(sheetname);
            if( m_state != Running ) return;
            //kDebug()<<"ScriptingReader::readSheet sheetname="<<sheetname<<endl;
            QVariantList range = m_ranges[sheetname];
            if( range.count() > 0 ) {
                foreach(QVariant r, range) {
                    QVariantList l = r.toList();
                    if( l.count() < 4 ) continue;
                    QRect rect(l[0].toInt(),l[1].toInt(),l[2].toInt(),l[3].toInt());
                    if( rect.isNull() || (rect.x()==0 && rect.y()==0 && rect.width()==0 && rect.height()==0) ) continue;
                    //kDebug()<<"  string="<<r.toString()<<" rect="<<rect<<endl;
                    KSpread::Region region(rect, m_currentSheet);
                    readRegion(region);
                    if( m_state != Running ) break;
                }
            }
            else {
                QRect area = m_currentSheet->usedArea();
                if( area.isNull() ) return;
                KSpread::Region region(area, m_currentSheet);
                readRegion(region);
            }
        }

        void readRegion(const KSpread::Region& region) {
            if( ! m_currentSheet || ! region.isValid() ) return;
            //kDebug()<<"ScriptingReader::readRegion name="<<region.name(m_currentSheet)<<endl;
            for(KSpread::Region::ConstIterator it = region.constBegin(); it != region.constEnd(); ++it) {
                QRect range = (*it)->rect();
                if( range.isNull() ) continue;
                //kDebug() << "  name=" << (*it)->name(m_currentSheet) << " range=" << range << endl;
                const int bottom = range.bottom();
                m_currentLeft = range.left();
                m_currentRight = range.right();
                for(int row = range.top(); row <= bottom; ++row) {
                    m_currentRow = row;
                    emit changedRow(row);
                    if( m_state != Running ) break;
                }
            }
        }
};

#endif
