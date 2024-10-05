#ifndef APP_H
#define APP_H

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS_COUNT 30
#define MAX_NAME_LENGTH 30
#define MAX_NB_STUDENTS 100

typedef struct
{
    enum CommandType command_type;
    char *arguments_list[MAX_ARGUMENTS_COUNT];
    int arguments_count;
} ParsedCommand;

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
    char Name[MAX_NAME_LENGTH];
    short NB_absence;
    short Student_ID;
    short Group;
} Student;

void handle_command(char *command);
void parse_command(char *command_line, ParsedCommand *parsed_command);

#endif