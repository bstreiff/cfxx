// Copyright (c) 2013, Brenda Streiff
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met: 
// 
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer. 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
// 3. Don't be a dick.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef __cfxx_base_h__
#define __cfxx_base_h__
#pragma once

#include <CoreFoundation/CoreFoundation.h>

namespace CoreFoundation
{

// CFReference handles RAII semantics for retain/releasing CoreFoundation references.
//
// It's modeled somewhat after std::shared_ptr.
template <typename T>
class CFReference
{
public:
   // Default reference points to nothing.
   CFReference()
      : m_ref(nullptr)
   {
   }

   // Create a CFReference pointing to an existing CoreFoundation object.
   CFReference(T raw)
      : m_ref(raw)
   {
      CFRetain(m_ref);
   }

   // Copy an existing CFReference.
   CFReference(const CFReference& other)
      : m_ref(other.get())
   {
      // Both ref and other.ref have references to the object, so we need
      // to bump the retain count.
      CFRetain(m_ref);
   }

   // Copy an existing CFReference.
   template<typename Y>
   CFReference(const CFReference<Y>& other)
      // TODO: This doesn't do any type-checking to make sure that this cast is valid
      // (e.g., we could be casting a CFReference<CFDataRef> to a CFReference<CFStringRef>)
      : m_ref(reinterpret_cast<T>(other.get()))
   {
      CFRetain(m_ref);
   }

   // Move from an existing CFReference.
   CFReference(CFReference&& other)
      : m_ref(other.get())
   {
      // 'other' now no longer holds a reference to the object.
      // We don't need to CFRetain() because the reference count isn't changing.
      other.m_ref = nullptr;
   }

   // Move from an existing CFReference.
   template<typename Y>
   CFReference(CFReference<T>&& other)
      : m_ref(other.get())
   {
      other.m_ref = nullptr;
   }

   // Destroy this CFReference, thus dropping the refcount.
   ~CFReference()
   {
      release();
   }

   CFReference& operator=(const CFReference& other)
   {
      if (&other != this)
      {
         release();
         m_ref = other.get();
         CFRetain(m_ref);
      }
      return *this;
   }

   template<typename Y>
   CFReference& operator=(const CFReference<Y>& other)
   {
      if (&other != this)
      {
         release();
         m_ref = other.get();
         CFRetain(m_ref);
      }
      return *this;
   }

   CFReference& operator=(CFReference&& other)
   {
      release();
      m_ref = other.get();
      CFRetain(m_ref);
      other.release();
      return *this;
   }

   template<typename Y>
   CFReference& operator=(CFReference<Y>&& other)
   {
      release();
      m_ref = other.get();
      CFRetain(m_ref);
      other.release();
      return *this;
   }

   // Get the retain count. Note: @VTPG says this can lie. Don't depend on this for anything important.
   long use_count() const
   {
      if (m_ref)
         return CFGetRetainCount(m_ref);
      else
         return 0;
   }

   explicit operator bool() const
   {
      return (m_ref != nullptr);
   }

   T get() const
   {
      return m_ref;
   }

   void release()
   {
      if (m_ref)
      {
         CFRelease(m_ref);
         m_ref = nullptr;
      }
   }

private:
   T m_ref;
};

// Helper to make a CFReference<T> from a newly-created object. It is assumed that 'arg' is the
// result of a function creating a new reference (like CFStringCreateCopy), and is therefore a
// temporary value that needs to be released.
template<typename T>
CFReference<T> make_CFReference(T arg)
{
   CFReference<T> x(arg);
   CFRelease(arg);
   return x;
}

// Base is the base class; it just owns the CFReference, so that all the derived
// classes don't need to duplicate it.
class Base
{
   operator CFTypeRef() const
   {
      return m_ref.get();
   }

protected:
   Base() :
      m_ref()
   { }

   template<typename T>
   Base(const CFReference<T>& ref) :
      m_ref(ref)
   { }

   CFReference<CFTypeRef> m_ref;
};

} // namespace CoreFoundation

#endif // __cfxx_base_h__
