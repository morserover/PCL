// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Geometry Process Module Version 01.01.00.0247
// ****************************************************************************
// IntegerResampleProcess.cpp - Released 2014/11/14 17:18:46 UTC
// ****************************************************************************
// This file is part of the standard Geometry PixInsight module.
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

#include "IntegerResampleProcess.h"
#include "IntegerResampleParameters.h"
#include "IntegerResampleInstance.h"
#include "IntegerResampleInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

IntegerResampleProcess* TheIntegerResampleProcess = 0;

// ----------------------------------------------------------------------------

#include "IntegerResampleIcon.xpm"

// ----------------------------------------------------------------------------

IntegerResampleProcess::IntegerResampleProcess() : MetaProcess()
{
   TheIntegerResampleProcess = this;

   // Instantiate process parameters

   new ZoomFactor( this );
   new IntegerDownsamplingMode( this );
   TheXResolutionIntegerResampleParameter = new XResolution( this );
   TheYResolutionIntegerResampleParameter = new YResolution( this );
   TheMetricResolutionIntegerResampleParameter = new MetricResolution( this );
   TheForceResolutionIntegerResampleParameter = new ForceResolution( this );
}

// ----------------------------------------------------------------------------

IsoString IntegerResampleProcess::Id() const
{
   return "IntegerResample";
}

// ----------------------------------------------------------------------------

IsoString IntegerResampleProcess::Category() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

uint32 IntegerResampleProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String IntegerResampleProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** IntegerResampleProcess::IconImageXPM() const
{
   return IntegerResampleIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* IntegerResampleProcess::DefaultInterface() const
{
   return TheIntegerResampleInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* IntegerResampleProcess::Create() const
{
   return new IntegerResampleInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* IntegerResampleProcess::Clone( const ProcessImplementation& p ) const
{
   const IntegerResampleInstance* instPtr = dynamic_cast<const IntegerResampleInstance*>( &p );
   return (instPtr != 0) ? new IntegerResampleInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF IntegerResampleProcess.cpp - Released 2014/11/14 17:18:46 UTC
