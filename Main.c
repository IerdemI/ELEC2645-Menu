#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "funcs.h"

// Menu handling function prototypes:
static void main_menu(void);
static void print_main_menu(void);
static int  get_user_input(void);
static void select_menu_item(int input);
static void go_back_to_main(void);
static int  is_integer(const char *s);

int main(void)
{
    // Runs forever until Exit is selected
    for(;;) {
        main_menu();
    }
    return 0;
}

static void main_menu(void)
{
    print_main_menu();
    int input = get_user_input();
    select_menu_item(input);
}

static int get_user_input(void)
{
    enum { MENU_ITEMS = 6 }; // 1–4 items, 5 = exit
    char buf[128];
    int valid = 0, value = 0;

    do {
        printf("\nSelect item: ");
        if (!fgets(buf, sizeof(buf), stdin)) {
            puts("Input error. Exiting.");
            exit(1);
        }

        buf[strcspn(buf, "\r\n")] = '\0'; // strip newline

        if (!is_integer(buf)) {
            printf("Enter an integer!\n");
        } else {
            value = (int)strtol(buf, NULL, 10);
            if (value >= 1 && value <= MENU_ITEMS) {
                valid = 1;
            } else {
                printf("Invalid menu option.\n");
            }
        }
    } while (!valid);

    return value;
}

static void select_menu_item(int input)
{
    switch (input) {
        case 1:
            menu_item_1();      // Ohms Law Calculator
            go_back_to_main();
            break;
        case 2:
            menu_item_2(); // Voltage Divider Calculator    
            go_back_to_main();
            break;
        case 3:
            menu_item_3();      
            go_back_to_main();
            break;
        case 4:
            menu_item_4();      
            go_back_to_main();
            break;
        case 5:
        default:
            printf("Bye!\n");
            exit(0);
        case 6:
            menu_item_6();      
            go_back_to_main();
            break;

    }
}

static void print_main_menu(void)
{
    printf("\n----------- Electronics Helper -----------\n");
    printf("\n"
           "\t1. Ohm's Law Calculator\n"
           "\t2. Voltage Divider Calculator\n"
           "\t3. Signal Generator\n"
           "\t4. Signal Analyzer\n"
           "\t5. Exit\n"
           "\t6. Menu item $@£!\n");
    printf("------------------------------------------\n");
}

static void go_back_to_main(void)
{
    // No extra key press needed anymore.
    // I already handled "back" inside Ohm menu item.
    printf("\nReturning to main menu\n");
}


static int is_integer(const char *s)
{
    if (!s || !*s) return 0;

    if (*s == '+' || *s == '-') s++;

    if (!isdigit((unsigned char)*s)) return 0;

    while (*s) {
        if (!isdigit((unsigned char)*s)) return 0;
        s++;
    }
    return 1;
}