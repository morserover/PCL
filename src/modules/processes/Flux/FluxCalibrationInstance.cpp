// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Flux Process Module Version 01.00.00.0066
// ****************************************************************************
// FluxCalibrationInstance.cpp - Released 2014/11/14 17:18:46 UTC
// ****************************************************************************
// This file is part of the standard Flux PixInsight module.
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

#include "FluxCalibrationInstance.h"
#include "FluxCalibrationParameters.h"

#include <pcl/View.h>
#include <pcl/StdStatus.h>
#include <pcl/Console.h>

namespace pcl
{

// ----------------------------------------------------------------------------

const double inv_ch = 1/1.986e-9;

// ----------------------------------------------------------------------------

FluxCalibrationInstance::FluxCalibrationInstance( const MetaProcess* m ) :
ProcessImplementation( m )
{
   p_wavelength.value = 0;
   p_wavelength.mode = FCParameterMode::Literal;
   p_wavelength.stdKeyword = "FLTWAVE";

   p_transmissivity.value = 1;
   p_transmissivity.mode = FCParameterMode::Literal;
   p_transmissivity.stdKeyword = "FLTTRANS";

   p_filterWidth.value = 0;
   p_filterWidth.mode = FCParameterMode::Literal;
   p_filterWidth.stdKeyword = "FLTWIDTH";

   p_aperture.value = 0;
   p_aperture.mode = FCParameterMode::StandardKeyword;
   p_aperture.stdKeyword = "APTDIA";

   p_centralObstruction.value = 0;
   p_centralObstruction.mode = FCParameterMode::StandardKeyword;
   p_centralObstruction.stdKeyword = "OBSTDIA";

   p_exposureTime.value = 0;
   p_exposureTime.mode = FCParameterMode::StandardKeyword;
   p_exposureTime.stdKeyword = "EXPTIME";

   p_atmosphericExtinction.value = 0;
   p_atmosphericExtinction.mode = FCParameterMode::Literal;
   p_atmosphericExtinction.stdKeyword = "ATMEXTIN";

   p_sensorGain.value = 1;
   p_sensorGain.mode = FCParameterMode::Literal;
   p_sensorGain.stdKeyword = "CCDSENS";

   p_quantumEfficiency.value = 1;
   p_quantumEfficiency.mode = FCParameterMode::Literal;
   p_quantumEfficiency.stdKeyword = "CCDQE";
}

FluxCalibrationInstance::FluxCalibrationInstance( const FluxCalibrationInstance& x ) :
ProcessImplementation( x )
{
   Assign( x );
}

void FluxCalibrationInstance::Assign( const ProcessImplementation& p )
{
   const FluxCalibrationInstance* x = dynamic_cast<const FluxCalibrationInstance*>( &p );
   if ( x != 0 )
   {
      p_wavelength = x->p_wavelength;
      p_transmissivity = x->p_transmissivity;
      p_filterWidth = x->p_filterWidth;
      p_aperture = x->p_aperture;
      p_centralObstruction = x->p_centralObstruction;
      p_exposureTime = x->p_exposureTime;
      p_atmosphericExtinction = x->p_atmosphericExtinction;
      p_sensorGain = x->p_sensorGain;
      p_quantumEfficiency = x->p_quantumEfficiency;
   }
}

UndoFlags FluxCalibrationInstance::UndoMode( const View& ) const
{
   return UndoFlag::Keywords | UndoFlag::PixelData;
}

class FluxCalibrationEngine
{
public:

