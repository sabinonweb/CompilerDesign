/*
 * Lexical Analyzer (Tokenizer)
 * Reads source code from File.txt, classifies each lexeme,
 * stores tokens in a table, and displays them.
 *
 * Token Types:
 *   KEYWORD        - reserved words (int, if, while, ...)
 *   IDENTIFIER     - variable/function names
 *   NUMBER         - integer or float literals
 *   OPERATOR       - +  -  *  /  =  ==  !=  <=  >= ...
 *   DELIMITER      - ;  ( )  { }  [ ]  ,  .
 *   STRING_LITERAL - "hello"
 *   UNKNOWN        - anything else
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LEN  100
#define MAX_TOKENS    1000

/* Token type enum */
typedef enum {
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    DELIMITER,
    STRING_LITERAL,
    UNKNOWN
} TokenType;

/* Token structure */
typedef struct {
    TokenType type;
    char      value[MAX_TOKEN_LEN];
} Token;

/* Global token storage */
Token tokenTable[MAX_TOKENS];
int   tokenCount = 0;

/* Reserved keyword list */
const char *keywords[] = {
    "int", "float", "double", "char", "void",
    "if", "else", "while", "for", "do",
    "return", "break", "continue", "switch", "case",
    "struct", "typedef", "sizeof", "const", "long",
    "short", "unsigned", "signed", "static", NULL
};

/* ----- Helper functions ----- */

int isKeyword(const char *word) {
    int i;
    for (i = 0; keywords[i] != NULL; i++)
        if (strcmp(word, keywords[i]) == 0) return 1;
    return 0;
}

const char *typeName(TokenType t) {
    switch (t) {
        case KEYWORD:        return "KEYWORD       ";
        case IDENTIFIER:     return "IDENTIFIER    ";
        case NUMBER:         return "NUMBER        ";
        case OPERATOR:       return "OPERATOR      ";
        case DELIMITER:      return "DELIMITER     ";
        case STRING_LITERAL: return "STRING LITERAL";
        default:             return "UNKNOWN       ";
    }
}

void storeToken(TokenType type, const char *value) {
    if (tokenCount >= MAX_TOKENS) {
        fprintf(stderr, "Warning: token table full.\n");
        return;
    }
    tokenTable[tokenCount].type = type;
    strncpy(tokenTable[tokenCount].value, value, MAX_TOKEN_LEN - 1);
    tokenTable[tokenCount].value[MAX_TOKEN_LEN - 1] = '\0';
    tokenCount++;
}

