#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"
#pragma warning(disable : 4996)

int main(void)
{
    int student_id_counter = 0;
    int absence_id_counter = 0;
    Student student_list[MAX_NB_STUDENTS];

    char current_command_line[MAX_COMMAND_LENGTH];

    // Handle user input until the user enters 'exit'
    while (fgets(current_command_line, MAX_COMMAND_LENGTH, stdin) != 0)
    {
        handle_command(current_command_line, &student_id_counter, &absence_id_counter, student_list);
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
void handle_command(char *command_line, int *nb_students, int *nb_absence, Student *student_list)
{
    // cleanup command by replacing trailing newline (\n) with string terminator (\0)
    command_line[strcspn(command_line, "\n")] = 0;
    // parse command and get sublist of arguments
    ParsedCommand parsed_command;
    parse_command(command_line, &parsed_command);

    switch (parsed_command.command_type)
    {
    case COMMAND_INSCRIPTION:
        handle_inscription(parsed_command, nb_students, student_list);
        break;
    case COMMAND_ABSENCE:
        handle_absence(parsed_command, nb_students, nb_absence, student_list);
        break;
    case COMMAND_ETUDIANTS:
        handle_etudiants(parsed_command, *nb_students, student_list);
        break;
    case COMMAND_JUSTIFICATIF:
        handle_justificatif(parsed_command, nb_students, student_list);
        break;
    case COMMAND_VALIDATIONS:
        handle_validations(nb_students, nb_absence, student_list);
        break;
    case COMMAND_VALIDATION:
        handle_validation(parsed_command, *nb_students, *nb_absence, student_list);
        break;
    case COMMAND_ETUDIANT:
        handle_etudiant(parsed_command, *nb_students, student_list);
        break;
    case COMMAND_DEFAILLANTS:
        handle_defaillants(parsed_command, *nb_students, student_list);
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
void handle_inscription(const ParsedCommand parsed_command, int *nb_students, Student *student_list)
{
    if (parsed_command.arguments_count < INSCRIPTION_ARGS_COUNT)
        return;

    for (int i = 0; i < *nb_students; ++i)
    {
        if ((((strcmp(parsed_command.arguments_list[0], student_list[i].name)) == 0) &&
             (atoi(parsed_command.arguments_list[1]) == student_list[i].group)))
        {
            puts("Nom incorrect");
            return;
        }
    }

    Student student;
    strcpy(student.name, parsed_command.arguments_list[0]);
    student.group = atoi(parsed_command.arguments_list[1]);
    student.student_id = ++*(nb_students);
    student.nb_absence = 0;
    student_list[*nb_students - 1] = student;

    printf("Inscription enregistree (%d)\n", student.student_id);
}

// Adds an absence to the student with the student_id
void handle_absence(const ParsedCommand parsed_command, int *nb_students, int *nb_absences, Student *student_list)
{
    if (parsed_command.arguments_count < ABSENCE_ARGS_COUNT)
        return;

    int student_id = atoi(parsed_command.arguments_list[0]);
    if (student_id > *nb_students || student_id == 0)
    {
        puts("Identifiant incorrect");
        return;
    }

    Student *student = &student_list[student_id - 1];
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
    absence->id_absence = ++*(nb_absences);
    absence->justified = ABSENCE_WAITING_JUSTIFICATION;
    absence->date = atoi(parsed_command.arguments_list[1]);
    absence->student_id = atoi(parsed_command.arguments_list[0]);
    strcpy(absence->justification, "");
    printf("Absence enregistree [%d]\n", absence->id_absence);
}

void handle_etudiants(ParsedCommand parsed_command, int nb_students, Student *student_list)
{
    if (parsed_command.arguments_count < ETUDIANTS_ARGS_COUNT)
        return;

    int current_day = atoi(parsed_command.arguments_list[0]);
    if (current_day < MIN_DAY)
    {
        puts("Date incorrecte");
        return;
    }

    // creating a temporary copy of student_list that we will sort by group
    Student *sorted_student_array = NULL;
    int mem = sizeof(Student) * nb_students;
    sorted_student_array = (Student *)malloc(mem);
    memcpy(sorted_student_array, student_list, mem);
    qsort(sorted_student_array, nb_students, sizeof(Student), compare_group);

    for (int i = 0; i < nb_students; ++i)
    {
        int total_absences = get_absence_count_before(&sorted_student_array[i], current_day);
        printf("(%d) %-12s %2d %d\n", sorted_student_array[i].student_id,
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

void handle_justificatif(ParsedCommand parsed_command, int *nb_students, Student *student_list)
{

    // check if absence exists, and get the id of the corresponding student
    int absence_id = atoi(parsed_command.arguments_list[0]);
    int student_id = check_absence_exists(absence_id, nb_students, student_list);
    if (student_id == -1)
    {
        puts("Identifiant incorrect");
        return;
    }

    // check if the date is correct
    Student *student = &student_list[student_id - 1];
    int student_absence_index = 0;
    for (int i = 0; i < student->nb_absence; ++i)
    {
        if (student->absences[i].id_absence == absence_id)
            student_absence_index = i;
    }

    if (atoi(parsed_command.arguments_list[1]) < student->absences[student_absence_index].date)
    {
        puts("Date incorrecte");
        return;
    }

    // check if the justification is given in time (3 days max)
    if ((atoi(parsed_command.arguments_list[1]) - student->absences[student_absence_index].date) > 3)
    {
        student->absences[student_absence_index].justified = ABSENCE_NOT_JUSTIFIED;
        {
            puts("Justificatif enregistre");
            strcpy(student->absences[student_absence_index].justification, parsed_command.arguments_list[2]);
            return;
        }
    }
    if (strlen(parsed_command.arguments_list[2]) > MAX_JUSTIFICATION_LENGTH)
    {
        return;
    }

    if (student->absences[student_absence_index].justified != ABSENCE_WAITING_JUSTIFICATION)
    {
        puts("Justificatif deja connu");
        return;
    }

    strcpy(student->absences[student_absence_index].justification, parsed_command.arguments_list[2]);
    student->absences[student_absence_index].justified = ABSENCE_WAITING_VALIDATION;
    puts("Justificatif enregistre");
}

int check_absence_exists(int absence_id, int *nb_students, Student *student_list)
{
    int student_id = -1;
    for (int i = 0; i < *nb_students; ++i)
    {
        for (int j = 0; j < student_list[i].nb_absence; ++j)
        {
            if (student_list[i].absences[j].id_absence == absence_id)
            {
                student_id = student_list[i].student_id;
                break;
            }
        }
    }
    return student_id;
}

void handle_validations(int *nb_students, int *nb_absences, Student *student_list)
{
    if (check_absence_status_exists(ABSENCE_WAITING_VALIDATION, *nb_students, student_list) == -1)
    {
        puts("Aucune validation en attente");
        return;
    }

    int nb_absences_waiting_validation = count_absence_status(ABSENCE_WAITING_VALIDATION, *nb_students, student_list);

    // creating a list with all the absences that we will later sort
    Absence *absences_waiting_validation_list = (Absence *)malloc(nb_absences_waiting_validation * sizeof(Absence));
    int count = 0;
    for (int i = 0; i < *nb_students; ++i)
    {
        for (int j = 0; j < *nb_absences; ++j)
        {
            if (student_list[i].absences[j].justified == ABSENCE_WAITING_VALIDATION)
            {
                memcpy(absences_waiting_validation_list + count, &student_list[i].absences[j], sizeof(Absence));
                ++count;
            }
        }
    }
    // sorting the absences by absence id
    qsort(absences_waiting_validation_list, nb_absences_waiting_validation, sizeof(Absence), compare_student_id);

    for (int i = 0; i < nb_absences_waiting_validation; ++i)
    {
        int absence_id = absences_waiting_validation_list[i].id_absence;
        int student_id = absences_waiting_validation_list[i].student_id;
        char student_name[MAX_NAME_LENGTH];
        strcpy(student_name, student_list[student_id - 1].name);
        int group = student_list[student_id - 1].group;
        int absence_date = absences_waiting_validation_list[i].date;
        char am_pm[3];
        strcpy(am_pm, absences_waiting_validation_list[i].am_pm);
        char justification[MAX_JUSTIFICATION_LENGTH];
        strcpy(justification, absences_waiting_validation_list[i].justification);

        printf("[%d] (%d) %-14s %-1d %d/%s (%s)\n", absence_id, student_id, student_name, group, absence_date, am_pm, justification);
    }
    free(absences_waiting_validation_list);
}

int check_absence_status_exists(enum AbsenceStatus status, int nb_students, Student *student_list)
{
    int exists = -1;
    for (int i = 0; i < nb_students; ++i)
    {
        for (int j = 0; j < student_list[i].nb_absence; ++j)
        {
            if (student_list[i].absences[j].justified == status)
            {
                exists = 1;
                return exists;
            }
        }
    }
    return exists;
}

int count_absence_status(enum AbsenceStatus status, int nb_students, Student *student_list)
{
    int absence_count = 0;
    for (int i = 0; i < nb_students; ++i)
    {
        for (int j = 0; j < student_list[i].nb_absence; ++j)
        {
            if (student_list[i].absences[j].justified == status)
            {
                ++absence_count;
            }
        }
    }
    return absence_count;
}
int compare_student_id(const void *a, const void *b)
{
    const Absence *a1 = (const Absence *)a;
    const Absence *a2 = (const Absence *)b;
    if (a1->student_id < a2->student_id)
        return -1;
    if (a1->student_id > a2->student_id)
        return 1;
    return a1->date - a2->date;
}

void handle_validation(ParsedCommand parsed_command, int nb_students, int nb_absence, Student *student_list)
{
    if (parsed_command.arguments_count != VALIDATION_ARGS_COUNT)
        return;

    int absence_id = atoi(parsed_command.arguments_list[0]);
    if (absence_id > nb_absence || absence_id <= 0)
    {
        puts("Identifiant incorrect");
        return;
    }

    char validation_code[3]; // 3 because "ok\0" or "ko\0"
    strcpy(validation_code, parsed_command.arguments_list[1]);
    if (strcmp(validation_code, "ok") != 0 && strcmp(validation_code, "ko") != 0)
    {
        puts("Code incorrect");
        return;
    }

    int student_id = 0;
    int student_absence_idx = 0;

    for (int i = 0; i < nb_students; ++i)
    {
        for (int j = 0; j < student_list[i].nb_absence; ++j)
        {
            if (student_list[i].absences[j].id_absence == absence_id)
            {
                student_id = student_list[i].absences[j].student_id;
                student_absence_idx = j;
                break;
            }
        }
    }
    Absence *absence = &student_list[student_id - 1].absences[student_absence_idx];
    if (absence->justified == ABSENCE_JUSTIFIED || absence->justified == ABSENCE_NOT_JUSTIFIED)
    {
        puts("Validation deja connue");
        return;
    }

    if (strcmp(validation_code, "ok") == 0)
    {
        absence->justified = ABSENCE_JUSTIFIED;
        puts("Validation enregistree");
        return;
    }

    if (strcmp(validation_code, "ko") == 0)
    {
        absence->justified = ABSENCE_NOT_JUSTIFIED;
        puts("Validation enregistree");
        return;
    }
}

void handle_etudiant(ParsedCommand parsed_command, int nb_students, Student *student_list)
{
    if (parsed_command.arguments_count != ETUDIANT_ARGS_COUNT)
        return;

    int student_id = atoi(parsed_command.arguments_list[0]);
    if (student_id > nb_students)
    {
        puts("Identifiant incorrect");
        return;
    }

    int date = atoi(parsed_command.arguments_list[1]);
    if (date < 1)
    {
        puts("Date incorrecte");
        return;
    }

    Student student = student_list[student_id - 1];
    printf("(%d) %s %d %d", student_id, student.name, student.group, student.nb_absence);

    int nb_absence_waiting_justification = count_absence_status(ABSENCE_WAITING_JUSTIFICATION, nb_students, student_list);
    int nb_absence_waiting_validation = count_absence_status(ABSENCE_WAITING_VALIDATION, nb_students, student_list);
    int nb_absence_justified = count_absence_status(ABSENCE_JUSTIFIED, nb_students, student_list);
    int nb_absence_not_justified = count_absence_status(ABSENCE_NOT_JUSTIFIED, nb_students, student_list);

    if (nb_absence_waiting_justification > 0)
    {
        puts("- En attente justificatif");
        for (int i = 0; i < nb_absence_waiting_justification; ++i)
            printf("\t[%d] %d/%s\n", student.absences[i].id_absence, student.absences[i].date, student.absences[i].am_pm);
    }

    if (nb_absence_waiting_validation > 0)
    {
        puts("- En attente validation");
        for (int i = 0; i < nb_absence_waiting_validation; ++i)
            printf("\t[%d] %d/%s (%s)\n", student.absences[i].id_absence, student.absences[i].date, student.absences[i].am_pm, student.absences[i].justification);
    }

    if (nb_absence_justified > 0)
    {
        puts("- Justifiees");
        for (int i = 0; i < nb_absence_justified; ++i)
            printf("\t[%d] %d/%s (%s)\n", student.absences[i].id_absence, student.absences[i].date, student.absences[i].am_pm, student.absences[i].justification);
    }

    if (nb_absence_not_justified > 0)
    {
        puts("- Non-justifiees");
        for (int i = 0; i < nb_absence_not_justified; ++i)
        {
            if (strcmp(student.absences[i].justification, "") != 0)
                printf("\t[%d] %d/%s (%s)\n", student.absences[i].id_absence, student.absences[i].date, student.absences[i].am_pm, student.absences[i].justification);
            else
                printf("\t[%d] %d/%s \n", student.absences[i].id_absence, student.absences[i].date, student.absences[i].am_pm);
        }
    }
}

void handle_defaillants(ParsedCommand parsed_command, int nb_students, Student *student_list)
{
    if (parsed_command.arguments_count < DEFAILLANTS_ARGS_COUNT)
        return;

    int current_day = atoi(parsed_command.arguments_list[0]);
    if (current_day < MIN_DAY)
    {
        puts("Date incorrecte");
        return;
    }

    int nb_defaillants = 0;

    for (int etu = 0; etu < nb_students; ++etu) // Flag les défaillants !
    {

        int absences_injustifiees = count_absences_injustifiees(etu, student_list);

        if (absences_injustifiees < 5)
        {
            student_list[etu].defaillance = PASDEFAILLANT;
        }
        else
        {
            student_list[etu].defaillance = DEFAILLANT;
            ++nb_defaillants;
        }
    }

    if (nb_defaillants == 0)
    {
        puts("Aucun defaillant");
        return;
    }
    else
    {
        for (int etu = 0; etu < nb_students; ++etu)
        {

            if (student_list[etu].defaillance == DEFAILLANT)
            {

                // C'EST LE MEME CODE QUE CELUI DE LA COMMANDE ETUDIANTS LIGNE 232
                // METTRE DANS UNE FONCTION ?

                int total_absences = get_absence_count_before(&student_list[etu], current_day);
                printf("(%d) %-13s %2d %d\n", student_list[etu].student_id,
                       student_list[etu].name,
                       student_list[etu].group,
                       total_absences);
            }
        }
    }
}

int count_absences_injustifiees(int student_id, Student *student_list)
{
    int count = 0;

    for (int abs = 0; abs < student_list[student_id].nb_absence; ++abs)
    {
        if (student_list[student_id].absences[abs].justified == ABSENCE_NOT_JUSTIFIED)
        {
            ++count;
        }
    }

    return count;
}