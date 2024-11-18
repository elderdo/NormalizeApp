#include <direct.h> // _getcwd
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h> // For GetModuleFileName

#define BUFFER_SIZE 1024

void normalize(FILE* input, FILE* output) {
    char buffer[BUFFER_SIZE];
    char accumulated[BUFFER_SIZE * 10] = ""; // Adjust size as needed
    size_t accumulated_len = 0;

    while (fgets(buffer, BUFFER_SIZE, input)) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0'; // Remove newline character
            len--;
        }

        // Check if the line ends with '>'
        if (len > 0 && buffer[len - 1] == '>') {
            strcat_s(accumulated, sizeof(accumulated), buffer);
            accumulated_len += len;
            fprintf(output, "%s\n", accumulated);
            accumulated[0] = '\0'; // Reset accumulated buffer
            accumulated_len = 0;
        }
        else {
            strcat_s(accumulated, sizeof(accumulated), buffer);
            accumulated_len += len;
        }
    }

    // If there is any remaining accumulated data, print it out
    if (accumulated_len > 0) {
        fprintf(output, "%s\n", accumulated);
    }
}

void ensure_xml_extension(char* filename, size_t size) {
    const char* extension = ".xml";
    size_t len = strlen(filename);
    if (len < 4 || strcmp(filename + len - 4, extension) != 0) {
        if (len + 4 < size) {
            strcat_s(filename, size, extension);
        }
    }
}

void get_executable_path(char* path, size_t size) {
    DWORD len = GetModuleFileName(NULL, path, size);
    if (len == 0 || len == size) {
        perror("Failed to get executable path");
        exit(EXIT_FAILURE);
    }
}

void get_output_file_path(char* output_path, const char* exe_path, const char* output_filename, size_t size) {
    const char* last_slash = strrchr(exe_path, '\\');
    if (last_slash) {
        size_t dir_len = last_slash - exe_path + 1;
        strncpy_s(output_path, size, exe_path, dir_len);
        strncat_s(output_path, size, output_filename, size - dir_len);
    }
    else {
        strncpy_s(output_path, size, output_filename, size);
    }
}

char* get_current_directory(char* buffer, size_t size) {
	return _getcwd(buffer, size);
}

int main(int argc, char* argv[]) {
    FILE* input = stdin;
    FILE* output = stdout;
    char output_path[FILENAME_MAX];
    char exe_path[FILENAME_MAX];
	char dir_path[FILENAME_MAX];
	get_current_directory(dir_path, FILENAME_MAX);
    if (argc > 1 && strcmp(argv[1], "-") != 0) {
        ensure_xml_extension(argv[1], FILENAME_MAX);
        if (fopen_s(&input, argv[1], "r") != 0) {
            perror("Failed to open input file");
            return EXIT_FAILURE;
        }
        printf("Input file: %s\n", argv[1]);
    }

    get_executable_path(exe_path, FILENAME_MAX);

    if (argc > 2 && strcmp(argv[2], "-") != 0) {
        ensure_xml_extension(argv[2], FILENAME_MAX);
        get_output_file_path(output_path, exe_path, argv[2], FILENAME_MAX);
    }
    else {
        get_output_file_path(output_path, exe_path, "output.xml", FILENAME_MAX);
    }

    if (fopen_s(&output, output_path, "w") != 0) {
        perror("Failed to open output file");
        if (input != stdin) fclose(input);
        return EXIT_FAILURE;
    }
    printf("Output file: %s\n", output_path);

    normalize(input, output);

    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);

    return EXIT_SUCCESS;
}
