// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/FileFormatInstance.cpp - Released 2014/11/14 17:17:00 UTC
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
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

namespace pcl
{

// ----------------------------------------------------------------------------

static void APIImageInfoToPCL( ImageInfo& info, const api_image_info& i )
{
   info.width            = int( i.width );
   info.height           = int( i.height );
   info.numberOfChannels = int( i.numberOfChannels );
   info.colorSpace       = int( i.colorSpace );
   info.supported        = i.supported;
}

static void APIImageOptionsToPCL( ImageOptions& opt, const api_image_options& o )
{
   opt.bitsPerSample      = o.bitsPerSample;
   opt.ieeefpSampleFormat = o.ieeefpSampleFormat;
   opt.complexSample      = o.complexSample;
   opt.signedIntegers     = o.signedIntegers;
   opt.metricResolution   = o.metricResolution;
   opt.readNormalized     = o.readNormalized;
   opt.embedICCProfile    = o.embedICCProfile;
   opt.embedMetadata      = o.embedMetadata;
   opt.embedThumbnail     = o.embedThumbnail;
   opt.embedProperties    = o.embedProperties;
   opt.lowerRange         = o.lowerRange;
   opt.upperRange         = o.upperRange;
   opt.xResolution        = o.xResolution;
   opt.yResolution        = o.yResolution;
   opt.isoSpeed           = o.isoSpeed;
   opt.exposure           = o.exposure;
   opt.aperture           = o.aperture;
   opt.focalLength        = o.focalLength;
   opt.cfaType            = o.cfaType;
}

static void PCLImageInfoToAPI( api_image_info& i, const ImageInfo& info )
{
   i.width            = uint32( info.width );
   i.height           = uint32( info.height );
   i.numberOfChannels = uint32( info.numberOfChannels );
   i.colorSpace       = uint32( info.colorSpace );
   i.supported        = info.supported;
}

static void PCLImageOptionsToAPI( api_image_options& o, const ImageOptions& opt )
{
   o.bitsPerSample      = opt.bitsPerSample;
   o.ieeefpSampleFormat = opt.ieeefpSampleFormat;
   o.complexSample      = opt.complexSample;
   o.signedIntegers     = opt.signedIntegers;
   o.metricResolution   = opt.metricResolution;
   o.readNormalized     = opt.readNormalized;
   o.embedICCProfile    = opt.embedICCProfile;
   o.embedMetadata      = opt.embedMetadata;
   o.embedThumbnail     = opt.embedThumbnail;
   o.embedProperties    = opt.embedProperties;
   o.lowerRange         = opt.lowerRange;
   o.upperRange         = opt.upperRange;
   o.xResolution        = opt.xResolution;
   o.yResolution        = opt.yResolution;
   o.isoSpeed           = opt.isoSpeed;
   o.exposure           = opt.exposure;
   o.aperture           = opt.aperture;
   o.focalLength        = opt.focalLength;
   o.cfaType            = opt.cfaType;
}

// ----------------------------------------------------------------------------

static void APIHackingAttempt( const String& routineId )
{
   throw Error( "* Warning * Hacking attempt detected in low-level API call: FileFormat->" + routineId );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

class FileFormatInstancePrivate
{
public:

   template <class I>
   static bool ReadImage( FileFormatInstance* instance, I& image )
   {
      if ( !image.IsShared() )
      {
         I tmp( (void*)0, 0, 0 );
         if ( !ReadImage( instance, tmp ) )
            return false;
         image.Assign( tmp );
         return true;
      }

      if ( (*API->FileFormat->ReadImage)( instance->handle, image.Allocator().Handle() ) == api_false )
         return false;
      image.Synchronize();
      return true;
   }

   template <class I>
   static bool WriteImage( FileFormatInstance* instance, const I& image )
   {
      if ( !image.IsShared() || !image.IsCompletelySelected() )
      {
         I tmp( (void*)0, 0, 0 );
         tmp.Assign( image );
         return WriteImage( instance, tmp );
      }

      image.PushSelections();
      image.ResetSelections();

      ImageInfo info1( image );

      image.PopSelections();

      bool ok = (*API->FileFormat->WriteImage)( instance->handle, image.Allocator().Handle() ) != api_false;

      image.PushSelections();
      image.ResetSelections();

      ImageInfo info2( image );

      image.PopSelections();

      if ( info1 != info2 )
         APIHackingAttempt( "WriteImage" );

      return ok;
   }
};

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

FileFormatInstance::FileFormatInstance( const FileFormat& fmt ) :
UIObject( (*API->FileFormat->CreateFileFormatInstance)( ModuleHandle(), fmt.Handle() ) )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateFileFormatInstance" );
}

// ----------------------------------------------------------------------------

FileFormatInstance::FileFormatInstance( void* h ) : UIObject( h )
{
}

// ----------------------------------------------------------------------------

FileFormatInstance& FileFormatInstance::Null()
{
   static FileFormatInstance* nullInstance = 0;
   static Mutex mutex;
   volatile AutoLock lock( mutex );
   if ( nullInstance == 0 )
      nullInstance = new FileFormatInstance( reinterpret_cast<void*>( 0 ) );
   return *nullInstance;
}

// ----------------------------------------------------------------------------

FileFormat FileFormatInstance::Format() const
{
   return FileFormat( (const void*)(*API->FileFormat->GetFileFormatInstanceFormat)( handle ) );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Close()
{
   return (*API->FileFormat->CloseImageFile)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::IsOpen() const
{
   return (*API->FileFormat->IsImageFileOpen)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

String FileFormatInstance::FilePath() const
{
   size_type len = 0;
   (*API->FileFormat->GetImageFilePath)( handle, 0, &len );

   String path;

   if ( len != 0 )
   {
      path.Reserve( len );

      if ( (*API->FileFormat->GetImageFilePath)( handle, path.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetImageFilePath" );
   }

   return path;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Open( ImageDescriptionArray& images,
                               const String& filePath, const IsoString& hints )
{
   images.Remove();

   if ( (*API->FileFormat->OpenImageFileEx)( handle, filePath.c_str(), hints.c_str(), 0/*flags*/ ) == api_false )
      return false;

   for ( uint32 i = 0, n = (*API->FileFormat->GetImageCount)( handle ); i < n; ++i )
   {
      IsoString id;
      size_type len = 0;
      (*API->FileFormat->GetImageId)( handle, 0, &len, i );
      if ( len != 0 )
      {
         id.Reserve( len );
         if ( (*API->FileFormat->GetImageId)( handle, id.c_str(), &len, i ) == api_false )
            throw APIFunctionError( "GetImageId" );
      }

      api_image_info info;
      api_image_options options;
      if ( (*API->FileFormat->GetImageDescription)( handle, &info, &options, i ) == api_false )
         throw APIFunctionError( "GetImageDescription" );

      ImageDescription d;
      d.id = id;
      APIImageInfoToPCL( d.info, info );
      APIImageOptionsToPCL( d.options, options );
      images.Add( d );
   }

   return true;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SelectImage( int index )
{
   return (*API->FileFormat->SelectImage)( handle, uint32( index ) ) != api_false;
}

// ----------------------------------------------------------------------------

int FileFormatInstance::SelectedImageIndex() const
{
   return int( (*API->FileFormat->GetSelectedImageIndex)( handle ) );
}

// ----------------------------------------------------------------------------

const void* FileFormatInstance::FormatSpecificData() const
{
   return (*API->FileFormat->GetFormatSpecificData)( handle );
}

// ----------------------------------------------------------------------------

String FileFormatInstance::ImageProperties() const
{
   size_type len = 0;
   (*API->FileFormat->GetImageProperties)( handle, 0, &len );

   String props;

   if ( len != 0 )
   {
      props.Reserve( len );

      if ( (*API->FileFormat->GetImageProperties)( handle, props.c_str(), &len ) == api_false )
         throw APIFunctionError( "GetImageProperties" );
   }

   return props;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Extract( FITSKeywordArray& keywords )
{
   try
   {
      keywords.Clear();

      if ( (*API->FileFormat->BeginKeywordExtraction)( handle ) == api_false )
         return false;

      for ( size_type i = 0, count = (*API->FileFormat->GetKeywordCount)( handle ); i < count; ++i )
      {
         IsoString name, value, comment;
         name.Reserve( 96 );
         value.Reserve( 96 );
         comment.Reserve( 96 );

         if ( (*API->FileFormat->GetNextKeyword)( handle,
                  name.c_str(), value.c_str(), comment.c_str(), 81 ) == api_false )
            throw APIFunctionError( "GetNextKeyword" );

         keywords.Add( FITSHeaderKeyword( name, value, comment ) );
      }

      (*API->FileFormat->EndKeywordExtraction)( handle );

      return true;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndKeywordExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Extract( ICCProfile& icc )
{
   try
   {
      icc.Clear();

      if ( (*API->FileFormat->BeginICCProfileExtraction)( handle ) == api_false )
         return false;

      const void* iccData = (*API->FileFormat->GetICCProfile)( handle );
      if ( iccData != 0 )
         icc.Set( iccData );

      (*API->FileFormat->EndICCProfileExtraction)( handle );
      return true;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndICCProfileExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Extract( void*& data, size_type& length )
{
   try
   {
      data = 0;
      length = 0;

      if ( (*API->FileFormat->BeginMetadataExtraction)( handle ) == api_false )
         return false;

      uint32 metadataSize;
      const void* metadata = (*API->FileFormat->GetMetadata)( handle, &metadataSize );

      if ( metadata != 0 && metadataSize != 0 )
      {
         data = (void*)new uint8[ length = metadataSize ];
         ::memcpy( data, metadata, metadataSize );
      }

      (*API->FileFormat->EndMetadataExtraction)( handle );

      return true;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndMetadataExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Extract( pcl::UInt8Image& thumbnail )
{
   try
   {
      thumbnail.FreeData();

      if ( (*API->FileFormat->BeginThumbnailExtraction)( handle ) == api_false )
         return false;

      if ( thumbnail.IsShared() )
      {
         (*API->FileFormat->GetThumbnail)( handle, thumbnail.Allocator().Handle() );
         thumbnail.Synchronize();
      }
      else
      {
         UInt8Image img( (void*)0, 0, 0 );
         (*API->FileFormat->GetThumbnail)( handle, img.Allocator().Handle() );
         img.Synchronize();
         thumbnail.Assign( img );
      }

      (*API->FileFormat->EndThumbnailExtraction)( handle );

      return true;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndThumbnailExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

static api_bool APIPropertyEnumerationCallback( const char* id, uint64 type, void* data )
{
   reinterpret_cast<ImagePropertyDescriptionArray*>( data )->Append(
         ImagePropertyDescription( id, VariantTypeFromAPIPropertyType( type ) ) );
   return api_true;
}

ImagePropertyDescriptionArray FileFormatInstance::Properties()
{
   ImagePropertyDescriptionArray properties;
   IsoString id;
   size_type len = 0;
   (*API->FileFormat->EnumerateImageProperties)( handle, 0, 0, &len, 0 ); // 1st call to get max identifier length
   if ( len > 0 )
   {
      id.Reserve( len+16 );
      if ( (*API->FileFormat->EnumerateImageProperties)( handle, APIPropertyEnumerationCallback,
                                                         id.c_str(), &len, &properties ) == api_false )
         throw APIFunctionError( "EnumerateImageProperties" );
   }
   return properties;
}

// ----------------------------------------------------------------------------

Variant FileFormatInstance::ReadProperty( const IsoString& property )
{
   try
   {
      if ( (*API->FileFormat->BeginPropertyExtraction)( handle ) == api_false )
         return Variant();

      api_property_value apiValue;
      if ( (*API->FileFormat->GetImageProperty)( handle, property.c_str(), &apiValue ) == api_false )
      {
         apiValue.data.blockValue = 0;
         apiValue.type = VTYPE_INVALID;
      }

      (*API->FileFormat->EndPropertyExtraction)( handle );

      return VariantFromAPIPropertyValue( apiValue );
   }
   catch ( ... )
   {
      (*API->FileFormat->EndPropertyExtraction)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteProperty( const IsoString& property, const Variant& value )
{
   try
   {
      if ( (*API->FileFormat->BeginPropertyEmbedding)( handle ) == api_false )
         return false;

      bool ok = true;

      if ( value.IsValid() )
      {
         api_property_value apiValue;
         APIPropertyValueFromVariant( apiValue, value );
         api_property_value safeCopy = apiValue;
         ok = (*API->FileFormat->SetImageProperty)( handle, property.c_str(), &safeCopy ) != api_false;

         if ( safeCopy.data.blockValue != apiValue.data.blockValue ||
              safeCopy.dimX != apiValue.dimX || safeCopy.dimY != apiValue.dimY ||
              safeCopy.dimZ != apiValue.dimZ || safeCopy.dimT != apiValue.dimT || safeCopy.type != apiValue.type )
         {
            APIHackingAttempt( "WriteProperty" );
         }
      }

      (*API->FileFormat->EndPropertyEmbedding)( handle );

      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndPropertyEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::ReadImage( pcl::Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( pcl::DImage& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( UInt8Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( UInt16Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

bool FileFormatInstance::ReadImage( UInt32Image& image )
{
   return FileFormatInstancePrivate::ReadImage( this, image );
}

// ----------------------------------------------------------------------------

static bool ReadPixels( file_format_handle handle,
      void* buffer, int startRow, int rowCount, int channel, int bitsPerSample, bool floatSample )
{
   return (*API->FileFormat->ReadPixels)( handle, buffer, startRow, rowCount, channel,
                                          bitsPerSample, floatSample, false ) != api_false;
}

bool FileFormatInstance::Read( pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return ReadPixels( handle, buffer, startRow, rowCount, channel, 32, true );
}

bool FileFormatInstance::Read( pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   return ReadPixels( handle, buffer, startRow, rowCount, channel, 64, true );
}

bool FileFormatInstance::Read( UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return ReadPixels( handle, buffer, startRow, rowCount, channel, 8, false );
}

bool FileFormatInstance::Read( UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return ReadPixels( handle, buffer, startRow, rowCount, channel, 16, false );
}

bool FileFormatInstance::Read( UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return ReadPixels( handle, buffer, startRow, rowCount, channel, 32, false );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WasInexactRead() const
{
   return (*API->FileFormat->WasInexactRead)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::QueryOptions( Array<ImageOptions>& options, Array<const void*>& formatOptions )
{
   uint32 n = uint32( options.Length() );

   Array<api_image_options> o( (size_type)n );
   for ( uint32 i = 0; i < n; ++i )
      PCLImageOptionsToAPI( o[i], options[i] );

   if ( (*API->FileFormat->QueryImageFileOptions)( handle, o.Begin(), formatOptions.Begin(), n ) == api_false )
      return false;

   for ( uint32 i = 0; i < n; ++i )
      APIImageOptionsToPCL( options[i], o[i] );
   return true;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Create( const String& filePath, const IsoString& hints, int count )
{
   return (*API->FileFormat->CreateImageFileEx)( handle, filePath.c_str(), uint32( count ),
                                                 hints.c_str(), 0/*flags*/ ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SetId( const IsoString& id )
{
   return (*API->FileFormat->SetImageId)( handle, id.c_str() ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SetOptions( const ImageOptions& options )
{
   api_image_options o;
   PCLImageOptionsToAPI( o, options );
   return (*API->FileFormat->SetImageOptions)( handle, &o ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::SetFormatSpecificData( const void* data )
{
   return (*API->FileFormat->SetFormatSpecificData)( handle, data ) != api_false;
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Embed( const FITSKeywordArray& keywords )
{
   try
   {
      if ( (*API->FileFormat->BeginKeywordEmbedding)( handle ) == api_false )
         return false;

      bool ok = true;

      for ( FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
         if ( (*API->FileFormat->AddKeyword)( handle,
                  i->name.c_str(), i->value.c_str(), i->comment.c_str() ) == api_false )
         {
            ok = false;
            break;
         }

      (*API->FileFormat->EndKeywordEmbedding)( handle );

      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndKeywordEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Embed( const ICCProfile& icc )
{
   try
   {
      if ( (*API->FileFormat->BeginICCProfileEmbedding)( handle ) == api_false )
         return false;

      bool ok = true;

      if ( icc.IsProfile() ) // ### should allow embedding empty profiles ?
      {
         ICCProfile safeCopy = icc;
         ok = (*API->FileFormat->SetICCProfile)( handle, safeCopy.ProfileData().Begin() ) != api_false;

         if ( !safeCopy.IsSameProfile( icc ) || safeCopy.FilePath() != icc.FilePath() )
            APIHackingAttempt( "SetICCProfile" );
      }

      (*API->FileFormat->EndICCProfileEmbedding)( handle );

      return ok;
   }
   catch ( ... )
   {
      (*API->FileFormat->EndICCProfileEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Embed( const void* data, size_type length )
{
   void* safeCopy = 0;

   try
   {
      if ( (*API->FileFormat->BeginMetadataEmbedding)( handle ) == api_false )
         return false;

      bool ok = true;

      if ( data != 0 && length != 0 ) // ### should allow embedding empty metadata ?
      {
         safeCopy = new uint8[ length ];
         ::memcpy( safeCopy, data, length );

         ok = (*API->FileFormat->SetMetadata)( handle, safeCopy, uint32( length ) ) != api_false;

         if ( ::memcmp( safeCopy, data, length ) != 0 )
            APIHackingAttempt( "SetMetadata" );

         delete [] static_cast<uint8*>( safeCopy );
         safeCopy = 0;
      }

      (*API->FileFormat->EndMetadataEmbedding)( handle );

      return ok;
   }
   catch ( ... )
   {
      if ( safeCopy != 0 )
         delete [] static_cast<uint8*>( safeCopy );
      (*API->FileFormat->EndMetadataEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::Embed( const pcl::UInt8Image& thumbnail )
{
   try
   {
      if ( (*API->FileFormat->BeginThumbnailEmbedding)( handle ) == api_false )
         return false;

      thumbnail.PushSelections();
      thumbnail.ResetSelections();

      ImageInfo info( thumbnail );

      thumbnail.PopSelections();

      bool ok;

      if ( thumbnail.IsShared() )
         ok = (*API->FileFormat->SetThumbnail)( handle, thumbnail.Allocator().Handle() ) != api_false;
      else
      {
         UInt8Image tmp( (void*)0, 0, 0 );
         tmp.Assign( thumbnail );
         ok = (*API->FileFormat->SetThumbnail)( handle, tmp.Allocator().Handle() ) != api_false;
      }

      thumbnail.PushSelections();
      thumbnail.ResetSelections();

      if ( info != ImageInfo( thumbnail ) )
         APIHackingAttempt( "SetThumbnail" );

      thumbnail.PopSelections();

      (*API->FileFormat->EndThumbnailEmbedding)( handle );

      return ok;
   }
   catch ( ... )
   {
      if ( thumbnail.CanPopSelections() )
         thumbnail.PopSelections();
      (*API->FileFormat->EndThumbnailEmbedding)( handle );
      throw;
   }
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WriteImage( const pcl::Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const pcl::DImage& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const UInt8Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const UInt16Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

bool FileFormatInstance::WriteImage( const UInt32Image& image )
{
   return FileFormatInstancePrivate::WriteImage( this, image );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::CreateImage( const ImageInfo& info )
{
   api_image_info i;
   PCLImageInfoToAPI( i, info );
   return (*API->FileFormat->CreateImage)( handle, &i ) != api_false;
}

// ----------------------------------------------------------------------------

static bool WritePixels( file_format_handle handle,
   const void* buffer, int startRow, int rowCount, int channel, int bitsPerSample, bool floatSample )
{
   return (*API->FileFormat->WritePixels)( handle, buffer, startRow, rowCount, channel,
                                             bitsPerSample, floatSample, false ) != api_false;
}

bool FileFormatInstance::Write( const pcl::Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return WritePixels( handle, buffer, startRow, rowCount, channel, 32, true );
}

bool FileFormatInstance::Write( const pcl::DImage::sample* buffer, int startRow, int rowCount, int channel )
{
   return WritePixels( handle, buffer, startRow, rowCount, channel, 64, true );
}

bool FileFormatInstance::Write( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return WritePixels( handle, buffer, startRow, rowCount, channel, 8, false );
}

bool FileFormatInstance::Write( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return WritePixels( handle, buffer, startRow, rowCount, channel, 16, false );
}

bool FileFormatInstance::Write( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel )
{
   return WritePixels( handle, buffer, startRow, rowCount, channel, 32, false );
}

// ----------------------------------------------------------------------------

bool FileFormatInstance::WasLossyWrite() const
{
   return (*API->FileFormat->WasLossyWrite)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void* FileFormatInstance::CloneHandle() const
{
   throw Error( "FileFormatInstance::CloneHandle(): Cannot clone a file format instance handle" );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/FileFormatInstance.cpp - Released 2014/11/14 17:17:00 UTC
