#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>
#include "helper.h"


static void prependChar(char **str, char ch)
{
    size_t len = strlen(*str) + 2;
    char *tmp = strDup(*str);

    free(*str);
    *str = malloc_(len);
    *str[0] = '\0';
    snprintf(*str, len, "%c%s", ch, tmp);

    free(tmp);
}


void *malloc_(size_t size)
{
    void *alloc_mem = malloc(size);

    if (!alloc_mem) {
        printf(ERROR_MEM_MSG);
        exit(-1);
    }

    return alloc_mem;
}


char *strDup(const char *src)
{
    char *new;
    size_t len;

    if (src == NULL) {
        return NULL;
    }

    len = strlen(src) + 1;
    new = malloc_(len);
    snprintf(new, len, "%s", src);

    return new;
}


int strncpy_(char *dest, const char *src, size_t n)
{
    snprintf(dest, n, "%s", src);

    return n;
}


double getRand(void)
{
    srand(time(NULL));
    return (double) abs(rand() * 100) / RAND_MAX;
}


double round_(double n, size_t digits)
{
    double decimals = pow(10, digits);
    n *= decimals;
    n = (n >= floor(n) + 0.5f) ?
        ceil(n) :
        floor(n);

    return n / decimals;
}


double strToDouble(const char *str)
{
    if (str == NULL) {
        return 0.0;
    }

    return strtod(str, NULL);
}


void addThousandsSep(char *str)
{
    int i, dot_index, char_n = 0;
    size_t len;
    size_t new_len;
    char *dot, *tmp = NULL;

    if (str == NULL) {
        return;
    }

    len = strlen(str);
    new_len = len;
    dot = strchr(str, '.');

    if (dot != NULL) {
        dot_index = dot - str;
        tmp = malloc_(len - dot_index + 1);
        strncpy_(tmp, str + dot_index, len - dot_index + 1);
    } else {
        dot_index = len;
        tmp = malloc_(1);
        tmp[0] = '\0';
    }

    for (i = dot_index - 1; i >= 0; i--) {
        if (i >= 0 && char_n > 0 && char_n % 3 == 0) {
            prependChar(&tmp, ',');
            new_len++;
        }

        prependChar(&tmp, str[i]);
        char_n++;
    }

    strncpy_(str, tmp, new_len + 1);
    free(tmp);
}


long int fact(long int n)
{
    long int next;

    if (n == 1 || n == 0) {
        return 1;
    }

    next = (n < 0) ? n + 1 : n - 1;

    return n * fact(next);
}


void getLine(const char *str, char *buffer, size_t size)
{
    size_t len;

    if (str != NULL) {
        printf("%s", str);
    }

    if (fgets(buffer, size, stdin) == NULL) {
        printf("\r");
        exit(0);
    }

    len = strlen(buffer);
    if (len <= 0 || buffer[len - 1] == '\n') {
        return;
    }

    while (getchar() != '\n');
}


void appendChar(char **str, char ch)
{
    const size_t len = strlen(*str);
    *str = realloc(*str, len + 2);
    snprintf((*str) + len, 2, "%c", ch);
}


bool isEmpty(const char *str)
{
    return str == NULL || !strcmp(str, "");
}


void clearScreen(void)
{
    #if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
        system("clear");
    #endif

    #if defined(_WIN32) || defined(_WIN64)
        system("cls");
    #endif
}
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"
#include "variable.h"
#include "lexer.h"

/**
 * Current token being used.
 */
char *current_token;

/**
 * Type of the current token being used.
 */
enum TOKEN_TYPE current_type;


static enum TOKEN_TYPE getType(const char ch)
{
    if ((ch >= '0' && ch <= '9') || ch == '.') {
        return T_Operand;
    }

    if (ch == '(' || ch == ')' ||
        ch == '+' || ch == '-' ||
        ch == '/' || ch == '*' ||
        ch == '^' || ch == '!' ||
        ch == '=') {
        return T_Operator;
    }

    if ((ch >= 'a' && ch <= 'z') ||
        (ch >= 'A' && ch <= 'Z')) {
        return T_Word;
    }

    return T_None;
}


static bool isIgnorableC(const char ch)
{
    return ch == ' ' || ch == ',';
}


