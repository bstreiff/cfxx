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

#include "cfxx.h"

int main(int argc, char* argv[])
{
   CoreFoundation::String awesomeStringOne = "foobar";
   CoreFoundation::String awesomeStringTwo("foo");

   CoreFoundation::MutableString mutableString(awesomeStringOne);

   printf("%d (%c)\n", static_cast<int>(awesomeStringOne[2]), static_cast<char>(awesomeStringOne[2]));

   printf("%s\n", mutableString.to_string().c_str());

   {
      const char* shortString = "this is a test";

      std::string shortStdStringToTestIteration(shortString);
      CoreFoundation::String shortCFStringToTestIteration(shortString);

      auto itr1 = shortStdStringToTestIteration.begin();
      auto itr2 = shortCFStringToTestIteration.begin();

      size_t charsCounted = 0;
      for (; itr1 != shortStdStringToTestIteration.end() && itr2 != shortCFStringToTestIteration.end(); ++itr1, ++itr2)
      {
         if (static_cast<UniChar>(*itr1) != *itr2)
         {
            printf("mismatch! %c != %c\n",
               static_cast<char>(*itr1),
               static_cast<char>(*itr2));
         }
         ++charsCounted;
      }
      printf("counted %d characters\n", static_cast<int>(charsCounted));
   }

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

      std::string longStdStringToTestIteration(longString);
      CoreFoundation::String longCFStringToTestIteration(longString);

      auto itr1 = longStdStringToTestIteration.begin();
      auto itr2 = longCFStringToTestIteration.begin();

      size_t charsCounted = 0;
      for (; itr1 != longStdStringToTestIteration.end() && itr2 != longCFStringToTestIteration.end(); ++itr1, ++itr2)
      {
         if (static_cast<UniChar>(*itr1) != *itr2)
         {
            printf("mismatch! %c != %c\n",
               static_cast<char>(*itr1),
               static_cast<char>(*itr2));
         }
         ++charsCounted;
      }
      printf("counted %d characters\n", static_cast<int>(charsCounted));
   }


   printf("Is CFTypeRef base of CFStringRef? %d\n", CoreFoundation::IsBaseOf<CFTypeRef, CFStringRef>::value);
   printf("Is CFStringRef base of CFMutableStringRef? %d\n", CoreFoundation::IsBaseOf<CFStringRef, CFMutableStringRef>::value);
   printf("Is CFTypeRef base of CFMutableStringRef? %d\n", CoreFoundation::IsBaseOf<CFTypeRef, CFMutableStringRef>::value);
   printf("Is CFTypeRef base of CFDataRef? %d\n", CoreFoundation::IsBaseOf<CFTypeRef, CFDataRef>::value);
   printf("Is CFStringRef base of CFDataRef? %d\n", CoreFoundation::IsBaseOf<CFStringRef, CFDataRef>::value);

   return 0;
}
