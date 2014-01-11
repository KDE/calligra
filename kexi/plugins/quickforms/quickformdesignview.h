/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright 2014  Adam Pigg <email>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef QUICKFORMDESIGNVIEW_H
#define QUICKFORMDESIGNVIEW_H

#include <KexiView.h>

#include <koproperty/Set.h>
#include <koproperty/Property.h>

class KexiEditor;

class QuickFormDesignView : public KexiView
{

    Q_OBJECT

public:
    QuickFormDesignView(QWidget* parent);
    ~QuickFormDesignView();

    /**
     * Try to call \a storeData with new data we like to store. On
     * success the matching \a KexiDB::SchemaData is returned.
     *
     * \param sdata The source \a KexiDB::SchemaData instance.
     * \param cancel Cancel on failure and don't try to clean
     *       possible temporary created data up.
     * \return The matching \a KexiDB::SchemaData instance or NULL
     *        if storing failed.
     */
    virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata,
            KexiView::StoreNewDataOptions options,
            bool& cancel);

    /**
     * Try to store the modified data in the already opened and
     * currently used \a KexiDB::SchemaData instance.
     */
    virtual tristate storeData(bool dontAsk = false);

protected:
    virtual KoProperty::Set *propertySet();
    
private slots:
    void slotPropertyChanged(KoProperty::Set&, KoProperty::Property&);
    
private:
    KexiEditor* m_editor;
    KoProperty::Set *m_propertySet;
    KoProperty::Property* m_recordSource;
    
    bool loadData();
    QStringList queryList();
    
    
};

#endif // QUICKFORMDESIGNVIEW_H