static bool isNumber(const char *str)
{
    size_t len = strlen(str);
    size_t i;

    for (i = 0; i < len; i++) {
        /* Signed number */
        if (i == 0 && (str[i] == '-' || str[i] == '+')) {
            continue;
        }

        /* Exponent number */
        if (i != 0 && i+1 < len && str[i] == 'e' &&
            (str[i+1] == '+' || str[i+1] == '-')) {
            i++;
            continue;
        }

        if (str[i] != '.' && (str[i] < '0' || str[i] > '9')) {
            return false;
        }
    }

    return true;
}


static bool isSigned(struct list *list, const char *str, const int i)
{
    if (str[i] != '+' && str[i] != '-') {
        return false;
    }

    if (i-1 < 0) {
        return true;
    }

    return
        (getType(str[i-1]) == T_Operator && str[i - 1] != ')' && str[i - 1] != '!') &&
        (list->last == NULL || !isNumber(list->last->value));
}


static enum OPCODE getOpcode(const char *str)
{
    if (!strcmp(str, "+")) {
        return OP_Plus;
    } else if (!strcmp(str, "-")) {
        return OP_Minus;
    } else if (!strcmp(str, "*")) {
        return OP_Multi;
    } else if (!strcmp(str, "/")) {
        return OP_Div;
    } else if (!strcmp(str, "=")) {
        return OP_Equal;
    } else if (!strcmp(str, "^")) {
        return OP_Pow;
    } else if (!strcmp(str, "!")) {
        return OP_Fact;
    } else if (!strcmp(str, "sqrt")) {
        return OP_Sqrt;
    } else if (!strcmp(str, "abs")) {
        return OP_Abs;
    } else if (!strcmp(str, "log")) {
        return OP_Log;
    } else if (!strcmp(str, "floor")) {
        return OP_Floor;
    } else if (!strcmp(str, "ceil")) {
        return OP_Ceil;
    } else if (!strcmp(str, "round")) {
        return OP_Round;
    } else if (!strcmp(str, "rand")) {
        return OP_Rand;
    } else if (!strcmp(str, "mb")) {
        return OP_Mb;
    } else if (!strcmp(str, "gb")) {
        return OP_Gb;
    } else if (!strcmp(str, "tb")) {
        return OP_Tb;
    } else if (!strcmp(str, "pb")) {
        return OP_Pb;
    } else if (!strcmp(str, "sin")) {
        return OP_Sin;
    } else if (!strcmp(str, "cos")) {
        return OP_Cos;
    } else if (!strcmp(str, "tan")) {
        return OP_Tan;
    } else if (!strcmp(str, "asin")) {
        return OP_Asin;
    } else if (!strcmp(str, "acos")) {
        return OP_Acos;
    } else if (!strcmp(str, "atan")) {
        return OP_Atan;
    } else if (!strcmp(str, "(")) {
        return OP_Open_parenthesis;
    } else if (!strcmp(str, ")")) {
        return OP_Closed_parenthesis;
    }

    return OP_None;
}


static void addToken(struct list *list, const char *token)
{
    struct token *new;

    if (isEmpty(token)) {
        return;
    }

    new = malloc_(sizeof(struct token));
    new->opcode = getOpcode(token);
    new->value = strDup(token);
    new->next = NULL;

    /* Set zero as argument if no argument is provided */
    if (list->last != NULL && !strcmp(list->last->value, "(") &&
        !strcmp(token, ")")) {
        addToken(list, "0");
    }

    if (list->last == NULL) {
        list->last = new;
        list->first = list->last;
        return;
    }

    list->last->next = new;
    list->last = new;
}


static void processChar(struct list *list,
                        const char *str,
                        const int i)
{
    if (isIgnorableC(str[i])) {
        return;
    }

    if (current_type == T_None) {
        current_type = getType(str[i]);
    }

    /* Add token */
    if (getType(str[i]) != current_type ||
        current_type == T_Operator) {
        addToken(list, current_token);

        free(current_token);
        current_token = malloc_(1);
        current_token[0] = '\0';
        current_type = getType(str[i]);
    }

    /* Allow signed numbers */
    if (isSigned(list, str, i)) {
        current_type = T_Operand;
        free(current_token);
        current_token = malloc_(2);
        current_token[0] = str[i];
        current_token[1] = '\0';

        return;
    }

    appendChar(&current_token, str[i]);
}


