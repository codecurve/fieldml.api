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

#include <string.h>

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "string_const.h"
#include "fieldml_write.h"
#include "fieldml_api.h"

using namespace std;

const char * MY_ENCODING = "ISO-8859-1";

const int tBufferLength = 256;


static void writeObjectName( xmlTextWriterPtr writer, const xmlChar *attribute, FmlSessionHandle handle, FmlObjectHandle object )
{
    xmlTextWriterWriteAttribute( writer, attribute, (const xmlChar*)Fieldml_GetObjectName( handle, object ) );
}


static void writeObjectName( xmlTextWriterPtr writer, const xmlChar *attribute, FmlSessionHandle handle, FmlObjectHandle object, string parentName )
{
    parentName += ".";
    
    string objectName = Fieldml_GetObjectName( handle, object );
    
    if( objectName.compare( 0, parentName.length(), parentName ) == 0 )
    {
        objectName = objectName.substr( parentName.length(), objectName.length() );
    }

    xmlTextWriterWriteAttribute( writer, attribute, (const xmlChar*)objectName.c_str() );
}


static void writeIntTableEntry( xmlTextWriterPtr writer, const xmlChar *tagName, FmlEnsembleValue key, const char *value )
{
    xmlTextWriterStartElement( writer, tagName );
    xmlTextWriterWriteFormatAttribute( writer, KEY_ATTRIB, "%d", key );
    xmlTextWriterWriteAttribute( writer, VALUE_ATTRIB, (const xmlChar*) value );
    xmlTextWriterEndElement( writer );
}


static void writeComponentEvaluator( xmlTextWriterPtr writer, const xmlChar *tagName, const xmlChar *attribName, FmlEnsembleValue key, const char *value )
{
    xmlTextWriterStartElement( writer, tagName );
    xmlTextWriterWriteFormatAttribute( writer, attribName, "%d", key );
    xmlTextWriterWriteAttribute( writer, EVALUATOR_ATTRIB, (const xmlChar*) value );
    xmlTextWriterEndElement( writer );
}


static int writeBinds( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    int count = Fieldml_GetBindCount( handle, object );

    FmlObjectHandle indexVariable = FML_INVALID_HANDLE;
    if( Fieldml_GetObjectType( handle, object ) == FHT_AGGREGATE_EVALUATOR )
    {
        indexVariable = Fieldml_GetIndexEvaluator( handle, object, 1 );
    }
    
    if( ( count <= 0 ) && ( indexVariable == FML_INVALID_HANDLE ) )
    {
        return 0;
    }

    xmlTextWriterStartElement( writer, BINDINGS_TAG );
    
    if( indexVariable != FML_INVALID_HANDLE )
    {
        xmlTextWriterStartElement( writer, BIND_INDEX_TAG );
        xmlTextWriterWriteAttribute( writer, VARIABLE_ATTRIB, (const xmlChar*)Fieldml_GetObjectName( handle, indexVariable ) );
        xmlTextWriterWriteAttribute( writer, INDEX_NUMBER_ATTRIB, (const xmlChar*)"1" );
        xmlTextWriterEndElement( writer );
    }
    
    for( int i = 1; i <= count; i++ )
    {
        FmlObjectHandle source = Fieldml_GetBindEvaluator( handle, object, i );
        FmlObjectHandle variable =  Fieldml_GetBindVariable( handle, object, i );
        if( ( source == FML_INVALID_HANDLE ) || ( variable == FML_INVALID_HANDLE ) )
        {
            continue;
        }

        xmlTextWriterStartElement( writer, BIND_TAG );
        xmlTextWriterWriteAttribute( writer, VARIABLE_ATTRIB, (const xmlChar*)Fieldml_GetObjectName( handle, variable ) );
        xmlTextWriterWriteAttribute( writer, SOURCE_ATTRIB, (const xmlChar*)Fieldml_GetObjectName( handle, source ) );
        xmlTextWriterEndElement( writer );
    }
    
    xmlTextWriterEndElement( writer );
    
    return 0;
}


