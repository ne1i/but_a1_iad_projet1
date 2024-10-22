#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#pragma warning(disable : 4996)

#define MAX_ABSENCES 80 // 80 absences car 40 jours possible * 2 demies journées possibles
#define MAX_JUSTIFICATION_LENGTH 50
#define MAX_COMMAND_LENGTH 256
#define AM_PM_LENGTH 3
#define VALIDATION_CODE_LENGTH 3
#define MAX_ARGUMENTS_COUNT 3
#define MAX_NAME_LENGTH 31
#define MAX_NB_STUDENTS 100
#define MAX_ASBENCE_DAY 40

#define DEFAILLANT_NB_ABSENCE 5
#define JUSTIFICATION_DEADLINE 3

#define MIN_DAY 1
#define MIN_ABSENCE_DAY 1

#define INSCRIPTION_ARGS_COUNT 2
#define ABSENCE_ARGS_COUNT 3
#define ETUDIANTS_ARGS_COUNT 1
#define JUSTIFICATIF_ARGS_COUNT 3
#define VALIDATIONS_ARGS_COUNT 0
#define VALIDATION_ARGS_COUNT 2
#define ETUDIANT_ARGS_COUNT 2
#define DEFAILLANTS_ARGS_COUNT 1

enum AbsenceStatus
{
    ABSENCE_FUTURE = -1,
    ABSENCE_JUSTIFIED = 0,
    ABSENCE_NOT_JUSTIFIED,
    ABSENCE_WAITING_JUSTIFICATION,
    ABSENCE_WAITING_VALIDATION
};

enum estDefaillant
{
    DEFAILLANT,
    PASDEFAILLANT
};

typedef struct
{
    int student_id;
    char justification[MAX_JUSTIFICATION_LENGTH];
    char am_pm[AM_PM_LENGTH];
    // char padding[3];
    int id_absence;
    int date;
    int date_justification;
    enum AbsenceStatus justified;
    int valide;

} Absence;

enum CommandType
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
    enum CommandType command_type;
    char *arguments_list[MAX_ARGUMENTS_COUNT];
    int arguments_count;
} ParsedCommand;

typedef struct
{
    char name[MAX_NAME_LENGTH];
    // char padding[2];
    int nb_absence;
    Absence absences[MAX_ABSENCES];
    int student_id;
    int group;
    enum estDefaillant defaillance;
} Student;

void handle_command(char *command, int *nb_students, int *nb_absence, Student *student_list);
void parse_command(char *command_line, ParsedCommand *parsed_command);
void parse_command_justificatif(ParsedCommand *parsed_command);

void handle_inscription(const ParsedCommand parsed_command, int *nb_students, Student *student_list);
void handle_absence(const ParsedCommand parsed_command, int *nb_students, int *nb_absences, Student *student_list);
void handle_etudiants(const ParsedCommand parsed_command, int nb_students, const Student *student_list);
void handle_justificatif(const ParsedCommand parsed_command, int *nb_students, Student *student_list);
void handle_validations(int *nb_students, const Student *student_list);
void handle_validation(const ParsedCommand parsed_command, int nb_student, int nb_absence, Student *student_list);
void handle_etudiant(const ParsedCommand parsed_command, int nb_students, const Student *student_list);
void handle_defaillants(const ParsedCommand parsed_command, int nb_students, Student *student_list);

int compare_group(const void *a, const void *b);
int compare_student_id(const void *a, const void *b);
int get_absence_count_before(const Student *student, int max_day);
int check_absence_exists(int student_id, int *nb_students, const Student *student_list);
int check_absence_status_exists(enum AbsenceStatus ABSENCE_STATUS, int nb_students, const Student *student_list);
int compare_absence_date(const void *a, const void *b);
int count_absence_status(enum AbsenceStatus status, int nb_students, const Student *student_list);
int count_student_absence_before(int student_id, int date, const Student *student_list);
int count_student_absence_status_before(enum AbsenceStatus status, Absence *absences, int nb_absences, int date);
int count_absences_injustifiees_before(const Student student, int date);

void *safe_calloc(size_t n);

int main(void)
{
    int student_id_counter = 0;
    int absence_id_counter = 0;
    Student *student_list = (Student *)safe_calloc(sizeof(Student) * MAX_NB_STUDENTS);
    char current_command_line[MAX_COMMAND_LENGTH];

    // Boucle jusqu'à ce que l'utilisateur rentre la commande exit
    while (fgets(current_command_line, MAX_COMMAND_LENGTH, stdin) != 0) // "!= 0" pour les cas où on rentre un fichier dans le flux d'execution
    {
        handle_command(current_command_line, &student_id_counter, &absence_id_counter, student_list);
    }
    return 0;
}

