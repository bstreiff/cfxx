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

#include "gtest/gtest.h"
#include "cfxx/cfxx.h"

TEST(TypeCheckingTests, IsBaseOf)
{
   // All CFStringRefs are CFTypeRefs.
   ASSERT_TRUE((CoreFoundation::IsBaseOf<CFTypeRef, CFStringRef>::value));

   // All CFMutableStringRefs are CFStringRefs.
   ASSERT_TRUE((CoreFoundation::IsBaseOf<CFStringRef, CFMutableStringRef>::value));

   // Given the above, by transitivity, all CFMutableStringRefs are CFTypeRefs.
   ASSERT_TRUE((CoreFoundation::IsBaseOf<CFTypeRef, CFMutableStringRef>::value));

   // All CFDataRefs are CFTypeRefs.
   ASSERT_TRUE((CoreFoundation::IsBaseOf<CFTypeRef, CFDataRef>::value));

   // However, CFDataRefs and CFStringRefs aren't bases of each other.
   ASSERT_FALSE((CoreFoundation::IsBaseOf<CFStringRef, CFDataRef>::value));
   ASSERT_FALSE((CoreFoundation::IsBaseOf<CFDataRef, CFStringRef>::value));
}
