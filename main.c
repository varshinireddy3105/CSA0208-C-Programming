/**
 * =====================================================================================
 * Automobile Service Centre Management System
 * Course Outcomes: CO 3, CO 4 & CO 5 (Bloom's Levels 3, 4, 5 & 6)
 * Language: C (C99 Standard)
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MAX_RECORDS 100
#define STR_LEN 50
#define FILE_NAME "service_records.dat"

/* =====================================================================================
 * STRUCTURE DEFINITIONS
 * Requirement (a): Structure for Date and Vehicle Service Record
 * =====================================================================================
 */

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    char vehicle_number[20];
    char customer_name[STR_LEN];
    char vehicle_model[STR_LEN];
    char service_type[STR_LEN];
    Date service_date;
    double parts_cost;
    double labour_cost;
    double total_service_cost;
} VehicleService;

/* =====================================================================================
 * FUNCTION DECLARATIONS
 * =====================================================================================
 */

// Core Utility & Pointer Functions - Requirement (b)
void calculate_total_cost(VehicleService *record);
void update_service_record(VehicleService *record, const char *new_model, const char *new_service_type, 
                           Date new_date, double new_parts_cost, double new_labour_cost);

// Validation Functions - Requirement (d)
bool is_leap_year(int year);
bool is_valid_date(Date date);
bool is_valid_cost(double cost);
int search_vehicle_index(const VehicleService records[], int count, const char *vnum);
void to_upper_str(char *str);
void read_string(const char *prompt, char *buffer, int max_len);
double read_double(const char *prompt);
Date read_date(const char *prompt);

// Menu Core Features - Requirement (c)
void register_vehicle(VehicleService records[], int *count);
void search_vehicle_menu(const VehicleService records[], int count);
void update_vehicle_menu(VehicleService records[], int count);
void calculate_bill_menu(const VehicleService records[], int count);
void sort_vehicles_by_cost(VehicleService records[], int count);
void display_high_cost_vehicles(const VehicleService records[], int count);
void generate_daily_summary(const VehicleService records[], int count);
int save_records_to_file(const VehicleService records[], int count, const char *filename);
int load_records_from_file(VehicleService records[], int *count, const char *filename);

// Helper Display Functions
void print_header(void);
void print_record(const VehicleService *rec, int index);
void quick_sort_records(VehicleService records[], int low, int high);
int partition(VehicleService records[], int low, int high);

/* =====================================================================================
 * IMPLEMENTATION: POINTER & COST CALCULATION FUNCTIONS
 * Requirement (b): Pointer-based record modification and cost computation
 * =====================================================================================
 */

/**
 * Calculates total service cost using pointer to VehicleService structure.
 * Total Cost = Parts Cost + Labour Cost
 */
void calculate_total_cost(VehicleService *record) {
    if (record != NULL) {
        record->total_service_cost = record->parts_cost + record->labour_cost;
    }
}

/**
 * Updates an existing vehicle service record in-place via structure pointer.
 */
void update_service_record(VehicleService *record, const char *new_model, const char *new_service_type, 
                           Date new_date, double new_parts_cost, double new_labour_cost) {
    if (record == NULL) return;

    if (new_model && strlen(new_model) > 0) {
        strncpy(record->vehicle_model, new_model, sizeof(record->vehicle_model) - 1);
        record->vehicle_model[sizeof(record->vehicle_model) - 1] = '\0';
    }
    if (new_service_type && strlen(new_service_type) > 0) {
        strncpy(record->service_type, new_service_type, sizeof(record->service_type) - 1);
        record->service_type[sizeof(record->service_type) - 1] = '\0';
    }
    record->service_date = new_date;
    record->parts_cost = new_parts_cost;
    record->labour_cost = new_labour_cost;

    // Recalculate total cost automatically after updating components
    calculate_total_cost(record);
}

/* =====================================================================================
 * IMPLEMENTATION: VALIDATION & INPUT HELPERS
 * Requirement (d): Validation logic for Dates, Costs, Strings, and Edge Cases
 * =====================================================================================
 */

bool is_leap_year(int year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

bool is_valid_date(Date date) {
    if (date.year < 2000 || date.year > 2100) return false;
    if (date.month < 1 || date.month > 12) return false;

    int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (date.month == 2 && is_leap_year(date.year)) {
        days_in_month[2] = 29;
    }

    return (date.day >= 1 && date.day <= days_in_month[date.month]);
}

bool is_valid_cost(double cost) {
    return cost >= 0.0;
}

void to_upper_str(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = (char)toupper((unsigned char)str[i]);
    }
}