static int writeVariables( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    int count = Fieldml_GetVariableCount( handle, object );
    if( count <= 0 )
    {
        return 0;
    }

    xmlTextWriterStartElement( writer, VARIABLES_TAG );
    
    for( int i = 1; i <= count; i++ )
    {
        FmlObjectHandle variable = Fieldml_GetVariable( handle, object, i );
        if( variable == FML_INVALID_HANDLE )
        {
            continue;
        }

        xmlTextWriterStartElement( writer, VARIABLE_TAG );
        xmlTextWriterWriteAttribute( writer, NAME_ATTRIB, (const xmlChar*)Fieldml_GetObjectName( handle, variable ) );
        xmlTextWriterEndElement( writer );
    }
    
    xmlTextWriterEndElement( writer );
    
    return 0;
}


static int writeContinuousType( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object, const xmlChar *tagName, string parentName )
{
    xmlTextWriterStartElement( writer, tagName );
    writeObjectName( writer, NAME_ATTRIB, handle, object, parentName );

    FmlObjectHandle componentType = Fieldml_GetTypeComponentEnsemble( handle, object );
    if( componentType != FML_INVALID_HANDLE )
    {
        int count = Fieldml_GetElementCount( handle, componentType );
        if( count > 0 )
        {
            xmlTextWriterStartElement( writer, COMPONENTS_TAG );
            writeObjectName( writer, NAME_ATTRIB, handle, componentType );
            xmlTextWriterWriteFormatAttribute( writer, COUNT_ATTRIB, "%d", count );
            xmlTextWriterEndElement( writer );
        }
    }

    xmlTextWriterEndElement( writer );
    
    return 0;
}


static void writeElements( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object, const xmlChar *tagName )
{
    xmlTextWriterStartElement( writer, tagName );

    EnsembleMembersType type = Fieldml_GetEnsembleMembersType( handle, object );
    if( type == MEMBER_RANGE )
    {
        FmlEnsembleValue min = Fieldml_GetEnsembleMembersMin( handle, object );
        FmlEnsembleValue max = Fieldml_GetEnsembleMembersMax( handle, object );
        int stride = Fieldml_GetEnsembleMembersStride( handle, object );
        
        xmlTextWriterStartElement( writer, MEMBER_RANGE_TAG );
        xmlTextWriterWriteFormatAttribute( writer, MIN_ATTRIB, "%d", min );
        xmlTextWriterWriteFormatAttribute( writer, MAX_ATTRIB, "%d", max );
        if( stride != 1 )
        {
            xmlTextWriterWriteFormatAttribute( writer, STRIDE_ATTRIB, "%d", stride );
        }
        xmlTextWriterEndElement( writer );
    }
    else if( ( type == MEMBER_LIST_DATA ) || ( type == MEMBER_RANGE_DATA ) || ( type == MEMBER_STRIDE_RANGE_DATA ) )
    {
        if( type == MEMBER_LIST_DATA )
        {
            xmlTextWriterStartElement( writer, MEMBER_LIST_DATA_TAG );
        }
        else if( type == MEMBER_RANGE_DATA )
        {
            xmlTextWriterStartElement( writer, MEMBER_RANGE_DATA_TAG );
        }
        else if( type == MEMBER_STRIDE_RANGE_DATA )
        {
            xmlTextWriterStartElement( writer, MEMBER_STRIDE_RANGE_DATA_TAG );
        }

        FmlObjectHandle dataObject = Fieldml_GetDataObject( handle, object );
        if( dataObject != FML_INVALID_HANDLE )
        {
            writeObjectName( writer, DATA_ATTRIB, handle, dataObject );
        }

        int count = Fieldml_GetElementCount( handle, object );
        
        xmlTextWriterWriteFormatAttribute( writer, COUNT_ATTRIB, "%d", count );

        xmlTextWriterEndElement( writer );
    }

    xmlTextWriterEndElement( writer );
}