void tokenize(struct list *list, const char *func)
{
    size_t i;

    current_type = T_None;
    current_token = malloc_(1);
    current_token[0] = '\0';

    for (i = 0; i <= strlen(func); i++) {
        processChar(list, func, i);
    }

    free(current_token);
}


void freeList(struct list *list)
{
    struct token *node;

    while ((node = list->first) != NULL) {
        list->first = list->first->next;
        free(node->value);
        free(node);
    }

    free(list);
}


void initList(struct list **list)
{
    (*list) = malloc_(sizeof(struct list));
    (*list)->last = NULL;
    (*list)->first = NULL;
}


int getPrec(enum OPCODE opcode)
{
    if (opcode == OP_Equal) {
        return 5;
    }

    if (opcode == OP_Open_parenthesis ||
        opcode == OP_Closed_parenthesis) {
        return 4;
    }

    if (opcode == OP_Plus ||
        opcode == OP_Minus) {
        return 3;
    }

    if (opcode == OP_Multi ||
        opcode == OP_Div) {
        return 2;
    }

    if (opcode == OP_Pow) {
        return 1;
    }

    return 0;
}


bool isOperator(enum OPCODE opcode)
{
    return
        opcode == OP_Plus || opcode == OP_Minus ||
        opcode == OP_Multi || opcode == OP_Div ||
        opcode == OP_Pow || opcode == OP_Equal;
}


bool isFunction(enum OPCODE opcode)
{
    return isTrigonometric(opcode) ||
        opcode == OP_Sqrt || opcode == OP_Log ||
        opcode == OP_Floor || opcode == OP_Ceil ||
        opcode == OP_Round || opcode == OP_Abs ||
        opcode == OP_Rand || opcode == OP_Fact;
}


bool isTrigonometric(enum OPCODE opcode)
{
    return
        opcode == OP_Sin || opcode == OP_Cos ||
        opcode == OP_Tan || opcode == OP_Asin ||
        opcode == OP_Acos || opcode == OP_Atan;
}


bool isDataUnit(enum OPCODE opcode)
{
    return
        opcode == OP_Mb || opcode == OP_Gb ||
        opcode == OP_Tb || opcode == OP_Pb;
}