void read_string(const char *prompt, char *buffer, int max_len) {
    printf("%s", prompt);
    if (fgets(buffer, max_len, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            // Clear standard input buffer if string exceeded max_len
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    }
}

double read_double(const char *prompt) {
    double val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%lf", &val) == 1 && is_valid_cost(val)) {
                return val;
            }
        }
        printf(" Error: Invalid cost! Please enter a non-negative number.\n");
    }
}

Date read_date(const char *prompt) {
    Date date;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d-%d-%d", &date.day, &date.month, &date.year) == 3 ||
                sscanf(buffer, "%d/%d/%d", &date.day, &date.month, &date.year) == 3) {
                if (is_valid_date(date)) {
                    return date;
                }
            }
        }
        printf(" Error: Invalid date! Please use DD-MM-YYYY format (Year 2000-2100).\n");
    }
}

int search_vehicle_index(const VehicleService records[], int count, const char *vnum) {
    char target[20];
    strncpy(target, vnum, sizeof(target) - 1);
    target[sizeof(target) - 1] = '\0';
    to_upper_str(target);

    for (int i = 0; i < count; i++) {
        char current[20];
        strncpy(current, records[i].vehicle_number, sizeof(current) - 1);
        current[sizeof(current) - 1] = '\0';
        to_upper_str(current);
        if (strcmp(current, target) == 0) {
            return i;
        }
    }
    return -1;
}

/* =====================================================================================
 * MENU FUNCTIONALITIES
 * Requirement (c): Register, Search, Update, Calculate, Sort, Filter, Summary, File I/O
 * =====================================================================================
 */

/**
 * 1. Register a new vehicle for service.
 * Handles Duplicate Vehicle Check and Array Capacity Limit.
 */
void register_vehicle(VehicleService records[], int *count) {
    printf("\n--- REGISTER NEW VEHICLE ---\n");
    
    // Check Maximum Record Capacity Requirement (d)
    if (*count >= MAX_RECORDS) {
        printf(" Error: Cannot register vehicle. System maximum capacity (%d) reached!\n", MAX_RECORDS);
        return;
    }

    char vnum[20];
    read_string("Enter Vehicle Number (e.g. KA01AB1234): ", vnum, sizeof(vnum));
    to_upper_str(vnum);

    // Check Duplicate Vehicle Requirement (d)
    if (search_vehicle_index(records, *count, vnum) != -1) {
        printf(" Error: Duplicate Registration! Vehicle Number '%s' is already registered.\n", vnum);
        return;
    }

    VehicleService new_rec;
    strncpy(new_rec.vehicle_number, vnum, sizeof(new_rec.vehicle_number) - 1);
    new_rec.vehicle_number[sizeof(new_rec.vehicle_number) - 1] = '\0';

    read_string("Enter Customer Name: ", new_rec.customer_name, sizeof(new_rec.customer_name));
    read_string("Enter Vehicle Model (e.g. Honda City): ", new_rec.vehicle_model, sizeof(new_rec.vehicle_model));
    read_string("Enter Service Type (e.g. Full Service, Oil Change): ", new_rec.service_type, sizeof(new_rec.service_type));
    
    new_rec.service_date = read_date("Enter Service Date (DD-MM-YYYY): ");
    new_rec.parts_cost = read_double("Enter Parts Cost (INR): ");
    new_rec.labour_cost = read_double("Enter Labour Cost (INR): ");

    // Use Pointer Function to calculate total cost
    calculate_total_cost(&new_rec);

    records[*count] = new_rec;
    (*count)++;

    printf(" Success: Vehicle '%s' registered successfully! Total Cost: INR %.2f\n", 
           new_rec.vehicle_number, new_rec.total_service_cost);
}

/**
 * 2. Search for a vehicle by vehicle number.
 */
void search_vehicle_menu(const VehicleService records[], int count) {
    printf("\n--- SEARCH VEHICLE RECORD ---\n");
    if (count == 0) {
        printf(" No vehicle records registered yet.\n");
        return;
    }

    char vnum[20];
    read_string("Enter Vehicle Number to Search: ", vnum, sizeof(vnum));
    int idx = search_vehicle_index(records, count, vnum);

    if (idx == -1) {
        printf(" Search Result: Vehicle '%s' not found in system.\n", vnum);
    } else {
        printf("\nVehicle Record Found:\n");
        print_header();
        print_record(&records[idx], idx + 1);
    }
}

/**
 * 3. Update vehicle service details.
 */
