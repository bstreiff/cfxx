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

TEST(StringTests, StringConstructor)
{
   CoreFoundation::String str1 = "foobar";
   CoreFoundation::String str2("foo");
}

TEST(StringTests, MutableStringConstructor)
{
   CoreFoundation::MutableString str1 = "foobar";
}

TEST(StringTests, StringIndex)
{
   CoreFoundation::String str1("foobar");

   ASSERT_EQ(static_cast<UniChar>('o'), str1[2]);
}

TEST(StringTests, ShortStringIteration)
{
   const char* shortString = "this is a test";

   const std::string shortStdString = shortString;
   CoreFoundation::String shortCFString = shortString;

   auto itr1 = shortStdString.begin();
   auto itr2 = shortCFString.begin();

   size_t charsCounted = 0;
   for (; itr1 != shortStdString.end() && itr2 != shortCFString.end(); ++itr1, ++itr2)
   {
      ASSERT_EQ(static_cast<UniChar>(*itr1), *itr2);
      ++charsCounted;
   }
   ASSERT_EQ(shortStdString.size(), charsCounted);
}

TEST(StringTests, LongStringIteration)
{
   // Via cupcakeipsum.com. This is a thing that exists. Thank you, internet.
   const char* longString =
      "Cupcake ipsum dolor sit amet powder bear claw NERDIES marshmallow. Chupa chups "
      "sweet roll gummies chocolate topping wafer I love oat cake. Chupa chups carrot "
      "cake donut halvah sugar plum carrot cake sweet roll dessert. I love chocolate "
      "cupcake I love I love ice cream biscuit pudding powder. I love chocolate candy "
      "canes. Jelly gummies pudding chocolate cake sweet roll. Tart cookie toffee "
      "topping bear claw. Tart chupa chups danish chocolate bar bear claw icing I love "
      "pudding.";

   const std::string longStdString = longString;
   CoreFoundation::String longCFString = longString;

   auto itr1 = longStdString.begin();
   auto itr2 = longCFString.begin();

   size_t charsCounted = 0;
   for (; itr1 != longStdString.end() && itr2 != longCFString.end(); ++itr1, ++itr2)
   {
      ASSERT_EQ(static_cast<UniChar>(*itr1), *itr2);
      ++charsCounted;
   }
   ASSERT_EQ(longStdString.size(), charsCounted);
}

TEST(StringTests, ReverseIteration)
{
   const char* testString =
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Integer nec odio. "
      "Praesent libero. Sed cursus ante dapibus diam. Sed nisi. Nulla quis sem at nibh "
      "elementum imperdiet. Duis sagittis ipsum. Praesent mauris. Fusce nec tellus sed "
      "augue semper porta. Mauris massa. Vestibulum lacinia arcu eget nulla. Class aptent "
      "taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. "
      "Curabitur sodales ligula in libero. Sed dignissim lacinia nunc.";

   const std::string testStdString = testString;
   CoreFoundation::String testCFString = testString;

   auto itr1 = testStdString.rbegin();
   auto itr2 = testCFString.rbegin();

   size_t charsCounted = 0;
   for (; itr1 != testStdString.rend() && itr2 != testCFString.rend(); ++itr1, ++itr2)
   {
      ASSERT_EQ(static_cast<UniChar>(*itr1), *itr2);
      ++charsCounted;
   }
   ASSERT_EQ(testStdString.size(), charsCounted);
}

TEST(StringTests, StringConversion)
{
   const char* testString = "this is a test";

   const std::string testStdString = testString;
   CoreFoundation::String testCFString = testString;

   const std::string convertedString = testCFString.to_string();

   ASSERT_EQ(testStdString, convertedString);
}

TEST(StringTests, StringComparison)
{
   CoreFoundation::String a = "aaa";
   CoreFoundation::String b = "bbb";
   CoreFoundation::String c = "ccc";

   ASSERT_TRUE(a < b);
   ASSERT_TRUE(a <= b);
   ASSERT_TRUE(a == a);
   ASSERT_TRUE(a != b);
   ASSERT_TRUE(b != c);
   ASSERT_TRUE(c > a);
   ASSERT_TRUE(c >= a);

   ASSERT_FALSE(a == b);
   ASSERT_FALSE(a > b);
   ASSERT_FALSE(b >= c);
   ASSERT_FALSE(c != c);
}

TEST(StringTests, MutableStringAppend)
{
   const char* prefix = "foo";
   const char* suffix = "bar";

   CoreFoundation::String foobar = "foobar";
   CoreFoundation::String bar = "bar";

   CoreFoundation::MutableString a = prefix;
   a.append(suffix);

   ASSERT_EQ(foobar, a);

   CoreFoundation::MutableString b = prefix;
   b.append(bar);

   ASSERT_EQ(foobar, b);
}

