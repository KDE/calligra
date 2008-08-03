/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXINAMEWIDGET_H
#define KEXINAMEWIDGET_H

#include <kexi_export.h>
#include <qlabel.h>
#include <qlayout.h>
//Added by qt3to4:
#include <Q3GridLayout>
#include <klineedit.h>

namespace KexiUtils
{
class Validator;
class MultiValidator;
}

class KEXIMAIN_EXPORT KexiNameWidget : public QWidget
{
    Q_OBJECT

public:
    KexiNameWidget(const QString& message, QWidget * parent = 0);

    KexiNameWidget(const QString& message,
                   const QString& nameLabel, const QString& nameText,
                   const QString& captionLabel, const QString& captionText,
                   QWidget * parent = 0);

    virtual ~KexiNameWidget();

    QLabel* captionLabel() const {
        return lbl_caption;
    }
    QLabel* nameLabel() const {
        return lbl_name;
    }
    KLineEdit* captionLineEdit() const {
        return le_caption;
    }
    KLineEdit* nameLineEdit() const {
        return le_name;
    }

    QString messageText() const {
        return lbl_message->text();
    }

    void setMessageText(const QString& msg);

    //! \return entered caption text
    QString captionText() const;

    void setCaptionText(const QString& capt);
    //! \return entered name text, always in lower case

    QString nameText() const;

    void setNameText(const QString& name);

    /*! Sets i18n'ed warning message displayed when user leaves 'name' field
     without filling it (if acceptsEmptyValue() is false).
     By default the message is equal "Please enter the name.". */
    void setWarningForName(const QString& txt) {
        m_nameWarning = txt;
    }

    /*! Sets i18n'ed warning message displayed when user leaves 'name' field
     without filling it (if acceptsEmptyValue() is false).
     By default the message is equal "Please enter the caption." */
    void setWarningForCaption(const QString& txt) {
        m_captionWarning = txt;
    }

    /*! \return true if name or caption is empty. */
    bool empty() const;

    KexiUtils::Validator *nameValidator() const;

    /*! Adds subvalidator for name field. In fact it's is added to internal
     multivalidator. If \a owned is true, \a validator will be owned by the object.
     \sa MultiValidator::addSubvalidator(). */
    void addNameSubvalidator(KexiUtils::Validator* validator, bool owned = true);

    /*! \return true if name text cannot be empty (true by default). */
    bool isNameRequired() const;

    void setNameRequired(bool set);

    /*! \return true if caption text cannot be empty (false by default). */
    bool isCaptionRequired() const {
        return m_caption_required;
    }

    void setCaptionRequired(bool set) {
        m_caption_required = set;
    }

public slots:
    /*! Clears both name and caption. */
    virtual void clear();

    /*! Checks if both fields have valid values
     (i.e. not empty if acceptsEmptyValue() is false).
     If not, warning message is shown and false is returned. */
    bool checkValidity();

signals:
    /*! Emitted whenever return key is pressed on name or caption label. */
    void returnPressed();

    /*! Emitted whenever the caption or the name text changes */
    void textChanged();

    /*! Emitted whenever the message changes */
    void messageChanged();

protected slots:
    void slotNameTxtChanged(const QString&);
    void slotCaptionTxtChanged(const QString&);
//  bool eventFilter( QObject *obj, QEvent *ev );

protected:
    void init(
        const QString& message,
        const QString& nameLabel, const QString& nameText,
        const QString& captionLabel, const QString& captionText);

    QLabel* lbl_message;
    QLabel* lbl_caption;
    QLabel* lbl_name;
    KLineEdit* le_caption;
    KLineEdit* le_name;
    Q3GridLayout* lyr;
    KexiUtils::MultiValidator *m_validator;
    QString m_nameWarning, m_captionWarning;

bool m_le_name_txtchanged_disable : 1;
bool m_le_name_autofill : 1;
bool m_caption_required : 1;

    friend class KexiNameDialog;
};

#endif