void update_vehicle_menu(VehicleService records[], int count) {
    printf("\n--- UPDATE SERVICE DETAILS ---\n");
    if (count == 0) {
        printf(" No vehicle records registered yet.\n");
        return;
    }

    char vnum[20];
    read_string("Enter Vehicle Number to Update: ", vnum, sizeof(vnum));
    int idx = search_vehicle_index(records, count, vnum);

    if (idx == -1) {
        printf(" Error: Vehicle '%s' not found.\n", vnum);
        return;
    }

    printf("\nCurrent Details for %s:\n", records[idx].vehicle_number);
    print_header();
    print_record(&records[idx], 1);

    printf("\nEnter Updated Details:\n");
    char new_model[STR_LEN];
    char new_service_type[STR_LEN];
    read_string("Enter New Vehicle Model: ", new_model, sizeof(new_model));
    read_string("Enter New Service Type: ", new_service_type, sizeof(new_service_type));
    Date new_date = read_date("Enter New Service Date (DD-MM-YYYY): ");
    double new_parts = read_double("Enter New Parts Cost (INR): ");
    double new_labour = read_double("Enter New Labour Cost (INR): ");

    // Use Pointer Function to update service record
    update_service_record(&records[idx], new_model, new_service_type, new_date, new_parts, new_labour);

    printf(" Success: Vehicle service details updated successfully!\n");
}

/**
 * 4. Calculate total bill for a specific vehicle.
 */
void calculate_bill_menu(const VehicleService records[], int count) {
    printf("\n--- CALCULATE TOTAL BILL ---\n");
    if (count == 0) {
        printf(" No vehicle records registered yet.\n");
        return;
    }

    char vnum[20];
    read_string("Enter Vehicle Number for Invoice: ", vnum, sizeof(vnum));
    int idx = search_vehicle_index(records, count, vnum);

    if (idx == -1) {
        printf(" Error: Vehicle '%s' not found.\n", vnum);
        return;
    }

    const VehicleService *rec = &records[idx];
    printf("\n==================================================\n");
    printf("              AUTOMOBILE SERVICE INVOICE           \n");
    printf("==================================================\n");
    printf(" Vehicle Number : %s\n", rec->vehicle_number);
    printf(" Customer Name  : %s\n", rec->customer_name);
    printf(" Model          : %s\n", rec->vehicle_model);
    printf(" Service Type   : %s\n", rec->service_type);
    printf(" Service Date   : %02d-%02d-%04d\n", rec->service_date.day, rec->service_date.month, rec->service_date.year);
    printf("--------------------------------------------------\n");
    printf(" Parts Cost     : INR %10.2f\n", rec->parts_cost);
    printf(" Labour Cost    : INR %10.2f\n", rec->labour_cost);
    printf("--------------------------------------------------\n");
    printf(" TOTAL BILL     : INR %10.2f\n", rec->total_service_cost);
    printf("==================================================\n");
}

/* =====================================================================================
 * SORTING ALGORITHM IMPLEMENTATION (QUICKSORT)
 * Requirement (e): Fast sorting based on total service cost
 * =====================================================================================
 */

int partition(VehicleService records[], int low, int high) {
    double pivot = records[high].total_service_cost;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        // Sort in descending order of service cost
        if (records[j].total_service_cost > pivot) {
            i++;
            VehicleService temp = records[i];
            records[i] = records[j];
            records[j] = temp;
        }
    }
    VehicleService temp = records[i + 1];
    records[i + 1] = records[high];
    records[high] = temp;
    return (i + 1);
}

void quick_sort_records(VehicleService records[], int low, int high) {
    if (low < high) {
        int pi = partition(records, low, high);
        quick_sort_records(records, low, pi - 1);
        quick_sort_records(records, pi + 1, high);
    }
}

/**
 * 5. Sort vehicles based on service cost.
 */
void sort_vehicles_by_cost(VehicleService records[], int count) {
    printf("\n--- SORT VEHICLES BY SERVICE COST ---\n");
    if (count == 0) {
        printf(" No records available to sort.\n");
        return;
    }

    quick_sort_records(records, 0, count - 1);
    printf(" Success: Sorted %d vehicle record(s) by total service cost (Descending Order).\n\n", count);

    print_header();
    for (int i = 0; i < count; i++) {
        print_record(&records[i], i + 1);
    }
}

/**
 * 6. Display vehicles whose service cost exceeds a specified amount.
 */
