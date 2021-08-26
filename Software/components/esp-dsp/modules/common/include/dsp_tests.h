// Copyright 2018-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _DSP_TESTS_H_
#define _DSP_TESTS_H_

#define TEST_ASSERT_EXEC_IN_RANGE(min_exec, max_exec, actual) \
    if (cycles >= max_exec) { \
        TEST_ASSERT_MESSAGE (false, "Exec time takes more than expected!");\
    }\
    if (cycles < min_exec) {\
        TEST_ASSERT_MESSAGE (false, "Exec time takes less then expected!");\
    }


#endif // _DSP_TESTS_H_