// utilise une variable de type ParsedCommand pour appeler la commande correspondante avec un switch
void handle_command(char *command_line, int *nb_students, int *nb_absence, Student *student_list)
{
    // remplace le \n a la fin de la commande par un \0
    command_line[strcspn(command_line, "\n")] = 0;
    // récupérer la commande séparée
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
        handle_validations(nb_students, student_list);
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
// OUT: ParsedCommand, constitué d'une variable de type CommandType, une liste d'argument et du nombre d'argument
// EXAMPLE : "inscription Lea 101\0" -> COMMAND_INSCRIPTION, ["Lea", "101"], 2

// Sépare la commande et renvoie à parsed_command la commande qui est appelée par l'utilisateur
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
        parsed_command->command_type = COMMAND_EXIT;
    else if (strcmp(word, "inscription") == 0)
        parsed_command->command_type = COMMAND_INSCRIPTION;
    else if (strcmp(word, "absence") == 0)
        parsed_command->command_type = COMMAND_ABSENCE;
    else if (strcmp(word, "etudiants") == 0)
        parsed_command->command_type = COMMAND_ETUDIANTS;
    else if (strcmp(word, "justificatif") == 0)
    {
        // le parcours de commande est différent pour la commande justificatif car le dernier argument est une suite de plusieurs mots
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
}

// Sépare les arguments de la commande Justificatif
void parse_command_justificatif(ParsedCommand *parsed_command)
{
    const char *space_separator = " ";
    const char *newline_separator = "\n";
    int nb_argument = 0;
    parsed_command->command_type = COMMAND_JUSTIFICATIF;
    parsed_command->arguments_list[0] = strtok(NULL, space_separator);
    ++nb_argument;
    parsed_command->arguments_list[1] = strtok(NULL, space_separator);
    ++nb_argument;
    parsed_command->arguments_list[2] = strtok(NULL, newline_separator);
    ++nb_argument;
    parsed_command->arguments_count = nb_argument;
}

// Gère la commande inscription :
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

// Gère la commande absence :
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
    absence->valide = 0;
    strcpy(absence->justification, "");
    printf("Absence enregistree [%d]\n", absence->id_absence);
}

// Gère la commande etudiants
void handle_etudiants(const ParsedCommand parsed_command, int nb_students, const Student *student_list)
{
    if (parsed_command.arguments_count < ETUDIANTS_ARGS_COUNT)
        return;

    if (nb_students == 0)
    {
        puts("Aucun inscrit");
        return;
    }

    int current_day = atoi(parsed_command.arguments_list[0]);
    if (current_day < MIN_DAY)
    {
        puts("Date incorrecte");
        return;
    }

    // copie temporaire de student_list
    Student *sorted_student_array = NULL;
    // utilisation de malloc pour ne pas stocker 100 etudiants même si il n'y a pas 100 etudiants inscrits
    int mem = sizeof(Student) * nb_students;
    sorted_student_array = (Student *)safe_calloc(mem);
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
    // rendu de la mémoire utilisée pour sorted_student_array
    free(sorted_student_array);
}

// Compare la classe (puis le nom si la classe est égale) de 2 étudiants et renvoie les valeurs voulues pour pouvoir être utilisée avec qsort
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

// Récupère le nombre d'absences d'un élève avant un jour donné
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

// Gère la commande justificatif
void handle_justificatif(const ParsedCommand parsed_command, int *nb_students, Student *student_list)
{
    // vérifie si l'absence existe, si elle existe on récupère le student_id
    int absence_id = atoi(parsed_command.arguments_list[0]);
    int student_id = check_absence_exists(absence_id, nb_students, student_list);
    if (student_id == -1)
    {
        puts("Identifiant incorrect");
        return;
    }

    // vérification de la date
    Student *student = &student_list[student_id - 1];
    int student_absence_index = 0;
    for (int i = 0; i < student->nb_absence; ++i)
    {
        if (student->absences[i].id_absence == absence_id)
            student_absence_index = i;
    }

    int date = atoi(parsed_command.arguments_list[1]);
    if (date < student->absences[student_absence_index].date)
    {
        puts("Date incorrecte");
        return;
    }

    if (student->absences[student_absence_index].justified != ABSENCE_WAITING_JUSTIFICATION)
    {
        puts("Justificatif deja connu");
        return;
    }

    // vérification du délai d'absence
    if ((atoi(parsed_command.arguments_list[1]) - student->absences[student_absence_index].date) > JUSTIFICATION_DEADLINE)
    {
        student->absences[student_absence_index].justified = ABSENCE_NOT_JUSTIFIED;
        {
            puts("Justificatif enregistre");
            strcpy(student->absences[student_absence_index].justification, parsed_command.arguments_list[2]);
            student->absences[student_absence_index].date_justification = date;
            return;
        }
    }
    if (strlen(parsed_command.arguments_list[2]) > MAX_JUSTIFICATION_LENGTH)
    {
        return;
    }

    strcpy(student->absences[student_absence_index].justification, parsed_command.arguments_list[2]);
    student->absences[student_absence_index].justified = ABSENCE_WAITING_VALIDATION;
    student->absences[student_absence_index].date_justification = date;
    puts("Justificatif enregistre");
}

