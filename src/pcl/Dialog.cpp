// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/Dialog.cpp - Released 2014/11/14 17:17:00 UTC
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

#include <pcl/Dialog.h>

#include <pcl/api/APIInterface.h>
#include <pcl/api/APIException.h>

namespace pcl
{

// ----------------------------------------------------------------------------

#define sender    (reinterpret_cast<Dialog*>( hSender ))
#define receiver  (reinterpret_cast<Control*>( hReceiver ))

class DialogEventDispatcher
{
public:

   static void Executed( control_handle hSender, control_handle hReceiver )
   {
      if ( sender->onExecute != 0 )
         (receiver->*sender->onExecute)( *sender );
   }

   static void Returned( control_handle hSender, control_handle hReceiver, int32 retVal )
   {
      if ( sender->onReturn != 0 )
         (receiver->*sender->onReturn)( *sender, retVal );
   }
}; // DialogEventDispatcher

#undef sender
#undef receiver

// ----------------------------------------------------------------------------

#ifdef _MSC_VER
#  pragma warning( disable: 4355 ) // 'this' : used in base member initializer list
#endif

Dialog::Dialog( Control& parent ) :
Control( (*API->Dialog->CreateDialog)( ModuleHandle(), this, parent.handle, 0 /*flags*/ ) ),
onExecute( 0 ),
onReturn( 0 )
{
   if ( handle == 0 )
      throw APIFunctionError( "CreateDialog" );
}

// ----------------------------------------------------------------------------

void Dialog::OnExecute( execute_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onExecute = 0;
   if ( (*API->Dialog->SetExecuteDialogEventRoutine)( handle, &receiver,
        (f != 0) ? DialogEventDispatcher::Executed : 0 ) == api_false )
   {
      throw APIFunctionError( "SetExecuteDialogEventRoutine" );
   }
   onExecute = f;
}

void Dialog::OnReturn( return_event_handler f, Control& receiver )
{
   __PCL_NO_ALIAS_HANDLER;
   onReturn = 0;
   if ( (*API->Dialog->SetReturnDialogEventRoutine)( handle, &receiver,
        (f != 0) ? DialogEventDispatcher::Returned : 0 ) == api_false )
   {
      throw APIFunctionError( "SetReturnDialogEventRoutine" );
   }
   onReturn = f;
}

// ----------------------------------------------------------------------------

int Dialog::Execute()
{
   return (*API->Dialog->ExecuteDialog)( handle );
}

// ----------------------------------------------------------------------------

void Dialog::Open()
{
   (*API->Dialog->OpenDialog)( handle );
}

// ----------------------------------------------------------------------------

void Dialog::Return( int retVal )
{
   (*API->Dialog->ReturnDialog)( handle, retVal );
}

// ----------------------------------------------------------------------------

bool Dialog::IsUserResizable() const
{
   return (*API->Dialog->GetDialogResizable)( handle ) != api_false;
}

// ----------------------------------------------------------------------------

void Dialog::EnableUserResizing( bool enable )
{
   (*API->Dialog->SetDialogResizable)( handle, enable );
}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF pcl/Dialog.cpp - Released 2014/11/14 17:17:00 UTC