   template <class P>
   static void Apply( GenericImage<P>& img, const View& view, const FluxCalibrationInstance& instance )
   {
      FITSKeywordArray inputKeywords;
      view.Window().GetKeywords( inputKeywords );

      if ( KeywordExists( inputKeywords, "FLXMIN" ) ||
           KeywordExists( inputKeywords, "FLXRANGE" ) ||
           KeywordExists( inputKeywords, "FLX2DN" ) )
      {
         throw Error( "Already calibrated image" );
      }

      if ( img.IsColor() )
         throw Error( "Can't calibrate a color image" );

      float Wc       =             instance.p_wavelength.GetValue( inputKeywords );
      float Tr       =  Max( 1.0F, instance.p_transmissivity.GetValue( inputKeywords ) );
      float Delta    =             instance.p_filterWidth.GetValue( inputKeywords );
      float Ap       =             instance.p_aperture.GetValue( inputKeywords ) / 10; // mm -> cm
      float Cobs     =  Max( 0.0F, instance.p_centralObstruction.GetValue( inputKeywords ) ) / 10; // mm -> cm
      float ExpT     =             instance.p_exposureTime.GetValue( inputKeywords );
      float AtmE     =  Max( 0.0F, instance.p_atmosphericExtinction.GetValue( inputKeywords ) );
      float G        =  Max( 1.0F, instance.p_sensorGain.GetValue( inputKeywords ) );
      float QEff     =  Max( 1.0F, instance.p_quantumEfficiency.GetValue( inputKeywords ) );

      if ( Wc <= 0 )
         throw Error( "Invalid filter wavelength" );

      if ( Tr <= 0 || Tr > 1 )
         throw Error( "Invalid filter transmissivity" );

      if ( Delta <= 0 )
         throw Error( "Invalid filter width" );

      if ( Ap <= 0 )
         throw Error( "Invalid aperture" );

      if ( Cobs < 0 || Cobs >= Ap )
         throw Error( "Invalid central obstruction area" );

      if ( ExpT <= 0 )
         throw Error( "Invalid exposure time" );

      if ( AtmE < 0 || AtmE >= 1 )
         throw Error( "Invalid atmospheric extinction" );

      if ( G <= 0 )
         throw Error( "Invalid sensor gain" );

      if ( QEff <= 0 || QEff > 1 )
         throw Error( "Invalid quantum efficiency" );

      FITSKeywordArray keywords;
      float pedestal = 0;
      bool  foundPedestal = false;
      for ( FITSKeywordArray::const_iterator i = inputKeywords.Begin(); i != inputKeywords.End(); ++i )
         if ( i->name == "PEDESTAL" )
         {
            if ( i->value.TryToFloat( pedestal ) )
               foundPedestal = true;
            pedestal /= 65535; // 2^16-1 maximum value of a 16bit CCD.
         }
         else
            keywords.Add( *i );

      if ( foundPedestal )
         Console().NoteLn( "<end><cbr><br>* FluxCalibration: PEDESTAL keyword found: " + view.FullId() );

      // double F = Wc * inv_ch * (1 - Tr) * Delta * Ap * Cobs * ExpT * AtmE * G * QEff;
      double F = Wc * inv_ch * (1 - AtmE) * Delta * ( Const<double>::pi() / 4 * ( Ap*Ap - Cobs*Cobs  ) ) * ExpT * Tr * G * QEff;

      size_type N = img.NumberOfPixels();
            typename P::sample* f   = img.PixelData( 0 );
      const typename P::sample* fN  = f + N;

      double flxMin = DBL_MAX;
      double flxMax = 0;
      for ( ; f < fN; ++f, ++img.Status() )
      {
         double I; P::FromSample( I, *f );
         I = (I - pedestal)/F;
         *f = P::ToSample( I );
         if ( I < flxMin )
            flxMin = I;
         if ( I > flxMax )
            flxMax = I;
      }

      img.Rescale();

      keywords.Add( FITSHeaderKeyword( "FLXMIN",
                                       IsoString().Format( "%.8e", flxMin ),
                                       "" ) );
      keywords.Add( FITSHeaderKeyword( "FLXRANGE",
                                       IsoString().Format( "%.8e", flxMax - flxMin ),
                                       "FLXRANGE*pixel_value + FLXMIN = erg/cm^2/s/nm" ) );
      keywords.Add( FITSHeaderKeyword( "FLX2DN",
                                       IsoString().Format( "%.8e", F*65535 ),
                                       "(FLXRANGE*pixel_value + FLXMIN)*FLX2DN = DN" ) );

      view.Window().SetKeywords( keywords );
   }

