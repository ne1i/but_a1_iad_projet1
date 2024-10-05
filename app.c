#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS_LENGTH 30
#define MAX_NAME_LENGHT 30
#define MAX_NB_STUDENTS 100

void handle_command(const char *command);

enum COMMAND
{
    // "exit" -> COMMAND_EXIT
    COMMAND_UNKNOWN = -1,
    COMMAND_EXIT = 0,
    COMMAND_INSCRIPTION,
    COMMAND_ABSENCE,
    COMMAND_ETUDIANTS,
    COMMAND_JUSTIFICATIF,
    COMMAND_VALIDATIONS,
    COMMAND_VALIDATION,
    COMMAND_ETUDIANT,
    COMMAND_DEFAILLANTS
};

typedef struct
{
    char Name[30];
    short NB_absence;
    short Student_ID;
    short Group;
} Student;

int current_nb_of_students = 0;
Student student_list[MAX_NB_STUDENTS];

int main(void)
{
    char current_command_line[MAX_COMMAND_LENGTH];

    // Handle user input until the user enters 'exit'
    while (1)
    {
        puts("\nEnter your commands\n");
        fgets(current_command_line, MAX_COMMAND_LENGTH, stdin);
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
// arguments = Lea 101
void handle_command(const char *command_line)
{
    // parse command and get sublist of arguments
    enum COMMAND command;
    char *arguments_list[MAX_ARGUMENTS_LENGTH];
    int arguments_count = 0;

    parse_command(command_line, &command, arguments_list, &arguments_count);
    switch (command)
    {
    case COMMAND_INSCRIPTION:
        break;
    case COMMAND_ABSENCE:
        break;
    case COMMAND_ETUDIANTS:
        break;
    case COMMAND_JUSTIFICATIF:
        break;
    case COMMAND_VALIDATIONS:
        break;
    case COMMAND_VALIDATION:
        break;
    case COMMAND_ETUDIANT:
        break;
    case COMMAND_DEFAILLANTS:
        break;
    case COMMAND_UNKNOWN:
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
void parse_command(const char *command_line, enum COMMAND *command, char **arguments_list, int *arguments_count)
{
    char *separator = " ";
    char *word = strtok(command_line, separator);
    if (arguments_count == 0)
    {
        if (word == "exit")
            *command = COMMAND_EXIT;
        if (word == "inscription")
            *command = COMMAND_INSCRIPTION;
        if (word == "abscence")
            *command = COMMAND_ABSENCE;
        if (word == "etudiants")
            *command = COMMAND_ETUDIANTS;
        if (word == "justificatif")
            *command = COMMAND_JUSTIFICATIF;
        if (word == "validations")
            *command = COMMAND_VALIDATIONS;
        if (word == "validation")
            *command = COMMAND_VALIDATION;
        if (word == "etudiant")
            *command = COMMAND_ETUDIANT;
        if (word == "defaillants")
            *command = COMMAND_DEFAILLANTS;
        else
            *command = COMMAND_UNKNOWN;
    }

    else
    {
        for (int i = 0; word != NULL; ++i)
        {
            word = strtok(NULL, separator);
            arguments_list[*arguments_count] = word;
            *arguments_count += 1;
        }
    }
    // Stupid test that made me lose 1 hour of my life
    // printf("command: %d\nargument_1 %s\nargument_2 %s\n", *command, arguments_list[0], arguments_list[1]);
}

// Signs a student up with his name and his group number
void student_sign_up(char **student_list, char Name[MAX_NAME_LENGHT], short group)
{
}
