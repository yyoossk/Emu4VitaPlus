#pragma once

#define ALIGN_UP(x, a) ((x) + ((a) - 1)) & ~((a) - 1)
#define ALIGN_UP_10H(x) ALIGN_UP(x, 0x10)

#define LOOP_PLUS_ONE(VALUE, TOTAL) (VALUE = ((VALUE + 1 == (TOTAL)) ? 0 : VALUE + 1))
#define LOOP_MINUS_ONE(VALUE, TOTAL) (VALUE = ((VALUE == 0) ? (TOTAL) - 1 : VALUE - 1))