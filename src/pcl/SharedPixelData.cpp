// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/SharedPixelData.cpp - Released 2014/11/14 17:17:00 UTC
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

#include <pcl/RGBColorSystem.h>
#include <pcl/SharedPixelData.h>

#include <pcl/api/APIException.h>
#include <pcl/api/APIInterface.h>

#include <new>

#ifdef _MSC_VER
#  include <malloc.h>
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

SharedPixelData::SharedPixelData( void* handle, int bitsPerSample, bool floatSample, bool complexSample ) :
m_handle( handle )
{
   if ( m_handle == 0 )
      throw APIError( "SharedPixelData: Null image handle" );

   if ( complexSample )
      throw APIError( "SharedPixelData: Shared complex images are not supported by this version of PCL" );

   uint32 n;
   api_bool f;
   if ( (*API->SharedImage->GetImageFormat)( m_handle, &n, &f ) == api_false )
      throw APIFunctionError( "GetImageFormat" );

   if ( n != uint32( bitsPerSample ) || (f != api_false) != floatSample )
      throw APIError( "SharedPixelData: Incompatible shared image format" );

   if ( (*API->SharedImage->AttachToImage)( m_handle, this/*owner*/ ) == api_false )
      throw APIFunctionError( "AttachToImage" );
}

// ----------------------------------------------------------------------------

SharedPixelData::SharedPixelData( int width, int height, int numberOfChannels,
                                  int bitsPerSample, bool floatSample, int colorSpace ) :
m_handle( (*API->SharedImage->CreateImage)( width, height, numberOfChannels,
                                            bitsPerSample, floatSample, colorSpace, this/*owner*/ ) )
{
   if ( m_handle == 0 )
      throw APIFunctionError( "CreateImage" );
}

// ----------------------------------------------------------------------------

bool SharedPixelData::IsAliased() const
{
   if ( m_handle == 0 )
      return false;

   uint32 n = 0;
   if ( (*API->SharedImage->GetImageRefCount)( m_handle, &n ) == api_false )
      throw APIFunctionError( "GetImageRefCount" );
   return n > 1;
}

// ----------------------------------------------------------------------------

bool SharedPixelData::IsOwner() const
{
   return m_handle != 0 && (*API->SharedImage->GetImageOwner)( m_handle ) == this;
}

// ----------------------------------------------------------------------------

void SharedPixelData::Attach()
{
   if ( m_handle != 0 )
      if ( (*API->SharedImage->AttachToImage)( m_handle, this ) == api_false )
         throw APIFunctionError( "AttachToImage" );
}

// ----------------------------------------------------------------------------

void SharedPixelData::Detach()
{
   if ( m_handle != 0 )
      if ( (*API->SharedImage->DetachFromImage)( m_handle, this ) == api_false )
         throw APIFunctionError( "DetachFromImage" );
}

// ----------------------------------------------------------------------------

void* SharedPixelData::Allocate( size_type size ) const
{
   if ( size == 0 )
      throw APIError( "<* Warning *> SharedPixelData::Allocate(): Zero block size" );

   if ( m_handle == 0 )
   {
#ifdef _MSC_VER
      void* p = _aligned_malloc( size, 16 );
#else
      void* p = _mm_malloc( size, 16 );
#endif
      if ( p == 0 )
         throw std::bad_alloc();
      return p;
   }

   void* p = (*API->Global->Allocate)( size );
   if ( p == 0 )
      throw std::bad_alloc();

   return p;
}

// ----------------------------------------------------------------------------

void SharedPixelData::Deallocate( void* p ) const
{
   if ( p == 0 )
      throw APIError( "<* Warning *> SharedPixelData::Deallocate(): Null block address" );

   if ( m_handle == 0 )
#ifdef _MSC_VER
      _aligned_free( p );
#else
      _mm_free( p );
#endif
   else if ( (*API->Global->Deallocate)( p ) == api_false )
      throw APIFunctionError( "Deallocate" );
}