void display_high_cost_vehicles(const VehicleService records[], int count) {
    printf("\n--- DISPLAY HIGH-COST SERVICES ---\n");
    if (count == 0) {
        printf(" No vehicle records available.\n");
        return;
    }

    double threshold = read_double("Enter Service Cost Threshold (INR): ");
    int matches = 0;

    printf("\nVehicles with Service Cost Exceeding INR %.2f:\n", threshold);
    print_header();
    for (int i = 0; i < count; i++) {
        if (records[i].total_service_cost > threshold) {
            print_record(&records[i], matches + 1);
            matches++;
        }
    }

    if (matches == 0) {
        printf(" No vehicles found exceeding the cost threshold of INR %.2f.\n", threshold);
    } else {
        printf("\n Found %d matching vehicle(s).\n", matches);
    }
}

/**
 * 7. Generate a daily service summary report.
 */
void generate_daily_summary(const VehicleService records[], int count) {
    printf("\n--- GENERATE DAILY SERVICE SUMMARY REPORT ---\n");
    if (count == 0) {
        printf(" No records registered yet for summary generation.\n");
        return;
    }

    Date target_date = read_date("Enter Target Summary Date (DD-MM-YYYY): ");
    int daily_count = 0;
    double total_revenue = 0.0;
    double total_parts = 0.0;
    double total_labour = 0.0;
    double max_cost = -1.0;
    char highest_paying_vnum[20] = "N/A";

    printf("\nService Summary for Date: %02d-%02d-%04d\n", target_date.day, target_date.month, target_date.year);
    print_header();

    for (int i = 0; i < count; i++) {
        if (records[i].service_date.day == target_date.day &&
            records[i].service_date.month == target_date.month &&
            records[i].service_date.year == target_date.year) {
            
            print_record(&records[i], daily_count + 1);
            daily_count++;
            total_parts += records[i].parts_cost;
            total_labour += records[i].labour_cost;
            total_revenue += records[i].total_service_cost;

            if (records[i].total_service_cost > max_cost) {
                max_cost = records[i].total_service_cost;
                strcpy(highest_paying_vnum, records[i].vehicle_number);
            }
        }
    }

    if (daily_count == 0) {
        printf(" No services registered on date %02d-%02d-%04d.\n", target_date.day, target_date.month, target_date.year);
        return;
    }

    printf("========================================================\n");
    printf("                 DAILY SUMMARY STATISTICS               \n");
    printf("========================================================\n");
    printf(" Total Vehicles Serviced    : %d\n", daily_count);
    printf(" Total Parts Revenue        : INR %10.2f\n", total_parts);
    printf(" Total Labour Revenue       : INR %10.2f\n", total_labour);
    printf(" TOTAL DAILY REVENUE        : INR %10.2f\n", total_revenue);
    printf(" Average Service Cost       : INR %10.2f\n", total_revenue / daily_count);
    printf(" Highest Service Bill       : INR %10.2f (Vehicle: %s)\n", max_cost, highest_paying_vnum);
    printf("========================================================\n");
}

/* =====================================================================================
 * FILE HANDLING IMPLEMENTATION & REDESIGN
 * Requirement (c) & (f): Persistent binary file storage & graceful error handling
 * =====================================================================================
 */

/**
 * 8. Store service records into binary file safely.
 */
int save_records_to_file(const VehicleService records[], int count, const char *filename) {
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf(" Error (File I/O): Failed to open file '%s' for writing.\n", filename);
        return 0;
    }

    // Write record count first
    if (fwrite(&count, sizeof(int), 1, fp) != 1) {
        printf(" Error (File I/O): Failed to write record header.\n");
        fclose(fp);
        return 0;
    }

    // Write all vehicle structures
    size_t written = fwrite(records, sizeof(VehicleService), count, fp);
    fclose(fp);

    if (written != (size_t)count) {
        printf(" Warning: Partial write occurred (%zu of %d records written).\n", written, count);
        return 0;
    }

    printf(" Success: Saved %d service record(s) to '%s'.\n", count, filename);
    return 1;
}

/**
 * 9. Load previous records from file when application starts or upon request.
 * Requirement (f): Graceful handling of missing files and corrupted data.
 */
