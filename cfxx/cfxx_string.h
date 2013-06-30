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

class String;
class MutableString;

template<>
struct IsParentCFType<CFTypeRef, CFStringRef> : public std::integral_constant<bool, true> {};

template<>
struct IsParentCFType<CFStringRef, CFMutableStringRef> : public std::integral_constant<bool, true> {};

template<>
struct ParentCFType<CFStringRef>
{
   typedef CFTypeRef type;
};

template<>
struct ParentCFType<CFMutableStringRef>
{
   typedef CFStringRef type;
};

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
      inline BufferingStringAccessor(const CFStringRef& stringRef)
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

      inline BufferingStringAccessor(const BufferingStringAccessor& other) :
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

      inline const UniChar& get(CFIndex index) const noexcept
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

      inline const UniChar& checkedGet(CFIndex index) const
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
      
      inline const_iterator(const String* parent, CFIndex position) noexcept :
         m_parent(parent),
         m_position(position)
      { }

      inline const_iterator(const const_iterator& other) noexcept :
         m_parent(other.m_parent),
         m_position(other.m_position)
      { }

      inline const_iterator& operator=(const const_iterator& other) noexcept
      {
         if (&other == this) return *this;

         m_parent = other.m_parent;
         m_position = other.m_position;

         return *this;
      }

      inline const_iterator& operator++() noexcept
      {
         ++m_position;
         return *this;
      }

      inline const_iterator& operator--() noexcept
      {
         --m_position;
         return *this;
      }

      inline const_iterator operator++(int) noexcept
      {
         return const_iterator(m_parent, m_position+1);
      }

      inline const_iterator operator--(int) noexcept
      {
         return const_iterator(m_parent, m_position-1);
      }

      inline const_iterator operator+(const difference_type& n) const noexcept
      {
         return const_iterator(m_parent, m_position + n);
      }

      inline const_iterator& operator+=(const difference_type& n) noexcept
      {
         m_position += n;
         return *this;
      }

      inline const_iterator operator-(const difference_type& n) const noexcept
      {
         return const_iterator(m_parent, m_position - n);
      }

      inline const_iterator& operator-=(const difference_type& n) noexcept
      {
         m_position -= n;
         return *this;
      }

      inline reference operator*() const noexcept
      {
         return (*m_parent)[m_position];
      }

      inline pointer operator->() const noexcept
      {
         return &(*m_parent)[m_position];
      }

      inline reference operator[](const difference_type& n) const noexcept
      {
         return (*m_parent)[m_position + n];
      }

      inline bool operator==(const const_iterator& other) const noexcept
      {
         return m_position == other.m_position;
      }

      inline bool operator!=(const const_iterator& other) const noexcept
      {
         return m_position != other.m_position;
      }

      inline bool operator<(const const_iterator& other) const noexcept
      {
         return m_position < other.m_position;
      }

      inline bool operator<=(const const_iterator& other) const noexcept
      {
         return m_position <= other.m_position;
      }

      inline bool operator>(const const_iterator& other) const noexcept
      {
         return m_position > other.m_position;
      }

      inline bool operator>=(const const_iterator& other) const noexcept
      {
         return m_position >= other.m_position;
      }

   protected:
      const String* m_parent;
      CFIndex m_position;
   };

   typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

   inline String() :
      Base(makeCFReferenceFromCopyOrCreate(
         CFStringCreateWithCString(
            kCFAllocatorDefault,
            "",
            kCFStringEncodingUTF8))),
      m_stringAccessor(reinterpret_cast<CFStringRef>(m_ref.get()))
   { }

   inline String(const char* s, CFStringEncoding encoding = kCFStringEncodingUTF8) :
      Base(makeCFReferenceFromCopyOrCreate(
         CFStringCreateWithCString(
            kCFAllocatorDefault,
            s,
            encoding))),
      m_stringAccessor(reinterpret_cast<CFStringRef>(m_ref.get()))
   { }

   inline String(const String& other) :
      // Strings are immutable, so we can share references.
      Base(other.m_ref),
      m_stringAccessor(reinterpret_cast<CFStringRef>(m_ref.get()))
   { }

   inline String(String&& other) :
      Base(std::move(other.m_ref)),
      m_stringAccessor(reinterpret_cast<CFStringRef>(m_ref.get()))
   { }

   inline const_iterator begin() const noexcept
   {
      return const_iterator(this, 0);
   }

   inline const_iterator end() const noexcept
   {
      return const_iterator(this, size());
   }

   inline const_reverse_iterator rbegin() const noexcept
   {
      return const_reverse_iterator(end());
   }

   inline const_reverse_iterator rend() const noexcept
   {
      return const_reverse_iterator(begin());
   }

   inline const_reference operator[](size_type index) const noexcept
   {
      return m_stringAccessor.get(index);
   }

   inline const_reference at(size_type index) const
   {
      return m_stringAccessor.checkedGet(index);
   }

   inline size_type size() const noexcept
   {
      return CFStringGetLength(getRef());
   }

   inline size_type length() const noexcept
   {
      return CFStringGetLength(getRef());
   }

   inline bool empty() const noexcept
   {
      return CFStringGetLength(getRef()) == 0;
   }

   inline std::string to_string(CFStringEncoding encoding = kCFStringEncodingUTF8) const
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

         // Get the string length.
         CFRange range = CFRangeMake(0, CFStringGetLength(getRef()));

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
         // Make large enough for that.
         buffer.resize(requiredBufferSize);

         // And then, fill the std::string. We use CFStringGetBytes and not CFStringGetCString, because
         // we don't want the null terminator (std::string handles that).
         CFStringGetBytes(
            getRef(),
            range,
            encoding,
            0,
            false,
            reinterpret_cast<UInt8*>(const_cast<char*>(buffer.data())),
            static_cast<CFIndex>(buffer.size()),
            nullptr);

         return buffer;
      }
   }

   inline operator CFStringRef() const noexcept
   {
      return getRef();
   }

   inline static CFTypeID class_type_id() noexcept
   {
      return CFStringGetTypeID();
   }

