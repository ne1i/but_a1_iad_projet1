#ifndef APP_H
#define APP_H

#define MAX_ABSENCES 40
#define MAX_JUSTIFICATION_LENGTH 50
#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS_COUNT 30
#define MAX_NAME_LENGTH 30
#define MAX_NB_STUDENTS 100
#define MAX_ASBENCE_DAY 40

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
    ABSENCE_FUTURE,
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
    char am_pm[3];
    int id_absence;
    int date;
    int date_justification;
    enum AbsenceStatus justified;

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
    int nb_absence;
    Absence absences[MAX_ABSENCES];
    int student_id;
    int group;
    enum estDefaillant defaillance;
} Student;

void handle_command(char *command, int *nb_students, int *nb_absence, Student *student_list);
void parse_command(char *command_line, ParsedCommand *parsed_command);
void parse_command_justificatif(ParsedCommand *parsed_command);

void handle_inscription(ParsedCommand parsed_command, int *nb_students, Student *student_list);
void handle_absence(const ParsedCommand parsed_command, int *nb_students, int *nb_absences, Student *student_list);
void handle_etudiants(ParsedCommand parsed_command, int nb_students, Student *student_list);
void handle_justificatif(ParsedCommand parsed_command, int *nb_students, Student *student_list);
void handle_validations(int *nb_students, int *nb_absences, Student *student_list);
void handle_validation(ParsedCommand parsed_command, int nb_student, int nb_absence, Student *student_list);
void handle_etudiant(ParsedCommand parsed_command, int nb_students, Student *student_list);
void handle_defaillants(ParsedCommand parsed_command, int nb_students, Student *student_list);

int compare_group(const void *a, const void *b);
int compare_student_id(const void *a, const void *b);
int get_absence_count_before(const Student *student, int max_day);
int check_absence_exists(int student_id, int *nb_students, Student *student_list);
int check_absence_status_exists(enum AbsenceStatus ABSENCE_STATUS, int nb_students, Student *student_list);
int count_absence_status(enum AbsenceStatus status, int nb_students, Student *student_list);
int count_student_absence_before(int student_id, int date, Student *student_list);
int count_student_absence_status_before(enum AbsenceStatus status, Absence *absences, int nb_absences, int date);
int count_absences_injustifiees(int student_id, Student *student_list);

#endif