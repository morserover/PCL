// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard FITS File Format Module Version 01.01.00.0282
// ****************************************************************************
// FITS.h - Released 2014/11/14 17:18:35 UTC
// ****************************************************************************
// This file is part of the standard FITS PixInsight module.
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

#ifndef __PCL_FITS_h
#define __PCL_FITS_h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_FITSHeaderKeyword_h
#include <pcl/FITSHeaderKeyword.h>
#endif

#ifndef __PCL_ImageStream_h
#include <pcl/ImageStream.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

struct FITSFileData;
struct FITSReaderData;
struct FITSWriterData;
struct FITSExtensionData;

// ----------------------------------------------------------------------------

/*
 * FITS-specific image file options
 */
class FITSImageOptions
{
public:

   bool unsignedIntegers                  :  1; // Write unsigned integer samples
   bool writeFixedFloatKeywords           :  1; // Use a fixed-precision exponential format for real keyword values
   bool writeScalingKeywordsForSignedData :  1; // Write BZERO=0 and BSCALE=1 for signed integer images
   bool reloadKeywords                    :  1; // Reload header keywords upon file write completion
   bool bottomUp                          :  1; // When this flag is true, the origin of coordinates is at the lower left corner of
                                                //   the image. When false, the origin is at the upper left corner
   bool signedIntegersArePhysical         :  1; // If true, truncate to [0,2^(n/2-1)] and do not rescale.
                                                //   Otherwise the full range is [-2^(n/2),+2^(n/2-1)]
   bool cleanupHeaders                    :  1; // Transform header keywords for FITS standard compliance before writing
   int  __rsv__                           : 25; // Reserved for future extension -- must be zero

   double zeroOffset; // Zero offset value of read pixel values
   double scaleRange; // Scaling value of read pixel values

   FITSImageOptions()
   {
      Reset();
   }

   FITSImageOptions( const FITSImageOptions& x )
   {
      (void)operator =( x );
   }

   FITSImageOptions& operator =( const FITSImageOptions& x )
   {
      unsignedIntegers                  = x.unsignedIntegers;
      writeFixedFloatKeywords           = x.writeFixedFloatKeywords;
      writeScalingKeywordsForSignedData = x.writeScalingKeywordsForSignedData;
      reloadKeywords                    = x.reloadKeywords;
      bottomUp                          = x.bottomUp;
      signedIntegersArePhysical         = x.signedIntegersArePhysical;
      cleanupHeaders                    = x.cleanupHeaders;
      __rsv__                           = 0;
      zeroOffset                        = x.zeroOffset;
      scaleRange                        = x.scaleRange;
      return *this;
   }

   void Reset()
   {
      unsignedIntegers                  = true;
      writeFixedFloatKeywords           = false;
      writeScalingKeywordsForSignedData = true;
      reloadKeywords                    = true;
      bottomUp                          = false;
      signedIntegersArePhysical         = false;
      cleanupHeaders                    = false;
      __rsv__                           = 0;
      zeroOffset                        = 0;
      scaleRange                        = 1;
   }
};

// ----------------------------------------------------------------------------

/*
 * Base class for the FITSReader and FITSWriter classes.
 */
class FITS
{
public:

   /*
    * FITS-specific exception class
    */
   class Exception : public ImageStream::Exception
   {
   public:

      Exception( const String& fn ) : ImageStream::Exception( fn )
      {
      }

      Exception( const FITS::Exception& x ) : ImageStream::Exception( x )
      {
      }

      virtual String ExceptionClass() const
      {
         return "PCL FITS Format Support";
      }

      virtual String Message() const; // reimplements File::Exception

      virtual String ErrorMessage() const;

   protected:

      virtual String __ErrorMessage() const = 0;
   };

#define PCL_DECLARE_FITS_EXCEPTION( className, errorMessage )                 \
   class className : public FITS::Exception                                   \
   {                                                                          \
   public:                                                                    \
      className( const String& fn ) : FITS::Exception( fn )                   \
      {                                                                       \
      }                                                                       \
      className( const className& x ) : FITS::Exception( x )                  \
      {                                                                       \
      }                                                                       \
   private:                                                                   \
      virtual String __ErrorMessage() const                                   \
      {                                                                       \
         return errorMessage;                                                 \
      }                                                                       \
   }