protected:
   inline String(const CFReference<CFStringRef>& ref) :
      Base(ref),
      m_stringAccessor(ref.get())
   { }

private:
   inline CFStringRef getRef() const noexcept
   {
      return reinterpret_cast<CFStringRef>(m_ref.get());
   }

   BufferingStringAccessor m_stringAccessor;
   
};


inline bool operator<(const String& a, const String& b) noexcept
{
   return CFStringCompare(a, b, 0) == kCFCompareLessThan;
}

inline bool operator>(const String& a, const String& b) noexcept
{
   return CFStringCompare(a, b, 0) == kCFCompareGreaterThan;
}

inline bool operator==(const String& a, const String& b) noexcept
{
   return CFStringCompare(a, b, 0) == kCFCompareEqualTo;
}

inline bool operator<=(const String& a, const String& b) noexcept
{
   const auto r = CFStringCompare(a, b, 0);
   return (r == kCFCompareLessThan || r == kCFCompareEqualTo);
}

inline bool operator>=(const String& a, const String& b) noexcept
{
   const auto r = CFStringCompare(a, b, 0);
   return (r == kCFCompareGreaterThan || r == kCFCompareEqualTo);
}

inline bool operator!=(const String& a, const String& b) noexcept
{
   return CFStringCompare(a, b, 0) != kCFCompareEqualTo;
}

class MutableString : public String
{
public:
   inline MutableString() :
      String(makeCFReferenceFromCopyOrCreate(
         CFStringCreateMutable(
            kCFAllocatorDefault, 0)))
   { }

   // This one is kind of annoying, there isn't a CFStringCreateMutableWithCString,
   // so we have to compose it ourselves.
   inline MutableString(const char* s, CFStringEncoding encoding = kCFStringEncodingUTF8) :
      String(
         makeCFReferenceFromCopyOrCreate(
            CFStringCreateMutableCopy(
               kCFAllocatorDefault,
               0,
               makeCFReferenceFromCopyOrCreate(
                  CFStringCreateWithCString(
                     kCFAllocatorDefault,
                     s,
                     encoding)).get())))
   { }

   inline MutableString(const String& other) noexcept :
      String(makeCFReferenceFromCopyOrCreate(
         CFStringCreateMutableCopy(
            kCFAllocatorDefault, 0, other)))
   { }

   inline MutableString(MutableString&& other) noexcept :
      String(std::move(other.m_ref))
   { }

   inline MutableString& append(const String& str) noexcept
   {
      CFStringAppend(getRef(), str);
      return *this;
   }

   inline MutableString& append(const UniChar* str, size_t n) noexcept
   {
      CFStringAppendCharacters(getRef(), str, n);
      return *this;
   }

   inline MutableString& append(const char* str, CFStringEncoding encoding = kCFStringEncodingUTF8) noexcept
   {
      CFStringAppendCString(getRef(), str, encoding);
      return *this;
   }
   
   inline operator CFMutableStringRef() const noexcept
   {
      return getRef();
   }

private:
   inline CFMutableStringRef getRef() const noexcept
   {
      return reinterpret_cast<CFMutableStringRef>(const_cast<void *>(m_ref.get()));
   }
};

} // namespace CoreFoundation

#endif // __cfxx_string_h__
