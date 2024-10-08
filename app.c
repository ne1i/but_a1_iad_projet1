#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"
#pragma warning(disable : 4996)

int main(void)
{
    char current_command_line[MAX_COMMAND_LENGTH];

    // Handle user input until the user enters 'exit'
    while (fgets(current_command_line, MAX_COMMAND_LENGTH, stdin) != 0)
    {
        handle_command(current_command_line);
    }
    return 0;
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
        handle_inscription(parsed_command);
        break;
    case COMMAND_ABSENCE:
        handle_absence(parsed_command);
        break;
    case COMMAND_ETUDIANTS:
        handle_etudiants(parsed_command);
        break;
    case COMMAND_JUSTIFICATIF:
        handle_justificatif(parsed_command);
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
    // creating a copy of command_line to use for justificatif
    // char *intact_command_line = NULL;
    // strcpy(intact_command_line, command_line);

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
    {
        parse_command_justificatif(parsed_command);
        return;
    }
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

void parse_command_justificatif(ParsedCommand *parsed_command)
{
    const char *space_separator = " ";
    const char *newline_separator = "\n";

    parsed_command->command_type = COMMAND_JUSTIFICATIF;
    parsed_command->arguments_list[0] = strtok(NULL, space_separator);
    parsed_command->arguments_list[1] = strtok(NULL, space_separator);
    parsed_command->arguments_list[2] = strtok(NULL, newline_separator);
}

// Signs a student up with his name and his group number
void handle_inscription(const ParsedCommand parsed_command)
{
    if (parsed_command.arguments_count < INSCRIPTION_ARGS_COUNT)
        return;

    for (int i = 0; i < student_id_counter; ++i)
    {
        if ((((strcmp(parsed_command.arguments_list[0], global_student_list[i].name)) == 0) &&
             (atoi(parsed_command.arguments_list[1]) == global_student_list[i].group)))
        {
            puts("Nom incorrect");
            return;
        }
    }

    Student student;
    strcpy(student.name, parsed_command.arguments_list[0]);
    student.group = atoi(parsed_command.arguments_list[1]);
    student.student_id = ++student_id_counter;
    student.nb_absence = 0;
    global_student_list[student_id_counter - 1] = student;

    printf("Inscription enregistree (%d)\n", student.student_id);
}

// Adds an absence to the student with the student_id
void handle_absence(const ParsedCommand parsed_command)
{
    if (parsed_command.arguments_count < ABSENCE_ARGS_COUNT)
        return;

    int student_id = atoi(parsed_command.arguments_list[0]);
    if (student_id > student_id_counter || student_id == 0)
    {
        puts("Identifiant incorrect");
        return;
    }

    Student *student = &global_student_list[student_id - 1];
    Absence *absence = &student->absences[student->nb_absence];
    absence->date = atoi(parsed_command.arguments_list[1]);
    if ((absence->date < MIN_ABSENCE_DAY) || (absence->date > MAX_ASBENCE_DAY))
    {
        puts("Date incorrecte");
        return;
    }

    strcpy(absence->am_pm, parsed_command.arguments_list[2]);

    if ((strcmp(absence->am_pm, "am") != 0) && (strcmp(absence->am_pm, "pm") != 0))
    {
        puts("Demi-journee incorrecte");
        return;
    }

    for (int i = 0; i < student->nb_absence; ++i)
    {
        Absence *curr = &student->absences[i];
        if (absence->date == curr->date && strcmp(absence->am_pm, curr->am_pm) == 0)
        {
            puts("Absence deja connue");
            return;
        }
    }
    student->nb_absence++;
    absence->id_absence = ++absence_id_counter;
    absence->justified = ABSENCE_WAITING_JUSTIFICATION;
    printf("Absence enregistree [%d]\n", absence->id_absence);
}

void handle_etudiants(ParsedCommand parsed_command)
{
    if (parsed_command.arguments_count < ETUDIANTS_ARGS_COUNT)
        return;

    int current_day = atoi(parsed_command.arguments_list[0]);
    if (current_day < MIN_DAY)
    {
        puts("Date incorrecte");
        return;
    }

    // creating a temporary copy of global_student_list that we will sort by group
    Student *sorted_student_array = NULL;
    int mem = sizeof(Student) * student_id_counter;
    sorted_student_array = (Student *)malloc(mem);
    memcpy(sorted_student_array, global_student_list, mem);
    qsort(sorted_student_array, student_id_counter, sizeof(Student), compare_group);

    for (int i = 0; i < student_id_counter; ++i)
    {
        int total_absences = get_absence_count_before(&sorted_student_array[i], current_day);
        printf("(%d) %-13s %2d %d\n", sorted_student_array[i].student_id,
               sorted_student_array[i].name,
               sorted_student_array[i].group,
               total_absences);
    }
    free(sorted_student_array);
}

int compare_group(const void *a, const void *b)
{
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    if (s1->group < s2->group)
        return -1;
    if (s1->group > s2->group)
        return 1;
    return strcmp(s1->name, s2->name);
}

int get_absence_count_before(const Student *student, int max_day)
{
    int total_absences = 0;
    for (int j = 0; j < student->nb_absence; ++j)
    {
        if (student->absences[j].date <= max_day)
            ++total_absences;
    }
    return total_absences;
}

void handle_justificatif(ParsedCommand parsed_command)
{
    // check if absence exists, and get the id of the corresponding student
    int absence_id = atoi(parsed_command.arguments_list[0]);
    int student_id = check_absence_exists(absence_id);
    if (student_id == -1)
    {
        puts("Identifiant incorrect");
        return;
    }

    // check if the date is correct
    if (atoi(parsed_command.arguments_list[1]) < global_student_list[student_id - 1].absences[absence_id - 1].date)
    {
        puts("Date incorrecte");
        return;
    }

    // check if the justification is given in time (3 days max)
    Student student = global_student_list[student_id - 1];
    if ((atoi(parsed_command.arguments_list[1]) - student.absences[absence_id - 1].date) > 3)
    {
        student.absences[absence_id - 1].justified = ABSENCE_NOT_JUSTIFIED;
        return;
    }
    if (strlen(parsed_command.arguments_list[2]) > MAX_JUSTIFICATION_LENGTH)
        return;

    if (strcmp(student.absences[absence_id - 1].justification, parsed_command.arguments_list[2]) == 0)
    {
        puts("Justificatif deja connu");
        return;
    }

    strcpy(student.absences[absence_id - 1].justification, parsed_command.arguments_list[2]);
    student.absences[absence_id - 1].justified = ABSENCE_JUSTIFIED;
    puts("Justificatif enregistre");
}

int check_absence_exists(int absence_id)
{
    int student_id = -1;
    for (int i = 0; i < student_id_counter; ++i)
    {
        for (int j = 0; j < global_student_list[i].nb_absence; ++j)
        {
            if (global_student_list[i].absences[j].id_absence == absence_id)
            {
                student_id = global_student_list[i].student_id;
                break;
            }
        }
    }
    return student_id;
}