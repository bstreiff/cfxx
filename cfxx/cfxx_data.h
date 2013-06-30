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

#ifndef __cfxx_data_h___
#define __cfxx_data_h___
#pragma once

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFData.h>
#include <algorithm>
#include <string>
#include <vector>

namespace CoreFoundation
{

class Data;
class MutableData;

template<>
struct IsParentCFType<CFTypeRef, CFDataRef> : public std::integral_constant<bool, true> {};

template<>
struct IsParentCFType<CFDataRef, CFMutableDataRef> : public std::integral_constant<bool, true> {};

template<>
struct ParentCFType<CFDataRef>
{
   typedef CFTypeRef type;
};

template<>
struct ParentCFType<CFMutableDataRef>
{
   typedef CFDataRef type;
};

class Data : public Base
{
public:
   typedef UInt8                                 value_type;
   typedef UInt8&                                reference;
   typedef const UInt8&                          const_reference;
   typedef UInt8*                                pointer;
   typedef const UInt8*                          const_pointer;
   typedef CFIndex                               size_type;
   typedef const_pointer                         const_iterator;
   typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

   inline Data() noexcept :
      Base()
   { }

   inline Data(const UInt8* bytes, size_type length) noexcept :
      Base(makeCFReferenceFromCopyOrCreate(
         CFDataCreate(
            kCFAllocatorDefault,
            reinterpret_cast<const UInt8*>(bytes),
            length)))
   { }

   inline Data(const Data& other) noexcept :
      // CFDatas are immutable, so we can share references.
      Base(other.m_ref)
   { }

   inline Data(Data&& other) noexcept :
      Base(std::move(other.m_ref))
   { }

   inline const_iterator begin() const noexcept
   {
      return CFDataGetBytePtr(getRef());
   }

   inline const_iterator end() const noexcept
   {
      return CFDataGetBytePtr(getRef()) + CFDataGetLength(getRef());
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
      return *(CFDataGetBytePtr(getRef()) + index);
   }

   inline const_reference at(size_type index) const
   {
      if (index < 0 || index >= size())
         throw std::out_of_range("Data");
      return operator[](index);
   }

   inline const_pointer data() const noexcept
   {
      return CFDataGetBytePtr(getRef());
   }

   inline size_type size() const noexcept
   {
      return CFDataGetLength(getRef());
   }

   inline size_type length() const noexcept
   {
      return CFDataGetLength(getRef());
   }

   inline bool empty() const noexcept
   {
      return CFDataGetLength(getRef()) == 0;
   }

   inline operator CFDataRef() const noexcept
   {
      return getRef();
   }

   inline static CFTypeID class_type_id()
   {
      return CFDataGetTypeID();
   }

protected:
   inline Data(const CFReference<CFDataRef>& ref) :
      Base(ref)
   { }

private:
   inline CFDataRef getRef() const
   {
      return reinterpret_cast<CFDataRef>(m_ref.get());
   }
};

class MutableData : public Data
{
public:
   typedef pointer                         iterator;
   typedef std::reverse_iterator<iterator> reverse_iterator;

   inline MutableData() noexcept :
      Data()
   { }

   inline MutableData(const Data& other) noexcept :
      Data(makeCFReferenceFromCopyOrCreate(
         CFDataCreateMutableCopy(
            kCFAllocatorDefault, 0, other)))
   { }

   inline MutableData(MutableData&& other) noexcept :
      Data(std::move(other.m_ref))
   { }

   inline MutableData& append(const UInt8* bytes, size_t n) noexcept
   {
      CFDataAppendBytes(getRef(), bytes, n);
      return *this;
   }

   inline MutableData& append(const Data& other) noexcept
   {
      return append(other.data(), other.size());
   }

   inline void erase(iterator pos) noexcept
   {
      CFDataDeleteBytes(getRef(),
         CFRangeMake(
            /* start  */ static_cast<CFIndex>(pos - begin()),
            /* length */ 1));
   }

   inline void erase(const_iterator pos) noexcept
   {
      CFDataDeleteBytes(getRef(),
         CFRangeMake(
            /* start  */ static_cast<CFIndex>(pos - begin()),
            /* length */ 1));
   }

   inline void erase(iterator first, iterator last) noexcept
   {
      CFDataDeleteBytes(getRef(),
         CFRangeMake(
            /* start  */ static_cast<CFIndex>(first - begin()),
            /* length */ static_cast<CFIndex>(last - first)));
   }

   inline void erase(const_iterator first, const_iterator last) noexcept
   {
      CFDataDeleteBytes(getRef(),
         CFRangeMake(
            /* start  */ static_cast<CFIndex>(first - begin()),
            /* length */ static_cast<CFIndex>(last - first)));
   }

   inline iterator begin() noexcept
   {
      return CFDataGetMutableBytePtr(getRef());
   }

   inline iterator end() noexcept
   {
      return CFDataGetMutableBytePtr(getRef()) + CFDataGetLength(getRef());
   }

   inline reverse_iterator rbegin() noexcept
   {
      return reverse_iterator(end());
   }

   inline reverse_iterator rend() noexcept
   {
      return reverse_iterator(begin());
   }

   inline reference operator[](size_type index) noexcept
   {
      return *(CFDataGetMutableBytePtr(getRef()) + index);
   }

   inline reference at(size_type index)
   {
      if (index < 0 || index >= size())
         throw std::out_of_range("MutableData");
      return operator[](index);
   }

   inline pointer data() noexcept
   {
      return CFDataGetMutableBytePtr(getRef());
   }

   inline void resize(size_type n) noexcept
   {
      CFDataSetLength(getRef(), n);
   }
   
   inline operator CFMutableDataRef() const noexcept
   {
      return getRef();
   }

private:
   inline CFMutableDataRef getRef() const noexcept
   {
      return reinterpret_cast<CFMutableDataRef>(const_cast<void *>(m_ref.get()));
   }
};

} // namespace CoreFoundation

#endif // __cfxx_data_h__
