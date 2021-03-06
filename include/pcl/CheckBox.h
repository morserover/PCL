// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/CheckBox.h - Released 2014/11/14 17:16:40 UTC
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

#ifndef __PCL_CheckBox_h
#define __PCL_CheckBox_h

/// \file pcl/CheckBox.h

#ifndef __PCL_BUILDING_PIXINSIGHT_APPLICATION

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Button_h
#include <pcl/Button.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class CheckBox
 * \brief Client-side interface to a PixInsight %CheckBox control
 *
 * ### TODO: Write a detailed description for %CheckBox
 */
class PCL_CLASS CheckBox : public Button
{
public:

   /*!
    * Constructs a %CheckBox with the specified \a text, as a child control of
    * \a parent.
    */
   CheckBox( const String& text = String(), Control& parent = Control::Null() );

   /*!
    * Destroys a %CheckBox control.
    */
   virtual ~CheckBox()
   {
   }

   /*!
    * Returns \c false, since check boxes are not pushable buttons.
    */
   virtual bool IsPushable() const
   {
      return false;
   }

   /*!
    * Returns \c true, since check boxes are checkable buttons.
    */
   virtual bool IsCheckable() const
   {
      return true;
   }

   /*!
    * Returns \c true if this check box can have three states.
    */
   bool IsTristateMode() const;

   /*!
    * Enables or disables <em>tristate mode</em>. In tristate mode, a check box
    * can have three states: checked, unchecked, and <em>semi-checked</em>
    * (also called <em>third state</em>).
    */
   void SetTristateMode( bool = true );
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PCL_BUILDING_PIXINSIGHT_APPLICATION

#endif   // __PCL_CheckBox_h

// ****************************************************************************
// EOF pcl/CheckBox.h - Released 2014/11/14 17:16:40 UTC