   static bool KeywordExists( const FITSKeywordArray& keywords, const String keywordName )
   {
      for ( FITSKeywordArray::const_iterator i = keywords.Begin(); i != keywords.End(); ++i )
         if ( String( i->name ) == keywordName )
            return true;
      return false;
   }
};

bool FluxCalibrationInstance::CanExecuteOn( const View& view, pcl::String& whyNot ) const
{
   if ( view.Image().IsComplexSample() )
   {
      whyNot = "FluxCalibration cannot be executed on complex images.";
      return false;
   }

   if ( view.Image().AnyImage()->IsColor() )
   {
      whyNot = "FluxCalibration cannot be executed on color images.";
      return false;
   }

   FITSKeywordArray inputKeywords;
   view.Window().GetKeywords( inputKeywords );

   if ( FluxCalibrationEngine::KeywordExists( inputKeywords, "FLXMIN" ) ||
        FluxCalibrationEngine::KeywordExists( inputKeywords, "FLXRANGE" ) ||
        FluxCalibrationEngine::KeywordExists( inputKeywords, "FLX2DN" ) )
   {
      whyNot = "FluxCalibration cannot be executed on an already flux-calibrated image.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool FluxCalibrationInstance::ExecuteOn( View& view )
{
   try
   {
      view.Lock();

      ImageVariant v;
      v = view.Image();
      AbstractImage* img = v.AnyImage();

      StandardStatus status;
      v.AnyImage()->SetStatusCallback( &status );
      img->Status().Initialize( "Flux calibration", img->NumberOfPixels() );

      Console().EnableAbort();

      if ( !v.IsComplexSample() )
         if ( v.IsFloatSample() )
            switch ( v.BitsPerSample() )
            {
            case 32: FluxCalibrationEngine::Apply( *static_cast<Image*>( v.AnyImage() ), view, *this ); break;
            case 64: FluxCalibrationEngine::Apply( *static_cast<DImage*>( v.AnyImage() ), view, *this ); break;
            }
         else
            switch ( v.BitsPerSample() )
            {
            case  8:
            case 16:
               {
                  ImageVariant tmp;
                  tmp.CreateFloatImage( 32 );
                  tmp.CopyImage( v );
                  FluxCalibrationEngine::Apply( *static_cast<Image*>( tmp.AnyImage() ), view, *this );
                  v.CopyImage( tmp );
               }
               break;
            case 32:
               {
                  ImageVariant tmp;
                  tmp.CreateFloatImage( 64 );
                  tmp.CopyImage( v );
                  FluxCalibrationEngine::Apply( *static_cast<DImage*>( tmp.AnyImage() ), view, *this );
                  v.CopyImage( tmp );
               }
               break;
            }

      view.Unlock();

      return true;
   }

   catch ( ... )
   {
      view.Unlock();
      throw;
   }
}

void* FluxCalibrationInstance::LockParameter( const MetaParameter* p, size_type /*tableRow*/ )
{
   if ( p == TheFCWavelengthValueParameter )
      return &p_wavelength.value;
   if ( p == TheFCWavelengthModeParameter )
      return &p_wavelength.mode;
   if ( p == TheFCWavelengthKeywordParameter )
      return p_wavelength.keyword.c_str();

   if ( p == TheFCTransmissivityValueParameter )
      return &p_transmissivity.value;
   if ( p == TheFCTransmissivityModeParameter )
      return &p_transmissivity.mode;
   if ( p == TheFCTransmissivityKeywordParameter )
      return p_transmissivity.keyword.c_str();

   if ( p == TheFCFilterWidthValueParameter )
      return &p_filterWidth.value;
   if ( p == TheFCFilterWidthModeParameter )
      return &p_filterWidth.mode;
   if ( p == TheFCFilterWidthKeywordParameter )
      return p_filterWidth.keyword.c_str();

   if ( p == TheFCApertureValueParameter )
      return &p_aperture.value;
   if ( p == TheFCApertureModeParameter )
      return &p_aperture.mode;
   if ( p == TheFCApertureKeywordParameter )
      return p_aperture.keyword.c_str();

   if ( p == TheFCCentralObstructionValueParameter )
      return &p_centralObstruction.value;
   if ( p == TheFCCentralObstructionModeParameter )
      return &p_centralObstruction.mode;
   if ( p == TheFCCentralObstructionKeywordParameter )
      return p_centralObstruction.keyword.c_str();

   if ( p == TheFCExposureTimeValueParameter )
      return &p_exposureTime.value;
   if ( p == TheFCExposureTimeModeParameter )
      return &p_exposureTime.mode;
   if ( p == TheFCExposureTimeKeywordParameter )
      return p_exposureTime.keyword.c_str();

   if ( p == TheFCAtmosphericExtinctionValueParameter )
      return &p_atmosphericExtinction.value;
   if ( p == TheFCAtmosphericExtinctionModeParameter )
      return &p_atmosphericExtinction.mode;
   if ( p == TheFCAtmosphericExtinctionKeywordParameter )
      return p_atmosphericExtinction.keyword.c_str();

   if ( p == TheFCSensorGainValueParameter )
      return &p_sensorGain.value;
   if ( p == TheFCSensorGainModeParameter )
      return &p_sensorGain.mode;
   if ( p == TheFCSensorGainKeywordParameter )
      return p_sensorGain.keyword.c_str();

   if ( p == TheFCQuantumEfficiencyValueParameter )
      return &p_quantumEfficiency.value;
   if ( p == TheFCQuantumEfficiencyModeParameter )
      return &p_quantumEfficiency.mode;
   if ( p == TheFCQuantumEfficiencyKeywordParameter )
      return p_quantumEfficiency.keyword.c_str();

   return 0;
}

bool FluxCalibrationInstance::AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow )
{
   if ( p == TheFCWavelengthKeywordParameter )
   {
      p_wavelength.keyword.Clear();
      if ( sizeOrLength > 0 )
         p_wavelength.keyword.Reserve( sizeOrLength );
   }
   else if ( p == TheFCTransmissivityKeywordParameter )
   {
      p_transmissivity.keyword.Clear();
      if ( sizeOrLength > 0 )
         p_transmissivity.keyword.Reserve( sizeOrLength );
   }
   else if ( p == TheFCFilterWidthKeywordParameter )
   {
      p_filterWidth.keyword.Clear();
      if ( sizeOrLength > 0 )
         p_filterWidth.keyword.Reserve( sizeOrLength );
   }
   else if ( p == TheFCApertureKeywordParameter )
   {
      p_aperture.keyword.Clear();
      if ( sizeOrLength > 0 )
         p_aperture.keyword.Reserve( sizeOrLength );
   }
   else if ( p == TheFCCentralObstructionKeywordParameter )
   {
      p_centralObstruction.keyword.Clear();
      if ( sizeOrLength > 0 )
         p_centralObstruction.keyword.Reserve( sizeOrLength );
   }
   else if ( p == TheFCExposureTimeKeywordParameter )
   {
      p_exposureTime.keyword.Clear();
      if ( sizeOrLength > 0 )
         p_exposureTime.keyword.Reserve( sizeOrLength );
   }
   else if ( p == TheFCAtmosphericExtinctionKeywordParameter )
   {
      p_atmosphericExtinction.keyword.Clear();
      if ( sizeOrLength > 0 )
         p_atmosphericExtinction.keyword.Reserve( sizeOrLength );
   }
   else if ( p == TheFCSensorGainKeywordParameter )
   {
      p_sensorGain.keyword.Clear();
      if ( sizeOrLength > 0 )
         p_sensorGain.keyword.Reserve( sizeOrLength );
   }
   else if ( p == TheFCQuantumEfficiencyKeywordParameter )
   {
      p_quantumEfficiency.keyword.Clear();
      if ( sizeOrLength > 0 )
         p_quantumEfficiency.keyword.Reserve( sizeOrLength );
   }
   else
      return false;
   return true;
}

size_type FluxCalibrationInstance::ParameterLength( const MetaParameter* p, size_type tableRow ) const
{
   if ( p == TheFCWavelengthKeywordParameter )
      return p_wavelength.keyword.Length();
   if ( p == TheFCTransmissivityKeywordParameter )
      return p_transmissivity.keyword.Length();
   if ( p == TheFCFilterWidthKeywordParameter )
      return p_filterWidth.keyword.Length();
   if ( p == TheFCApertureKeywordParameter )
      return p_aperture.keyword.Length();
   if ( p == TheFCCentralObstructionKeywordParameter )
      return p_centralObstruction.keyword.Length();
   if ( p == TheFCExposureTimeKeywordParameter )
      return p_exposureTime.keyword.Length();
   if ( p == TheFCAtmosphericExtinctionKeywordParameter )
      return p_atmosphericExtinction.keyword.Length();
   if ( p == TheFCSensorGainKeywordParameter )
      return p_sensorGain.keyword.Length();
   if ( p == TheFCQuantumEfficiencyKeywordParameter )
      return p_quantumEfficiency.keyword.Length();
   return 0;
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF FluxCalibrationInstance.cpp - Released 2014/11/14 17:18:46 UTC
