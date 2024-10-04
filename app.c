#include <stdio.h>
#include <stdlib.h>
#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS_LENGTH 256
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

typedef struct {
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
    char *arguments[MAX_ARGUMENTS_LENGTH];
    int arguments_count;

    parse_command(command_line, &command, arguments, &arguments_count);
    switch (command)
    {
    case COMMAND_INSCRIPTION:
        inscription();
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
    default:
    }
}
// IN: command_line
// OUT: command enum value, arguments list, and arguments count
// In this function we transform the command line into a command enum value and a list of arguments
// EXAMPLE : "inscription Lea 101\0" -> COMMAND_INSCRIPTION, ["Lea", "101"], 2
void parse_command(const char *command_line, enum COMMAND *command, char **arguments, int *arguments_count)
{


    // split command line over space character into list of strings
    // strtok can be used to do this

    // then, check the first element (which is the command) and compare it's value with all the possible commands
    // if it's a valid command, then set the command enum value
    // else, set the command enum value to COMMAND_UNKNOWN

    // NOTE(Valentin): ill need to use <string.h> functions to do this
}

// Signs a student up with his name and his group number
void student_sign_up(char **student_list, char Name[MAX_NAME_LENGHT], short group) {
    Student* pstudent_list = &student_list;
    Student new_student = &(pstudent_list + sizeof(Student) * current_nb_of_students);

}

// Skibidi test