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
#include <algorithm>
#include <string>
#include <vector>

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

   // This serves essentially the same function as CFStringInlineBuffer, but
   // doesn't have the macro weirdness that CFString.h has going on with it.
   class BufferingStringAccessor
   {
   public:
      BufferingStringAccessor(const CFStringRef& stringRef)
      {
         // Merely copy the reference; we don't increment the reference count. This
         // object has undefined behavior if it has a lifetime exceeding the parent
         // String object.
         m_stringRef = stringRef;
         // The string is ostensibly immutable, so the length should not change.
         m_stringLength = CFStringGetLength(stringRef);

         // This will either return the pointer, or NULL. If non-NULL, no buffering!
         m_directPtr = CFStringGetCharactersPtr(stringRef);
         if (m_directPtr == nullptr)
         {
            // Alas. Initialize some space.
            m_buffer.resize(std::min<size_t>(64, m_stringLength));
         }

         m_bufferBeginPosition = m_bufferEndPosition = 0;
      }

      BufferingStringAccessor(const BufferingStringAccessor& other) :
         m_stringRef(other.m_stringRef),
         m_directPtr(other.m_directPtr),
         m_stringLength(other.m_stringLength),
         m_bufferBeginPosition(0),
         m_bufferEndPosition(0)
      {
         if (m_directPtr == nullptr)
         {
            m_buffer.resize(std::min<size_t>(64, m_stringLength));
         }
      }

      const UniChar& get(CFIndex index) const
      {
         if (m_directPtr)
         {
            return m_directPtr[index];
         }
         else
         {
            if (index < m_bufferBeginPosition || index >= m_bufferEndPosition)
            {
               // We can't do fast-access, and the index we just accessed is outside
               // the buffer.

               if ((m_bufferBeginPosition = index - 4) < 0)
                  m_bufferBeginPosition = 0;

               m_bufferEndPosition = m_bufferBeginPosition + m_buffer.size();

               if (m_bufferEndPosition > m_stringLength)
                  m_bufferEndPosition = m_stringLength;

               CFStringGetCharacters(
                  m_stringRef,
                  CFRangeMake(m_bufferBeginPosition, static_cast<CFIndex>(m_buffer.size())),
                  m_buffer.data());
            }

            return m_buffer[index - m_bufferBeginPosition];
         }
      }

      const UniChar& checkedGet(CFIndex index) const
      {
         if (index < 0 || index >= m_stringLength)
            throw std::out_of_range("BufferingStringAccessor");
         return get(index);
      }

   private:
      mutable std::vector<UniChar> m_buffer;

      CFStringRef m_stringRef;
      const UniChar* m_directPtr;
      CFIndex m_stringLength;

      mutable CFIndex m_bufferBeginPosition;
      mutable CFIndex m_bufferEndPosition;
   };


   class const_iterator :
      public std::iterator<std::random_access_iterator_tag, const UniChar>
   {
   public:
      typedef std::random_access_iterator_tag iterator_category;
      typedef typename std::iterator<std::random_access_iterator_tag, const UniChar>::value_type value_type;
      typedef typename std::iterator<std::random_access_iterator_tag, const UniChar>::difference_type difference_type;
      typedef typename std::iterator<std::random_access_iterator_tag, const UniChar>::reference reference;
      typedef typename std::iterator<std::random_access_iterator_tag, const UniChar>::pointer pointer;
      
      const_iterator(const String* parent, CFIndex position) :
         m_parent(parent),
         m_position(position)
      { }

      const_iterator(const const_iterator& other) :
         m_parent(other.m_parent),
         m_position(other.m_position)
      { }

      const_iterator& operator=(const const_iterator& other)
      {
         if (&other == this) return *this;

         m_parent = other.m_parent;
         m_position = other.m_position;

         return *this;
      }

      const_iterator& operator++()
      {
         ++m_position;
         return *this;
      }

      const_iterator& operator--()
      {
         --m_position;
         return *this;
      }

      const_iterator operator++(int)
      {
         return const_iterator(m_parent, m_position+1);
      }

      const_iterator operator--(int) 
      {
         return const_iterator(m_parent, m_position-1);
      }

      const_iterator operator+(const difference_type& n) const
      {
         return const_iterator(m_parent, m_position + n);
      }

      const_iterator& operator+=(const difference_type& n)
      {
         m_position += n;
         return *this;
      }

      const_iterator operator-(const difference_type& n) const
      {
         return const_iterator(m_parent, m_position - n);
      }
      const_iterator& operator-=(const difference_type& n)
      {
         m_position -= n;
         return *this;
      }

      reference operator*() const
      {
         return (*m_parent)[m_position];
      }

      pointer operator->() const
      {
         return &(*m_parent)[m_position];
      }

      reference operator[](const difference_type& n) const
      {
         return (*m_parent)[m_position + n];
      }

      bool operator==(const const_iterator& other) const
      {
         return m_position == other.m_position;
      }

      bool operator!=(const const_iterator& other) const
      {
         return m_position != other.m_position;
      }

      bool operator<(const const_iterator& other) const
      {
         return m_position < other.m_position;
      }

      bool operator<=(const const_iterator& other) const
      {
         return m_position <= other.m_position;
      }

      bool operator>(const const_iterator& other) const
      {
         return m_position > other.m_position;
      }

      bool operator>=(const const_iterator& other) const
      {
         return m_position >= other.m_position;
      }

   protected:
      const String* m_parent;
      CFIndex m_position;
   };

   typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

   String() :
      Base(make_CFReference(CFStringCreateWithCString(
         kCFAllocatorDefault,
         "",
         kCFStringEncodingUTF8))),
      m_stringAccessor(reinterpret_cast<CFStringRef>(m_ref.get()))
   { }

   String(const char* s, CFStringEncoding encoding = kCFStringEncodingUTF8) :
      Base(make_CFReference(CFStringCreateWithCString(
         kCFAllocatorDefault,
         s,
         encoding))),
      m_stringAccessor(reinterpret_cast<CFStringRef>(m_ref.get()))
   { }

   String(const String& other) :
      // Strings are immutable, so we can share references.
      Base(other.m_ref),
      m_stringAccessor(reinterpret_cast<CFStringRef>(m_ref.get()))
   { }

   String(String&& other) :
      Base(std::move(other.m_ref)),
      m_stringAccessor(reinterpret_cast<CFStringRef>(m_ref.get()))
   { }

   const_iterator begin() const
   {
      return const_iterator(this, 0);
   }

   const_iterator end() const
   {
      return const_iterator(this, size());
   }

   const_reverse_iterator rbegin() const
   {
      return const_reverse_iterator(end());
   }

   const_reverse_iterator rend() const
   {
      return const_reverse_iterator(begin());
   }

   const_reference operator[](size_type index) const
   {
      return m_stringAccessor.get(index);
   }

   const_reference at(size_type index) const
   {
      return m_stringAccessor.checkedGet(index);
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

protected:
   String(const CFReference<CFStringRef>& ref) :
      Base(ref),
      m_stringAccessor(ref.get())
   { }

private:
   inline CFStringRef getRef() const
   {
      return reinterpret_cast<CFStringRef>(m_ref.get());
   }

   BufferingStringAccessor m_stringAccessor;
   
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
   MutableString() :
      String(make_CFReference(CFStringCreateMutable(
         kCFAllocatorDefault, 0)))
   { }

   MutableString(const String& other) :
      String(make_CFReference(CFStringCreateMutableCopy(
         kCFAllocatorDefault, 0, other)))
   { }

   MutableString(MutableString&& other) :
      String(std::move(other.m_ref))
   { }

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
      return reinterpret_cast<CFMutableStringRef>(const_cast<void *>(m_ref.get()));
   }
};

} // namespace CoreFoundation

#endif // __cfxx_string_h__
