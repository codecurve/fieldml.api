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

#include <algorithm>

#include "string_const.h"
#include "fieldml_structs.h"
#include "fieldml_sax.h"
#include "fieldml_library_0.3.h"

using namespace std;

static vector<FieldmlSession *> sessions;

FieldmlSession *FieldmlSession::handleToSession( FmlHandle handle )
{
    if( ( handle < 0 ) || ( handle >= sessions.size() ) )
    {
        return NULL;
    }
    
    return sessions.at( handle );
}


long FieldmlSession::addSession( FieldmlSession *session )
{
    sessions.push_back( session );
    return sessions.size() - 1;
}


FieldmlSession::FieldmlSession()
{
    handle = addSession( this );
    
    region = NULL;
}


FieldmlSession::~FieldmlSession()
{
    sessions[handle] = NULL;
}


int FieldmlSession::setErrorAndLocation( const char *file, const int line, const int error )
{
    lastError = error;

    if( ( error != FML_ERR_NO_ERROR ) && ( error != FML_ERR_IO_NO_DATA ) )
    {
        if( debug )
        {
            printf("FIELDML %s (%s): Error %d at %s:%d\n", FML_VERSION_STRING, __DATE__, error, file, line );
        }
    }
    
    return error;
}


void FieldmlSession::addError( const string string )
{
    errors.push_back( string );
}


const int FieldmlSession::getLastError()
{
    return lastError;
}


const int FieldmlSession::getErrorCount()
{
    return errors.size();
}


const string FieldmlSession::getError( const int index )
{
    if( ( index < 0 ) || ( index >= errors.size() ) )
    {
        return NULL;
    }
    
    return errors[index];
}


void FieldmlSession::setDebug( const int debugValue )
{
    debug = debugValue;
}


FmlHandle FieldmlSession::getHandle()
{
    return handle;
}


void FieldmlSession::logError( const char *error, const char *name1, const char *name2 )
{
    string errorString = error;

    if( name1 != NULL )
    {
        errorString = errorString + ": " + name1;
    }
    if( name2 != NULL )
    {
        errorString = errorString + ":: " + name2;
    }
    
    fprintf( stderr, "%s\n", errorString.c_str() );
    
    addError( errorString );
}