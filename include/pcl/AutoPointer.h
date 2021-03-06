// ****************************************************************************
// PixInsight Class Library - PCL 02.00.13.0692
// ****************************************************************************
// pcl/AutoPointer.h - Released 2014/11/14 17:16:40 UTC
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

#ifndef __PCL_AutoPointer_h
#define __PCL_AutoPointer_h

/// \file pcl/AutoPointer.h

#ifndef __PCL_Defs_h
#include <pcl/Defs.h>
#endif

#ifndef __PCL_Diagnostics_h
#include <pcl/Diagnostics.h>
#endif

namespace pcl
{

// ----------------------------------------------------------------------------

/*!
 * \class StandardDeleter
 * \brief An object deleter that uses the standard delete operator
 *
 * Deleter objects are used by smart pointer classes (e.g., AutoPointer) to
 * destroy and deallocate dynamically allocated objects. A valid deleter class
 * must implement the following member functions:
 *
 * \li Default constructor.
 *
 * \li Copy constructor.
 *
 * \li Function call operator with a single argument of type T* (pointer to T).
 * This member function will destroy and deallocate the T object pointed to by
 * its argument.
 *
 * \li Function call operator with a single argument of type T[] (array of T).
 * This member function will destroy and deallocate a sequence of objects
 * stored at the location pointed to by its argument.
 *
 * %StandardDeleter implements object and array destruction/deallocation by
 * calling the standard \c delete operator.
 *
 * \sa AutoPointer
 */
template <typename T>
class PCL_CLASS StandardDeleter
{
public:

   /*!
    * Represents the type of objects to destroy and deallocate.
    */
   typedef T         value_type;

   /*!
    * Represents a pointer to an object to destroy and deallocate.
    */

   typedef T*        pointer;

   /*!
    * Default constructor.
    */
   StandardDeleter()
   {
   }

   /*!
    * Copy constructor.
    */
   StandardDeleter( const StandardDeleter& )
   {
   }