static int writeEnsembleType( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object, const xmlChar *tagName, string parentName )
{
    if( Fieldml_IsEnsembleComponentType( handle, object ) == 1 )
    {
        return 0;
    }
    
    xmlTextWriterStartElement( writer, tagName );

    writeObjectName( writer, NAME_ATTRIB, handle, object, parentName );
    writeElements( writer, handle, object, MEMBERS_TAG );

    xmlTextWriterEndElement( writer );
    
    return 0;
}


static int writeMeshType( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    xmlTextWriterStartElement( writer, MESH_TYPE_TAG );
    
    writeObjectName( writer, NAME_ATTRIB, handle, object );
    
    FmlObjectHandle elementsType = Fieldml_GetMeshElementsType( handle, object );
    if( elementsType != FML_INVALID_HANDLE )
    {
        writeEnsembleType( writer, handle, elementsType, ELEMENTS_TAG, Fieldml_GetObjectName( handle, object ) );
    }

    FmlObjectHandle xiType = Fieldml_GetMeshXiType( handle, object );
    if( xiType != FML_INVALID_HANDLE )
    {
        writeContinuousType( writer, handle, xiType, XI_TAG, Fieldml_GetObjectName( handle, object ) );
    }
    
    int elementCount = Fieldml_GetElementCount( handle, elementsType );
    
    xmlTextWriterStartElement( writer, MESH_SHAPES_TAG );
    const char *defaultShape = Fieldml_GetMeshDefaultShape( handle, object );
    if( defaultShape != NULL )
    {
        xmlTextWriterWriteFormatAttribute( writer, DEFAULT_ATTRIB, "%s", defaultShape );
    }
    
    //TODO Not robust, especially for sparse ensembles. Intended for replacement anyway.
    for( FmlEnsembleValue i = 1; i <= elementCount; i++ )
    {
        const char *shape = Fieldml_GetMeshElementShape( handle, object, i, 0 );
        if( shape == NULL )
        {
            continue;
        }
        writeIntTableEntry( writer, MESH_SHAPE_TAG, i, shape );
    }
    xmlTextWriterEndElement( writer );

    xmlTextWriterEndElement( writer );
    
    return 0;
}


static int writeDataObject( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    char tBuffer[tBufferLength];

    xmlTextWriterStartElement( writer, DATA_OBJECT_TAG );
    writeObjectName( writer, NAME_ATTRIB, handle, object );

    xmlTextWriterStartElement( writer, SOURCE_TAG );
    
    DataSourceType type = Fieldml_GetDataObjectSourceType( handle, object );
    if( type == SOURCE_INLINE )
    {
        xmlTextWriterStartElement( writer, INLINE_SOURCE_TAG );
        
        int offset = 0;
        int length = 1;
        
        while( length > 0 )
        {
            length = Fieldml_CopyInlineData( handle, object, tBuffer, tBufferLength - 1, offset );
            if( length > 0 )
            {
                xmlTextWriterWriteFormatString( writer, "%s", tBuffer );
                offset += length;
            }
        }
        
        xmlTextWriterEndElement( writer );
    }
    else if( type == SOURCE_TEXT_FILE )
    {
        xmlTextWriterStartElement( writer, TEXT_FILE_SOURCE_TAG );
        
        Fieldml_CopyDataObjectFilename( handle, object, tBuffer, tBufferLength );
        xmlTextWriterWriteAttribute( writer, FILENAME_ATTRIB, (const xmlChar*)tBuffer );

        xmlTextWriterWriteFormatAttribute( writer, FIRST_LINE_ATTRIB, "%d", Fieldml_GetDataObjectFileOffset( handle, object ) );
        
        xmlTextWriterEndElement( writer );
    }
    
    xmlTextWriterEndElement( writer );

    xmlTextWriterStartElement( writer, ENTRIES_TAG );

    xmlTextWriterWriteFormatAttribute( writer, COUNT_ATTRIB, "%d", Fieldml_GetDataObjectEntryCount( handle, object ) );
    xmlTextWriterWriteFormatAttribute( writer, LENGTH_ATTRIB, "%d", Fieldml_GetDataObjectEntryLength( handle, object ) );
    xmlTextWriterWriteFormatAttribute( writer, HEAD_ATTRIB, "%d", Fieldml_GetDataObjectEntryHead( handle, object ) );
    xmlTextWriterWriteFormatAttribute( writer, TAIL_ATTRIB, "%d", Fieldml_GetDataObjectEntryTail( handle, object ) );
    
    xmlTextWriterEndElement( writer );

    xmlTextWriterEndElement( writer );
    
    return 0;
}


