/* \file
 * $Id$
 * \author Caton Little
 * \brief 
 *
 * \section LICENSE
 *
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is FieldML
 *
 * The Initial Developer of the Original Code is Auckland Uniservices Ltd,
 * Auckland, New Zealand. Portions created by the Initial Developer are
 * Copyright (C) 2010 the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 */

#include <cstring>
#include <cstdio>

#include <libxml/SAX.h>
#include <libxml/globals.h>
#include <libxml/xmlerror.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml/xmlschemas.h>

#include "String_InternalLibrary.h"
#include "String_InternalXSD.h"
#include "string_const.h"

#include "fieldml_sax.h"
#include "SaxHandlers.h"

using namespace std;


//========================================================================

void addSaxContextError( void *context, const char *msg, ... )
{
    SaxContext *saxContext = (SaxContext*)context;

    char message[256];

    va_list vargs;  
    va_start( vargs, msg );  
    int retval = vsnprintf( message, 255, msg, vargs );  
    va_end( vargs);
    
    if( retval > 0 )
    {
        //libxml likes to put \n at the end of its error messages
        if( message[retval-1] == '\n' )
        {
            message[retval-1] = 0;
        }
        saxContext->session->addError( message );
    }
}

//========================================================================

class SaxParser
{
public:
    virtual int parse( xmlSAXHandlerPtr saxHandler, SaxContext *context ) = 0;
    
    virtual int validate( SaxContext *context, xmlParserInputBufferPtr buffer, const char *resourceName );
    
    virtual const char *getSource() = 0; 
};


int SaxParser::validate( SaxContext *context, xmlParserInputBufferPtr buffer, const char *resourceName )
{
    xmlSchemaPtr schemas = NULL;
    xmlSchemaParserCtxtPtr sctxt;
    xmlSchemaValidCtxtPtr vctxt;
    
    LIBXML_TEST_VERSION

    xmlSubstituteEntitiesDefault( 1 );

    if( buffer == NULL )
    {
        return false;
    }

    sctxt = xmlSchemaNewMemParserCtxt( FML_STRING_FIELDML_XSD, strlen( FML_STRING_FIELDML_XSD ) );
    xmlSchemaSetParserErrors( sctxt, (xmlSchemaValidityErrorFunc)addSaxContextError, (xmlSchemaValidityWarningFunc)addSaxContextError, context );
    schemas = xmlSchemaParse( sctxt );
    if( schemas == NULL )
    {
        xmlGenericError( xmlGenericErrorContext, "Internal schema failed to compile\n" );
    }
    xmlSchemaFreeParserCtxt( sctxt );

    vctxt = xmlSchemaNewValidCtxt( schemas );
    xmlSchemaSetValidErrors( vctxt, (xmlSchemaValidityErrorFunc)addSaxContextError, (xmlSchemaValidityWarningFunc)addSaxContextError, context );

    int result = xmlSchemaValidateStream( vctxt, buffer, (xmlCharEncoding)0, NULL, NULL );

    xmlSchemaFreeValidCtxt( vctxt );

    xmlSchemaFree( schemas );
    
    return result;
}

//========================================================================

class SaxFileParser :
    public SaxParser
{
private:
    const char *filename;
    
public:
    SaxFileParser( const char *_filename );
    
    virtual int parse( xmlSAXHandlerPtr saxHandler, SaxContext *context );
    
    virtual const char *getSource();
};


SaxFileParser::SaxFileParser( const char *_filename ) :
    filename( _filename )
{
}


int SaxFileParser::parse( xmlSAXHandlerPtr saxHandler, SaxContext *context )
{
    xmlParserInputBufferPtr buffer = xmlParserInputBufferCreateFilename( filename, XML_CHAR_ENCODING_NONE );
    
    int res = validate( context, buffer, filename );
    
    if( res != 0 )
    {
        string error = filename;
        error += " failed to validate";
        context->session->addError( error );
        return res;
    }

    return xmlSAXUserParseFile( saxHandler, context, filename );
}


const char *SaxFileParser::getSource()
{
    return filename;
}

//========================================================================

class SaxStringParser :
    public SaxParser
{
private:
    const char *string;
    
    const char *stringDescription;
    
public:
    SaxStringParser( const char *_string, const char *_stringDescription );
    
    virtual int parse( xmlSAXHandlerPtr saxHandler, SaxContext *context );
    
    virtual const char *getSource();
};


SaxStringParser::SaxStringParser( const char *_string, const char *_stringDescription ) :
    string( _string ),
    stringDescription( _stringDescription )
{
}


int SaxStringParser::parse( xmlSAXHandlerPtr saxHandler, SaxContext *context )
{
    xmlParserInputBufferPtr buffer = xmlParserInputBufferCreateMem( string, strlen( string ), XML_CHAR_ENCODING_NONE );
    
    int res = validate( context, buffer, stringDescription );
    
    if( res != 0 )
    {
        std::string error = stringDescription;
        error += " failed to validate";
        context->session->addError( error );
        return res;
    }

    return xmlSAXUserParseMemory( saxHandler, context, string, strlen( string ) );
}


const char *SaxStringParser::getSource()
{
    return stringDescription;
}

//========================================================================
//
// SAX handlers
//
//========================================================================

int isStandalone( void *context )
{
    return 0;
}


int hasInternalSubset( void *context )
{
    return 0;
}


int hasExternalSubset( void *context )
{
    return 0;
}


void onInternalSubset( void *context, const xmlChar *name, const xmlChar *externalID, const xmlChar *systemID )
{
}


void externalSubset( void *context, const xmlChar *name, const xmlChar *externalID, const xmlChar *systemID )
{
}


