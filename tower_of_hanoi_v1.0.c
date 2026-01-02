/* Tower of Hanoi - Console Version
   VERSION 2.0
   Author: Abhinav Dwivedi
   Notes:
   - Portable C (no Windows-specific console calls)
   - Interactive: move from rod A/B/C to A/B/C
   - Validates moves and detects win
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_N 6     /* Max number of disks (adjustable) */
#define RODS 3

/* Rods: each rod has a stack of disks. Smaller numbers = smaller disks.
   Top index points to the next free slot (stack semantics). */
typedef struct {
    int disks[MAX_N];
    int top; /* number of items on rod (index of next push) */
} Rod;

static Rod rods[RODS];
static int N = 4; /* default disk count; you can change at runtime */

/* Function declarations */
void init_game(int n);
void print_state(void);
int rod_index_from_char(char c);
int can_move(int from, int to);
int move_disk(int from, int to);
int is_won(void);
void instructions(void);

int main(void) {
    int running = 1;

    while (running) {
        system("cls"); /* On Linux/macOS, replace with: system("clear"); */
        printf("=== Tower of Hanoi ===\n");
        printf("1. Play\n");
        printf("2. Instructions\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");

        int choice = 0;
        if (scanf("%d", &choice) != 1) {
            /* Clean input on failure */
            int ch;
            while ((ch = getchar()) != EOF && ch != '\n');
            continue;
        }

        if (choice == 1) {
            /* Select number of disks */
            system("cls");
            printf("Enter number of disks (3 to %d): ", MAX_N);
            if (scanf("%d", &N) != 1 || N < 3 || N > MAX_N) {
                printf("Invalid. Using %d disks.\n", N = 4);
            }

            init_game(N);

            /* Game loop */
            while (1) {
                system("cls");
                print_state();
                printf("\nMove format: from(to) using letters A/B/C (e.g., A C)\n");
                printf("Enter move (or Q to quit to menu): ");

                /* Read move */
                char fromC = 0, toC = 0;
                int read = scanf(" %c", &fromC);
                if (read != 1) {
                    int ch;
                    while ((ch = getchar()) != EOF && ch != '\n');
                    continue;
                }

                if (toupper((unsigned char)fromC) == 'Q') {
                    break;
                }

                /* Expect second token */
                read = scanf(" %c", &toC);
                if (read != 1) {
                    int ch;
                    while ((ch = getchar()) != EOF && ch != '\n');
                    continue;
                }

                int from = rod_index_from_char(fromC);
                int to   = rod_index_from_char(toC);

                if (from < 0 || to < 0) {
                    printf("Invalid rods. Use A, B, or C.\n");
                    printf("Press Enter to continue..."); getchar(); getchar();
                    continue;
                }

                if (!can_move(from, to)) {
                    printf("Illegal move: cannot place larger disk on smaller one.\n");
                    printf("Press Enter to continue..."); getchar(); getchar();
                    continue;
                }

                if (!move_disk(from, to)) {
                    printf("Move failed (empty source?).\n");
                    printf("Press Enter to continue..."); getchar(); getchar();
                    continue;
                }

                if (is_won()) {
                    system("cls");
                    print_state();
                    printf("\nYou win! All disks moved to rod C.\n");
                    printf("Press Enter to return to menu...");
                    getchar(); getchar();
                    break;
                }
            }

        } else if (choice == 2) {
            system("cls");
            instructions();
            printf("\nPress Enter to return to menu...");
            getchar(); getchar();

        } else if (choice == 3) {
            running = 0;

        } else {
            printf("Invalid choice.\n");
        }
    }

    return 0;
}

/* Initialize the rods: all disks on rod A (index 0), smallest on top */
void init_game(int n) {
    for (int r = 0; r < RODS; r++) {
        rods[r].top = 0;
        for (int i = 0; i < MAX_N; i++) {
            rods[r].disks[i] = 0;
        }
    }
    /* Fill rod A with disks n..1 (largest at bottom, smallest at top) */
    for (int d = n; d >= 1; d--) {
        rods[0].disks[rods[0].top++] = d;
    }
}

/* Render the rods and disks */
void print_state(void) {
    printf("Rods:\n");
    for (int r = 0; r < RODS; r++) {
        char name = 'A' + r;
        printf("%c: ", name);
        if (rods[r].top == 0) {
            printf("-");
        } else {
            /* Print from bottom to top */
            for (int i = 0; i < rods[r].top; i++) {
                int d = rods[r].disks[i];
                printf("%d", d);
                if (i < rods[r].top - 1) printf(",");
            }
        }
        printf("\n");
    }
}

/* Map A/B/C (case-insensitive) to 0/1/2 */
int rod_index_from_char(char c) {
    c = toupper((unsigned char)c);
    if (c == 'A') return 0;
    if (c == 'B') return 1;
    if (c == 'C') return 2;
    return -1;
}

/* Check if moving top disk from 'from' to 'to' is valid */
int can_move(int from, int to) {
    if (from < 0 || from >= RODS || to < 0 || to >= RODS) return 0;
    if (rods[from].top == 0) return 0; /* empty source */

    int moving = rods[from].disks[rods[from].top - 1];
    if (rods[to].top == 0) return 1; /* empty target OK */

    int targetTop = rods[to].disks[rods[to].top - 1];
    return moving < targetTop; /* smaller onto larger */
}

/* Execute the move */
int move_disk(int from, int to) {
    if (rods[from].top == 0) return 0;
    int d = rods[from].disks[--rods[from].top];
    rods[to].disks[rods[to].top++] = d;
    return 1;
}

/* Win when all disks moved to rod C (index 2) */
int is_won(void) {
    if (rods[2].top != N) return 0;
    /* Verify order: bottom largest to top smallest */
    for (int i = 0; i < N - 1; i++) {
        if (rods[2].disks[i] <= rods[2].disks[i + 1]) {
            /* disks stored bottom->top increasing? We pushed smallest last,
               so bottom should be largest, ascending upward. Check ascending. */
            continue;
        } else {
            return 0;
        }
    }
    return 1;
}

/* Show instructions */
void instructions(void) {
    printf("Instructions:\n");
    printf("- Move all disks from rod A to rod C.\n");
    printf("- You can move only the top disk of a rod.\n");
    printf("- A larger disk cannot be placed on a smaller disk.\n");
    printf("- Input moves as two letters: source and destination (e.g., A C).\n");
    printf("- Press Q during a move prompt to return to the menu.\n");
}





















































