static int writeElementSequence( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    xmlTextWriterStartElement( writer, ELEMENT_SEQUENCE_TAG );
    
    writeObjectName( writer, NAME_ATTRIB, handle, object );
    writeObjectName( writer, VALUE_TYPE_ATTRIB, handle, Fieldml_GetValueType( handle, object ) );

    writeElements( writer, handle, object, ELEMENTS_TAG );
    
    xmlTextWriterEndElement( writer );
    
    return 0;
}


static int writeAbstractEvaluator( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    xmlTextWriterStartElement( writer, ABSTRACT_EVALUATOR_TAG );
    
    writeObjectName( writer, NAME_ATTRIB, handle, object );
    writeObjectName( writer, VALUE_TYPE_ATTRIB, handle, Fieldml_GetValueType( handle, object ) );

    writeVariables( writer, handle, object );

    xmlTextWriterEndElement( writer );
    
    return 0;
}


static int writeExternalEvaluator( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    xmlTextWriterStartElement( writer, EXTERNAL_EVALUATOR_TAG );
    
    writeObjectName( writer, NAME_ATTRIB, handle, object );
    writeObjectName( writer, VALUE_TYPE_ATTRIB, handle, Fieldml_GetValueType( handle, object ) );

    writeVariables( writer, handle, object );

    xmlTextWriterEndElement( writer );
    
    return 0;
}


static int writeReferenceEvaluator( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    xmlTextWriterStartElement( writer, REFERENCE_EVALUATOR_TAG );
    
    writeObjectName( writer, NAME_ATTRIB, handle, object );
    writeObjectName( writer, EVALUATOR_ATTRIB, handle, Fieldml_GetReferenceSourceEvaluator( handle, object ) );

    writeVariables( writer, handle, object );
    
    writeBinds( writer, handle, object );

    xmlTextWriterEndElement( writer );
    
    return 0;
}


static int writePiecewiseEvaluator( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    xmlTextWriterStartElement( writer, PIECEWISE_EVALUATOR_TAG );
    
    writeObjectName( writer, NAME_ATTRIB, handle, object );
    writeObjectName( writer, VALUE_TYPE_ATTRIB, handle, Fieldml_GetValueType( handle, object ) );

    writeVariables( writer, handle, object );
    
    writeBinds( writer, handle, object );
    
    xmlTextWriterStartElement( writer, INDEX_EVALUATORS_TAG );
    
    FmlObjectHandle indexEvaluator = Fieldml_GetIndexEvaluator( handle, object, 1 );
    if( indexEvaluator != FML_INVALID_HANDLE )
    {
        xmlTextWriterStartElement( writer, INDEX_EVALUATOR_TAG );

        xmlTextWriterWriteFormatAttribute( writer, EVALUATOR_ATTRIB, "%s", Fieldml_GetObjectName( handle, indexEvaluator ) );
        xmlTextWriterWriteFormatAttribute( writer, INDEX_NUMBER_ATTRIB, "%d", 1 );

        xmlTextWriterEndElement( writer );
    }
    
    xmlTextWriterEndElement( writer );

    int count = Fieldml_GetEvaluatorCount( handle, object );
    FmlObjectHandle defaultEvaluator = Fieldml_GetDefaultEvaluator( handle, object );
    if( ( count > 0 ) || ( defaultEvaluator != FML_INVALID_HANDLE ) )
    {
        xmlTextWriterStartElement( writer, ELEMENT_EVALUATORS_TAG );
        
        if( defaultEvaluator != FML_INVALID_HANDLE )
        {
            xmlTextWriterWriteFormatAttribute( writer, DEFAULT_ATTRIB, "%s", Fieldml_GetObjectName( handle, defaultEvaluator ) );
        }
    
        for( int i = 1; i <= count; i++ )
        {
            FmlEnsembleValue element = Fieldml_GetEvaluatorElement( handle, object, i );
            FmlObjectHandle evaluator = Fieldml_GetEvaluator( handle, object, i );
            if( ( element <= 0 ) || ( evaluator == FML_INVALID_HANDLE ) )
            {
                continue;
            }
            writeComponentEvaluator( writer, ELEMENT_EVALUATOR_TAG, INDEX_VALUE_ATTRIB, element, Fieldml_GetObjectName( handle, evaluator ) );
        }

        xmlTextWriterEndElement( writer );
    }

    xmlTextWriterEndElement( writer );
    
    return 0;
}


