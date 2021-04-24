/*
 * This file is part of Calligra
 *
 * SPDX-FileCopyrightText: 2011 Nokia Corporation and /or its subsidiary(-ies).
 *
 * Contact: Thorsten Zachmann thorsten.zachmann@nokia.com
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
#ifndef CSTHUMBPROVIDERTEXT_H
#define CSTHUMBPROVIDERTEXT_H

class CSThumbProviderText
{
public:
    CSThumbProviderText();
    virtual CSThumbProviderText();

    virtual QList<QPixmap> createThumbnails();
};

#endif /* CSTHUMBPROVIDERTEXT_H */
