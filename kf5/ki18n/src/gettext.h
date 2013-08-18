/* Convenience header for conditional use of GNU <libintl.h>.
   Copyright (C) 1995-1998, 2000-2002, 2004-2006 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU Library General Public License as published
   by the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
   USA.  */

#ifndef _LIBGETTEXT_H
#define _LIBGETTEXT_H 1

/* Get declarations of GNU message catalog functions.  */
# include <libintl.h>

/* You can set the DEFAULT_TEXT_DOMAIN macro to specify the domain used by
   the gettext() and ngettext() macros.  This is an alternative to calling
   textdomain(), and is useful for libraries.  */
# ifdef DEFAULT_TEXT_DOMAIN
#  undef gettext
#  define gettext(Msgid) \
     dgettext (DEFAULT_TEXT_DOMAIN, Msgid)
#  undef ngettext
#  define ngettext(Msgid1, Msgid2, N) \
     dngettext (DEFAULT_TEXT_DOMAIN, Msgid1, Msgid2, N)
# endif

/* The separator between msgctxt and msgid in a .mo file.  */
#define GETTEXT_CONTEXT_GLUE "\004"

/* Pseudo function calls, taking a MSGCTXT and a MSGID instead of just a
   MSGID.  MSGCTXT and MSGID must be string literals.  MSGCTXT should be
   short and rarely need to change.
   The letter 'p' stands for 'particular' or 'special'.  */
#ifdef DEFAULT_TEXT_DOMAIN
# define pgettext(Msgctxt, Msgid) \
   pgettext_aux (DEFAULT_TEXT_DOMAIN, Msgctxt GETTEXT_CONTEXT_GLUE Msgid, Msgid, LC_MESSAGES)
#else
# define pgettext(Msgctxt, Msgid) \
   pgettext_aux (NULL, Msgctxt GETTEXT_CONTEXT_GLUE Msgid, Msgid, LC_MESSAGES)
#endif
#define dpgettext(Domainname, Msgctxt, Msgid) \
  pgettext_aux (Domainname, Msgctxt GETTEXT_CONTEXT_GLUE Msgid, Msgid, LC_MESSAGES)
#define dcpgettext(Domainname, Msgctxt, Msgid, Category) \
  pgettext_aux (Domainname, Msgctxt GETTEXT_CONTEXT_GLUE Msgid, Msgid, Category)
#ifdef DEFAULT_TEXT_DOMAIN
# define npgettext(Msgctxt, Msgid, MsgidPlural, N) \
   npgettext_aux (DEFAULT_TEXT_DOMAIN, Msgctxt GETTEXT_CONTEXT_GLUE Msgid, Msgid, MsgidPlural, N, LC_MESSAGES)
#else
# define npgettext(Msgctxt, Msgid, MsgidPlural, N) \
   npgettext_aux (NULL, Msgctxt GETTEXT_CONTEXT_GLUE Msgid, Msgid, MsgidPlural, N, LC_MESSAGES)
#endif
#define dnpgettext(Domainname, Msgctxt, Msgid, MsgidPlural, N) \
  npgettext_aux (Domainname, Msgctxt GETTEXT_CONTEXT_GLUE Msgid, Msgid, MsgidPlural, N, LC_MESSAGES)
#define dcnpgettext(Domainname, Msgctxt, Msgid, MsgidPlural, N, Category) \
  npgettext_aux (Domainname, Msgctxt GETTEXT_CONTEXT_GLUE Msgid, Msgid, MsgidPlural, N, Category)

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static const char *
pgettext_aux (const char *domain,
	      const char *msg_ctxt_id, const char *msgid,
	      int category)
{
  const char *translation = dcgettext (domain, msg_ctxt_id, category);
  if (translation == msg_ctxt_id)
    return msgid;
  else
    return translation;
}

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static const char *
npgettext_aux (const char *domain,
	       const char *msg_ctxt_id, const char *msgid,
	       const char *msgid_plural, unsigned long int n,
	       int category)
{
  const char *translation =
    dcngettext (domain, msg_ctxt_id, msgid_plural, n, category);
  if (translation == msg_ctxt_id || translation == msgid_plural)
    return (n == 1 ? msgid : msgid_plural);
  else
    return translation;
}

/* The same thing extended for non-constant arguments.  Here MSGCTXT and MSGID
   can be arbitrary expressions.  But for string literals these macros are
   less efficient than those above.  */