bool isValid(struct token *node)
{
    return isOperator(node->opcode) ||
        isFunction(node->opcode) ||
        isDataUnit(node->opcode) ||
        node->opcode == OP_Open_parenthesis ||
        node->opcode == OP_Closed_parenthesis ||
        isVariable(node->value) ||
        isNumber(node->value);
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "helper.h"
#include "lexer.h"
#include "variable.h"
#include "parser.h"

/**
 * Number of decimals used for the numbers.
 */
int precision = -1;

/**
 * Number of decimals used for the result.
 */
int result_precision = -1;

/**
 * Manage angles in degree or not.
 */
bool degree = false;

/**
 * Warning about division by zero.
 */
bool division_warning = false;

/**
 * Warning about an invalid value for
 * a trigonometric function.
 */
bool trigonometric_warning = false;

/**
 * The current operation is defining a variable or not.
 */
bool inside_def = false;


static void push(struct list **list, const struct token *node)
{
    struct token *new = malloc_(sizeof(struct token));

    if (isEmpty(node->value)) {
        return;
    }

    new->next = NULL;
    new->prev = NULL;
    new->opcode = node->opcode;
    new->value = strDup(node->value);

    if ((*list)->last == NULL) {
        (*list)->last = new;
        (*list)->first = new;
    } else {
        (*list)->last->next = new;
        new->prev = (*list)->last;
        (*list)->last = new;
    }
}


static void moveToken(struct list **dest, struct list **src)
{
    struct token *cpy, *tmp;

    if (!strcmp((*src)->last->value, "(")) {
        tmp = (*src)->last;
        (*src)->last = (*src)->last->prev;
        free(tmp->value);
        free(tmp);
    }

    cpy = malloc_(sizeof(struct token));
    cpy->prev = (*dest)->last;
    cpy->next = NULL;
    cpy->opcode = (*src)->last->opcode;
    cpy->value = strDup((*src)->last->value);

    if ((*dest)->last != NULL) {
        (*dest)->last->next = cpy;
    } else {
        (*dest)->first = cpy;
    }

    (*dest)->last = cpy;
    tmp = (*src)->last;
    (*src)->last = (*src)->last->prev;
    free(tmp->value);
    free(tmp);

    if ((*src)->last != NULL) {
        (*src)->last->next = NULL;
    } else {
        (*src)->first = NULL;
    }
}


static void migrateUntilParenthesis(struct list *output,
                                    struct list *operators)
{
    struct token *tmp;

    while (operators->last != NULL &&
        strcmp(operators->last->value, "(") != 0) {
        moveToken(&output, &operators);
    }

    if (operators->last != NULL && !strcmp(operators->last->value, "(")) {
        tmp = operators->last;
        operators->last = operators->last->prev;
        free(tmp->value);
        free(tmp);
    }
}


static int hasHigherEqualPrec(struct token *first, struct token *second)
{
    return first != NULL && second != NULL &&
        getPrec(first->opcode) >= getPrec(second->opcode);
}


static void infixToPostfix(struct list *tokens,
                           struct list *output,
                           struct list *operators)
{
    struct token *node = tokens->first;

    division_warning = false;
    trigonometric_warning = false;

    while (node != NULL) {
        if (node->opcode == OP_Open_parenthesis) {
            push(&operators, node);
        } else if (node->opcode == OP_Closed_parenthesis) {
            migrateUntilParenthesis(output, operators);
        } else if (isFunction(node->opcode)) {
            push(&operators, node);
        } else if (isOperator(node->opcode)) {
            while (hasHigherEqualPrec(node, operators->last)) {
                moveToken(&output, &operators);
            }

            push(&operators, node);
        } else {
            push(&output, node);
        }

        node = node->next;
    }

    while (operators->last != NULL) {
        moveToken(&output, &operators);
    }
}


static double getValue(const char *str)
{
    return isVariable(str) ?
        getVariableValue(str) :
        strToDouble(str);
}


static double getOpResult(enum OPCODE op,
                          const char *a,
                          const char *b)
{
    double x, y;

    inside_def = op == OP_Equal;
    if (inside_def) {
        addVariable(a, getValue(b));
        return 0;
    }

    x = getValue(a);
    y = getValue(b);

    if (precision >= 0) {
        x = round_(x, precision);
        y = round_(y, precision);
    }

    if (op == OP_Plus) {
        return x + y;
    }

    if (op == OP_Minus) {
        return x - y;
    }

    if (op == OP_Multi) {
        return x * y;
    }

    if (op == OP_Div) {
        if (y == 0) {
            division_warning = true;
            return 0;
        }

        return x / y;
    }

    if (op == OP_Pow) {
        return pow(x, y);
    }

    if (op == OP_Fact) {
        return fact((int)y);
    }

    if (op == OP_Sqrt) {
        return sqrt(y);
    }

    if (op == OP_Abs) {
        return fabs(y);
    }

    if (op == OP_Log) {
        return log(y);
    }

    if (op == OP_Floor) {
        return floor(y);
    }

    if (op == OP_Ceil) {
        return ceil(y);
    }

    if (op == OP_Round) {
        return round(y);
    }

    if (op == OP_Rand) {
        return getRand();
    }

    if (op == OP_Mb) {
        return y * ONE_MB;
    }

    if (op == OP_Gb) {
        return y * ONE_GB;
    }

    if (op == OP_Tb) {
        return y * ONE_TB;
    }

    if (op == OP_Pb) {
        return y * ONE_PT;
    }

    if (degree && isTrigonometric(op)) {
        y = y / 180 * MATH_PI;
    }

    if (op == OP_Sin) {
        return sin(y);
    }

    if (op == OP_Cos) {
        return cos(y);
    }

    if (op == OP_Tan) {
        return tan(y);
    }

    if ((op == OP_Asin ||
        op == OP_Acos ||
        op == OP_Atan) &&
        (y < -1 || y > 1)) {
        trigonometric_warning = true;
        return 0;
    }

    if (op == OP_Asin) {
        return asin(y);
    }

    if (op == OP_Acos) {
        return acos(y);
    }

    if (op == OP_Atan) {
        return atan(y);
    }

    return 0;
}


static char *pop(struct list *list)
{
    struct token *tmp;
    char *str;

    if (list->last == NULL) {
        return 0;
    }

    str = strDup(list->last->value);
    tmp = list->last;
    list->last = list->last->prev;
    free(tmp->value);
    free(tmp);

    return str;
}


static void pushResult(struct list *list, const struct token *node)
{
    struct token *new;
    char *x = NULL, *y = NULL;
    double result;

    if (list == NULL || list->last == NULL) {
        return;
    }

    y = pop(list);

    if (!isFunction(node->opcode) &&
        !isDataUnit(node->opcode)) {
        x = pop(list);
    }

    result = getOpResult(node->opcode, x, y);
    if (precision >= 0) {
        result = round_(result, precision);
    }

    new = malloc_(sizeof(struct token));
    new->value = malloc_(BUFFER);
    snprintf(new->value, BUFFER, NUMBER_FORMAT, result);

    push(&list, new);
    free(new->value);
    free(new);
    free(y);

    if (x != NULL) {
        free(x);
    }
}


static double getPostfixResult(const struct list *postfix)
{
    struct list *result_list;
    struct token *node = postfix->first;
    double result = 0.0;

    initList(&result_list);

    while (node != NULL) {
        if (isOperator(node->opcode) ||
            isFunction(node->opcode) ||
            isDataUnit(node->opcode)) {
            pushResult(result_list, node);
        } else {
            push(&result_list, node);
        }

        node = node->next;
    }

    if (result_list != NULL && result_list->last != NULL) {
        result = getValue(result_list->last->value);
    }

    if (result_precision >= 0) {
        result = round_(result, result_precision);
    }

    freeList(result_list);

    return result;
}


char *getResult(const char *func,
                struct list *tokens,
                struct list *output)
{
    struct list *operators;
    char *result = malloc_(BUFFER);
    inside_def = false;
    initList(&operators);

    tokenize(tokens, func);
    infixToPostfix(tokens, output, operators);
    snprintf(result, BUFFER, NUMBER_FORMAT, getPostfixResult(output));
    freeList(operators);

    if (inside_def) {
        free(result);
        return NULL;
    }

    return result;
}


size_t warnings_number=0;
void printWarnings(const struct list *list)
{
    struct token *node = list->first;
    size_t warnings_quantity = 0;

    while (node != NULL) {
        if (!isValid(node)) {
            printf(WARNING_TOKEN, node->value);
            warnings_quantity++;
        }

        node = node->next;
    }

    if (division_warning) {
        printf(WARNING_ZERO_DIV);
        warnings_quantity++;
    }

    if (trigonometric_warning) {
        printf(WARNING_TRIGONOMETRIC);
        warnings_quantity++;
    }

    if (warnings_quantity > 0) {
        printf(MSG_INACCURATE_RESULT);
        warnings_number=warnings_quantity;
    }
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "helper.h"
#include "lexer.h"
#include "parser.h"
#include "variable.h"
#include "quich.h"

/**
 * Verbose mode or not.
 */
bool verbose = false;

/**
 * Number of flags used.
 */
int flags_quantity = 0;

/**
 * Interactive mode or not.
 */
bool interactive_mode = false;

/**
 * Using thousands separator or not.
 */
bool thousands_sep = false;

/**
 * Format to display the results.
 */
char *format = NULL;


static void printVerbose(struct list *tokens, struct list *output)
{
    struct token *node = tokens->first;

    /* Tokens */
    printf("Tokens > ");
    while (node != NULL) {
        printf(isValid(node) ? "'%s' " : "'%s'? ", node->value);
        node = node->next;
    }

    node = output->first;

    /* Postfix operation */
    printf("\nPosfix > ");
    while (node != NULL) {
        printf("%s ", node->value);
        node = node->next;
    }

    printf("\nResult > ");
}


static void printResult(char *func)
{
    struct list *tokens, *output;
    char *result;

    initList(&tokens);
    initList(&output);

    result = getResult(func, tokens, output);

    if (!isEmpty(format)) {
        snprintf(result, BUFFER, format, strToDouble(result));
    }

    if (verbose) {
        printVerbose(tokens, output);
    }

    if (thousands_sep) {
        addThousandsSep(result);
    }

    if (result != NULL) {
        printf("%s\n", result);
        free(result);
    } else if (verbose) {
        printf(MSG_DEFINITION);
    }

    printWarnings(output);
    freeList(tokens);
    freeList(output);
}


static void printAll(char *func)
{
    char *statements = strtok(func, STMT_SEPARATOR);

    while (statements != NULL) {
        printResult(statements);
        statements = strtok(NULL, STMT_SEPARATOR);

        if (statements != NULL && verbose) {
            printf("\n");
        }
    }
}


/**
 * Returns true if the program flow must be stopped,
 * false otherwise.
 */
static bool mapArgs(int argc, char *argv[])
{
    int i;

    for (i = 0; i < argc; i++) {
        /* Angles in degree */
        if (!strcmp(argv[i], "-d") || !strcmp(argv[i], "--degree")) {
            degree = true;
            flags_quantity++;
        }

        /* Verbose mode */
        if (!strcmp(argv[i], "-vvv") || !strcmp(argv[i], "--verbose")) {
            verbose = true;
            flags_quantity++;
        }

        /* Interactive */
        if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--interactive")) {
            interactive_mode = true;
            flags_quantity++;
        }

        /* Version */
        if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
            printf(MSG_VERSION);
            return true;
        }

        /* Help */
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            printf(MSG_HELP);
            return true;
        }

        /* Thousands separator */
        if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--thousands")) {
            thousands_sep = true;
            flags_quantity++;
        }

        /* The flags below work with values */
        if (i+1 >= argc) {
            return false;
        }

        /* Result format */
        if (!strcmp(argv[i], "-f") || !strcmp(argv[i], "--format")) {
            format = argv[++i];
            flags_quantity += 2;
        }

        /* Precision */
        if (!strcmp(argv[i], "-p") || !strcmp(argv[i], "--precision")) {
            precision = (int)strToDouble(argv[++i]);
            flags_quantity += 2;
        }

        /* Result precision */
        if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--round")) {
            result_precision = (int)strToDouble(argv[++i]);
            flags_quantity += 2;
        }
    }

    return false;
}


