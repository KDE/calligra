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

#include "kohtmljob.h"
#include "kohtmljob.moc"

#include <unistd.h>

#include <qfile.h>

#include <k2url.h>
#include <kio_error.h>
#include <kapp.h>

#include "kfileio.h"

KoHTMLJob::KoHTMLJob(KHTMLView *_topParent, KHTMLView *_parent, const char *_url, JobType _jType)
:KIOJob()
{
  topParent = _topParent;
  parent = _parent;
  url = _url;    
  jType = _jType;
  if (jType == Image) tmpFile = tmpnam(0);
  else
   {
     tmpFile = "";
     html = "";
     htmlLen = 0;
   }     

  K2URL u(url);
  
  isHTTP = (strcmp("http", u.protocol()) == 0);
  
  enableGUI(false);
  
  connect(this, SIGNAL(sigFinished(int)),
          this, SLOT(slotJobFinished()));
  connect(this, SIGNAL(sigError(int, int, const char *)),
          this, SLOT(slotError(int, int, const char *)));	  
  connect(this, SIGNAL(sigData(int, const char *, int)),
          this, SLOT(slotJobData(int, const char *, int)));
  connect(this, SIGNAL(sigRedirection(int, const char *)),
          this, SLOT(slotJobRedirection(int, const char *)));
}

KoHTMLJob::~KoHTMLJob()
{
  cout << "KoHTMLJob::~KoHTMLJob()" << endl;
  if (!tmpFile.isEmpty())
     unlink(tmpFile.data());
}
  
void KoHTMLJob::start()
{
  if (jType == Image) copy(url.data(), tmpFile.data());
  else 
    {
      if (isHTTP)
         get(url.data());
      else	 
         {
	   tmpFile = tmpnam(0);
	   copy(url.data(), tmpFile.data());
	 }
    }  
}

void KoHTMLJob::slotJobFinished()
{
  cout << "KoHTMLJob::slotJobFinished()" << endl;

  if (jType == Image) emit jobDone(this, topParent, parent, url.data(), tmpFile.data());
  else 
    {
      if (!isHTTP)
         {
	   QFile f(tmpFile);
	   if (f.exists())
	      {
	        html = kFileToString(tmpFile);
	        htmlLen = html.length();
	      }
	   else
	      {
	        html = "";
		htmlLen = 0;
	      }      
	 }
       emit jobDone(this, topParent, parent, url.data(), html.data(), htmlLen);
     }      
}

void KoHTMLJob::slotJobRedirection(int id, const char *_url)
{
  url = _url;
  start();
}

void KoHTMLJob::slotJobData(int id, const char *data, int len)
{
  assert( jType == HTML );
  assert( isHTTP );
  
  QString tmp(data, len);
  html += tmp;
  htmlLen += len;
}

void KoHTMLJob::slotError(int id, int errid, const char *txt)
{
  html = "";
  htmlLen = 0;

  QString msg;
  msg.sprintf(i18n("error while loading:\n%s\nerror message:\n%s"), url.data(), kioErrorString(errid, txt).data());

  emit jobError(msg);

  if (jType == Image) emit jobDone(this, topParent, parent, url.data(), 0L);
  else emit jobDone(this, topParent, parent, url.data(), html.data(), htmlLen);
}
