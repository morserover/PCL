// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// Standard Global Process Module Version 01.02.05.0260
// ****************************************************************************
// PreferencesInstance.h - Released 2014/11/14 17:18:47 UTC
// ****************************************************************************
// This file is part of the standard Global PixInsight module.
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

#ifndef __PreferencesInstance_h
#define __PreferencesInstance_h

#include <pcl/ProcessImplementation.h>
#include <pcl/MetaParameter.h> // pcl_bool, pcl_enum

namespace pcl
{

// ----------------------------------------------------------------------------

struct ApplicationPreferences
{
   String      styleSheetFile;
   String      resourceFile01;
   String      resourceFile02;
   String      resourceFile03;
   String      resourceFile04;
   String      resourceFile05;
   String      resourceFile06;
   String      resourceFile07;
   String      resourceFile08;
   String      resourceFile09;
   String      resourceFile10;
};

struct MainWindowPreferences
{
   pcl_bool    maximizeAtStartup;
   pcl_bool    fullScreenAtStartup;
   pcl_bool    showSplashAtStartup;
   pcl_bool    checkForUpdatesAtStartup;
   pcl_bool    confirmProgramTermination;
   pcl_bool    acceptDroppedFiles;
   pcl_bool    doubleClickLaunchesOpenDialog;
   pcl_bool    hoverableAutoHideWindows;
   pcl_bool    desktopSettingsAware;
   pcl_bool    nativeMenuBar; // Mac OS X only
   pcl_bool    capitalizedMenuBars;
   pcl_bool    translucentWindows;
   pcl_bool    translucentChildWindows; // only enabled by default on X11 because of Qt bugs
   pcl_bool    fadeWindows;
   pcl_bool    fadeAutoHideWindows;
   pcl_bool    fadeWorkspaces;
   pcl_bool    fadeMenu;
   pcl_bool    fadeToolTip;
   pcl_bool    explodeIcons;
   pcl_bool    implodeIcons;
   pcl_bool    animateWindows;
   pcl_bool    animateMenu;
   pcl_bool    animateCombo;
   pcl_bool    animateToolTip;
   pcl_bool    animateToolBox;
   int32       maxRecentFiles;
};

struct ImageWindowPreferences
{
   pcl_bool    backupFiles;
   pcl_bool    defaultMasksShown;
   int32       defaultMaskMode;
   int32       defaultTransparencyMode;
   double      defaultHorizontalResolution;
   double      defaultVerticalResolution;
   pcl_bool    defaultMetricResolution;
   pcl_bool    defaultEmbedMetadata;
   pcl_bool    defaultEmbedThumbnails;
   pcl_bool    defaultEmbedProperties;
   String      defaultFileExtension;
   pcl_bool    nativeFileDialogs;
   pcl_bool    rememberFileOpenType;
   pcl_bool    rememberFileSaveType;
   pcl_bool    strictFileSaveMode;
   pcl_bool    useFileNamesAsImageIdentifiers;
   int32       cursorTolerance;
   pcl_bool    touchEvents;
   double      pinchSensitivity;
   pcl_bool    fastScreenRenditions;
   int32       fastScreenRenditionThreshold;
   pcl_bool    default24BitScreenLUT;
   StringList  swapDirectories;
   String      downloadsDirectory;
   pcl_bool    followDownloadLocations;
   pcl_bool    verboseNetworkOperations;
   pcl_bool    showCaptionCurrentChannels;
   pcl_bool    showCaptionZoomRatios;
   pcl_bool    showCaptionIdentifiers;
   pcl_bool    showCaptionFullPaths;
   pcl_bool    showActiveSTFIndicators;
   int32       transparencyBrush; // corresponds to pcl::BackgroundBrush::value_type
   uint32      transparencyBrushForegroundColor;
   uint32      transparencyBrushBackgroundColor;
   uint32      defaultTransparencyColor;
};

struct IdentifiersPreferences
{
   String      workspacePrefix;
   String      imagePrefix;
   String      previewPrefix;
   String      processIconPrefix;
   String      imageContainerIconPrefix;
   String      newImageCaption;
   String      clonePostfix;
   String      noViewsAvailableText;
   String      noViewSelectedText;
   String      noPreviewsAvailableText;
   String      noPreviewSelectedText;
   String      brokenLinkText;
};

struct ProcessPreferences
{
   pcl_bool    enableParallelProcessing;
   pcl_bool    enableParallelCoreRendering;
   pcl_bool    enableParallelCoreColorManagement;
   pcl_bool    enableParallelModuleProcessing;
   pcl_bool    enableThreadCPUAffinity;
   int32       maxModuleThreadPriority;
   int32       maxProcessors;
   pcl_bool    backupFiles; // for PSM files
   pcl_bool    generateScriptComments;
   int32       maxConsoleLines;
   int32       consoleDelay;  // in ms
   int32       autoSavePSMPeriod; // seconds, <= 0 : disabled, >= 30 otherwise
   pcl_bool    alertOnProcessCompleted;
};

// ----------------------------------------------------------------------------

class PreferencesInstance : public ProcessImplementation
{
public:

   PreferencesInstance( const MetaProcess* );
   PreferencesInstance( const PreferencesInstance& );

   virtual void Assign( const ProcessImplementation& );

   virtual bool CanExecuteOn( const View&, pcl::String& whyNot ) const;
   virtual bool CanExecuteGlobal( pcl::String& whyNot ) const;

   virtual bool ExecuteGlobal();

   virtual void* LockParameter( const MetaParameter*, size_type tableRow );

   virtual bool AllocateParameter( size_type sizeOrLength, const MetaParameter* p, size_type tableRow );
   virtual size_type ParameterLength( const MetaParameter* p, size_type tableRow ) const;

   void LoadDefaultSettings();
   void LoadCurrentSettings();

   // -------------------------------------------------------------------------

private:

   ApplicationPreferences application;
   MainWindowPreferences  mainWindow;
   ImageWindowPreferences imageWindow;
   IdentifiersPreferences identifiers;
   ProcessPreferences     process;

   String* StringParameterFromMetaParameter( const MetaParameter* );

   friend class PreferencesProcess;
   friend class PreferencesInterface;

   friend class MainWindowPreferencesPage;
   friend class ResourcesPreferencesPage;
   friend class GUIEffectsPreferencesPage;
   friend class FileIOPreferencesPage;
   friend class DirectoriesAndNetworkPreferencesPage;
   friend class DefaultImageResolutionPreferencesPage;
   friend class DefaultMaskSettingsPreferencesPage;
   friend class DefaultTransparencySettingsPreferencesPage;
   friend class MiscImageWindowSettingsPreferencesPage;
   friend class IdentifiersPreferencesPage;
   friend class StringsPreferencesPage;
   friend class ParallelProcessingPreferencesPage;
   friend class MiscProcessingPreferencesPage;
   friend class TransparencyColorsPreferencesPage;
};

// ----------------------------------------------------------------------------

} // pcl

#endif   // __PreferencesInstance_h

// ****************************************************************************
// EOF PreferencesInstance.h - Released 2014/11/14 17:18:47 UTC
