// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Geometry Process Module Version 01.01.00.0247
// ****************************************************************************
// ResampleProcess.cpp - Released 2014/11/14 17:18:46 UTC
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

#include "ResampleProcess.h"
#include "ResampleParameters.h"
#include "ResampleInstance.h"
#include "ResampleInterface.h"

namespace pcl
{

// ----------------------------------------------------------------------------

ResampleProcess* TheResampleProcess = 0;

// ----------------------------------------------------------------------------

#include "ResampleIcon.xpm"

// ----------------------------------------------------------------------------

ResampleProcess::ResampleProcess() : MetaProcess()
{
   TheResampleProcess = this;

   // Instantiate process parameters

   new XSize( this );
   new YSize( this );
   new ResampleMode( this );
   new AbsoluteResampleMode( this );
   TheXResolutionResampleParameter = new XResolution( this );
   TheYResolutionResampleParameter = new YResolution( this );
   TheMetricResolutionResampleParameter = new MetricResolution( this );
   TheForceResolutionResampleParameter = new ForceResolution( this );
   TheInterpolationAlgorithmResampleParameter = new InterpolationAlgorithm( this );
   TheClampingThresholdResampleParameter = new ClampingThreshold( this );
   TheSmoothnessResampleParameter = new Smoothness( this );
}

// ----------------------------------------------------------------------------

IsoString ResampleProcess::Id() const
{
   return "Resample";
}

// ----------------------------------------------------------------------------

IsoString ResampleProcess::Category() const
{
   return "Geometry";
}

// ----------------------------------------------------------------------------

uint32 ResampleProcess::Version() const
{
   return 0x100;
}

// ----------------------------------------------------------------------------

String ResampleProcess::Description() const
{
   return "";
}

// ----------------------------------------------------------------------------

const char** ResampleProcess::IconImageXPM() const
{
   return ResampleIcon_XPM;
}

// ----------------------------------------------------------------------------

ProcessInterface* ResampleProcess::DefaultInterface() const
{
   return TheResampleInterface;
}

// -------------------------------------------------------------------------

ProcessImplementation* ResampleProcess::Create() const
{
   return new ResampleInstance( this );
}

// ----------------------------------------------------------------------------

ProcessImplementation* ResampleProcess::Clone( const ProcessImplementation& p ) const
{
   const ResampleInstance* instPtr = dynamic_cast<const ResampleInstance*>( &p );
   return (instPtr != 0) ? new ResampleInstance( *instPtr ) : 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF ResampleProcess.cpp - Released 2014/11/14 17:18:46 UTC
