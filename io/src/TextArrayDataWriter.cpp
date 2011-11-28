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

#include "StringUtil.h"
#include "FieldmlIoApi.h"

#include "ArrayDataWriter.h"
#include "TextArrayDataWriter.h"

using namespace std;


/**
 * A pseudo-lambda class that sets/appends the result of a text-array writing operation to a string-based FieldML
 * data resource 
 */
class SetStringResourceTask :
    public StreamCloseTask
{
private:
    const FmlSessionHandle sessionHandle;
    const FmlObjectHandle dataResource;
    const bool append;
    
public:
    SetStringResourceTask( FmlSessionHandle _sessionHandle, FmlObjectHandle _dataResource, bool _append ) :
        sessionHandle( _sessionHandle ),
        dataResource( _dataResource ),
        append( _append )
    {
    }
    

    FmlIoErrorNumber onStreamClose( const std::string &info )
    {
        FmlIoErrorNumber err;
        
        if( append )
        {
            err = Fieldml_AddInlineData( sessionHandle, dataResource, info.c_str(), info.size() );
        }
        else
        {
            err = Fieldml_SetInlineData( sessionHandle, dataResource, info.c_str(), info.size() );
        }
        
        return err;
    }
    
    
    virtual ~SetStringResourceTask() {}
};

/**
 * A pseudo-lambda class that removes the need to duplicate the slab and slice writing implementations.
 * No point in making this a template class, as we need to use a different method on stream depending on the type,
 * and there's no superclass functionality, so template specialization is redundant.
 */
class BufferWriter
{
protected:
    int bufferPos;
    FieldmlOutputStream * const stream;
    
public:
    BufferWriter( FieldmlOutputStream *_stream ) :
        stream( _stream ), bufferPos( 0 ) {}
    
    virtual ~BufferWriter() {}
    
    virtual void write( int count ) = 0;
};


class DoubleBufferWriter :
    public BufferWriter
{
private:
    double * const buffer;
    
public:
    DoubleBufferWriter( FieldmlOutputStream *_stream, double *_buffer ) :
        BufferWriter( _stream ), buffer( _buffer ) {}
    
    void write( int count )
    {
        for( int i = 0; i < count; i++ )
        {
            stream->writeDouble( buffer[bufferPos++] );
        }
    }
};


class IntBufferWriter :
    public BufferWriter
{
private:
    int * const buffer;
    
public:
    IntBufferWriter( FieldmlOutputStream *_stream, int *_buffer ) :
        BufferWriter( _stream ), buffer( _buffer ) {}
    
    void write( int count )
    {
        for( int i = 0; i < count; i++ )
        {
            stream->writeInt( buffer[bufferPos++] );
        }
    }
};


class BooleanBufferWriter :
    public BufferWriter
{
private:
    bool * const buffer;
    
public:
    BooleanBufferWriter( FieldmlOutputStream *_stream, bool *_buffer ) :
        BufferWriter( _stream ), buffer( _buffer ) {}
    
    void write( int count )
    {
        for( int i = 0; i < count; i++ )
        {
            stream->writeBoolean( buffer[bufferPos++] );
        }
    }
};


TextArrayDataWriter *TextArrayDataWriter::create( FieldmlIoContext *context, string root, FmlObjectHandle source, FieldmlHandleType handleType, bool append, int *sizes, int rank )
{
    TextArrayDataWriter *writer = NULL;
    
    FmlObjectHandle resource = Fieldml_GetDataSourceResource( context->getSession(), source );
    string format;
    
    if( !StringUtil::safeString( Fieldml_GetDataResourceFormat( context->getSession(), resource ), format ) )
    {
        context->setError( FML_IOERR_CORE_ERROR );
        return NULL;
    }
    
    if( format != StringUtil::PLAIN_TEXT_NAME )
    {
        context->setError( FML_IOERR_UNSUPPORTED );
        return writer;
    }
    
    writer = new TextArrayDataWriter( context, root, source, handleType, append, sizes, rank );
    if( !writer->ok )
    {
        delete writer;
        writer = NULL;
    }
    
    return writer;
}