void tokenize(FILE *fp) {
    int  ch;
    char buffer[MAX_TOKEN_LEN];
    int  idx;
    int  next;
    char op[3];
    char delim[2];
    char unk[2];

    while ((ch = fgetc(fp)) != EOF) {

        /* Step 2: skip whitespace */
        if (isspace(ch)) continue;

        /* Handle '/' -- could be division, single-line comment, or block comment */
        if (ch == '/') {
            next = fgetc(fp);
            if (next == '/') {
                /* single-line comment: skip until newline */
                while ((ch = fgetc(fp)) != EOF && ch != '\n');
                continue;
            } else if (next == '*') {
                /* block comment: skip until closing star-slash */
                int prev = 0;
                while ((ch = fgetc(fp)) != EOF) {
                    if (prev == '*' && ch == '/') break;
                    prev = ch;
                }
                continue;
            } else {
                ungetc(next, fp);
                storeToken(OPERATOR, "/");
                continue;
            }
        }

        /* String literal */
        if (ch == '"') {
            idx = 0;
            buffer[idx++] = '"';
            while ((ch = fgetc(fp)) != EOF && ch != '"' && ch != '\n')
                if (idx < MAX_TOKEN_LEN - 2) buffer[idx++] = (char)ch;
            buffer[idx++] = '"';
            buffer[idx]   = '\0';
            storeToken(STRING_LITERAL, buffer);
            continue;
        }

        /* Step 3: keyword or identifier */
        if (isalpha(ch) || ch == '_') {
            idx = 0;
            buffer[idx++] = (char)ch;
            while ((ch = fgetc(fp)) != EOF && (isalnum(ch) || ch == '_'))
                if (idx < MAX_TOKEN_LEN - 1) buffer[idx++] = (char)ch;
            buffer[idx] = '\0';
            ungetc(ch, fp);
            storeToken(isKeyword(buffer) ? KEYWORD : IDENTIFIER, buffer);
            continue;
        }

        /* Step 4: number (integer or float) */
        if (isdigit(ch)) {
            idx = 0;
            buffer[idx++] = (char)ch;
            while ((ch = fgetc(fp)) != EOF && (isdigit(ch) || ch == '.'))
                if (idx < MAX_TOKEN_LEN - 1) buffer[idx++] = (char)ch;
            buffer[idx] = '\0';
            ungetc(ch, fp);
            storeToken(NUMBER, buffer);
            continue;
        }

        /* Step 5: operator with double-operator check */
        if (strchr("+-*%=<>!&|^~", ch)) {
            next = fgetc(fp);
            op[0] = (char)ch; op[1] = '\0'; op[2] = '\0';

            if ((ch == '=' && next == '=') ||
                (ch == '!' && next == '=') ||
                (ch == '<' && next == '=') ||
                (ch == '>' && next == '=') ||
                (ch == '+' && next == '+') ||
                (ch == '-' && next == '-') ||
                (ch == '+' && next == '=') ||
                (ch == '-' && next == '=') ||
                (ch == '*' && next == '=') ||
                (ch == '&' && next == '&') ||
                (ch == '|' && next == '|') ||
                (ch == '<' && next == '<') ||
                (ch == '>' && next == '>')) {
                op[1] = (char)next;
            } else {
                ungetc(next, fp);
            }
            storeToken(OPERATOR, op);
            continue;
        }

        /* Step 6: delimiter */
        if (strchr(";(){}[],.", ch)) {
            delim[0] = (char)ch; delim[1] = '\0';
            storeToken(DELIMITER, delim);
            continue;
        }

        /* Step 7: unknown */
        unk[0] = (char)ch; unk[1] = '\0';
        storeToken(UNKNOWN, unk);
    }
}

/* ----- Display functions ----- */

void displayTokens() {
    int i;
    printf("\n+------+----------------+-----------------------------------+\n");
    printf("| No.  | Token Type     | Value                             |\n");
    printf("+------+----------------+-----------------------------------+\n");
    for (i = 0; i < tokenCount; i++) {
        printf("| %-4d | %s | %-33s |\n",
               i + 1,
               typeName(tokenTable[i].type),
               tokenTable[i].value);
    }
    printf("+------+----------------+-----------------------------------+\n");
    printf("  Total tokens stored: %d\n", tokenCount);
}

void displaySummary() {
    int i;
    int counts[7] = {0};
    for (i = 0; i < tokenCount; i++) counts[tokenTable[i].type]++;

    printf("\n+-----------------------------+-------+\n");
    printf("|         Token Summary       | Count |\n");
    printf("+-----------------------------+-------+\n");
    printf("| Keywords                    | %-5d |\n", counts[KEYWORD]);
    printf("| Identifiers                 | %-5d |\n", counts[IDENTIFIER]);
    printf("| Numbers                     | %-5d |\n", counts[NUMBER]);
    printf("| Operators                   | %-5d |\n", counts[OPERATOR]);
    printf("| Delimiters                  | %-5d |\n", counts[DELIMITER]);
    printf("| String Literals             | %-5d |\n", counts[STRING_LITERAL]);
    printf("| Unknown                     | %-5d |\n", counts[UNKNOWN]);
    printf("+-----------------------------+-------+\n");
    printf("| TOTAL                       | %-5d |\n", tokenCount);
    printf("+-----------------------------+-------+\n\n");
}

/* ----- main ----- */

int main() {
    FILE *fp = fopen("File.txt", "r");
    if (!fp) {
        fprintf(stderr, "Error: Could not open File.txt\n");
        return 1;
    }

    printf("========================================\n");
    printf("  Lexical Analyzer -- reading File.txt  \n");
    printf("========================================\n");

    tokenize(fp);   /* Step 1-7: scan and store */
    fclose(fp);

    displayTokens();    /* show token table */
    displaySummary();   /* show counts by type */

    return 0;
}
