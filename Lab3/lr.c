#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PROD   20
#define MAX_LEN    100

int main() {
    char A[10];
    int  n;
    char productions[MAX_PROD][MAX_LEN];
    char alpha[MAX_PROD][MAX_LEN];       
    char beta[MAX_PROD][MAX_LEN];    

    int  alpha_count = 0;
    int  beta_count  = 0;
    int  alen;

    printf("=== Immediate Left Recursion Remover ===\n\n");

    printf("Enter the non-terminal symbol (e.g. E): ");
    scanf("%s", A);
    alen = strlen(A);

    printf("Enter the number of productions for %s: ", A);
    scanf("%d", &n);

    if (n <= 0 || n > MAX_PROD) {
        printf("Error: number of productions must be between 1 and %d.\n", MAX_PROD);
        return 1;
    }

    printf("Enter the right-hand side of each production for %s:\n", A);
    for (int i = 0; i < n; i++) {
        printf("  Production %d: ", i + 1);
        scanf("%s", productions[i]);
    }

    printf("\n--- Classification ---\n");
    for (int i = 0; i < n; i++) {
        int starts_with_A  = (strncmp(productions[i], A, alen) == 0);
        int is_A_prime     = (productions[i][alen] == '\'');

        if (starts_with_A && !is_A_prime) {
            strcpy(alpha[alpha_count++], productions[i] + alen);
            printf("  %s -> %s   [Left-recursive]\n", A, productions[i]);
        } else {
            strcpy(beta[beta_count++], productions[i]);
            printf("  %s -> %s   [Non-recursive ]\n", A, productions[i]);
        }
    }

    if (alpha_count == 0) {
        printf("\nNo immediate left recursion found.\n");
        printf("Original grammar is suitable for top-down parsing.\n\n");
        printf("--- Original Grammar ---\n");
        for (int i = 0; i < n; i++) {
            printf("  %s -> %s\n", A, productions[i]);
        }
        return 0;
    }

    if (beta_count == 0) {
        printf("\nUnable to execute:\n");
        printf("  All productions are left-recursive — no beta exists.\n");
        printf("  Left recursion cannot be eliminated.\n");
        return 0;
    }

    char Ap[12];
    snprintf(Ap, sizeof(Ap), "%s'", A);   
    printf("\n--- Transformation Steps ---\n");

    printf("  alpha (left-recursive suffixes) : ");
    for (int i = 0; i < alpha_count; i++)
        printf("%s%s", alpha[i], i < alpha_count - 1 ? ", " : "\n");

    printf("  beta  (non-recursive bodies)    : ");
    for (int i = 0; i < beta_count; i++)
        printf("%s%s", beta[i], i < beta_count - 1 ? ", " : "\n");

    printf("  Introducing new non-terminal    : %s\n", Ap);

    printf("\n--- Transformed Grammar ---\n");

    printf("  %s  -> ", A);
    for (int i = 0; i < beta_count; i++) {
        printf("%s%s", beta[i], Ap);
        if (i < beta_count - 1) printf(" | ");
    }
    printf("\n");

    printf("  %s -> ", Ap);
    for (int i = 0; i < alpha_count; i++) {
        printf("%s%s | ", alpha[i], Ap);
    }
    printf("e\n");   
    return 0;
}