// ----------------------------------------------------------------------------

void** SharedPixelData::GetSharedData() const
{
   if ( m_handle == 0 )
      return 0;

   void** data;
   if ( (*API->SharedImage->GetImagePixelData)( m_handle, &data ) == api_false )
      throw APIFunctionError( "GetImagePixelData" );
   return data;
}

// ----------------------------------------------------------------------------

void SharedPixelData::SetSharedData( void** ptrToShared )
{
   if ( m_handle != 0 )
      if ( (*API->SharedImage->SetImagePixelData)( m_handle, ptrToShared ) == api_false )
         throw APIFunctionError( "SetImagePixelData" );
}

// ----------------------------------------------------------------------------

void SharedPixelData::GetSharedGeometry( int& width, int& height, int& numberOfChannels )
{
   if ( m_handle != 0 )
   {
      uint32 w, h, n;
      if ( (*API->SharedImage->GetImageGeometry)( m_handle, &w, &h, &n ) == api_false )
         throw APIFunctionError( "GetImageGeometry" );

      width = int( w );
      height = int( h );
      numberOfChannels = int( n );
   }
   else
   {
      width = height = numberOfChannels = 0;
   }
}

// ----------------------------------------------------------------------------

void SharedPixelData::SetSharedGeometry( int width, int height, int numberOfChannels )
{
   if ( m_handle != 0 )
      if ( (*API->SharedImage->SetImageGeometry)( m_handle, width, height, numberOfChannels ) == api_false )
         throw APIFunctionError( "SetImageGeometry" );
}

// ----------------------------------------------------------------------------

void SharedPixelData::GetSharedColor( color_space& colorSpace, RGBColorSystem& RGBWS )
{
   if ( m_handle != 0 )
   {
      uint32 cs;
      if ( (*API->SharedImage->GetImageColorSpace)( m_handle, &cs ) == api_false )
         throw APIFunctionError( "GetImageColorSpace" );

      api_RGBWS rgbws;
      if ( (*API->SharedImage->GetImageRGBWS)( m_handle, &rgbws ) == api_false )
         throw APIFunctionError( "GetImageRGBWS" );

      colorSpace = color_space( cs );
      RGBWS = RGBColorSystem( rgbws.gamma, rgbws.isSRGBGamma != api_false, rgbws.x, rgbws.y, rgbws.Y );
   }
   else
   {
      colorSpace = ColorSpace::Unknown;
      RGBWS = RGBColorSystem();
   }
}

// ----------------------------------------------------------------------------

void SharedPixelData::SetSharedColor( color_space colorSpace, const RGBColorSystem& /*RGBWS*/ )
{
   if ( m_handle != 0 )
      if ( (*API->SharedImage->SetImageColorSpace)( m_handle, colorSpace ) == api_false )
         throw APIFunctionError( "SetImageColorSpace" );

   /*
    * The following code has been omitted because we cannot modify a shared
    * image's RGBWS directly.
    *
    * RGB working spaces of shared images are handled by image windows in the
    * PixInsight core application.
    */
   /*
   api_RGBWS rgbws;
   rgbws.gamma = RGBWS.Gamma();
   rgbws.isSRGBGamma = RGBWS.IsSRGB();
   memcpy( rgbws.x, *RGBWS.ChromaticityXCoordinates(), sizeof( rgbws.x ) );
   memcpy( rgbws.y, *RGBWS.ChromaticityYCoordinates(), sizeof( rgbws.y ) );
   memcpy( rgbws.Y, *RGBWS.LuminanceCoefficients(), sizeof( rgbws.Y ) );
   if ( (*API->SetImageRGBWS)( m_handle, &rgbws ) == api_false )
      throw APIFunctionError( "SetImageRGBWS" );
   */
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/SharedPixelData.cpp - Released 2014/11/14 17:17:00 UTC
