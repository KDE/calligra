/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef __rdf_RdfForward_h__
#define __rdf_RdfForward_h__

#include <QExplicitlySharedDataPointer>

class KoDocumentRdf;
class KoRdfSemanticItem;
class KoSemanticStylesheet;
class KoDocumentRdfEditWidget;
class KoTextInlineRdf;
class KoRdfPrefixMapping;
class KoRdfSemanticTreeWidgetItem;
class KoTextEditor;
class KoRdfBasicSemanticItem;

namespace Soprano
{
class Model;
class Statement;
class Node;
}

typedef QExplicitlySharedDataPointer<KoRdfSemanticItem> hKoRdfSemanticItem;
typedef QExplicitlySharedDataPointer<KoSemanticStylesheet> hKoSemanticStylesheet;
typedef QExplicitlySharedDataPointer<KoRdfBasicSemanticItem> hKoRdfBasicSemanticItem;

#endif
