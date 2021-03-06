// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/FFTConvolution.h - Released 2014/11/14 17:16:39 UTC
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

#ifndef __PCL_FFTConvolution_h
#define __PCL_FFTConvolution_h

/// \file pcl/FFTConvolution.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_ImageTransformation_h
#include <pcl/ImageTransformation.h>
#endif

#ifndef __PCL_KernelFilter_h
#include <pcl/KernelFilter.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class FFTConvolution
 * \brief Fourier-based two-dimensional convolution
 *
 * %FFTConvolution implements a high-performance, two-dimensional discrete
 * convolution algorithm via fast Fourier transforms. It performs automatic
 * fixing of border artifacts by applying Neumann boundary conditions
 * (mirroring), and is able to convolve images and response functions of
 * arbitrary sizes, only limited by the available memory.
 *
 * \sa Convolution, SeparableConvolution, ATrousWaveletTransform, ImageTransformation
*/
class PCL_CLASS FFTConvolution : public ImageTransformation
{
public:

   /*!
    * Default constructor.
    *
    * \note This constructor yields an uninitialized instance that cannot be
    * used before explicit association with a response function, specified
    * either as a KernelFilter object or as an ImageVariant.
    */
   FFTConvolution() :
   ImageTransformation(), m_filter( 0 ), m_image(),
   m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS ), m_h( 0 )
   {
   }

   /*!
    * Constructs a %FFTConvolution instance.
    *
    * \param filter  Response function, or <em>convolution filter</em>. The
    *                specified object does not have to remain valid while this
    *                %FFTConvolution instance is actively used, since it owns a
    *                private copy of the filter (note that KernelFilter is a
    *                reference-counted class).
    */
   FFTConvolution( const KernelFilter& filter ) :
   ImageTransformation(), m_filter( filter.Clone() ), m_image(),
   m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS ), m_h( 0 )
   {
      PCL_CHECK( !m_filter->IsEmpty() )
   }

   /*!
    * Constructs a %FFTConvolution instance.
    *
    * \param f    A reference to an ImageVariant whose transported image is the
    *             <em>response function</em>, or <em>convolution filter</em>.
    *             The transported image <em>must remain valid</em> while this
    *             %FFTConvolution instance is actively used, or until a new
    *             response function is associated with this instance. The
    *             specified %ImageVariant doesn't have to remain valid, since a
    *             %FFTConvolution instance owns a private %ImageVariant that
    *             transports the response function image.
    */
   FFTConvolution( const ImageVariant& f ) :
   ImageTransformation(), m_filter( 0 ), m_image( f ),
   m_parallel( true ), m_maxProcessors( PCL_MAX_PROCESSORS ), m_h( 0 )
   {
      PCL_CHECK( bool( m_image ) )
   }

   /*!
    * Copy constructor.
    */
   FFTConvolution( const FFTConvolution& x ) :
   ImageTransformation( x ),
   m_filter( x.m_filter ? x.m_filter->Clone() : 0 ), m_image( x.m_image ),
   m_parallel( x.m_parallel ), m_maxProcessors( x.m_maxProcessors ), m_h( 0 )
   {
   }

   /*!
    * Destroys this %FFTConvolution object.
    */
   virtual ~FFTConvolution()
   {
      Destroy();
   }

   /*!
    * Assignment operator.
    */
   FFTConvolution& operator =( const FFTConvolution& x )
   {
      if ( &x != this )
      {
         Destroy();
         if ( x.m_filter != 0 )
            m_filter = x.m_filter->Clone();
         else
            m_image = x.m_image;
         m_parallel = x.m_parallel;
         m_maxProcessors = x.m_maxProcessors;
      }
      return *this;
   }

   /*!
    * Returns true if this %FFTConvolution object uses a KernelFilter object as
    * its response function; false if it uses an image.
    */
   bool UsingFilter() const
   {
      return m_filter != 0;
   }

   /*!
    * Returns a constant reference to the kernel filter currently associated
    * with this %FFTConvolution object. If this object does not use a kernel
    * filter, either because it uses an image as its response function, or
    * because this object has not been initialized, the returned filter is
    * empty (i.e., it has no filter coefficients and zero dimensions).
    */
   const KernelFilter& Filter() const
   {
      PCL_PRECONDITION( m_filter != 0 )
      ValidateFilter();
      return *m_filter;
   }

   /*!
    * Returns a reference to the filter image currently associated with this
    * %FFTConvolution object. The referenced ImageVariant object will transport
    * no image if this object does not use a filter image, either because it
    * uses a kernel filter as its response function, or because this object has
    * not been initialized.
    *
    * The response function is actually the image transported by the returned
    * ImageVariant. It can be an image of any supported data type.
    */
   const ImageVariant& FilterImage() const
   {
      return m_image;
   }

   /*!
    * Sets a new kernel filter as the response function to be applied by this
    * %FFTConvolution object.
    *
    * The specified object does not have to remain valid while this
    * %FFTConvolution instance is actively used, since it owns a private copy
    * of the filter (note that KernelFilter is a reference-counted class).
    */
   void SetFilter( const KernelFilter& filter )
   {
      Destroy();
      m_filter = filter.Clone();
      PCL_CHECK( !m_filter->IsEmpty() )
   }

   /*!
    * Sets a new filter image as the response function to be applied by this
    * %FFTConvolution object.
    *
    * The image transported by the specified ImageVariant must remain valid
    * while this %FFTConvolution instance is actively used, or until a new
    * response function is associated with this instance. The %ImageVariant
    * doesn't have to remain valid, since this object owns a private
    * %ImageVariant that transports the response function image.
    */
   void SetFilter( const ImageVariant& filter )
   {
      Destroy();
      m_image = filter;
      PCL_CHECK( bool( m_image ) )
   }

   /*!
    * Returns a pointer to the discrete Fourier transform (DFT) of the complex
    * <em>response function</em> used internally by this %FFTConvolution
    * object, or zero if this object has not been yet initialized.
    *
    * The internal DFT of the response function is created and initialized the
    * first time this %FFTConvolution object is used to perform a convolution.
    * It is reused to save memory and CPU resources, as long as successive
    * transformations apply to target images with the same dimensions, or
    * otherwise it is re-created on the fly, as necessary. It is destroyed when
    * a new filter is associated with this object.
    *
    * This function returns a pointer to a complex image that stores the DFT of
    * the original filter after transforming it to <em>wrap around order</em>.
    * This means that the original filter data has been splitted, mirrored, and
    * redistributed to occupy the four corners of the complex 2-D matrix prior
    * to calculating its DFT.
    */
   ComplexImage* ResponseFunctionDFT()
   {
      return m_h;
   }

   /*!
    * Returns a pointer to the (constant) discrete Fourier transform (DFT) of
    * the complex <em>response function</em> used internally by this
    * %FFTConvolution object, or zero if this object has not been initialized.
    *
    * See the documentation for non-const ResponseFunctionDFT().
    */
   const ComplexImage* ResponseFunctionDFT() const
   {
      return m_h;
   }

   /*!
    * Returns true if this object is allowed to use multiple parallel execution
    * threads (when multiple threads are permitted and available).
    */
   bool IsParallelProcessingEnabled() const
   {
      return m_parallel;
   }

   /*!
    * Enables parallel processing for this instance of %FFTConvolution.
    *
    * \param enable  Whether to enable or disable parallel processing. True by
    *                default.
    *
    * \param maxProcessors    The maximum number of processors allowed for this
    *                instance of %FFTConvolution. If \a enable is false
    *                this parameter is ignored. A value <= 0 is ignored. The
    *                default value is zero.
    */
   void EnableParallelProcessing( bool enable = true, int maxProcessors = 0 )
   {
      m_parallel = enable;
      if ( enable && maxProcessors > 0 )
         SetMaxProcessors( maxProcessors );
   }

   /*!
    * Disables parallel processing for this instance of %FFTConvolution.
    *
    * This is a convenience function, equivalent to:
    * EnableParallelProcessing( !disable )
    */
   void DisableParallelProcessing( bool disable = true )
   {
      EnableParallelProcessing( !disable );
   }

   /*!
    * Returns the maximum number of processors allowed for this instance of
    * %FFTConvolution.
    *
    * Irrespective of the value returned by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   int MaxProcessors() const
   {
      return m_maxProcessors;
   }

   /*!
    * Sets the maximum number of processors allowed for this instance of
    * %FFTConvolution.
    *
    * In the current version of PCL, a module can use a maximum of 1023
    * processors. The term \e processor actually refers to the number of
    * threads a module can execute concurrently.
    *
    * Irrespective of the value specified by this function, a module should not
    * use more processors than the maximum number of parallel threads allowed
    * for external modules on the PixInsight platform. This number is given by
    * the "Process/MaxProcessors" global variable (refer to the GlobalSettings
    * class for information on global variables).
    */
   void SetMaxProcessors( int maxProcessors )
   {
      m_maxProcessors = unsigned( Range( maxProcessors, 1, PCL_MAX_PROCESSORS ) );
   }