   PCL_DECLARE_FITS_EXCEPTION( InvalidFilePath,
      "Internal error: Invalid file path" );
   PCL_DECLARE_FITS_EXCEPTION( InvalidReadOperation,
      "Invalid read operation in FITS file" );
   PCL_DECLARE_FITS_EXCEPTION( InvalidImage,
      "Invalid image" );
   PCL_DECLARE_FITS_EXCEPTION( InvalidNormalizationRange,
      "Invalid pixel normalization range" );
   PCL_DECLARE_FITS_EXCEPTION( UnableToAccessCurrentHDU,
      "Unable to access the current FITS HDU" );
   PCL_DECLARE_FITS_EXCEPTION( FileReadError,
      "FITS file read error" );
   PCL_DECLARE_FITS_EXCEPTION( ReadCoordinatesOutOfRange,
      "Coordinates out of range in incremental read operation" );
   PCL_DECLARE_FITS_EXCEPTION( UnableToOpenFile,
      "Unable to open FITS file" );
   PCL_DECLARE_FITS_EXCEPTION( UnableToAccessPrimaryHDU,
      "Unable to access the primary FITS HDU" );
   PCL_DECLARE_FITS_EXCEPTION( KeywordReadError,
      "Error reading FITS header keyword" );
   PCL_DECLARE_FITS_EXCEPTION( NoImageHDU,
      "The FITS file contains no image HDU" );
   PCL_DECLARE_FITS_EXCEPTION( NoReadableImage,
      "The FITS file contains no readable image" );
   PCL_DECLARE_FITS_EXCEPTION( UnsupportedThumbnailImageDimensions,
      "Unsupported thumbnail image dimensions" );
   PCL_DECLARE_FITS_EXCEPTION( InvalidThumbnailImage,
      "Invalid thumbnail image" );
   PCL_DECLARE_FITS_EXCEPTION( UnsupportedThumbnailSampleFormat,
      "Unsupported thumbnail sample format" );
   PCL_DECLARE_FITS_EXCEPTION( InvalidExtensionHDU,
      "Invalid FITS extension HDU" );
   PCL_DECLARE_FITS_EXCEPTION( FileWriteError,
      "FITS file write error" );
   PCL_DECLARE_FITS_EXCEPTION( InvalidWriteOperation,
      "Invalid write operation in FITS file" );
   PCL_DECLARE_FITS_EXCEPTION( WriteCoordinatesOutOfRange,
      "Coordinates out of range in incremental write operation" );
   PCL_DECLARE_FITS_EXCEPTION( UnableToCreateFile,
      "Unable to create FITS file" );
   PCL_DECLARE_FITS_EXCEPTION( UnableToCreatePrimaryHDU,
      "Unable to create primary FITS HDU" );
   PCL_DECLARE_FITS_EXCEPTION( KeywordWriteError,
      "Error writing FITS header keyword" );
   PCL_DECLARE_FITS_EXCEPTION( UnsupportedComplexFITS,
      "This implementation does not support complex FITS images" );
   PCL_DECLARE_FITS_EXCEPTION( Unsupported64BitFITS,
      "This implementation does not support 64-bit integer FITS images" );
   PCL_DECLARE_FITS_EXCEPTION( UnableToCreateImageHDU,
      "Unable to create image FITS HDU" );
   PCL_DECLARE_FITS_EXCEPTION( InvalidCloseImageOperation,
      "Invalid close operation in FITS file" );
   PCL_DECLARE_FITS_EXCEPTION( UnableToCreateThumbnailHDU,
      "Unable to create thumbnail image HDU" );

   FITS() : fileData( 0 )
   {
   }

   virtual ~FITS()
   {
      __Close();
   }

protected:

   FITSFileData* fileData;

   bool __IsOpen() const;
   void __Close(); // ### derived must call base

private:

   // Disable copy
   FITS( const FITS& ) { PCL_CHECK( 0 ) }
   void operator =( const FITS& ) { PCL_CHECK( 0 ) }
};

// ----------------------------------------------------------------------------

/*
 * FITS image file reader
 */
class FITSReader : public ImageReader, public FITS
{
public:

   FITSReader() : ImageReader(), FITS(), data( 0 )
   {
   }

   virtual ~FITSReader()
   {
      Close(); // in case MSVC fails
   }

   virtual bool IsOpen() const;

   virtual void Close();

   virtual void Open( const String& filePath );

   /*
    * Returns a reference to the immutable FITS-specific options of the current
    * image in this FITS input stream.
    */
   const FITSImageOptions& FITSOptions() const;

   /*
    * Sets new FITS-specific options for the current image in this FITS input
    * stream. If you change options, you should do that \e before reading the
    * next image from this FITS file.
    */
   void SetFITSOptions( const FITSImageOptions& );

