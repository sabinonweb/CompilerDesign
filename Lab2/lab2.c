/*
 * Regular Expression Checker
 * Checks if a string matches the pattern:
 *   (a(b+a)*b) + (b(a+b)*a)
 *
 * This means: strings over {a, b} that either
 *   - start with 'a' and end with 'b', OR
 *   - start with 'b' and end with 'a'
 * with length >= 2, and all characters must be 'a' or 'b'.
 * 'e' is treated as epsilon (null) -> INVALID.
 */

#include <stdio.h>
#include <string.h>

#define MAX_LEN 200

/* Result codes */
#define VALID   1
#define INVALID 0

/* Check the string against the regex pattern */
int checkPattern(const char *str, char *reason) {
    int len = (int)strlen(str);
    int i;

    /* Step 2: compute length (already done via strlen) */

    /* Step 3 & 4: validate every character */
    for (i = 0; i < len; i++) {
        if (str[i] == 'e') {
            sprintf(reason, "Character 'e' (epsilon/null) found at position %d -- NULL is INVALID", i + 1);
            return INVALID;
        }
        if (str[i] != 'a' && str[i] != 'b') {
            sprintf(reason, "Invalid character '%c' at position %d -- only 'a' and 'b' are allowed", str[i], i + 1);
            return INVALID;
        }
    }

    /* Step 5: length must be >= 2 */
    if (len < 2) {
        if (len == 0)
            sprintf(reason, "Empty string -- length must be >= 2");
        else
            sprintf(reason, "String too short (length = %d) -- minimum length is 2", len);
        return INVALID;
    }

    /* Step 6: check first and last character */
    char first = str[0];
    char last  = str[len - 1];

    if ((first == 'a' && last == 'b') || (first == 'b' && last == 'a')) {
        sprintf(reason,
            "first='%c', last='%c' -- matches %s",
            first, last,
            (first == 'a') ? "a(b+a)*b" : "b(a+b)*a");
        return VALID;
    } else {
        sprintf(reason,
            "first='%c', last='%c' -- does not satisfy (first=a,last=b) OR (first=b,last=a)",
            first, last);
        return INVALID;
    }
}

/* Print a decorated result banner */
void printResult(const char *str, int result, const char *reason) {
    printf("\n  Input string : \"%s\"\n", str);
    printf("  Length       : %d\n", (int)strlen(str));
    printf("  Reason       : %s\n", reason);
    printf("  Result       : ");
    if (result == VALID)
        printf("*** VALID ***\n");
    else
        printf("*** INVALID ***\n");
    printf("  %s\n", "----------------------------------------");
}

int main() {
    char input[MAX_LEN];
    char reason[300];
    char again;

    printf("==========================================\n");
    printf("  Regular Expression Checker             \n");
    printf("  Pattern: (a(b+a)*b) + (b(a+b)*a)      \n");
    printf("  Valid chars: a, b  |  e = epsilon (null)\n");
    printf("==========================================\n");

    do {
        printf("\nEnter string (a/b only, 'e' = epsilon): ");
        if (fgets(input, MAX_LEN, stdin) == NULL) break;

        /* strip trailing newline */
        int inputLen = (int)strlen(input);
        if (inputLen > 0 && input[inputLen - 1] == '\n')
            input[inputLen - 1] = '\0';

        int result = checkPattern(input, reason);
        printResult(input, result, reason);

        printf("  Check another string? (y/n): ");
        scanf(" %c", &again);
        /* flush leftover newline */
        while (getchar() != '\n');

    } while (again == 'y' || again == 'Y');

    printf("\nProgram terminated.\n");
    return 0;
}
