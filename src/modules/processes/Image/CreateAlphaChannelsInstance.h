// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Image Process Module Version 01.02.08.0283
// ****************************************************************************
// CreateAlphaChannelsInstance.h - Released 2014/11/14 17:19:21 UTC
// ****************************************************************************
// This file is part of the standard Image PixInsight module.
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

#ifndef __CreateAlphaChannelsInstance_h
#define __CreateAlphaChannelsInstance_h

#include <pcl/ProcessImplementation.h>

#include "CreateAlphaChannelsParameters.h"

namespace pcl
{

// ----------------------------------------------------------------------------
// CreateAlphaChannelsInstance
// ----------------------------------------------------------------------------

class CreateAlphaChannelsInstance : public ProcessImplementation
{
public:

   CreateAlphaChannelsInstance( const MetaProcess* );
   CreateAlphaChannelsInstance( const CreateAlphaChannelsInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool IsMaskable( const View&, const ImageWindow& ) const;

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool ExecuteOn( View& );

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );
   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   // -------------------------------------------------------------------------

private:

   pcl_bool fromImage;
   String   imageId;
   pcl_bool invertSourceImage;
   pcl_bool closeSourceImage;
   double   transparency;
   pcl_bool replace;
   int32    count; // only accesible through command-line interface

   friend class CreateAlphaChannelsProcess;
   friend class CreateAlphaChannelsInterface;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __CreateAlphaChannelsInstance_h

// ****************************************************************************
// EOF CreateAlphaChannelsInstance.h - Released 2014/11/14 17:19:21 UTC