   /*!
    * Function call operator. Destroys and deallocates the object pointed to by
    * the specified pointer \a p.
    */
   void operator()( pointer p ) const
   {
      PCL_PRECONDITION( p != 0 )
      delete p;
   }
};

// ----------------------------------------------------------------------------

/*!
 * \class AutoPointer
 * \brief A smart pointer with sole object ownership and automatic object destruction
 *
 * %AutoPointer stores a pointer to an object of which it is the sole owner.
 * The owned object can optionally be destroyed when the owner %AutoPointer
 * instance is destroyed.
 *
 * The template argument T represents the type of the objects owned by this
 * template instantiation. The template argument D represents a class
 * responsible for deletion of objects of type T. By default, %AutoPointer uses
 * the StandardDeleter template class, which is a simple wrapper for the
 * standard delete operator.
 *
 * Smart pointers are useful entities to guarantee proper destruction and
 * deallocation of data in a variety of scenarios, such as exception driven
 * code where the same objects have to be destroyed at different locations in
 * the execution workflow. For example, consider the following pseudocode:
 *
 * \code
 * struct Foo { ... };
 * Foo* one = 0, * two = 0;
 * try
 * {
 *    // ... some code that may create new Foo objects ...
 *    if ( condition1 ) one = new Foo;
 *    if ( condition2 ) two = new Foo;
 *    // ... some code that can throw exceptions ...
 *    if ( one != 0 ) delete one;
 *    if ( two != 0 ) delete two;
 * }
 * catch ( ... )
 * {
 *    if ( one != 0 ) delete one;
 *    if ( two != 0 ) delete two;
 *    // ... code to handle exceptions
 * }
 * \endcode
 *
 * Note that the objects pointed to by the \c one and \c two variables have to
 * be destroyed at two locations: at the bottom of the \c try block (normal
 * execution), and when an exception is caught, within the \c catch block. If
 * the routine were more complex, even more deallocations might be necessary at
 * different locations, making the code intricate and prone to memory leaks.
 *
 * All of these complexities and potential problems can be avoided easily with
 * smart pointers. For example, the following snippet would be equivalent to
 * the pseudocode above:
 *
 * \code
 * struct Foo { ... };
 * AutoPointer<Foo> one, two;
 * try
 * {
 *    // ... some code that may create new Foo objects ...
 *    if ( condition1 ) one = new Foo;
 *    if ( condition2 ) two = new Foo;
 *    // ... some code that can throw exceptions ...
 * }
 * catch ( ... )
 * {
 *    // ... code to handle exceptions
 * }
 * \endcode
 *
 * With smart pointers, there's no need to explicitly destroy the dynamically
 * allocated objects \c one and \c two: The %AutoPointer objects will destroy
 * and deallocate them automatically when they get out of scope. On the other
 * hand, the %AutoPointer instances behave just like normal pointers allowing
 * indirections, pointer assignments and structure member selections for their
 * owned objects transparently. The resulting code is simpler and more robust.
 *
 * By default, when an %AutoPointer instance is destroyed it also destroys the
 * object pointed to by its contained pointer (if the %AutoPointer stores a
 * non-null pointer). This <em>automatic deletion</em> feature can be disabled
 * in some situations where a single %AutoPointer can store either a pointer to
 * a dynamically allocated object, or a pointer to an already existing object
 * that must be preserved (e.g., an object living in the stack). For example:
 *
 * \code
 * void Foo( Image& image )
 * {
 *    //
 *    // Extract the CIE L* component of a color image ...
 *    // ... or use the same image as the working lightness if it is grayscale.
 *    //
 *    AutoPointer<Image> lightness;
 *    if ( image.IsColor() )
 *    {
 *       lightness = new Image;
 *       image.ExtractLightness( *lightness );
 *    }
 *    else
 *    {
 *       lightness = &image;
 *       lightness.DisableAutoDeletion();
 *    }
 *
 *    DoSomeStuffWithTheLightnessComponent( *lightness );
 *
 *    //
 *    // Insert the modified CIE L* component back in the color image.
 *    //
 *    if ( image.IsColor() )
 *       image.SetLightness( *lightness );
 * }
 * \endcode
 *
 * In the above code, the \c lightness variable can either store a newed image,
 * if the passed \a image is a color image, or a pointer to \a image if it is a
 * grayscale image. In the latter case we have disabled the automatic deletion
 * feature for the \c lightness %AutoPointer, so it won't delete its stored
 * pointer when it gets out of scope.
 *
 * \sa StandardDeleter
 */
template <class T, class D = StandardDeleter<T> >
class PCL_CLASS AutoPointer
{
public:

   /*!
    * Represents the type of the object pointed to by this smart pointer.
    */
   typedef T         value_type;

   /*!
    * Represents a pointer stored in this smart pointer.
    */
   typedef T*        pointer;

   /*!
    * Represents a pointer to an immutable object stored in this smart pointer.
    */
   typedef const T*  const_pointer;

   /*!
    * Represents the type of the object responsible for object deletion.
    */
   typedef D         deleter;

   /*!
    * Constructs a null smart pointer.
    *
    * \param autoDelete    Initial state of the automatic deletion feature. The
    *                      default value is true, so auto deletion is always
    *                      enabled by default for newly created %AutoPointer
    *                      objects.
    *
    * \param d             Deleter object, responsible for object destruction
    *                      when the automatic deletion feature is enabled.
    *
    * A null smart pointer stores a null pointer, so it does not point to a
    * valid object.
    *
    * A copy of the specified deleter \a d will be used. If no deleter is
    * specified, this object will use a default-constructed instance of the
    * deleter template argument class.
    */
   AutoPointer( bool autoDelete = true, const deleter& d = deleter() ) :
   m_pointer( 0 ), m_deleter( d ), m_autoDelete( true )
   {
   }