static void writeSemidenseIndexes( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object, const int isSparse )
{
    int count = Fieldml_GetSemidenseIndexCount( handle, object, isSparse );
    if( count > 0 )
    {
        if( isSparse )
        {
            xmlTextWriterStartElement( writer, SPARSE_INDEXES_TAG );
        }
        else
        {
            xmlTextWriterStartElement( writer, DENSE_INDEXES_TAG );
        }

        for( int i = 1; i <= count; i++ )
        {
            FmlObjectHandle index = Fieldml_GetSemidenseIndexEvaluator( handle, object, i, isSparse );
            if( index == FML_INVALID_HANDLE )
            {
                continue;
            }
            xmlTextWriterStartElement( writer, INDEX_EVALUATOR_TAG );
            xmlTextWriterWriteAttribute( writer, EVALUATOR_ATTRIB, (const xmlChar*)Fieldml_GetObjectName( handle, index ) );

            FmlObjectHandle order = Fieldml_GetSemidenseIndexOrder( handle, object, i );
            if( order != FML_INVALID_HANDLE )
            {
                xmlTextWriterWriteAttribute( writer, ORDER_ATTRIB, (const xmlChar*)Fieldml_GetObjectName( handle, order ) );
            }

            xmlTextWriterEndElement( writer );
        }

        xmlTextWriterEndElement( writer );
    }
}

static void writeSemidenseData( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    xmlTextWriterStartElement( writer, SEMI_DENSE_DATA_TAG );
    
    FmlObjectHandle dataObject = Fieldml_GetDataObject( handle, object );
    xmlTextWriterWriteAttribute( writer, DATA_ATTRIB, (const xmlChar*)Fieldml_GetObjectName( handle, dataObject ) );
    
    writeSemidenseIndexes( writer, handle, object, 1 );
    writeSemidenseIndexes( writer, handle, object, 0 );

    xmlTextWriterEndElement( writer );
}


static int writeParameterEvaluator( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    if( Fieldml_GetObjectType( handle, object ) != FHT_PARAMETER_EVALUATOR )
    {
        return 1;
    }

    xmlTextWriterStartElement( writer, PARAMETER_EVALUATOR_TAG );

    writeObjectName( writer, NAME_ATTRIB, handle, object );
    writeObjectName( writer, VALUE_TYPE_ATTRIB, handle, Fieldml_GetValueType( handle, object ) );

    writeVariables( writer, handle, object );

    DataDescriptionType description = Fieldml_GetParameterDataDescription( handle, object );
    if( description == DESCRIPTION_SEMIDENSE )
    {
        writeSemidenseData( writer, handle, object );
    }
    
    xmlTextWriterEndElement( writer );
    
    return 0;
}


