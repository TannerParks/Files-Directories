#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

struct movie {
    // Since title and year are the only values I'm leaving the rest out of the list
    char *title;
    int year;

    struct movie *next;
} *head;


struct movie *add_to_list(struct movie *list, char *title, int year) {
    struct movie *node;
    node = malloc(sizeof(struct movie));

    node->title = strdup(title);
    node->year = year;
    node->next = list;

    return node;
}


int delete_list() { // Deletes list after every directory is made
    struct movie *temp;

    while (head != NULL) {
        temp = head;
        head = head->next;

        free(temp);
    }

    return 0;
}


struct movie *get_data(char *line) {
    char *movie;
    int year;

    char token_list[110][110]; // 110 = max # languages * max char length + semicolons + bracket
    char *context = NULL; // used so strtok_r saves its state
    char *token = strtok_r(line, ",", &context);
    int counter = 0;

    while (token != NULL) { // Splits each line into categories (title, year, rating, languages)
        strcpy(token_list[counter], token);
        counter++;
        token = strtok_r(NULL, ",", &context);
    };

    movie = token_list[0];
    year = atoi(token_list[1]); 

    head = add_to_list(head, movie, year);    // New movies become the head of the list

    return head;
}

char *write_file(struct movie *list) {
    FILE *file;
    struct movie *temp;
    int random = (rand() * rand()) % 100001;
    char *dir_name = malloc(22);
    char year_file[260];
    char cmd[32];

    random = abs(random);   // Gets the absolute value of the random number

    sprintf(dir_name, "parksta.movies.%d", random); // makes directory with random number

    mkdir(dir_name, 0750);
    //mkdir(dir_name);

    temp = list;
    while (temp != NULL) {
        sprintf(year_file, "parksta.movies.%d/%d.txt", random, temp->year);
        file = fopen(year_file, "a");
        //printf("%s\n", temp->title);
        fprintf(file, "%s\n", temp->title);
        fclose(file);
        sprintf(cmd, "chmod 640 %s", year_file);    // Changes the file permissions
        system(cmd);
        temp = temp->next;
    }

    return dir_name;
}


int main() {
    srand(time(NULL));
    FILE *movie_data;
    char str[500];  // Used to store the lines of the file
    struct movie *list = NULL;
    char header[] = "Title,Year,Languages,Rating Value";

    // User input variables and returns
    int compare;
    int menu_loop;  // Used to loop back to second menu after an inputted file isn't found
    int input = 0;
    int file_size = 0;  // File size input (1,2 or 3)
    char *input_name;
    input_name = malloc(20);
    char *ext = ".csv"; // File extension to check for
    char *ext_check;
    char *dir_name; // Return value to get the name of the directory that was made
    int file_found; // Variable to check to see if the file being looked for was found
    
    // File size variables
    struct stat stats;
    char *prefix = "movies_";
    DIR *currDir;
    struct dirent *aDir;
    int file_stats_large;
    int file_stats_small;
    char chosen_file[255];  // Largest or smallest file
    
    while (1) {
        printf("1. Select file to process\n2. Exit the Program\n\nEnter a choice: ");
        scanf("%d", &input);
    
        if (input == 1) {
            menu_loop = 1;
            while (1) {
                printf("\nWhich file do you want to process?\nEnter 1 to pick the largest file\nEnter 2 to pick the smallest file\nEnter 3 to specify file name\n\nEnter a choice: ");
                scanf("%d", &file_size);
                file_stats_large = 0;  // sets/resets file size to 0
                file_stats_small = 999999999;
    
                if (file_size == 1) {
                    // find the largest file with the extension csv in the current directory 
                    currDir = opendir(".");
                    while((aDir = readdir(currDir)) != NULL){
                        ext_check = strstr(aDir->d_name, ext);
                        if((strncmp(prefix, aDir->d_name, strlen(prefix)) == 0) && ext_check){  // Checks for prefix and extension
                            stat(aDir->d_name, &stats);

                            if (stats.st_size > file_stats_large) {
                                file_stats_large = stats.st_size;

                                strncpy(chosen_file, aDir->d_name, 254);
                            }
                        }
                    }
                    closedir(currDir);

                    printf("Largest file: %s\n", chosen_file);
                    movie_data = fopen(chosen_file, "r");
                    while (1){
                        if (fgets(str, 500, movie_data) != NULL) {
                            compare = strncmp(header, str, strlen(header));
                            if (compare <= 0) { // skips the header line
                                continue;
                            }
                            list = get_data(str);
                        }
                        if (feof(movie_data)) { // breaks at end of the file
                            fclose(movie_data);
                            dir_name = write_file(list); // Return directory name
                            printf("Created directory with the name %s\n\n", dir_name);
                            delete_list(list);
                            free(dir_name);
                            break;
                        }
                    }
                    break;
                }
    
                else if (file_size == 2) {
                    // find the smallest file with the extension csv in the current directory
                    currDir = opendir("."); 
                    while((aDir = readdir(currDir)) != NULL){
                        ext_check = strstr(aDir->d_name, ext);
                        if((strncmp(prefix, aDir->d_name, strlen(prefix)) == 0) && ext_check){  // Checks for prefix and extension
                            stat(aDir->d_name, &stats);

                            if (stats.st_size < file_stats_small) {
                                file_stats_small = stats.st_size;
                                strncpy(chosen_file, aDir->d_name, 254);
                            }
                        }
                    }
                    closedir(currDir);

                    printf("Smallest file: %s\n", chosen_file);
                    movie_data = fopen(chosen_file, "r");
                    while (1){
                        if (fgets(str, 500, movie_data) != NULL) {
                            compare = strncmp(header, str, strlen(header));
                            if (compare <= 0) { // skips the header line
                                continue;
                            }
                            list = get_data(str);
                        }
                        if (feof(movie_data)) { // breaks at end of the file
                            fclose(movie_data);
                            dir_name = write_file(list); // Return directory name
                            printf("Created directory with the name %s\n\n", dir_name);
                            delete_list(list);
                            free(dir_name);
                            break;
                        }
                    }
                    break;
                }
    
                else if (file_size == 3) {
                    printf("\nEnter complete file name: ");
                    scanf("%s", input_name);
                    movie_data = fopen(input_name, "r");
                    ext_check = strstr(input_name, ext);
                    file_found = 0; // Resets back to zero if option is chosen again
                    
                    while (1){

                        if (!ext_check) {
                            printf("Not a .csv file. Please try again with the .csv file extension.\n");
                            break;
                        }

                        if (movie_data == NULL) {
                            printf("No file found by the name %s\n", input_name);
                            break;
                        }

                        if (file_found == 0) {
                            printf("Processing file named %s\n", input_name);
                            file_found = 1;
                        }

                        if (fgets(str, 500, movie_data) != NULL) {
                            compare = strncmp(header, str, strlen(header));
                            if (compare <= 0) { // skips the header line
                                continue;
                            }
                            list = get_data(str);
                        }
                
                        if (feof(movie_data)) { // breaks at end of the file
                            menu_loop = 0; 
                            fclose(movie_data);
                            dir_name = write_file(list);
                            printf("Created directory with the name %s\n\n", dir_name);
                            delete_list(list);
                            free(dir_name);
                            break;
                        }
                    }
                    if (menu_loop == 0) {   // Used so the menu will go back to the main menu after successfully finding a file
                        break;
                    }
                }

                else {
                    printf("Invalid input\n");
                }
            }
        }

        else if (input == 2) {
            exit(0);
        }

        else {
            printf("Invalid input\n");
        }
    }
}