   virtual bool Extract( FITSKeywordArray& );
   virtual bool Extract( ICCProfile& icc );
   virtual bool Extract( UInt8Image& thumbnail );

   /*
    * Extracts a FITS extension with the specified name.
    *
    * Returns true if the specified extension exists. In such case, a copy of
    * the extension's data is stored in the specified ByteArray container.
    */
   bool Extract( ByteArray& data, const IsoString& name );

   /*
    * Returns a list of data extensions in this FITS file.
    */
   IsoStringList DataExtensionNames() const;

   /*
    * Image reading
    */
   virtual void ReadImage( FImage& img );
   virtual void ReadImage( DImage& img );
   virtual void ReadImage( UInt8Image& img );
   virtual void ReadImage( UInt16Image& img );
   virtual void ReadImage( UInt32Image& img );

   virtual void ReadImage( ImageVariant& v )
   {
      ImageReader::ReadImage( v );
   }

   /*
    * Incremental reading
    */
   virtual void Read( FImage::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Read( DImage::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Read( UInt8Image::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Read( UInt16Image::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Read( UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

private:

   FITSReaderData* data;

   // Image streams are unique
   FITSReader( const FITSReader& ) { PCL_CHECK( 0 ) }
   void operator =( const FITSReader& ) { PCL_CHECK( 0 ) }

   friend class FITSReaderPrivate;
};

// ----------------------------------------------------------------------------

/*
 * FITS image file writer
 */
class FITSWriter : public ImageWriter, public FITS
{
public:

   FITSWriter() : ImageWriter(), FITS(), data( 0 )
   {
   }

   virtual ~FITSWriter()
   {
      Close(); // in case MSVC fails
   }

   virtual bool IsOpen() const;

   virtual void Close();

   virtual void Create( const String& filePath, int count );

   virtual void Create( const String& filePath )
   {
      Create( filePath, 1 );
   }

   /*
    * Returns a reference to the immutable FITS-specific options of the current
    * image in this FITS output stream.
    */
   const FITSImageOptions& FITSOptions() const;

   /*
    * Sets new FITS-specific options for the current image in this FITS output
    * stream. If you change options, you should do that \e before writing the
    * next image to this FITS file.
    */
   void SetFITSOptions( const FITSImageOptions& );

   /*
    * Embeddings
    */
   virtual void Embed( const FITSKeywordArray& );
   virtual void Embed( const ICCProfile& );
   virtual void Embed( const UInt8Image& );

   /*
    * Embeds an arbitrary FITS extension with the specified data and name.
    */
   void Embed( const ByteArray& extData, const IsoString& extName );

   /*
    * Returns a reference to the list of embedded FITS keywords. The returned
    * list is empty if no keywords have been embedded in this FITSWriter
    * object, or if an empty set has been embedded.
    */
   const FITSKeywordArray& EmbeddedKeywords() const;

   /*
    * Returns a pointer to the embedded ICC profile. Returns a zero pointer if
    * no ICC profile has been embedded in this FITSWriter object.
    */
   const ICCProfile* EmbeddedICCProfile() const;

   /*
    * Returns a pointer to the embedded thumbnail image. Returns a zero pointer
    * if no thumbnail has been embedded in this FITSWriter object.
    */
   const UInt8Image* EmbeddedThumbnail() const;

   /*
    * Image writing
    */

   virtual void WriteImage( const FImage& );
   virtual void WriteImage( const DImage& );
   virtual void WriteImage( const UInt8Image& );
   virtual void WriteImage( const UInt16Image& );
   virtual void WriteImage( const UInt32Image& );

   virtual void WriteImage( const ImageVariant& v )
   {
      ImageWriter::WriteImage( v );
   }

   /*
    * Incremental writing
    */

   virtual void CreateImage( const ImageInfo& );

   virtual void Write( const FImage::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Write( const DImage::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Write( const UInt8Image::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Write( const UInt16Image::sample* buffer, int startRow, int rowCount, int channel );
   virtual void Write( const UInt32Image::sample* buffer, int startRow, int rowCount, int channel );

   void Reset()
   {
      Close();
   }

private:

   FITSWriterData* data;

   void WriteExtensionHDU( const FITSExtensionData& );
   void CloseImage();

   // Image streams are unique.
   FITSWriter( const FITSWriter& ) { PCL_CHECK( 0 ) }
   void operator =( const FITSWriter& ) { PCL_CHECK( 0 ) }

   friend class FITSWriterPrivate;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FITS_h

// ****************************************************************************
// EOF FITS.h - Released 2014/11/14 17:18:35 UTC
