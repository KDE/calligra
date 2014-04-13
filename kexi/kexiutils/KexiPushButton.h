/* This file is part of the KDE project
   Copyright (C) 2013 Oleg Kukharchuk <oleg.kuh@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIPUSHBUTTON_H
#define KEXIPUSHBUTTON_H

#include <kexiutils/kexiutils_export.h>

#include <QPushButton>
#include <kexiutils/utils.h>

class KexiPushButtonPrivate;

/*!
 * \short A PushButton widget with hyperlinks support
 * \author Oleg Kukharchuk <oleg.kuh@gmail.com>
 */
class KEXIUTILS_EXPORT KexiPushButton : public QPushButton
{
    Q_OBJECT
    Q_ENUMS(HyperlinkType)

    Q_PROPERTY(QString hyperlink READ hyperlink WRITE setHyperlink)
    Q_PROPERTY(HyperlinkType hyperlinkType READ hyperlinkType WRITE setHyperlinkType)
    Q_PROPERTY(KexiUtils::OpenHyperlinkOptions::HyperlinkTool hyperlinkTool READ hyperlinkTool WRITE setHyperlinkTool)
    Q_PROPERTY(bool hyperlinkExecutable READ isHyperlinkExecutable WRITE setHyperlinkExecutable)
    Q_PROPERTY(bool remoteHyperlink READ isRemoteHyperlink WRITE setRemoteHyperlink)
public:

    /*!
     * Type of the Hyperlink
     */
    enum HyperlinkType {
        NoHyperlink, /*!< Hyperlinks are not supported */
        StaticHyperlink, /*!< Static hyperlink provided by \a hyperlink property*/
        DynamicHyperlink /*!< Dynamic hyperlink (used in data-aware widgets) */
    };

    //! Default contructor
    explicit KexiPushButton(QWidget *parent = 0);

    //! Constructor, that sets the button-text to text
    explicit KexiPushButton(const QString &text, QWidget *parent = 0);
    ~KexiPushButton();

    /*! \property KexiPushButton::hyperlink
     * Sets hyperlink for the button
     */
    void setHyperlink(const QString &url);
    QString hyperlink() const;

    /*! \property KexiPushButton::hyperlinkType
     * Sets the hyperlink type to
     * The valid vaues are descibed by the  KexiPushButton::HyperlinkType enum
     *
     * The default value is NoHyperlink
     */
    void setHyperlinkType(HyperlinkType type);
    HyperlinkType hyperlinkType() const;

    /*! \property KexiPushButton::hyperlinkTool
     * Sets the tool for opening hyperlinks
     * The valid values are described by the KexiUtils::OpenHyperlinkOptions::HyperlinkTool enum
     * 
     * The default value is DefaultHyperlinkTool
     */
    void setHyperlinkTool(KexiUtils::OpenHyperlinkOptions::HyperlinkTool tool);
    KexiUtils::OpenHyperlinkOptions::HyperlinkTool hyperlinkTool() const;

    /*! \property KexiPushButton::hyperlinkExecutable
     * If true hyperlink can be an executable file
     *
     * The default value is false
     */
    void setHyperlinkExecutable(bool exec);
    bool isHyperlinkExecutable() const;

    /*! \property KexiPushButton::hyperlinkLocal
     * If \a true only local hyperlink are allowed
     *
     * The default value is true
     */
    void setRemoteHyperlink(bool remote);
    bool isRemoteHyperlink() const;

protected:
    void setLocalBasePath(const QString &basePath);
private:
    friend class KexiPushButtonPrivate;
    KexiPushButtonPrivate * const d;
    Q_PRIVATE_SLOT(d, void slotClicked())
};
#endif // KEXIPUSHBUTTON_H
