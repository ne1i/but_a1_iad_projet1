#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"
#pragma warning(disable : 4996)

int main(void)
{
    char current_command_line[MAX_COMMAND_LENGTH];

    // Handle user input until the user enters 'exit'
    while (1)
    {
        char *result = fgets(current_command_line, MAX_COMMAND_LENGTH, stdin);
        if (result == 0)
        {
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

    switch (parsed_command.command_type)
    {
    case COMMAND_INSCRIPTION:
        inscription(global_student_list, parsed_command, &current_nb_of_students);
        break;
    case COMMAND_ABSENCE:
        GestionAbsence absence_args;
        absence_args.student_id = atoi(parsed_command.arguments_list[0]);
        absence_args.date = atoi(parsed_command.arguments_list[1]);
        strcpy(absence_args.am_pm, parsed_command.arguments_list[2]);
        absence(parsed_command, absence_args);
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
    // Stupid test that made me lose 1 hour of my life
    // printf("command: %d\nargument_1 %s\nargument_2 %s\n", *command, arguments_list[0], arguments_list[1]);
    // NOTE(Valentin): never use printf to test your code, use breakpoints !
}

// Signs a student up with his name and his group number

void inscription(Student *student_list, const ParsedCommand parsed_command, int* current_nb_of_students)
{
    for (int i = 0; i < *current_nb_of_students; ++i)
    {
        if ((((strcmp(parsed_command.arguments_list[0], student_list[i].Name)) == 0) &&
         (atoi(parsed_command.arguments_list[1]) == student_list[i].Group)))
        {
            puts("Nom incorrect");
            return;
        }
    }
    if (parsed_command.arguments_count != SIGN_UP_PARAMETERS_COUNT)
        return;

    Student student;
    strcpy(student.Name, parsed_command.arguments_list[0]);
    student.Group = atoi(parsed_command.arguments_list[1]);
    student.Student_ID = ++(*current_nb_of_students);
    student.NB_absence = 0;
    student_list[*current_nb_of_students - 1] = student;
    
    printf("Inscription enregistree (%d)\n", student.Student_ID);
}

// Adds an absence to the student with the student_id
void absence(const ParsedCommand parsed_command, GestionAbsence absence_args)
{
    if (parsed_command.arguments_count != SIGN_UP_PARAMETERS_COUNT)
        return;

    if (absence_args.student_id > current_nb_of_students)
    {
        puts("Identifiant incorrect");
        return;
    }

    if ((absence_args.date < 1) || (absence_args.date > 40))
    {
        puts("Date incorrecte");
        return;
    }

    if ((strcmp(absence_args.am_pm, "am") != 0) && (strcmp(absence_args.am_pm, "pm") != 0))
    {
        puts("Demi-journée incorrecte");
        return;
    }


}