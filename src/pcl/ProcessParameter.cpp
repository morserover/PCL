// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/ProcessParameter.cpp - Released 2014/11/14 17:17:01 UTC
// ****************************************************************************
// This file is part of the PixInsight Class Library (PCL).
// PCL is a multiplatform C++ framework for development of PixInsight modules.
//
// Copyright (c) 2003-2014, Pleiades Astrophoto S.L. All Rights Reserved.
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ****************************************************************************

#include <pcl/AutoLock.h>
#include <pcl/Process.h>
#include <pcl/ProcessParameter.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

class ProcessParameterPrivate
{
public:

   meta_parameter_handle handle;
   Process*              process;

   ProcessParameterPrivate() : handle( 0 ), process( 0 )
   {
   }

   ProcessParameterPrivate( meta_parameter_handle hParam ) : handle( 0 ), process( 0 )
   {
      if ( hParam != 0 )
      {
         meta_process_handle hProcess = (*API->Process->GetParameterProcess)( hParam );
         if ( hProcess == 0 )
            throw Error( "ProcessParameter: Internal error: Invalid process handle" );

         handle = hParam;
         process = new Process( hProcess );
      }
   }

   ~ProcessParameterPrivate()
   {
      if ( process != 0 )
         delete process, process = 0;
      handle = 0;
   }
};

// ----------------------------------------------------------------------------

api_bool InternalParameterEnumerator::ParameterCallback( meta_parameter_handle hParam, void* data )
{
   reinterpret_cast<Process::parameter_list*>( data )->Add( ProcessParameter( hParam ) );
   return api_true;
}

// ----------------------------------------------------------------------------

ProcessParameter::ProcessParameter( const Process& process, const IsoString& paramId ) :
data( new ProcessParameterPrivate( (*API->Process->GetParameterByName)( process.Handle(), paramId.c_str() ) ) )
{
   if ( data->handle == 0 )
   {
      if ( paramId.IsEmpty() )
         throw Error( "ProcessParameter: Empty process parameter identifier specified" );
      if ( !paramId.IsValidIdentifier() )
         throw Error( "ProcessParameter: Invalid process parameter identifier specified: \'" + paramId + '\'' );

      throw Error( "ProcessParameter: No parameter was found "
                   "with the specified identifier \'" + paramId + "\' for process \'" + process.Id() + '\'' );
   }
}

ProcessParameter::ProcessParameter( const ProcessParameter& table, const IsoString& colId ) :
data( new ProcessParameterPrivate( (*API->Process->GetTableColumnByName)( table.data->handle, colId.c_str() ) ) )
{
   if ( data->handle == 0 )
   {
      if ( table.IsNull() )
         throw Error( "ProcessParameter: Null table parameter" );
      if ( colId.IsEmpty() )
         throw Error( "ProcessParameter: Empty table column parameter identifier specified" );
      if ( !colId.IsValidIdentifier() )
         throw Error( "ProcessParameter: Invalid table column parameter identifier specified: \'" + colId + '\'' );
      if ( !table.IsTable() )
         throw Error( "ProcessParameter: The specified parameter \'" + table.Id() + "\' is not a table parameter" );

      throw Error( "ProcessParameter: No table column parameter was found "
                   "with the specified identifier \'" + colId + "\' "
                   "for table parameter \'" + table.Id() + "\' "
                   "of process \'" + table.ParentProcess().Id() + '\'' );
   }
}

ProcessParameter::ProcessParameter( const ProcessParameter& p ) :
data( new ProcessParameterPrivate( p.data->handle ) )
{
}

ProcessParameter::ProcessParameter( const void* hParam ) :
data( new ProcessParameterPrivate( hParam ) )
{
}

ProcessParameter::~ProcessParameter()
{
   if ( data != 0 )
      delete data, data = 0;
}

// ----------------------------------------------------------------------------

bool ProcessParameter::IsNull() const
{
   return data->handle == 0;
}