// vérifie si l'absence d'id absence_id existe
int check_absence_exists(int absence_id, int *nb_students, const Student *student_list)
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

// Gère la commande validations
void handle_validations(int *nb_students, const Student *student_list)
{
    if (check_absence_status_exists(ABSENCE_WAITING_VALIDATION, *nb_students, student_list) == -1)
    {
        puts("Aucune validation en attente");
        return;
    }

    int nb_absences_waiting_validation = count_absence_status(ABSENCE_WAITING_VALIDATION, *nb_students, student_list);

    // liste des absences en attente de validation
    Absence *absences_waiting_validation_list = (Absence *)safe_calloc(nb_absences_waiting_validation * sizeof(Absence));
    int count = 0;
    for (int i = 0; i < *nb_students; ++i)
    {
        for (int j = 0; j < student_list[i].nb_absence; ++j)
        {
            if (student_list[i].absences[j].justified == ABSENCE_WAITING_VALIDATION)
            {
                memcpy(absences_waiting_validation_list + count, &student_list[i].absences[j], sizeof(Absence));
                ++count;
            }
        }
    }
    // tri des absences par id
    qsort(absences_waiting_validation_list, nb_absences_waiting_validation, sizeof(Absence), compare_student_id);

    for (int i = 0; i < nb_absences_waiting_validation; ++i)
    {
        int absence_id = absences_waiting_validation_list[i].id_absence;
        int student_id = absences_waiting_validation_list[i].student_id;
        char student_name[MAX_NAME_LENGTH];
        strcpy(student_name, student_list[student_id - 1].name);
        int group = student_list[student_id - 1].group;
        int absence_date = absences_waiting_validation_list[i].date;
        char am_pm[AM_PM_LENGTH];
        strcpy(am_pm, absences_waiting_validation_list[i].am_pm);
        char justification[MAX_JUSTIFICATION_LENGTH];
        strcpy(justification, absences_waiting_validation_list[i].justification);

        printf("[%d] (%d) %-14s %-1d %d/%s (%s)\n", absence_id, student_id, student_name, group, absence_date, am_pm, justification);
    }
    free(absences_waiting_validation_list);
}

// vérifie si il y a au moins une absence de statut status
int check_absence_status_exists(enum AbsenceStatus status, int nb_students, const Student *student_list)
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

// Compte le nombre d'absence de tous les elèves en fonction du statut de l'absence
int count_absence_status(enum AbsenceStatus status, int nb_students, const Student *student_list)
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

// compare les identifiants des etudiants de 2 absences (puis la date de l'absence si les id sont égaux) et renvoie des valeurs en accord avec la fonction qsort
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

// Gère la commande validation
void handle_validation(const ParsedCommand parsed_command, int nb_students, int nb_absence, Student *student_list)
{
    if (parsed_command.arguments_count != VALIDATION_ARGS_COUNT)
        return;

    int student_id = 0;
    int student_absence_idx = 0;
    int absence_id = atoi(parsed_command.arguments_list[0]);

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

    if (absence->valide == 1)
    {
        puts("Validation deja connue");
        return;
    }

    if (absence_id > nb_absence || absence_id <= 0 || absence->justified != ABSENCE_WAITING_VALIDATION)
    {
        puts("Identifiant incorrect");
        return;
    }

    char validation_code[VALIDATION_CODE_LENGTH]; // 3 car "ok\0" ou "ko\0"
    strcpy(validation_code, parsed_command.arguments_list[1]);
    if (strcmp(validation_code, "ok") != 0 && strcmp(validation_code, "ko") != 0)
    {
        puts("Code incorrect");
        return;
    }

    if (strcmp(validation_code, "ok") == 0)
    {
        absence->justified = ABSENCE_JUSTIFIED;
        absence->valide = 1;
        puts("Validation enregistree");
        return;
    }

    if (strcmp(validation_code, "ko") == 0)
    {
        absence->justified = ABSENCE_NOT_JUSTIFIED;
        absence->valide = 1;
        puts("Validation enregistree");
        return;
    }
}

