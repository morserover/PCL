// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// Standard TIFF File Format Module Version 01.00.05.0227
// ****************************************************************************
// TIFFInstance.h - Released 2014/10/29 07:34:49 UTC
// ****************************************************************************
// This file is part of the standard TIFF PixInsight module.
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

#ifndef __TIFFInstance_h
#define __TIFFInstance_h

#include "TIFF.h"

#include <pcl/FileFormatImplementation.h>

namespace pcl
{

class TIFFFormat;
class TIFFReadHints;

// ----------------------------------------------------------------------------
// TIFFInstance
// ----------------------------------------------------------------------------

class TIFFInstance : public FileFormatImplementation
{
public:

   TIFFInstance( const TIFFFormat* );

   virtual ~TIFFInstance();

   virtual ImageDescriptionArray Open( const String& filePath, const IsoString& hints );
   virtual bool IsOpen() const;
   virtual String FilePath() const;
   virtual void Close();

   virtual void* FormatSpecificData() const;
   virtual String ImageProperties() const;

   virtual void Extract( ICCProfile& icc );
   /*
   // ### TODO: XML metadata and thumbnail support
   virtual void Extract( void*& data, size_type& length );
   virtual void Extract( pcl::UInt8Image& thumbnail );
   */

   virtual void ReadImage( Image& );
   virtual void ReadImage( DImage& );
   virtual void ReadImage( UInt8Image& );
   virtual void ReadImage( UInt16Image& );
   virtual void ReadImage( UInt32Image& );

   virtual bool QueryOptions( Array<ImageOptions>& options, Array<void*>& formatOptions );
   virtual void Create( const String& filePath, int numberOfImages, const IsoString& hints );
   virtual void SetOptions( const ImageOptions& options );
   virtual void SetFormatSpecificData( const void* data );
   virtual void Embed( const ICCProfile& icc );
   /*
   // ### TODO: XML metadata and thumbnail support
   virtual void Embed( const void* data, size_type length );
   virtual void Embed( const pcl::UInt8Image& image );
   */

   virtual void WriteImage( const Image& );
   virtual void WriteImage( const DImage& );
   virtual void WriteImage( const UInt8Image& );
   virtual void WriteImage( const UInt16Image& );
   virtual void WriteImage( const UInt32Image& );

   // -------------------------------------------------------------------------

private:

   TIFFReader*    reader;
   TIFFWriter*    writer;
   TIFFReadHints* readHints;

   bool queriedOptions; // did us query options to the user?

   ICCProfile* embeddedICCProfile;
   // ### TODO XML metadata and thumbnail images
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __TIFFInstance_h

// ****************************************************************************
// EOF TIFFInstance.h - Released 2014/10/29 07:34:49 UTC