ProcessParameter& ProcessParameter::Null()
{
   static ProcessParameter* nullParameter = 0;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullParameter == 0 )
      nullParameter = new ProcessParameter( (const void*)0 );
   return *nullParameter;
}

// ----------------------------------------------------------------------------

Process& ProcessParameter::ParentProcess() const
{
   if ( data->process == 0 )
      throw Error( "ProcessParameter::ParentProcess(): Invoked for a null ProcessParameter object" );
   return *data->process;
}

ProcessParameter ProcessParameter::ParentTable() const
{
   return ProcessParameter( (*API->Process->GetParameterTable)( data->handle ) );
}

// ----------------------------------------------------------------------------

IsoString ProcessParameter::Id() const
{
   size_type len = 0;
   (*API->Process->GetParameterIdentifier)( data->handle, 0, &len );

   IsoString id;

   if ( len != 0 )
   {
      id.Reserve( len );
      if ( (*API->Process->GetParameterIdentifier)( data->handle, id.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetParameterIdentifier" );
   }

   return id;
}

// ----------------------------------------------------------------------------

IsoStringList ProcessParameter::Aliases() const
{
   size_type len = 0;
   (*API->Process->GetParameterAliasIdentifiers)( data->handle, 0, &len );

   IsoStringList aliases;

   if ( len != 0 )
   {
      IsoString csAliases;
      csAliases.Reserve( len );
      if ( (*API->Process->GetParameterAliasIdentifiers)( data->handle, csAliases.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetParameterAliasIdentifiers" );

      csAliases.Break( aliases, ',' );
   }

   return aliases;
}

// ----------------------------------------------------------------------------

bool ProcessParameter::IsRequired() const
{
   return (*API->Process->GetParameterRequired)( data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool ProcessParameter::IsReadOnly() const
{
   return (*API->Process->GetParameterReadOnly)( data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

String ProcessParameter::Description() const
{
   size_type len = 0;
   (*API->Process->GetParameterDescription)( data->handle, 0, &len );

   String description;

   if ( len != 0 )
   {
      description.Reserve( len );
      if ( (*API->Process->GetParameterDescription)( data->handle, description.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetParameterDescription" );
   }

   return description;
}

// ----------------------------------------------------------------------------

String ProcessParameter::ScriptComment() const
{
   size_type len = 0;
   (*API->Process->GetParameterScriptComment)( data->handle, 0, &len );

   String comment;

   if ( len != 0 )
   {
      comment.Reserve( len );
      if ( (*API->Process->GetParameterScriptComment)( data->handle, comment.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetParameterScriptComment" );
   }

   return comment;
}

// ----------------------------------------------------------------------------

ProcessParameter::data_type ProcessParameter::Type() const
{
   if ( IsNull() )
      return ProcessParameterType::Invalid;

   uint32 apiType = (*API->Process->GetParameterType)( data->handle ) & PTYPE_TYPE_MASK;
   if ( apiType == 0 )
      throw APIFunctionError( "GetParameterType" );

   switch ( apiType )
   {
   case PTYPE_UINT8:  return ProcessParameterType::UInt8;
   case PTYPE_INT8:   return ProcessParameterType::Int8;
   case PTYPE_UINT16: return ProcessParameterType::UInt16;
   case PTYPE_INT16:  return ProcessParameterType::Int16;
   case PTYPE_UINT32: return ProcessParameterType::UInt32;
   case PTYPE_INT32:  return ProcessParameterType::Int32;
   case PTYPE_UINT64: return ProcessParameterType::UInt64;
   case PTYPE_INT64:  return ProcessParameterType::Int64;
   case PTYPE_FLOAT:  return ProcessParameterType::Float;
   case PTYPE_DOUBLE: return ProcessParameterType::Double;
   case PTYPE_BOOL:   return ProcessParameterType::Boolean;
   case PTYPE_ENUM:   return ProcessParameterType::Enumeration;
   case PTYPE_STRING: return ProcessParameterType::String;
   case PTYPE_TABLE:  return ProcessParameterType::Table;
   case PTYPE_BLOCK:  return ProcessParameterType::Block;
   default:
      throw Error( "ProcessParameter::Type(): Internal error: Unknown parameter type" );
   }
}

ProcessParameter::data_type ProcessParameter::DataInterpretation() const
{
   if ( !IsBlock() )
      return ProcessParameterType::Invalid;

   uint32 apiType = (*API->Process->GetParameterType)( data->handle ) & PTYPE_BLOCK_INTERPRETATION;
   if ( apiType == 0 )
      throw APIFunctionError( "GetParameterType" );

   switch ( apiType )
   {
   case PTYPE_BLOCK_UI8:    return ProcessParameterType::UInt8;
   case PTYPE_BLOCK_I8:     return ProcessParameterType::Int8;
   case PTYPE_BLOCK_UI16:   return ProcessParameterType::UInt16;
   case PTYPE_BLOCK_I16:    return ProcessParameterType::Int16;
   case PTYPE_BLOCK_UI32:   return ProcessParameterType::UInt32;
   case PTYPE_BLOCK_I32:    return ProcessParameterType::Int32;
   case PTYPE_BLOCK_UI64:   return ProcessParameterType::UInt64;
   case PTYPE_BLOCK_I64:    return ProcessParameterType::Int64;
   case PTYPE_BLOCK_FLOAT:  return ProcessParameterType::Float;
   case PTYPE_BLOCK_DOUBLE: return ProcessParameterType::Double;
   default:
      throw Error( "ProcessParameter::DataInterpretation(): Internal error: Unknown parameter type" );
   }
}

// ----------------------------------------------------------------------------

Variant ProcessParameter::DefaultValue() const
{
   if ( IsNull() )
      return Variant();

   uint32 apiType = (*API->Process->GetParameterType)( data->handle ) & PTYPE_TYPE_MASK;
   if ( apiType == 0 )
      throw APIFunctionError( "GetParameterType" );

   if ( apiType == PTYPE_TABLE )
      throw Error( "ProcessParameter::DefaultValue(): Invoked for a table parameter" );

   if ( apiType == PTYPE_BLOCK )
      return ByteArray();

   if ( apiType == PTYPE_STRING )
   {
      size_type len = 0;
      (*API->Process->GetParameterDefaultValue)( data->handle, 0, &len );

      String value;

      if ( len != 0 )
      {
         value.Reserve( len );
         if ( (*API->Process->GetParameterDefaultValue)( data->handle, value.c_str(), &len ) == api_false )
            throw APIFunctionError( "GetParameterDefaultValue" );
      }

      return value;
   }

   if ( apiType == PTYPE_ENUM )
   {
      int index = (*API->Process->GetParameterDefaultElementIndex)( data->handle );
      if ( index < 0 )
         throw APIFunctionError( "GetParameterDefaultElementIndex" );
      return index;
   }

   union
   {
      uint8    u8;
      int8     i8;
      uint16   u16;
      int16    i16;
      uint32   u32;
      int32    i32;
      uint64   u64;
      int64    i64;
      float    f;
      double   d;
      api_bool b;
   }
   value;

   if ( (*API->Process->GetParameterDefaultValue)( data->handle, &value, 0 ) == api_false )
      throw APIFunctionError( "GetParameterDefaultValue" );

   switch ( apiType )
   {
   case PTYPE_UINT8:  return value.u8;
   case PTYPE_INT8:   return value.i8;
   case PTYPE_UINT16: return value.u16;
   case PTYPE_INT16:  return value.i16;
   case PTYPE_UINT32: return value.u32;
   case PTYPE_INT32:  return value.i32;
   case PTYPE_UINT64: return value.u64;
   case PTYPE_INT64:  return value.i64;
   case PTYPE_FLOAT:  return value.f;
   case PTYPE_DOUBLE: return value.d;
   case PTYPE_BOOL:   return value.b;
   default:
      throw Error( "ProcessParameter::DefaultValue(): Internal error: Unknown parameter type" );
   }
}

// ----------------------------------------------------------------------------

void ProcessParameter::GetNumericRange( double& minValue, double& maxValue ) const
{
   if ( !IsNumeric() )
   {
      minValue = maxValue = 0;
      return;
   }

   if ( (*API->Process->GetParameterRange)( data->handle, &minValue, &maxValue ) == api_false )
      throw APIFunctionError( "GetParameterRange" );
}

// ----------------------------------------------------------------------------

int ProcessParameter::Precision() const
{
   if ( !IsReal() )
      return 0;

   return (*API->Process->GetParameterPrecision)( data->handle );
}

// ----------------------------------------------------------------------------

bool ProcessParameter::ScientificNotation() const
{
   if ( !IsReal() )
      return false;

   return (*API->Process->GetParameterScientificNotation)( data->handle ) != api_false;
}

// ----------------------------------------------------------------------------

void ProcessParameter::GetLengthLimits( size_type& minLength, size_type& maxLength ) const
{
   if ( !IsVariableLength() )
   {
      minLength = maxLength = 0;
      return;
   }

   if ( (*API->Process->GetParameterLengthLimits)( data->handle, &minLength, &maxLength ) == api_false )
      throw APIFunctionError( "GetParameterLengthLimits" );
}

// ----------------------------------------------------------------------------

ProcessParameter::enumeration_element_list ProcessParameter::EnumerationElements() const
{
   if ( !IsEnumeration() )
      return enumeration_element_list();

   size_type count = (*API->Process->GetParameterElementCount)( data->handle );
   if ( count == 0 )
      throw APIFunctionError( "GetParameterElementCount" );

   enumeration_element_list elements( count );

   for ( size_type i = 0; i < count; ++i )
   {
      size_type len = 0;
      (*API->Process->GetParameterElementIdentifier)( data->handle, i, 0, &len );
      if ( len == 0 )
         throw APIFunctionError( "GetParameterElementIdentifier" );
      elements[i].id.Reserve( len );
      if ( (*API->Process->GetParameterElementIdentifier)( data->handle, i, elements[i].id.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetParameterElementIdentifier" );

      len = 0;
      (*API->Process->GetParameterElementAliasIdentifiers)( data->handle, i, 0, &len );
      if ( len > 0 )
      {
         IsoString aliases;
         aliases.Reserve( len );
         if ( (*API->Process->GetParameterElementAliasIdentifiers)( data->handle, i, aliases.c_str(), &len ) == api_false )
            throw APIFunctionError( "GetParameterElementAliasIdentifiers" );
         aliases.Break( elements[i].aliases, ',' );
      }

      elements[i].value = (*API->Process->GetParameterElementValue)( data->handle, i );
   }

   return elements;
}

// ----------------------------------------------------------------------------

String ProcessParameter::AllowedCharacters() const
{
   String allowed;

   if ( IsString() )
   {
      size_type len = 0;
      (*API->Process->GetParameterAllowedCharacters)( data->handle, 0, &len );

      if ( len != 0 )
      {
         allowed.Reserve( len );
         if ( (*API->Process->GetParameterAllowedCharacters)( data->handle, allowed.c_str(), &len ) == api_false )
            throw APIFunctionError( "GetParameterAllowedCharacters" );
      }
   }

   return allowed;
}

// ----------------------------------------------------------------------------

ProcessParameter::parameter_list ProcessParameter::TableColumns() const
{
   parameter_list parameters;
   if ( IsTable() )
      if ( (*API->Process->EnumerateTableColumns)( data->handle,
                                    InternalParameterEnumerator::ParameterCallback, &parameters ) == api_false )
         throw APIFunctionError( "EnumerateTableColumns" );
   return parameters;
}

// ----------------------------------------------------------------------------

const void* ProcessParameter::Handle() const
{
   return data->handle;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/ProcessParameter.cpp - Released 2014/11/14 17:17:01 UTC