// Gère la commande etudiant
void handle_etudiant(const ParsedCommand parsed_command, int nb_students, const Student *student_list)
{
    if (parsed_command.arguments_count != ETUDIANT_ARGS_COUNT)
        return;

    int student_id = atoi(parsed_command.arguments_list[0]);
    if (student_id > nb_students || student_id <= 0)
    {
        puts("Identifiant incorrect");
        return;
    }

    int date = atoi(parsed_command.arguments_list[1]);
    if (date < MIN_DAY)
    {
        puts("Date incorrecte");
        return;
    }

    Student student = student_list[student_id - 1];
    // création d'une copie du tableau student.absences
    // pour pouvoir modifier le statut des absences en fonction de la date
    int mem = sizeof(Absence) * student.nb_absence;
    Absence *student_absence_list_copy = (Absence *)safe_calloc(mem);
    memcpy(student_absence_list_copy, student.absences, mem);

    // modification du statut des absences en fonction de la date
    for (int i = 0; i < student.nb_absence; ++i)
    {
        Absence *absence = &student_absence_list_copy[i];
        if (absence->date > date)
        {
            absence->justified = ABSENCE_FUTURE;
            continue;
        }

        if (absence->date_justification > date)
        {
            absence->justified = ABSENCE_WAITING_JUSTIFICATION;
        }

        if (absence->justified == ABSENCE_WAITING_JUSTIFICATION && date > (absence->date + JUSTIFICATION_DEADLINE))
        {
            absence->justified = ABSENCE_NOT_JUSTIFIED;
            if (absence->date_justification > date)
                strcpy(absence->justification, "");
        }
    }

    qsort(student_absence_list_copy, student.nb_absence, sizeof(Absence), compare_absence_date);

    int nb_absence_before = count_student_absence_before(student_id, date, student_list);
    printf("(%d) %s %d %d\n", student_id, student.name, student.group, nb_absence_before);

    if (count_student_absence_status_before(ABSENCE_WAITING_JUSTIFICATION, student_absence_list_copy, student.nb_absence, date) > 0)
    {
        puts("- En attente justificatif");
        for (int i = 0; i < student.nb_absence; ++i)
        {
            if (student_absence_list_copy[i].justified == ABSENCE_WAITING_JUSTIFICATION && student_absence_list_copy[i].date <= date)
                printf("  [%d] %d/%s\n", student_absence_list_copy[i].id_absence, student_absence_list_copy[i].date, student_absence_list_copy[i].am_pm);
        }
    }

    if (count_student_absence_status_before(ABSENCE_WAITING_VALIDATION, student_absence_list_copy, student.nb_absence, date) > 0)
    {
        puts("- En attente validation");
        for (int i = 0; i < student.nb_absence; ++i)
        {
            if (student_absence_list_copy[i].justified == ABSENCE_WAITING_VALIDATION && student_absence_list_copy[i].date <= date)
                printf("  [%d] %d/%s (%s)\n", student_absence_list_copy[i].id_absence, student_absence_list_copy[i].date, student_absence_list_copy[i].am_pm, student_absence_list_copy[i].justification);
        }
    }

    if (count_student_absence_status_before(ABSENCE_JUSTIFIED, student_absence_list_copy, student.nb_absence, date) > 0)
    {
        puts("- Justifiees");
        for (int i = 0; i < student.nb_absence; ++i)
        {
            if (student_absence_list_copy[i].justified == ABSENCE_JUSTIFIED && student_absence_list_copy[i].date <= date)
                printf("  [%d] %d/%s (%s)\n", student_absence_list_copy[i].id_absence, student_absence_list_copy[i].date, student_absence_list_copy[i].am_pm, student_absence_list_copy[i].justification);
        }
    }

    if (count_student_absence_status_before(ABSENCE_NOT_JUSTIFIED, student_absence_list_copy, student.nb_absence, date) > 0)
    {
        puts("- Non-justifiees");
        for (int i = 0; i < student.nb_absence; ++i)
        {
            if (student_absence_list_copy[i].justified == ABSENCE_NOT_JUSTIFIED && student_absence_list_copy[i].date <= date)
            {
                if (strcmp(student_absence_list_copy[i].justification, "") != 0)
                    printf("  [%d] %d/%s (%s)\n", student_absence_list_copy[i].id_absence, student_absence_list_copy[i].date, student_absence_list_copy[i].am_pm, student_absence_list_copy[i].justification);
                else
                    printf("  [%d] %d/%s \n", student_absence_list_copy[i].id_absence, student_absence_list_copy[i].date, student_absence_list_copy[i].am_pm);
            }
        }
    }
    free(student_absence_list_copy);
}