   /*!
    * Constructs a smart pointer to store a given pointer.
    *
    * \param p             The pointer to store in this %AutoPointer instance.
    *
    * \param autoDelete    Initial state of the automatic deletion feature. The
    *                      default value is true, so auto deletion is always
    *                      enabled by default for newly created %AutoPointer
    *                      objects.
    *
    * \param d             Deleter object, responsible for object destruction
    *                      when the automatic deletion feature is enabled.
    *
    * A copy of the specified deleter \a d will be used. If no deleter is
    * specified, this object will use a default-constructed instance of the
    * deleter template argument class.
    */
   explicit
   AutoPointer( pointer p, bool autoDelete = true, const deleter& d = deleter() ) :
   m_pointer( p ), m_deleter( d ), m_autoDelete( autoDelete )
   {
   }

   /*!
    * Copy constructor. Constructs a smart pointer by transferring the pointer
    * stored in another smart pointer \a x.
    *
    * The automatic deletion feature for this object will be in the same state
    * as it is currently set for the source object \a x.
    *
    * The smart pointer \a x will be a null pointer after this instance is
    * constructed. This happens irrespective of the state of the automatic
    * deletion feature. This guarantees that, as long as no two %AutoPointer
    * instances have been \e explicitly constructed to store the same pointer,
    * no two %AutoPointer instances can share the same pointer accidentally,
    * and hence multiple deletions are not possible.
    */
   AutoPointer( const AutoPointer& x ) :
   m_pointer( x.m_pointer ), m_deleter( x.m_deleter ), m_autoDelete( x.m_autoDelete )
   {
      const_cast<AutoPointer&>( x ).m_pointer = 0;
   }

   /*!
    * Destructor.
    *
    * If this instance stores a non-null pointer, and the automatic deletion
    * feature is enabled, the pointed object will be destroyed by calling the
    * deleter object.
    */
   ~AutoPointer()
   {
      Reset();
   }

   /*!
    * Causes this smart pointer to store the specified pointer \a p.
    *
    * If this instance stores a non-null pointer when this function is called,
    * and the automatic deletion feature is enabled, the pointed object is
    * destroyed by calling the deleter object.
    */
   void SetPointer( pointer p )
   {
      if ( p != m_pointer )
      {
         if ( m_autoDelete )
            if ( m_pointer != 0 )
               m_deleter( m_pointer );
         m_pointer = p;
      }
   }

   /*!
    * Causes this smart pointer to store a null pointer.
    *
    * If this instance stores a non-null pointer when this function is called,
    * and the automatic deletion feature is enabled, the pointed object is
    * destroyed by calling the deleter object.
    *
    * This member function is a convenience synonym for SetPointer( 0 ).
    */
   void Reset()
   {
      SetPointer( 0 );
   }

   /*!
    * Returns the pointer stored in this %AutoPointer, and causes this object
    * to \e forget it by storing a null pointer.
    *
    * The object pointed is never destroyed by this function, irrespective of
    * the state of automatic deletion. In this way, ownership of the pointed
    * object (that is, the responsibility for destroying it) is transferred to
    * the caller.
    */
   pointer Release()
   {
      pointer p = m_pointer;
      m_pointer = 0;
      return p;
   }

   /*!
    * Returns a pointer to the immutable object pointed to by this %AutoPointer
    * instance.
    */
   const_pointer Pointer() const
   {
      return m_pointer;
   }

   /*!
    * Returns a copy of the pointer stored in this %AutoPointer instance.
    */
   pointer Pointer()
   {
      return m_pointer;
   }

   /*!
    * Returns true if this instance stores a null pointer.
    */
   bool IsNull() const
   {
      return m_pointer == 0;
   }

   /*!
    * Returns true if the automatic deletion feature of %AutoPointer is
    * currently enabled for this object.
    *
    * When automatic deletion is enabled, the object pointed to by this
    * instance will be destroyed (by calling the deleter object) when this
    * instance is destroyed, or when it is assigned with a different pointer.
    *
    * When automatic deletion is disabled, the pointed object will not be
    * destroyed automatically.
    *
    * See the detailed description for the %AutoPointer class for more
    * information, including code examples.
    *
    * \sa EnableAutoDeletion(), DisableAutoDeletion()
    */
   bool IsAutoDeletion() const
   {
      return m_autoDelete;
   }

   /*!
    * Enables (or disables) the automatic deletion feature of %AutoPointer for
    * this object.
    *
    * \sa IsAutoDeletion(), DisableAutoDeletion()
    */
   void EnableAutoDeletion( bool enable = true )
   {
      m_autoDelete = enable;
   }

