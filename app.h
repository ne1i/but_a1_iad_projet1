#ifndef APP_H
#define APP_H

#define MAX_COMMAND_LENGTH 256
#define MAX_ARGUMENTS_COUNT 30
#define MAX_NAME_LENGTH 30
#define MAX_NB_STUDENTS 100

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
    char Name[MAX_NAME_LENGTH];
    short NB_absence;
    short Student_ID;
    short Group;
} Student;

void handle_command(char *command);
void parse_command(char *command_line, ParsedCommand *parsed_command);

// Utility function to print the command and its arguments
void debug_print(ParsedCommand *parsed_command)
{
    if (parsed_command->command_type == COMMAND_UNKNOWN)
        return;
    puts("Parsed command = {");
    printf("\tcommand_type = %d\n", parsed_command->command_type);
    printf("\targuments_count = %d\n", parsed_command->arguments_count);
    fputs("\targuments_list = [ ", stdout);
    // use for loop to print all arguments
    for (int i = 0; i < parsed_command->arguments_count; i++)
    {
        printf("%s ", parsed_command->arguments_list[i]);
        if (i < parsed_command->arguments_count - 1)
            printf(", ");
    }
    puts("]");
    puts("}");
}

#endif