// Compare la date  (puis la demi-journée si la date est la même) de 2 absences et renvoie les valeurs voulues pour pouvoir être utilisée avec qsort
int compare_absence_date(const void *a, const void *b)
{
    const Absence *a1 = (const Absence *)a;
    const Absence *a2 = (const Absence *)b;
    if (a1->date < a2->date)
        return -1;
    if (a1->date > a2->date)
        return 1;
    return strcmp(a1->am_pm, a2->am_pm); // cela marche parce qu'alphabétiquement am est avant pm (code horrible mais ça marche)
}

// Renvoie le nombre d'absence de statut status d'un certain élève avant une certaine date
int count_student_absence_before(int student_id, int date, const Student *student_list)
{
    Student student = student_list[student_id - 1];
    int count = 0;
    for (int i = 0; i < student.nb_absence; ++i)
    {
        if (student.absences[i].date <= date)
            ++count;
    }
    return count;
}

int count_student_absence_status_before(enum AbsenceStatus status, Absence *absences, int nb_absences, int date)
{
    int count = 0;
    for (int i = 0; i < nb_absences; ++i)
    {
        if (absences[i].date <= date && absences[i].justified == status)
            ++count;
    }
    return count;
}

// Gère la commande defaillants
void handle_defaillants(const ParsedCommand parsed_command, int nb_students, Student *student_list)
{
    if (parsed_command.arguments_count < DEFAILLANTS_ARGS_COUNT)
        return;

    int date = atoi(parsed_command.arguments_list[0]);
    if (date < MIN_DAY)
    {
        puts("Date incorrecte");
        return;
    }

    int nb_defaillants = 0;

    // Vérifie si chaque etudiant est defaillant, et incrémente nb_defaillants dans le cas échéant
    for (int i = 0; i < nb_students; ++i)
    {

        int absences_injustifiees = count_absences_injustifiees_before(student_list[i], date);

        if (absences_injustifiees < DEFAILLANT_NB_ABSENCE)
        {
            student_list[i].defaillance = PASDEFAILLANT;
        }
        else
        {
            student_list[i].defaillance = DEFAILLANT;
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
        int mem = sizeof(Student) * nb_students;
        Student *sorted_student_array = (Student *)safe_calloc(mem);
        memcpy(sorted_student_array, student_list, mem);
        qsort(sorted_student_array, nb_students, sizeof(Student), compare_group);

        for (int i = 0; i < nb_students; ++i)
        {

            if (sorted_student_array[i].defaillance == DEFAILLANT)
            {
                int total_absences = get_absence_count_before(&sorted_student_array[i], date);
                printf("(%d) %-30s %d %d\n", sorted_student_array[i].student_id,
                       sorted_student_array[i].name,
                       sorted_student_array[i].group,
                       total_absences);
            }
        }
        free(sorted_student_array);
    }
}

// Compte et renvoie le nombre d'absences injustifiées d'un élève avant une date donnée
int count_absences_injustifiees_before(const Student student, int date)
{
    int count = 0;
    const Absence *absences = student.absences;
    for (int i = 0; i < student.nb_absence; ++i)
    {
        Absence absence = absences[i];
        int is_before_date = absence.date <= date;

        if (is_before_date == 0)
        {
            continue;
        }

        if (absence.justified == ABSENCE_JUSTIFIED || absence.justified == ABSENCE_WAITING_VALIDATION)
        {
            continue;
        }

        int is_too_late_to_justify = date - absence.date > JUSTIFICATION_DEADLINE;
        if (is_too_late_to_justify == 0 && absence.justified == ABSENCE_WAITING_JUSTIFICATION)
        {
            continue;
        }

        int was_justified_before_date = date >= absence.date_justification;
        int is_not_justified = absence.justified == ABSENCE_NOT_JUSTIFIED;
        if (is_not_justified && was_justified_before_date)
        {
            ++count;
            continue;
        }

        if (is_too_late_to_justify)
        {
            ++count;
            continue;
        }
    }
    return count;
}

void *safe_calloc(size_t n)
{
    void *p = calloc(1, n);
    if (p == NULL)
    {
        fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", n);
        abort();
    }
    return p;
}