static bool processLine(void)
{
    char buffer[OPERATION_BUFFER];
    char *op;
    size_t len;

    getLine(INPUT_LINE, buffer, sizeof(buffer));

    len = strlen(buffer) + 1;
    op = malloc(len);
    /* length-1 to ignore newline character at the end */
    strncpy_(op, buffer, len - 1);

    if (!strcmp(op, CLEAR_COMMAND)) {
        clearScreen();
        free(op);
        return true;
    }

    if (!strcmp(op, EXIT_COMMAND)) {
        printf(MSG_BYE);
        free(op);
        return false;
    }

    printAll(op);
    free(op);
    return true;
}


static int interactive(void)
{
    int result;
    printf(MSG_INIT);
    while ((result = processLine()));

    return result;
}


static void addPredefValues(void)
{
    addVariable("PI", MATH_PI);
    addVariable("E", MATH_E);
    addVariable("G", MATH_G);
}


int main(int argc, char* argv[])
{
    if (mapArgs(argc, argv)) {
        return 0;
    }

    addPredefValues();
    if (interactive_mode || flags_quantity >= argc - 1) {
        interactive();
    } else {
        printAll(argv[1]);
    }

    freeVariables();

    if(warnings_number>0)
      return 1;
    else
      return 0;
}
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include "lexer.h"
#include "variable.h"