xmlParserInputPtr resolveEntity( void *context, const xmlChar *publicId, const xmlChar *systemId )
{
    return NULL;
}


xmlEntityPtr getEntity( void *context, const xmlChar *name )
{
    return NULL;
}


xmlEntityPtr getParameterEntity( void *context, const xmlChar *name )
{
    return NULL;
}


void onEntityDecl( void *context, const xmlChar *name, int type, const xmlChar *publicId, const xmlChar *systemId, xmlChar *content )
{
}


static void onAttributeDecl( void *context, const xmlChar * elem, const xmlChar * name, int type, int def, const xmlChar * defaultValue, xmlEnumerationPtr tree )
{
    xmlFreeEnumeration( tree );
}


static void onElementDecl( void *context, const xmlChar *name, int type, xmlElementContentPtr content )
{
}


static void onNotationDecl( void *context, const xmlChar *name, const xmlChar *publicId, const xmlChar *systemId )
{
}

static void onUnparsedEntityDecl( void *context, const xmlChar *name, const xmlChar *publicId, const xmlChar *systemId, const xmlChar *notationName )
{
}


void setDocumentLocator( void *context, xmlSAXLocatorPtr loc )
{
    /*
     At the moment (libxml 2.7.2), this is worse than useless.
     The locator only wraps functions which require the library's internal
     parsing context, which is only passed into this function if you pass
     in 'NULL' as the user-data which initiating the SAX parse...
     which is exactly what we don't want to do.
     */
}


static void onStartDocument( void *context )
{
}


static void onEndDocument( void *context )
{
}


static void onCharacters( void *context, const xmlChar *xmlChars, int len )
{
    SaxContext *saxContext = (SaxContext*)context;
    saxContext->handler->onCharacters( xmlChars, len );
}


static void onReference( void *context, const xmlChar *name )
{
}


static void onIgnorableWhitespace( void *context, const xmlChar *ch, int len )
{
}


static void onProcessingInstruction( void *context, const xmlChar *target, const xmlChar *data )
{
}


static void onCdataBlock( void *context, const xmlChar *value, int len )
{
}


void comment( void *context, const xmlChar *value )
{
}


static void XMLCDECL warning( void *context, const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    fprintf( stdout, "SAX.warning: " );
    vfprintf( stdout, msg, args );
    va_end( args );
}


static void XMLCDECL error( void *context, const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    fprintf( stdout, "SAX.error: " );
    vfprintf( stdout, msg, args );
    va_end( args );
}


static void XMLCDECL fatalError( void *context, const char *msg, ... )
{
    va_list args;

    va_start( args, msg );
    fprintf( stdout, "SAX.fatalError: " );
    vfprintf( stdout, msg, args );
    va_end( args );
}


static void onStartElementNs( void *context, const xmlChar *elementName, const xmlChar *prefix, const xmlChar *URI, int nb_namespaces, const xmlChar **namespaces,
    int nb_attributes, int nb_defaulted, const xmlChar **attributes )
{
    SaxAttributes saxAttributes = SaxAttributes( nb_attributes, attributes );
    SaxContext *saxContext = (SaxContext*)context;
    SaxHandler *newHandler = saxContext->handler->onElementStart( elementName, saxAttributes ); 

    if( saxContext->handler != newHandler )
    {
        saxContext->handler = newHandler;
    }
}


static void onEndElementNs( void *context, const xmlChar *elementName, const xmlChar *prefix, const xmlChar *URI )
{
    SaxContext *saxContext = (SaxContext*)context;
    if( saxContext->handler->elementName == NULL )
    {
        //Cannot 'pop' the root handler.
    }
    else if( xmlStrcmp( elementName, saxContext->handler->elementName ) == 0 )
    {
        SaxHandler *newHandler = saxContext->handler->getParent();
        delete saxContext->handler;
        saxContext->handler = newHandler;
    }
}


static xmlSAXHandler SAX2HandlerStruct =
{
    onInternalSubset,
    isStandalone,
    hasInternalSubset,
    hasExternalSubset,
    resolveEntity,
    getEntity,
    onEntityDecl,
    onNotationDecl,
    onAttributeDecl,
    onElementDecl,
    onUnparsedEntityDecl,
    setDocumentLocator,
    onStartDocument,
    onEndDocument,
    NULL,
    NULL,
    onReference,
    onCharacters,
    onIgnorableWhitespace,
    onProcessingInstruction,
    comment,
    warning,
    error,
    fatalError,
    getParameterEntity,
    onCdataBlock,
    externalSubset,
    XML_SAX2_MAGIC,
    NULL,
    onStartElementNs,
    onEndElementNs,
    NULL
};

static xmlSAXHandlerPtr SAX2Handler = &SAX2HandlerStruct;

//========================================================================
//
// Main
//
//========================================================================


static int parseFieldml( SaxParser *parser, FieldmlSession *session )
{
    SaxContext context;
    RootSaxHandler rootHandler( NULL, &context );
    
    context.session = session;
    context.source = parser->getSource();
    context.handler = &rootHandler;

    LIBXML_TEST_VERSION

    xmlSubstituteEntitiesDefault( 1 );

    return parser->parse( SAX2Handler, &context );
}


int parseFieldmlFile( const char *filename, FieldmlSession *session )
{
    SaxFileParser parser( filename );
    
    return parseFieldml( &parser, session );
}


int parseFieldmlString( const char *string, const char *stringDescription, FieldmlSession *session )
{
    SaxStringParser parser( string, stringDescription );
    
    return parseFieldml( &parser, session );
}
