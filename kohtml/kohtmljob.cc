/*

	Copyright (C) 1998 Simon Hausmann
                       <tronical@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
//
// $Id$
//

#include "kohtmljob.h"
#include "kohtmljob.moc"

#include <unistd.h>

#include <qfile.h>

#include <k2url.h>
#include <kio_error.h>
#include <kapp.h>

#include "kfileio.h"

KoHTMLJob::KoHTMLJob(KHTMLView *_topParent, KHTMLView *_parent, const char *_url, JobType _jType)
:CachedKIOJob()
{
  m_pTopParent = _topParent;
  m_pParent = _parent;
  m_strURL = _url;    
  m_eJType = _jType;
  if (m_eJType == Image) m_strTmpFile = tmpnam(0);
  else
   {
     m_strTmpFile = "";
     m_strHTML = "";
     m_htmlLen = 0;
   }     

  K2URL u(m_strURL);
  
  m_bIsHTTP = (strcmp("http", u.protocol()) == 0);

  enableGUI(false);
  
  connect(this, SIGNAL(sigFinished(int)),
          this, SLOT(slotJobFinished()));
  connect(this, SIGNAL(sigError(int, int, const char *)),
          this, SLOT(slotError(int, int, const char *)));	  
  connect(this, SIGNAL(sigData(int, const char *, int)),
          this, SLOT(slotJobData(int, const char *, int)));
  connect(this, SIGNAL(sigRedirection(int, const char *)),
          this, SLOT(slotJobRedirection(int, const char *)));
  connect(this, SIGNAL(sigTotalSize(int, unsigned long)),
          this, SLOT(slotJobSize(int, unsigned long)));	  
	  
  m_sizeInKBytes = 0L; //...uh...	  
}

KoHTMLJob::~KoHTMLJob()
{
  cout << "KoHTMLJob::~KoHTMLJob() ( " << m_strURL << " ) " << endl;
  if (!m_strTmpFile.isEmpty())
     unlink(m_strTmpFile.data());
}
  
void KoHTMLJob::start()
{
  if (m_eJType == Image) copy(m_strURL.data(), m_strTmpFile.data());
  else 
    {
      if (m_bIsHTTP)
         get(m_strURL.data(), false);
      else	 
         {
	   m_strTmpFile = tmpnam(0);
	   copy(m_strURL.data(), m_strTmpFile.data());
	 }
    }  
}

void KoHTMLJob::slotJobFinished()
{
  cout << "KoHTMLJob::slotJobFinished()" << endl;

  if (m_eJType == Image) emit jobDone(this, m_pTopParent, m_pParent, m_strURL.data(), m_strTmpFile.data());
  else 
    {
      if (!m_bIsHTTP)
         {
	   QFile f(m_strTmpFile);
	   if (f.exists())
	      {
	        m_strHTML = kFileToString(m_strTmpFile);
	        m_htmlLen = m_strHTML.length();
	      }
	   else
	      {
	        m_strHTML = "";
		m_htmlLen = 0;
	      }      
	 }
	
       emit jobDone(this, m_pTopParent, m_pParent, m_strURL.data(), m_strHTML.data(), m_htmlLen);
     }      
}

void KoHTMLJob::slotJobRedirection(int id, const char *_url)
{
  m_strURL = _url;
}

void KoHTMLJob::slotJobData(int id, const char *data, int len)
{
  assert( m_eJType == HTML );
  assert( m_bIsHTTP );
  
  QString tmp(data, len);
  m_strHTML += tmp;
  m_htmlLen += len;
}

void KoHTMLJob::slotError(int id, int errid, const char *txt)
{
  m_strHTML = "";
  m_htmlLen = 0;

  QString msg;
  msg.sprintf(i18n("error while loading:\n%s\nerror message:\n%s"), m_strURL.data(), kioErrorString(errid, txt).data());

  emit jobError(msg);

  if (m_eJType == Image) emit jobDone(this, m_pTopParent, m_pParent, m_strURL.data(), 0L);
  else emit jobDone(this, m_pTopParent, m_pParent, m_strURL.data(), m_strHTML.data(), m_htmlLen);
}

void KoHTMLJob::slotJobSize(int id, unsigned long bytes)
{
  m_sizeInKBytes = (bytes + 1023) / 1024;
}