static int writeAggregateEvaluator( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    xmlTextWriterStartElement( writer, AGGREGATE_EVALUATOR_TAG );
    writeObjectName( writer, NAME_ATTRIB, handle, object );
    writeObjectName( writer, VALUE_TYPE_ATTRIB, handle, Fieldml_GetValueType( handle, object ) );

    writeVariables( writer, handle, object );
    
    writeBinds( writer, handle, object );
    
    int count = Fieldml_GetEvaluatorCount( handle, object );
    FmlObjectHandle defaultEvaluator = Fieldml_GetDefaultEvaluator( handle, object );
    
    if( ( count > 0 ) || ( defaultEvaluator != FML_INVALID_HANDLE ) )
    {
        xmlTextWriterStartElement( writer, COMPONENT_EVALUATORS_TAG );
        
        if( defaultEvaluator != FML_INVALID_HANDLE )
        {
            xmlTextWriterWriteFormatAttribute( writer, DEFAULT_ATTRIB, "%s", Fieldml_GetObjectName( handle, defaultEvaluator ) );
        }
    
        for( int i = 1; i <= count; i++ )
        {
            FmlEnsembleValue element = Fieldml_GetEvaluatorElement( handle, object, i );
            FmlObjectHandle evaluator = Fieldml_GetEvaluator( handle, object, i );
            if( ( element <= 0 ) || ( evaluator == FML_INVALID_HANDLE ) )
            {
                continue;
            }
            writeComponentEvaluator( writer, COMPONENT_EVALUATOR_TAG, COMPONENT_ATTRIB, element, Fieldml_GetObjectName( handle, evaluator ) );
        }

        xmlTextWriterEndElement( writer );
    }
    
    xmlTextWriterEndElement( writer );

    return 0;
}


static int writeFieldmlObject( xmlTextWriterPtr writer, FmlSessionHandle handle, FmlObjectHandle object )
{
    switch( Fieldml_GetObjectType( handle, object ) )
    {
    case FHT_CONTINUOUS_TYPE:
        return writeContinuousType( writer, handle, object, CONTINUOUS_TYPE_TAG, "" );
    case FHT_ENSEMBLE_TYPE:
        return writeEnsembleType( writer, handle, object, ENSEMBLE_TYPE_TAG, "" );
    case FHT_MESH_TYPE:
        return writeMeshType( writer, handle, object );
    case FHT_DATA_OBJECT:
        return writeDataObject( writer, handle, object );
//NYI    case FHT_ELEMENT_SEQUENCE:
//        return writeElementSequence( writer, handle, object );
    case FHT_ABSTRACT_EVALUATOR:
        return writeAbstractEvaluator( writer, handle, object );
    case FHT_EXTERNAL_EVALUATOR:
        return writeExternalEvaluator( writer, handle, object );
    case FHT_REFERENCE_EVALUATOR:
        return writeReferenceEvaluator( writer, handle, object );
    case FHT_PIECEWISE_EVALUATOR:
        return writePiecewiseEvaluator( writer, handle, object );
    case FHT_PARAMETER_EVALUATOR:
        return writeParameterEvaluator( writer, handle, object );
    case FHT_AGGREGATE_EVALUATOR:
        return writeAggregateEvaluator( writer, handle, object );
    default:
        break;
    }
    
    return 0;
}