#include <string.h>

#ifndef __STRICT_ANSI__
#define __STRICT_ANSI__
#define __STRICT_ANSI_FORCED__
#endif

#define _LIBGETTEXT_HAVE_VARIABLE_SIZE_ARRAYS \
  (__STRICT_ANSI__ - 0 == 0) && (__GNUC__ >= 3 || __GNUG__ >= 2 /* || __STDC_VERSION__ >= 199901L */ )

#if !_LIBGETTEXT_HAVE_VARIABLE_SIZE_ARRAYS
#include <stdlib.h>
#endif

#define pgettext_expr(Msgctxt, Msgid) \
  dcpgettext_expr (NULL, Msgctxt, Msgid, LC_MESSAGES)
#define dpgettext_expr(Domainname, Msgctxt, Msgid) \
  dcpgettext_expr (Domainname, Msgctxt, Msgid, LC_MESSAGES)

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static const char *
dcpgettext_expr (const char *domain,
		 const char *msgctxt, const char *msgid,
		 int category)
{
  size_t msgctxt_len = strlen (msgctxt) + 1;
  size_t msgid_len = strlen (msgid) + 1;
  const char *translation;
#if _LIBGETTEXT_HAVE_VARIABLE_SIZE_ARRAYS
  char msg_ctxt_id[msgctxt_len + msgid_len];
#else
  char buf[1024];
  char *msg_ctxt_id =
    (msgctxt_len + msgid_len <= sizeof (buf)
     ? buf
     : (char *) malloc (msgctxt_len + msgid_len));
  if (msg_ctxt_id != NULL)
#endif
    {
      memcpy (msg_ctxt_id, msgctxt, msgctxt_len - 1);
      msg_ctxt_id[msgctxt_len - 1] = '\004';
      memcpy (msg_ctxt_id + msgctxt_len, msgid, msgid_len);
      translation = dcgettext (domain, msg_ctxt_id, category);
#if !_LIBGETTEXT_HAVE_VARIABLE_SIZE_ARRAYS
      if (msg_ctxt_id != buf)
	free (msg_ctxt_id);
#endif
      if (translation != msg_ctxt_id)
	return translation;
    }
  return msgid;
}

#define npgettext_expr(Msgctxt, Msgid, MsgidPlural, N) \
  dcnpgettext_expr (NULL, Msgctxt, Msgid, MsgidPlural, N, LC_MESSAGES)
#define dnpgettext_expr(Domainname, Msgctxt, Msgid, MsgidPlural, N) \
  dcnpgettext_expr (Domainname, Msgctxt, Msgid, MsgidPlural, N, LC_MESSAGES)

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static const char *
dcnpgettext_expr (const char *domain,
		  const char *msgctxt, const char *msgid,
		  const char *msgid_plural, unsigned long int n,
		  int category)
{
  size_t msgctxt_len = strlen (msgctxt) + 1;
  size_t msgid_len = strlen (msgid) + 1;
  const char *translation;
#if _LIBGETTEXT_HAVE_VARIABLE_SIZE_ARRAYS
  char msg_ctxt_id[msgctxt_len + msgid_len];
#else
  char buf[1024];
  char *msg_ctxt_id =
    (msgctxt_len + msgid_len <= sizeof (buf)
     ? buf
     : (char *) malloc (msgctxt_len + msgid_len));
  if (msg_ctxt_id != NULL)
#endif
    {
      memcpy (msg_ctxt_id, msgctxt, msgctxt_len - 1);
      msg_ctxt_id[msgctxt_len - 1] = '\004';
      memcpy (msg_ctxt_id + msgctxt_len, msgid, msgid_len);
      translation = dcngettext (domain, msg_ctxt_id, msgid_plural, n, category);
#if !_LIBGETTEXT_HAVE_VARIABLE_SIZE_ARRAYS
      if (msg_ctxt_id != buf)
	free (msg_ctxt_id);
#endif
      if (!(translation == msg_ctxt_id || translation == msgid_plural))
	return translation;
    }
  return (n == 1 ? msgid : msgid_plural);
}

#ifdef __STRICT_ANSI_FORCED__
#undef __STRICT_ANSI__
#undef __STRICT_ANSI_FORCED__
#endif

#endif /* _LIBGETTEXT_H */