/**
 * Pointer to the first variable of the variables list.
 */
static struct variable *variables_first;


static bool replaceVariable(const char *key, double val)
{
    struct variable *var = variables_first;

    while (var != NULL) {
        if (!strcmp(key, var->key)) {
            var->value = val;
            return true;
        }

        var = var->next;
    }

    return false;
}


bool isVariable(const char *str)
{
    struct variable *var = variables_first;

    if (str == NULL) {
        return false;
    }

    while (var != NULL) {
        if (!strcmp(str, var->key)) {
            return true;
        }

        var = var->next;
    }

    return false;
}


void addVariable(const char *key, double val)
{
    struct variable *var;

    if (replaceVariable(key, val)) {
        return;
    }

    var = malloc_(sizeof(struct variable));
    var->key = strDup(key);
    var->value = val;

    var->next = variables_first;
    variables_first = var;
}


double getVariableValue(const char *key)
{
    struct variable *var = variables_first;

    while (var != NULL) {
        if (!strcmp(key, var->key)) {
            return var->value;
        }

        var = var->next;
    }

    return 0;
}


void freeVariables(void)
{
    struct variable *var;

    while ((var = variables_first) != NULL) {
        variables_first = variables_first->next;
        free(var->key);
        free(var);
    }

    free(variables_first);
}

