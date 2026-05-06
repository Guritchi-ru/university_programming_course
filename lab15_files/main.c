#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char name[60];
    char id[30];
    char level[30];
} employee;

int count_workers(FILE *file){
    int k = 0;
    char ch;
    long pos = ftell(file);

    while ((ch = fgetc(file)) != EOF){
        if (ch == '\n'){
            k++;
        }
    }

    fseek(file, pos, SEEK_SET);
    return k;
}

employee *read_file(char *filename, int *count){
    FILE *file = fopen(filename, "r");
    if (file == NULL){
        perror("Error opening file");
        exit(1);
    }

    *count = count_workers(file);
    if (*count == 0) {
        printf("В список не внесли ни одного сотрудника");
        return NULL;
    }

    employee *workers = (employee *)malloc((*count)* sizeof(employee));

    char line[128];
    int num_worker = 0;
    
    while (fgets(line, sizeof(line), file) != NULL){
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) == 0) {
            continue;
        }

        char *name = strtok(line, ";");
        char *id = strtok(NULL, ";");
        char *level = strtok(NULL, ";");

        strcpy(workers[num_worker].name, name);
        strcpy(workers[num_worker].id, id);
        strcpy(workers[num_worker].level, level);

        num_worker++;
    }
    *count = num_worker;

    fclose(file);
    return workers;
}

void write_file(char *filename, employee *workers, int count){
    FILE *file = fopen(filename, "wb");
    if (file == NULL){
        perror("Error opening file");
        exit(1);
    }

    size_t written = fwrite(workers, sizeof(employee), count, file);
    if (written != count) {
        printf("Ошибка: записаны не все сотрудники\n");
    } else {
        printf("Успешно сохранено %d сотрудников в файл '%s'\n", count, filename);
    }
    
    fclose(file);
}

void search_file(char *filename){
    FILE *file = fopen(filename, "rb");
    if (file == NULL){
        perror("Error opening file");
        exit(1);
    }

    char search_id[30];
    printf("Введите ID сотрудника для поиска: ");
    scanf("%s", search_id);
    
    employee emp;
    int found = 0;
    
    while (fread(&emp, sizeof(employee), 1, file) == 1) {
        if (strcmp(emp.id, search_id) == 0) {
            found = 1;
            break;
        }
    }
    
    if (found) {
        printf("\n");
        printf("Сотрудник найден:\n");
        printf("Имя: %s\n", emp.name);
        printf("ID: %s\n", emp.id);
        printf("Уровень доступа: %s\n", emp.level);
    } else {
        printf("\nОшибка: Сотрудник с ID %s не найден в базе данных\n", search_id);
    }
    
    fclose(file);
}

void compress(char *input_file, char *output_file){
    FILE *input = fopen(input_file, "rb");
    if (input == NULL){
        perror("Error opening file");
        exit(1);
    }

    FILE *output = fopen(output_file, "wb");
    if (output == NULL){
        perror("Error opening file");
        exit(1);
    }

    fseek(input, 0, SEEK_END);
    long orig_size = ftell(input);
    fseek(input, 0, SEEK_SET);

    unsigned char cur_byte, prev_byte;
    unsigned char count =1;

    if (fread(&prev_byte, 1, 1, input) != 1){
        printf("Файл пуст\n");
        fclose(input);
        fclose(output);
        return;
    }

    while (fread(&cur_byte, 1, 1, input) == 1){
        if (cur_byte == prev_byte && count < 255){
            count++;
        } else {
            fwrite(&count, 1, 1, output);
            fwrite(&prev_byte, 1, 1, output);

            prev_byte = cur_byte;
            count = 1;
        }
    }

    fwrite(&count, 1, 1, output);
    fwrite(&prev_byte, 1, 1, output);

    long compressed_size = ftell(output);
    double compression = (1.0 - (double)compressed_size / orig_size) * 100;

    printf("\nОТЧЁТ О СЖАТИИ\n");
    printf("Размер до: %.2f Кб, Размер после: %.2f Кб. Сжатие: %.2f%%\n", orig_size / 1024.0, compressed_size / 1024.0, compression);

    fclose(input);
    fclose(output);
}

void decompress(char *input_file, char *output_file){
    FILE *input = fopen(input_file, "rb");
    if (input == NULL){
        perror("Error opening file");
        exit(1);
    }

    FILE *output = fopen(output_file, "wb");
    if (output == NULL){
        perror("Error opening file");
        exit(1);
    }

    unsigned char count, value;
    int pair_count = 0;

    while (fread(&count, 1, 1, input) == 1){
        if (fread(&value, 1, 1, input) != 1){
            break;
        }
        pair_count++;

        for (int i = 0; i < count; i++){
            fwrite(&value, 1, 1, output);
        }
    }

    fseek(input, 0, SEEK_END);
    long orig_size = ftell(input);
    fseek(input, 0, SEEK_SET);

    long compressed_size = ftell(output);
    printf("Размер до: %.2f Кб, Сжатие: %.2f Кб\n", orig_size / 1024.0, compressed_size / 1024.0);

    fclose(input);
    fclose(output);
}

int main(int argc, char *argv[]){
    if (argc < 3){
        printf("Usage: %s staff.csv database.dat\n", argv[0]);
        exit(1);
    }

    char *input_file = argv[1];
    char *output_file = argv[2];
    char compressed_file[] = "database.rle";
    char decompressed_file[] = "database_restored.dat";
    char compressed_img[] = "kosmodes.rle";
    char decompressed_img[] = "kosmodes_restored.png";
    int k = 0;
    
    employee *workers = read_file(input_file, &k);
    if (workers == NULL) {
        return 1;
    }
    
    write_file(output_file, workers, k);
    free(workers);

    printf("\nПоиск сотрудника\n");
    search_file(output_file);

    compress(output_file, compressed_file);
    decompress(compressed_file, decompressed_file);


    compress("kosmodes.png", compressed_img);
    decompress(compressed_img, decompressed_img);

    return 0;
}