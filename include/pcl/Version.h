// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0689
// ****************************************************************************
// pcl/Version.h - Released 2014/10/29 07:34:11 UTC
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

#ifndef __PCL_Version_h
#define __PCL_Version_h

/// \file pcl/Version.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

#ifndef __PCL_String_h
#include <pcl/String.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class Version
 * \brief Provides PCL version numbers.
 *
 * The %Version class reads PCL version data stored in static read-only
 * variables. It provides version information for the PCL libraries that have
 * been linked to the current module.
 */
class PCL_CLASS Version
{
public:

   /*!
    * Returns the PCL major version number.
    */
   static int Major();

   /*!
    * Returns the PCL minor version number.
    */
   static int Minor();

   /*!
    * Returns the PCL release version number.
    */
   static int Release();

   /*!
    * Returns the PCL build number.
    */
   static int Build();

   /*!
    * Returns the beta version number of the PCL, or zero if the PCL framework
    * is a release (non-beta) version.
    */
   static int BetaRelease();

   /*!
    * Returns an ISO 639.2 language code that identifies the primary language
    * of the PCL framework.
    */
   static String LanguageCode();  // ISO 639.2 language code

   /*!
    * Returns an integer that corresponds to the full PCL version.
    *
    * The returned value is equal to:
    *
    * \code Major()*1000000 + Minor()*1000 + Release(); \endcode
    */
   static int Number()
   {
      return Major()*1000000 + Minor()*1000 + Release();
   }

   /*!
    * Returns a readable representation of the PCL version.
    */
   static String AsString();

private:

   // Not an instantiable class
   Version() { PCL_CHECK( 0 ) }
   Version( const Version& ) { PCL_CHECK( 0 ) }
   virtual ~Version() { PCL_CHECK( 0 ) }
};

// ----------------------------------------------------------------------------

/*!
 * \class PixInsightVersion
 * \brief Provides version information for the PixInsight core application.
 *
 * The %PixInsightVersion class retrieves version data from the PixInsight core
 * application where the current module has been installed.
 */
class PCL_CLASS PixInsightVersion
{
public:

   /*!
    * Returns the major version number of the PixInsight core application.
    */
   static int Major();

   /*!
    * Returns the minor version number of the PixInsight core application.
    */
   static int Minor();

   /*!
    * Returns the release version number of the PixInsight core application.
    */
   static int Release();

   /*!
    * Returns the build number of the PixInsight core application.
    */
   static int Build();

   /*!
    * Returns the beta or release candidate version number of the PixInsight
    * core application:
    *
    * \li Returns zero for a release version.
    * \li Returns a positive nonzero integer for a beta version.
    * \li Returns a negative integer for a release candidate (RC) version.
    */
   static int BetaRelease();

   /*!
    * Returns true if the PixInsight core is a confidential (pre-release)
    * edition.
    */
   static bool Confidential();

   /*!
    * Returns true if the PixInsight core is a limited edition (LE) release.
    */
   static bool LE();

   /*!
    * Returns an ISO 639.2 language code that identifies the primary language
    * of the PixInsight core application.
    */
   static String LanguageCode();

   /*!
    * Returns the codename for the current PixInsight Core application version.
    * Starting from version 1.7, each major PixInsight version is given a
    * codename that uniquely identifies it.
    *
    * Codename history:
    *
    * PixInsight 1.7 Starbuck \n
    * PixInsight 1.8 Ripley
    */
   static String Codename();

   /*!
    * Returns an integer that corresponds to the full PixInsight core version.
    *
    * The returned value is equal to:
    *
    * \code Major()*1000000 + Minor()*1000 + Release(); \endcode
    */
   static int Number()
   {
      return Major()*1000000 + Minor()*1000 + Release();
   }

   /*!
    * Returns a readable representation of the PixInsight core version.
    *
    * \param withCodename  If this parameter is true, the returned version
    *                      string will include the codename of the current
    *                      PixInsight core application. The default value of
    *                      this parameter is false.
    */
   static String AsString( bool withCodename = false );

private:

   // Not an instantiable class
   PixInsightVersion() { PCL_CHECK( 0 ) }
   PixInsightVersion( const PixInsightVersion& ) { PCL_CHECK( 0 ) }
   virtual ~PixInsightVersion() { PCL_CHECK( 0 ) }

   static void Initialize();

   friend class GlobalContextDispatcher;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_Version_h

// ****************************************************************************
// EOF pcl/Version.h - Released 2014/10/29 07:34:11 UTC