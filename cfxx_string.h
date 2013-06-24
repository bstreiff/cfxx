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

#ifndef __cfxx_string_h__
#define __cfxx_string_h__
#pragma once

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFString.h>
#include <string>

namespace CoreFoundation
{

class String : public Base
{
public:
   enum { npos = -1 };

   typedef UniChar value_type;
   typedef UniChar& reference;
   typedef const UniChar& const_reference;
   typedef UniChar* pointer;
   typedef const UniChar* const_pointer;
   typedef CFIndex size_type;

   String()
   {
      // I suppose this should be a CFSTR or something?
      _ref = make_CFReference(CFStringCreateWithCString(
         kCFAllocatorDefault,
         "",
         kCFStringEncodingUTF8));
   }

   String(const char* s, CFStringEncoding encoding = kCFStringEncodingUTF8)
   {
      _ref = make_CFReference(CFStringCreateWithCString(
         kCFAllocatorDefault,
         s,
         encoding));
   }

   String(const String& other)
   {
      // Strings are immutable, so we can share references.
      _ref = other._ref;
   }

   String(String&& other)
   {
      _ref = std::move(other._ref);
   }

   // TODO: support begin()/end()...
   // The 'easy' way would be to just have pointers into CFStringGetCharactersPtr but that's
   // not guaranteed to work... I think the 'real' way is going to be to define an iterator
   // class that uses CFStringInitInlineBuffer/CFStringGetCharacterFromInlineBuffer.

   UniChar operator[](size_type index) const
   {
      return CFStringGetCharacterAtIndex(getRef(), index);
   }

   size_type size() const
   {
      return CFStringGetLength(getRef());
   }

   size_type length() const
   {
      return CFStringGetLength(getRef());
   }

   bool empty() const
   {
      return CFStringGetLength(getRef()) == 0;
   }

   std::string to_string(CFStringEncoding encoding = kCFStringEncodingUTF8) const
   {
      // First, try the fast approach.
      const char* strPtr = CFStringGetCStringPtr(getRef(), encoding);
      if (strPtr)
      {
         return std::string(strPtr);
      }
      else
      {
         // Well, that didn't work, so we have to do it the hard way.
         std::string buffer;

         // We need to figure out how long a buffer we need. So first, we need a range representing the whole string.
         CFRange range;
         range.location = 0;
         range.length = CFStringGetLength(getRef());

         // Now, use GetBytes to find the buffer length.
         CFIndex requiredBufferSize = 0;
         CFStringGetBytes(
            getRef(),
            range,
            encoding,
            0,
            false,
            nullptr,
            0,
            &requiredBufferSize);
         // Make large enough for that, plus terminating NULL.
         buffer.resize(requiredBufferSize + 1);

         // And then, fill the std::string.
         CFStringGetCString(getRef(), const_cast<char*>(buffer.data()), static_cast<CFIndex>(buffer.size()), encoding);

         return buffer;
      }
   }

   operator CFStringRef() const
   {
      return getRef();
   }

private:
   inline CFStringRef getRef() const
   {
      return reinterpret_cast<CFStringRef>(_ref.get());
   }
   
};


bool operator<(const String& a, const String& b)
{
   return CFStringCompare(a, b, 0) == kCFCompareLessThan;
}

bool operator>(const String& a, const String& b)
{
   return CFStringCompare(a, b, 0) == kCFCompareGreaterThan;
}

bool operator==(const String& a, const String& b)
{
   return CFStringCompare(a, b, 0) == kCFCompareEqualTo;
}

bool operator<=(const String& a, const String& b)
{
   const auto r = CFStringCompare(a, b, 0);
   return (r == kCFCompareLessThan || r == kCFCompareEqualTo);
}

bool operator>=(const String& a, const String& b)
{
   const auto r = CFStringCompare(a, b, 0);
   return (r == kCFCompareGreaterThan || r == kCFCompareEqualTo);
}

bool operator!=(const String& a, const String& b)
{
   return CFStringCompare(a, b, 0) != kCFCompareEqualTo;
}

class MutableString : public String
{
public:
   MutableString()
   {
      _ref = make_CFReference(CFStringCreateMutable(
         kCFAllocatorDefault, 0));
   }

   MutableString(const String& other)
   {
      _ref = make_CFReference(CFStringCreateMutableCopy(
         kCFAllocatorDefault, 0, other));
   }

   MutableString(MutableString&& other)
   {
      _ref = std::move(other._ref);
   }

   MutableString& append(const String& str)
   {
      CFStringAppend(getRef(), str);
      return *this;
   }

   MutableString& append(const UniChar* str, size_t n)
   {
      CFStringAppendCharacters(getRef(), str, n);
      return *this;
   }

   MutableString& append(const char* str, CFStringEncoding encoding = kCFStringEncodingUTF8)
   {
      CFStringAppendCString(getRef(), str, encoding);
      return *this;
   }
   
   operator CFMutableStringRef() const
   {
      return getRef();
   }

private:
   inline CFMutableStringRef getRef() const
   {
      return reinterpret_cast<CFMutableStringRef>(const_cast<void *>(_ref.get()));
   }
};

} // namespace CoreFoundation

#endif // __cfxx_string_h__
