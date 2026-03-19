/**
 * Weather Analyzer.
 */
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MONTHS 12        // Months in a year
#define MAX_ENTRIES 512  // Maximum years in a file
#define BUFF_SIZE 80     // Maximum length per line
#define ERROR_DATA -1

typedef struct
{
    double temperature;
    double precipitations;
} WMonth;

typedef struct
{
    int year;
    WMonth months[MONTHS];
    double temperature;
    double precipitations;
} WYear;

typedef struct
{
    WYear years[MAX_ENTRIES];
    int start;
} WData;

typedef struct
{
    char *in_filename;
    char *out_filename;
    bool binary_output;
} Options;

void version(FILE *fp)
{
    fprintf(fp,
            "Version 0.0.1 "
            "Copyright(c) HEIG-VD\n");
}

void help(FILE *fp)
{
    fprintf(fp,
            "USAGE: ./weather-analyser [options] [filename]\n\n"
            "This program processes weather data issues from the internet. \n"
            "It reads from [filename] or if missing, \n"
            "directly from STDIN.\n\n"
            "The output format is CSV compliant.\n\n"
            "OPTIONS:\n\n"
            "    --version      Shows the version\n"
            "    --help         Shows this help dialog\n\n"
            "    --binary, -b   Output in binary mode, not CSV\n\n"
            "    -o<filename>   Write output on <filename>\n\n");
}

int collect_data(WData *data, FILE *f) {
    char buff[BUFF_SIZE];
    do{
        int year, month;
        double temp, prec;
        if(fgets(buff, BUFF_SIZE, f) == NULL)
            break;

        if (sscanf(buff, "%d\t%d\t%lf\t%lf", &year, &month, &temp, &prec) != 4)
            continue;
        
        if(month < 1 || month > MONTHS || year < 0)
            exit(ERROR_DATA);
        
        if(data->start == 0)
            data->start = year;

        const int index_year = year - data->start;

        data->years[index_year].year = year;
        data->years[index_year].months[month-1].temperature = temp;
        data->years[index_year].months[month-1].precipitations = prec;
    
    }while (true);
    
    return 0;
}

void process_data(WData *data) {

    for (int j = 0; j < MAX_ENTRIES; j++){

        if(data->years[j].year == 0)
            continue;

        double moy_temp = 0.0;
        double moy_prec = 0.0;

        for (int i = 0; i < MONTHS; i++){
            moy_temp += data->years[j].months[i].temperature;
            moy_prec += data->years[j].months[i].precipitations;
        }

        moy_temp = moy_temp / MONTHS;
        data->years[j].temperature = moy_temp;
        data->years[j].precipitations = moy_prec;

    }
}

void fprint_csv(FILE *fp, WData *data) {}

void fprint_binary(FILE *fp, WData *data) {}

void process_arg(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++)
    {
        if (strncmp(argv[i], "--version", 9) == 0)
            version(stdout);

        if (strncmp(argv[i], "--help", 6) == 0)
            help(stdout);
    }
}

int main(int argc, char *argv[])
{
    char* filename = argv[1];
    process_arg(argc, argv);
    FILE* file = fopen(filename,"r");
    WData d = {0};
    collect_data(&d, file);
    fclose(file);
    process_data(&d);

    printf("year;temperature;precipitations\n");
    for (int i = 0; i < MAX_ENTRIES; i++){
        if(d.years[i].year == 0)
            continue;

        printf("%d;%g;%g\n",d.years[i].year, d.years[i].temperature, d.years[i].precipitations);
    }
    

    return 0;
}