int load_records_from_file(VehicleService records[], int *count, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        // File does not exist yet (Normal behavior on initial run)
        *count = 0;
        return 0;
    }

    int file_count = 0;
    if (fread(&file_count, sizeof(int), 1, fp) != 1 || file_count < 0) {
        printf(" Warning: Corrupted file header in '%s'. Starting with empty records.\n", filename);
        fclose(fp);
        *count = 0;
        return 0;
    }

    if (file_count > MAX_RECORDS) {
        printf(" Warning: File contains %d records, exceeding capacity (%d). Truncating.\n", 
               file_count, MAX_RECORDS);
        file_count = MAX_RECORDS;
    }

    size_t read_cnt = fread(records, sizeof(VehicleService), file_count, fp);
    fclose(fp);

    *count = (int)read_cnt;
    printf(" Success: Loaded %d previous service record(s) from '%s'.\n", *count, filename);
    return 1;
}

/* =====================================================================================
 * PRESENTATION HELPERS & DISPLAY TABLE
 * =====================================================================================
 */

void print_header(void) {
    printf("+----+------------+----------------------+-------------------+-------------------+------------+------------+------------+------------+\n");
    printf("| S# | Veh Number | Customer Name        | Vehicle Model     | Service Type      | Date       | Parts(INR) | Labour(INR)| Total(INR) |\n");
    printf("+----+------------+----------------------+-------------------+-------------------+------------+------------+------------+------------+\n");
}

void print_record(const VehicleService *rec, int index) {
    printf("| %-2d | %-10s | %-20.20s | %-17.17s | %-17.17s | %02d-%02d-%04d | %10.2f | %10.2f | %10.2f |\n",
           index, rec->vehicle_number, rec->customer_name, rec->vehicle_model,
           rec->service_type, rec->service_date.day, rec->service_date.month,
           rec->service_date.year, rec->parts_cost, rec->labour_cost, rec->total_service_cost);
    printf("+----+------------+----------------------+-------------------+-------------------+------------+------------+------------+------------+\n");
}

/* =====================================================================================
 * MAIN MENU & DRIVER SYSTEM
 * Requirement (c): Menu-Driven Application
 * =====================================================================================
 */

int main(void) {
    VehicleService records[MAX_RECORDS];
    int record_count = 0;

    printf("=====================================================================================\n");
    printf("             AUTOMOBILE SERVICE CENTRE MANAGEMENT SYSTEM (C-BASED)                   \n");
    printf("=====================================================================================\n");
    
    // Automatically load previous records on application start - Requirement (c) & (f)
    load_records_from_file(records, &record_count, FILE_NAME);

    int choice;
    char choice_buf[50];

    while (1) {
        printf("\n=====================================================================================\n");
        printf("                                    MAIN MENU                                        \n");
        printf("=====================================================================================\n");
        printf(" 1. Register a Vehicle for Service\n");
        printf(" 2. Search Vehicle by Vehicle Number\n");
        printf(" 3. Update Service Details\n");
        printf(" 4. Calculate Total Bill (Generate Invoice)\n");
        printf(" 5. Sort Vehicles based on Service Cost (QuickSort)\n");
        printf(" 6. Display Vehicles Exceeding Specified Cost Threshold\n");
        printf(" 7. Generate Daily Service Summary Report\n");
        printf(" 8. Store Service Records to File\n");
        printf(" 9. Reload Previous Records from File\n");
        printf(" 0. Save & Exit Application\n");
        printf("=====================================================================================\n");
        printf("Enter your choice (0-9): ");

        if (fgets(choice_buf, sizeof(choice_buf), stdin) == NULL) {
            break;
        }

        if (sscanf(choice_buf, "%d", &choice) != 1) {
            printf(" Error: Invalid choice! Please enter a number between 0 and 9.\n");
            continue;
        }

        switch (choice) {
            case 1:
                register_vehicle(records, &record_count);
                break;
            case 2:
                search_vehicle_menu(records, record_count);
                break;
            case 3:
                update_vehicle_menu(records, record_count);
                break;
            case 4:
                calculate_bill_menu(records, record_count);
                break;
            case 5:
                sort_vehicles_by_cost(records, record_count);
                break;
            case 6:
                display_high_cost_vehicles(records, record_count);
                break;
            case 7:
                generate_daily_summary(records, record_count);
                break;
            case 8:
                save_records_to_file(records, record_count, FILE_NAME);
                break;
            case 9:
                load_records_from_file(records, &record_count, FILE_NAME);
                break;
            case 0:
                printf("\nSaving records to '%s' before exit...\n", FILE_NAME);
                save_records_to_file(records, record_count, FILE_NAME);
                printf("Thank you for using Automobile Service Centre Management System. Goodbye!\n");
                return 0;
            default:
                printf(" Error: Invalid choice! Please enter a number between 0 and 9.\n");
        }
    }

    return 0;
}
