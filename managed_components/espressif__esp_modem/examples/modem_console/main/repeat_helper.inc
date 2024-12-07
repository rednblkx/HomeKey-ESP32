/* Modem console example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef MODEM_CONSOLE_REPEAT_HELPER_INC_H
#define MODEM_CONSOLE_REPEAT_HELPER_INC_H

/**
 * @brief This header is used to generate a defined macro many times with iterator
 */

#if MAX_REPEAT_NR > 20
#error "Not enough items to repeat"
#endif

#define _DO_REPEAT_ITEM(a) \
ITEM_TO_REPEAT(0) \
ITEM_TO_REPEAT(1) \
ITEM_TO_REPEAT(2) \
ITEM_TO_REPEAT(3) \
ITEM_TO_REPEAT(4) \
ITEM_TO_REPEAT(5) \
ITEM_TO_REPEAT(6) \
ITEM_TO_REPEAT(7) \
ITEM_TO_REPEAT(8) \
ITEM_TO_REPEAT(9) \
ITEM_TO_REPEAT(10) \
ITEM_TO_REPEAT(11) \
ITEM_TO_REPEAT(12) \
ITEM_TO_REPEAT(13) \
ITEM_TO_REPEAT(14) \
ITEM_TO_REPEAT(15) \
ITEM_TO_REPEAT(16) \
ITEM_TO_REPEAT(17) \
ITEM_TO_REPEAT(18) \
ITEM_TO_REPEAT(19) \
ITEM_TO_REPEAT(20)

#endif //MODEM_CONSOLE_REPEAT_HELPER_INC_H
