// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/BidimensionalInterpolation.h - Released 2014/11/14 17:16:39 UTC
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

#ifndef __PCL_BidimensionalInterpolation_h
#define __PCL_BidimensionalInterpolation_h

/// \file pcl/BidimensionalInterpolation.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_Exception_h
#include <pcl/Exception.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class BidimensionalInterpolation
 * \brief A generic interface to two-dimensional interpolation algorithms.
 *
 * %BidimensionalInterpolation is a base class for all two-dimensional
 * interpolation algorithm implementations in the PCL.
 */
template <typename T>
class PCL_CLASS BidimensionalInterpolation
{
public:

   /*!
    * Constructs a %BidimensionalInterpolation object.
    */
   BidimensionalInterpolation() :
   m_data( 0 ), m_width( 0 ), m_height( 0 ), m_fillValue( T( 0 ) ), m_fillBorder( false )
   {
   }

   /*!
    * Destroys a %BidimensionalInterpolation object.
    */
   virtual ~BidimensionalInterpolation()
   {
      Clear();
   }

   /*!
    * Initializes a new interpolation.
    *
    * \param data    Two-dimensional matrix of function values.
    *
    * \param width   Horizontal dimension (most rapidly varying dimension) of
    *                the data array.
    *
    * \param height  Vertical dimension (most slowly varying dimension) of the
    *                data array.
    */
   virtual void Initialize( const T* data, int width, int height )
   {
      if ( data == 0 )
         throw Error( "Null data pointer in BidimensionalInterpolation::Initialize()" );
      if ( width <= 0 || height <= 0 )
         throw Error( "Invalid dimensions in BidimensionalInterpolation::Initialize()" );
      m_data = data;
      m_width = width;
      m_height = height;
   }

   /*!
    * Returns an interpolated function value at \a x, \a y location.
    */
   virtual double operator()( double x, double y ) const = 0;

   /*!
    * Clears auxiliary/intermediate data (and/or whatever requires clean up).
    * Derived classes overriding this function should call their base class
    * version via explicit downcast.
    */
   virtual void Clear()
   {
      m_data = 0;
      m_width = m_height = 0;
   }

   /*!
    * Returns a pointer to the interpolated data array.
    */
   const T* BeingInterpolated() const
   {
      return m_data;
   }

   /*!
    * Returns the width (number of columns) of the interpolated data matrix.
    */
   int Width() const
   {
      return m_width;
   }

   /*!
    * Returns the height (number of rows) of the interpolated data matrix.
    */
   int Height() const
   {
      return m_height;
   }

   /*!
    * Enables (or disables) border filling.
    *
    * When border filling is enabled, a user-defined constant fill value is
    * used to interpolate at locations near the borders of the data matrix.
    *
    * When border filling is disabled, existing border values are extended to
    * interpolate at border locations (Neumann boundary conditions).
    */
   void EnableBorderFilling( bool enable = true )
   {
      m_fillBorder = enable;
   }

   /*!
    * Disables (or enables) border filling.
    *
    * This is a convenience member function, equivalent to:
    *
    * EnableBorderFilling( !disable );
    */
   void DisableBorderFilling( bool disable = true )
   {
      m_fillBorder = !disable;
   }

   /*!
    * Returns true if border filling is enabled for this interpolation. See the
    * documentation for EnableBorderFilling() for more information.
    */
   bool IsBorderFillingEnabled() const
   {
      return m_fillBorder;
   }

   /*!
    * Sets the border fill value \a v. See the documentation for
    * EnableBorderFilling() for more information about border filling and
    * boundary conditions.
    *
    * \param v    Border fill value. It is the responsibility of the caller to
    *             ensure that the specified value is within the valid range of
    *             the data type used by this interpolation.
    */
   void SetBorderFillValue( double v )
   {
      m_fillValue = v;
   }

   /*!
    * Returns the current border fill value for this interpolation. See the
    * documentation for EnableBorderFilling() for more information about border
    * filling and boundary conditions.
    */
   double BorderFillValue() const
   {
      return m_fillValue;
   }

protected:

   const T* m_data;           // functional data being interpolated
   int      m_width;          // width of the data table
   int      m_height;         // height of the data table
   double   m_fillValue;      // fill value, when m_fillBorder = true
   bool     m_fillBorder : 1; // don't apply Neumann boundary conditions
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_BidimensionalInterpolation_h

// ****************************************************************************
// EOF pcl/BidimensionalInterpolation.h - Released 2014/11/14 17:16:39 UTC
