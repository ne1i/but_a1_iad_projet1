#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <conio.h>
#include "app.h"

#pragma warning(disable : 4996)

int current_nb_of_students = 0;
Student global_student_list[MAX_NB_STUDENTS];

// void wait_for_user_input(void)
// {
//     while (1)
//         if (kbhit())
//             break;
// }

int main(void)
{
    char current_command_line[MAX_COMMAND_LENGTH];

    // Handle user input until the user enters 'exit'
    while (1)
    {
        puts("Type in your command:");
        char *result = fgets(current_command_line, MAX_COMMAND_LENGTH, stdin);
        if (result == 0)
        {
            puts("Nothing to read. Press any key to exit...");
            // wait_for_user_input();
            exit(0);
        }
        handle_command(current_command_line);
    }
}

// NOTE(Valentin): Naming conventions
// camelCase -> starts with lowercase and separates each word with a capital letter
// PascalCase -> starts with uppercase and separates each word with a capital letter
// snake_case -> separates each word with an underscore
// kebab-case -> separates each word with a hyphen

// EXAMPLE: "inscription Lea 101"
// command = inscription
// arguments = Lea, 101
void handle_command(char *command_line)
{
    // cleanup command by replacing trailing newline (\n) with string terminator (\0)
    command_line[strcspn(command_line, "\n")] = 0;
    // parse command and get sublist of arguments
    ParsedCommand parsed_command;
    parse_command(command_line, &parsed_command);
    debug_print(&parsed_command);

    switch (parsed_command.command_type)
    {
    case COMMAND_INSCRIPTION:
        puts("INSCRIPTION");
        student_sign_up(global_student_list, &parsed_command, &current_nb_of_students);
        break;
    case COMMAND_ABSENCE:
        puts("ABSENCE");
        break;
    case COMMAND_ETUDIANTS:
        puts("ETUDIANTS");
        break;
    case COMMAND_JUSTIFICATIF:
        puts("JUSTIFICATIF");
        break;
    case COMMAND_VALIDATIONS:
        puts("VALIDATIONS");
        break;
    case COMMAND_VALIDATION:
        puts("VALIDATION");
        break;
    case COMMAND_ETUDIANT:
        puts("ETUDIANT");
        break;
    case COMMAND_DEFAILLANTS:
        puts("DEFAILLANTS");
        break;
    case COMMAND_UNKNOWN:
        puts("Unknown command");
        break;
    case COMMAND_EXIT:
        puts("Exiting...\n");
        exit(0);
    }
}

// IN: command_line
// OUT: command enum value, arguments list, and arguments count
// In this function we transform the command line into a command enum value and a list of arguments
// EXAMPLE : "inscription Lea 101\0" -> COMMAND_INSCRIPTION, ["Lea", "101"], 2
void parse_command(char *command_line, ParsedCommand *parsed_command)
{
    const char *separator = " ";
    char *word = strtok(command_line, separator);

    if (word == NULL)
    {
        parsed_command->command_type = COMMAND_UNKNOWN;
        puts("Error: empty command");
        return;
    }

    if (strcmp(word, "exit") == 0)
        // -> instead of . because parsed_command is a ParsedCommand*
        parsed_command->command_type = COMMAND_EXIT;
    else if (strcmp(word, "inscription") == 0)
        parsed_command->command_type = COMMAND_INSCRIPTION;
    else if (strcmp(word, "absence") == 0)
        parsed_command->command_type = COMMAND_ABSENCE;
    else if (strcmp(word, "etudiants") == 0)
        parsed_command->command_type = COMMAND_ETUDIANTS;
    else if (strcmp(word, "justificatif") == 0)
        parsed_command->command_type = COMMAND_JUSTIFICATIF;
    else if (strcmp(word, "validations") == 0)
        parsed_command->command_type = COMMAND_VALIDATIONS;
    else if (strcmp(word, "validation") == 0)
        parsed_command->command_type = COMMAND_VALIDATION;
    else if (strcmp(word, "etudiant") == 0)
        parsed_command->command_type = COMMAND_ETUDIANT;
    else if (strcmp(word, "defaillants") == 0)
        parsed_command->command_type = COMMAND_DEFAILLANTS;
    else
        parsed_command->command_type = COMMAND_UNKNOWN;

    int count = 0;
    while ((word = strtok(NULL, separator)) != NULL && count < MAX_ARGUMENTS_COUNT)
    {
        parsed_command->arguments_list[count] = word;
        count++;
    };
    parsed_command->arguments_count = count;
    // int memory_to_allocate = count * sizeof(char *);
    // char *a = malloc(memory_to_allocate);

    // Stupid test that made me lose 1 hour of my life
    // printf("command: %d\nargument_1 %s\nargument_2 %s\n", *command, arguments_list[0], arguments_list[1]);
    // NOTE(Valentin): never use printf to test your code, use breakpoints !
}

// Signs a student up with his name and his group number

void student_sign_up(Student *student_list, ParsedCommand* parsed_command, int* current_nb_of_students)
{
    if (parsed_command->arguments_count < SIGN_UP_PARAMETERS_COUNT)
    {
        puts("Not enough parameters were given (2 were expected)");
        return;
    }
    if (parsed_command->arguments_count > SIGN_UP_PARAMETERS_COUNT)
    {
        puts("Too many parameters were given (2 were expected)");
        return;
    }
    Student student;
    strcpy(student.Name, parsed_command->arguments_list[0]);
    student.Group = atoi(parsed_command->arguments_list[1]);
    student.Student_ID = ++(*current_nb_of_students);

    student_list[*current_nb_of_students - 1] = student;
    printf("Sign up completed. (%d)\n", student.Student_ID);
}

// Adds an absence to the student with the student_id Idé
void absence(int student_id, HalfDay half_day, Student* global_student_list);