int writeFieldmlFile( FmlSessionHandle handle, const char *filename )
{
    FmlObjectHandle object;
    int i, count, length;
    int rc = 0;
    char tBuffer[tBufferLength];
    xmlTextWriterPtr writer;

    writer = xmlNewTextWriterFilename( filename, 0 );
    if( writer == NULL )
    {
        printf( "testXmlwriterFilename: Error creating the xml writer\n" );
        return 1;
    }

    xmlTextWriterSetIndent( writer, 1 );
    xmlTextWriterStartDocument( writer, NULL, MY_ENCODING, NULL );

    xmlTextWriterStartElement( writer, FIELDML_TAG );
    xmlTextWriterWriteAttribute( writer, VERSION_ATTRIB, (const xmlChar*)FML_VERSION_STRING );
    xmlTextWriterWriteAttribute( writer, (const xmlChar*)"xsi:noNamespaceSchemaLocation", (const xmlChar*)"Fieldml_0.3.5.xsd" );
    xmlTextWriterWriteAttribute( writer, (const xmlChar*)"xmlns:xsi", (const xmlChar*)"http://www.w3.org/2001/XMLSchema-instance" );        
    xmlTextWriterStartElement( writer, REGION_TAG );
    
    const char *regionName = Fieldml_GetRegionName( handle );
    if( ( regionName != NULL ) && ( strlen( regionName ) > 0 ) ) 
    {
        xmlTextWriterWriteAttribute( writer, NAME_ATTRIB, (const xmlChar*)regionName );        
    }
    
    
    int importCount = Fieldml_GetImportSourceCount( handle );
    for( int importSourceIndex = 1; importSourceIndex <= importCount; importSourceIndex++ )
    {
        int objectCount = Fieldml_GetImportCount( handle, importSourceIndex );
        if( objectCount <= 0 )
        {
            continue;
        }

        xmlTextWriterStartElement( writer, IMPORT_TAG );
        
        length = Fieldml_CopyImportSourceLocation( handle, importSourceIndex, tBuffer, tBufferLength );
        if( length > 0 )
        {
            xmlTextWriterWriteAttribute( writer, LOCATION_ATTRIB, (const xmlChar*)tBuffer );        
        }
        
        length = Fieldml_CopyImportSourceRegionName( handle, importSourceIndex, tBuffer, tBufferLength );
        if( length > 0 )
        {
            xmlTextWriterWriteAttribute( writer, REGION_ATTRIB, (const xmlChar*)tBuffer );        
        }
        
        for( int importIndex = 1; importIndex <= objectCount; importIndex++ )
        {
            FmlObjectHandle objectHandle = Fieldml_GetImportObject( handle, importSourceIndex, importIndex );
            if( objectHandle == FML_INVALID_HANDLE )
            {
                continue;
            }
            FieldmlHandleType objectType = Fieldml_GetObjectType( handle, objectHandle );
            if( ( objectType == FHT_CONTINUOUS_TYPE ) ||
                ( objectType == FHT_ENSEMBLE_TYPE ) ||
                ( objectType == FHT_MESH_TYPE ) )
            {
                xmlTextWriterStartElement( writer, IMPORT_TYPE_TAG );
            }
            else
            {
                xmlTextWriterStartElement( writer, IMPORT_EVALUATOR_TAG );
            }
            
            
            length = Fieldml_CopyImportLocalName( handle, importSourceIndex, importIndex, tBuffer, tBufferLength );
            if( length > 0 )
            {
                xmlTextWriterWriteAttribute( writer, LOCAL_NAME_ATTRIB, (const xmlChar*)tBuffer );
            }

            length = Fieldml_CopyImportRemoteName( handle, importSourceIndex, importIndex, tBuffer, tBufferLength );
            if( length > 0 )
            {
                xmlTextWriterWriteAttribute( writer, REMOTE_NAME_ATTRIB, (const xmlChar*)tBuffer );
            }

            xmlTextWriterEndElement( writer );
        }
        
        xmlTextWriterEndElement( writer );
    }
    
    count = Fieldml_GetTotalObjectCount( handle );
    for( i = 1; i <= count; i++ )
    {
        object = Fieldml_GetObjectByIndex( handle, i );
        if( Fieldml_IsObjectLocal( handle, object ) )
        {
            writeFieldmlObject( writer, handle, object );
        }
        else
        {
            //Not a local object. Don't serialize it.
        }
    }

    rc = xmlTextWriterEndDocument( writer );
    if( rc < 0 )
    {
        printf( "testXmlwriterFilename: Error at xmlTextWriterEndDocument\n" );
        return 1;
    }

    xmlFreeTextWriter( writer );

    return 0;
}