protected:

   /*
    * The response function for convolution is defined as either a KernelFilter
    * instance or as an image. In the latter case, the image transported by the
    * ImageVariant must remain valid while this object is actively used.
    */
   KernelFilter* m_filter;
   ImageVariant  m_image;
   bool          m_parallel      : 1;
   unsigned      m_maxProcessors : PCL_MAX_PROCESSORS_BITCOUNT; // Maximum number of processors allowed

   /*
    * Internal DFT of the response function. Initially zero. This matrix is
    * generated/reused/regenerated as this object is applied to convolve
    * different target images. It is destroyed when a new filter is specified.
    */
   mutable ComplexImage* m_h;

   /*
    * In-place Fourier-based 2-D convolution algorithm.
    */
   virtual void Apply( pcl::Image& ) const;
   virtual void Apply( pcl::DImage& ) const;
   virtual void Apply( pcl::UInt8Image& ) const;
   virtual void Apply( pcl::UInt16Image& ) const;
   virtual void Apply( pcl::UInt32Image& ) const;
   virtual void Apply( pcl::ComplexImage& ) const;
   virtual void Apply( pcl::DComplexImage& ) const;

   friend class PCL_FFTConvolutionEngine;

private:

   void Validate() const;
   void ValidateFilter() const;

   void Destroy()
   {
      DestroyResponseFunction();
      DestroyDFT();
   }

   void DestroyResponseFunction()
   {
      if ( m_filter != 0 )
         delete m_filter, m_filter = 0;
      m_image.Free();
   }

   void DestroyDFT()
   {
      if ( m_h != 0 )
         delete m_h, m_h = 0;
   }
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_FFTConvolution_h

// ****************************************************************************
// EOF pcl/FFTConvolution.h - Released 2014/11/14 17:16:39 UTC