   /*!
    * Disables (or enables) the automatic deletion feature of %AutoPointer for
    * this object.
    *
    * \sa IsAutoDeletion(), EnableAutoDeletion()
    */
   void DisableAutoDeletion( bool disable = true )
   {
      EnableAutoDeletion( !disable );
   }

   /*!
    * Returns a reference to the immutable deleter object in this instance.
    */
   const deleter& Deleter() const
   {
      return m_deleter;
   }

   /*!
    * Returns a reference to the deleter object in this instance.
    */
   deleter& Deleter()
   {
      return m_deleter;
   }

   /*!
    * Transfers the pointer stored in another smart pointer to this object.
    *
    * This assignment operator performs the following actions:
    *
    * (1) If this smart pointer stores a valid (non-null) pointer, and the
    * automatic deletion feature is enabled, the pointed object is destroyed by
    * the deleter object.
    *
    * (2) The pointer stored in the other smart pointer \a x is copied to this
    * instance.
    *
    * (3) The other smart pointer \a x is forced to store a null pointer.
    *
    * (4) Returns a reference to this object.
    *
    * This operator function does nothing if the specified %AutoPointer \a x
    * stores the same pointer as this object. This prevents multiple deletions.
    */
   AutoPointer& operator =( const AutoPointer& x )
   {
      if ( x.m_pointer != m_pointer )
      {
         Reset();
         m_pointer = x.m_pointer;
         const_cast<AutoPointer&>( x ).m_pointer = 0;
      }
      return *this;
   }

   /*!
    * Causes this smart pointer to store the specified pointer \a p. Returns a
    * reference to this object.
    *
    * If this instance stores a non-null pointer when this function is called,
    * and the automatic deletion feature is enabled, the pointed object is
    * destroyed by the deleter object.
    *
    * This member function is equivalent to:
    *
    * \code
    * SetPointer( p );
    * \endcode
    */
   AutoPointer& operator =( pointer p )
   {
      SetPointer( p );
      return *this;
   }

   /*!
    * Returns a pointer to the immutable object pointed to by this instance.
    *
    * This operator is a synonym for the Pointer() const member function.
    */
   operator const_pointer() const
   {
      return Pointer();
   }

   /*!
    * Returns a copy of the pointer stored in this %AutoPointer instance.
    *
    * This operator is a synonym for the Pointer() member function.
    */
   operator pointer()
   {
      return Pointer();
   }

   /*!
    * Structure member selection operator. Returns a pointer to the immutable
    * object pointed to by this %AutoPointer instance.
    */
   const_pointer operator ->() const
   {
      PCL_PRECONDITION( m_pointer != 0 )
      return Pointer();
   }

   /*!
    * Structure member selection operator. Returns a copy of the pointer stored
    * in this %AutoPointer instance.
    */
   pointer operator ->()
   {
      PCL_PRECONDITION( m_pointer != 0 )
      return Pointer();
   }

   /*!
    * Dereference operator. Returns a reference to the immutable object pointed
    * to by this smart pointer.
    */
   const value_type& operator *() const
   {
      PCL_PRECONDITION( m_pointer != 0 )
      return *Pointer();
   }

   /*!
    * Dereference operator. Returns a reference to the object pointed to by
    * this smart pointer.
    */
   value_type& operator *()
   {
      PCL_PRECONDITION( m_pointer != 0 )
      return *Pointer();
   }

   /*!
    * Returns true if this smart pointer stores a non-null pointer. This
    * operator is equivalent to:
    *
    * \code
    * AutoPointer<T> p;
    * return !p.IsNull();
    * \endcode
    */
   operator bool() const
   {
      return !IsNull();
   }

private:

   pointer m_pointer;
   deleter m_deleter;
   bool    m_autoDelete : 1;
};

// ----------------------------------------------------------------------------

} // pcl

#endif  // __PCL_AutoPointer_h

// ****************************************************************************
// EOF pcl/AutoPointer.h - Released 2014/11/14 17:16:40 UTC