TextArrayDataWriter::TextArrayDataWriter( FieldmlIoContext *_context, const string root, FmlObjectHandle _source, FieldmlHandleType handleType, bool append, int *sizes, int _rank ) :
    ArrayDataWriter( _context ),
    source( _source ),
    sourceSizes( NULL ),
    closed( false )
{
    offset = 0;
    
    ok = false;
    
    sourceRank = Fieldml_GetArrayDataSourceRank( context->getSession(), source );
    if( sourceRank <= 0 )
    {
        return;
    }

    sourceSizes = new int[sourceRank];
    Fieldml_GetArrayDataSourceSizes( context->getSession(), source, sourceSizes );
    
    FmlObjectHandle resource = Fieldml_GetDataSourceResource( context->getSession(), source );
    DataResourceType type = Fieldml_GetDataResourceType( context->getSession(), resource );
    
    if( type == DATA_RESOURCE_HREF )
    {
        string href;
        string path;
        
        if( !StringUtil::safeString( Fieldml_GetDataResourceHref( context->getSession(), resource ), href ) )
        {
            context->setError( FML_IOERR_CORE_ERROR );
        }
        else
        {
            string path = StringUtil::makeFilename( root, href );
            stream = FieldmlOutputStream::createTextFileStream( path, append );
        }
    }
    else if( type == DATA_RESOURCE_INLINE )
    {
        StreamCloseTask *task = new SetStringResourceTask( context->getSession(), resource, append );
        stream = FieldmlOutputStream::createStringStream( task );
    }
    
    if( stream != NULL )
    {
        ok = true;
    }
}


FmlIoErrorNumber TextArrayDataWriter::writeSlice( int *sizes, int depth, BufferWriter &writer )
{
    if( depth == sourceRank - 1 )
    {
        writer.write( sizes[depth] );
        return FML_IOERR_NO_ERROR;
    }
    
    int err;
    for( int i = 0; i < sizes[depth]; i++ )
    {
        err = writeSlice( sizes, depth + 1, writer );
        if( err != FML_IOERR_NO_ERROR )
        {
            return err;
        }
    }
    
    return FML_IOERR_NO_ERROR;
}
    

FmlIoErrorNumber TextArrayDataWriter::writeSlab( int *offsets, int *sizes, BufferWriter &writer )
{
    if( offsets[0] != offset )
    {
        return context->setError( FML_IOERR_UNSUPPORTED );
    }
    
    for( int i = 1; i < sourceRank; i++ )
    {
        if( offsets[i] != 0 )
        {
            return context->setError( FML_IOERR_UNSUPPORTED );
        }
        
        if( sizes[i] != sourceSizes[i] )
        {
            return context->setError( FML_IOERR_UNSUPPORTED );
        }
    }
    
    int err = writeSlice( sizes, 0, writer );

    if( err == FML_IOERR_NO_ERROR )
    {
        offset += sizes[0];
    }

    return err;
}


FmlIoErrorNumber TextArrayDataWriter::writeIntSlab( int *offsets, int *sizes, int *valueBuffer )
{
    if( closed )
    {
        return FML_IOERR_RESOURCE_CLOSED;
    }
    
    IntBufferWriter writer( stream, valueBuffer );
    
    return writeSlab( offsets, sizes, writer );
}


FmlIoErrorNumber TextArrayDataWriter::writeDoubleSlab( int *offsets, int *sizes, double *valueBuffer )
{
    if( closed )
    {
        return FML_IOERR_RESOURCE_CLOSED;
    }
    
    DoubleBufferWriter writer( stream, valueBuffer );
    
    return writeSlab( offsets, sizes, writer );
}


FmlIoErrorNumber TextArrayDataWriter::writeBooleanSlab( int *offsets, int *sizes, bool *valueBuffer )
{
    if( closed )
    {
        return FML_IOERR_RESOURCE_CLOSED;
    }
    
    BooleanBufferWriter writer( stream, valueBuffer );
    
    return writeSlab( offsets, sizes, writer );
}


FmlIoErrorNumber TextArrayDataWriter::close()
{
    if( closed )
    {
        return FML_IOERR_NO_ERROR;
    }
    
    closed = true;

    //TODO: This behaviour should be controllable from elsewhere.
    stream->writeNewline();
    
    return stream->close();
}


TextArrayDataWriter::~TextArrayDataWriter()
{
    if( stream != NULL )
    {
        delete stream;
    }
    
    delete sourceSizes;
}
