#ifndef HELPER_H_
#define HELPER_H_
#define ERROR_MEM_MSG "Error: Could not allocate memory\n"

#include <stdbool.h>

#define BUFFER 32

/**
 * The functions defined here are general helper functions
 * that aren't related to any specific module of this software.
 */

/**
 * Just a safer malloc function.
 * @param size the memory to allocate.
 */
void *malloc_(size_t size);

/**
 * Returns a copy of the given string.
 * @param src the source string.
 * @return a copy of the given string.
 */
char *strDup(const char *src);

/**
 * Just a safer strncpy function.
 * @param dest the destination string.
 * @param src the source string.
 * @param n the number of characters to copy
 * @return the number of characters to copy.
 */
int strncpy_(char *dest, const char *src, size_t n);

/**
 * Returns a random number between 0 and 1.
 * This function follows the PRNG.
 * @return the random number.
 */
double getRand(void);

/**
 * Returns the number rounded to the given digits.
 * @param n the number.
 * @param digits the digits to round after the decimal point.
 * @return the number rounded.
 */
double round_(double n, size_t digits);

/**
 * Returns the double value of the giving string.
 * @param str the string.
 * @return the double value of the giving string.
 */
double strToDouble(const char *str);

/**
 * Adds thousands separators to the given
 * string.
 * @param str the string.
 */
void addThousandsSep(char *str);

/**
 * Returns the factorial of the giving number.
 * @param n the number.
 * @return the factorial of the giving number.
 */
long int fact(long int n);

/**
 * Reads a line from the stdin.
 * @param str the string to display when reading the input.
 * @param buffer the string where the input will be stored.
 * @param size the input size to store.
 */
void getLine(const char *str, char *buffer, size_t size);

/**
 * Appends a char to the given string
 * @param str the string.
 * @param ch the char to append to the string.
 */
void appendChar(char **str, char ch);

/**
 * Returns true if the given string is empty,
 * false otherwise.
 * @param str the string
 * @return true if the given string is empty,
 * false otherwise.
 */
bool isEmpty(const char *str);

/**
 * Calls the function to clear the screen.
 */
void clearScreen(void);

#if defined(_WIN32) || defined(WIN32)
/**
 * Reference to the snprintf function of the stdio library.
 * Because you know, some compilers are not compliant with
 * the C standard.
 */
int snprintf(char *buf, size_t size, const char *fmt, ...);
#endif

#endif /* HELPER_H_ */
