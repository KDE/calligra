/*
 * Copyright (C) 2003 Cédric Pasteur <cedric.pasteur@free.fr>
 */

#ifndef _TEST_H_
#define _TEST_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>

class KexiPropertyBuffer;

/**
 * @short Application Main Window
 * @author Cédric Pasteur <cedric.pasteur@free.fr>
 * @version 0.1
 */
class test : public KMainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    test();

    /**
     * Default Destructor
     */
    virtual ~test();
    
private:
	KexiPropertyBuffer *m_buffer;
};

#endif // _TEST_H_
