#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> // Required for fabs() and trigonometric calculations

#define INITIAL_CAPACITY 2
#define MAX_SIGNATURES 6
#define BINARY_FILE "sonar_database.bin"
#define MATCH_TOLERANCE_HZ 20.0f
#define TORPEDO_SPEED_KNOTS 45.0f // Standard heavy torpedo speed

// Struct 1: Signature entry in database
struct AcousticSignature {
    char vessel_class[32];
    float base_freq_hz;
    int is_threat;
};

// Struct 2: Active contact record in memory
struct SonarContact {
    int contact_id;
    float detected_freq;
    float doppler_shift;
    float bearing_deg;
    float range_nm;
    char matched_class[32];
    int threat_level;
};

// Function prototypes
void clear_input_buffer(void);
void get_safe_string(char *buffer, int size);
void display_menu(void);
void init_signature_library(struct AcousticSignature *lib);
int match_signature(const struct AcousticSignature *lib, int lib_size, float freq);
void add_contact(struct SonarContact **contacts, int *size, int *capacity, const struct AcousticSignature *lib, int lib_size);
void display_contacts(const struct SonarContact *contacts, int size);
void calculate_fire_solution(const struct SonarContact *contacts, int size);
void save_contacts_to_file(const struct SonarContact *contacts, int size, const char *filename);
void load_contacts_from_file(struct SonarContact **contacts, int *size, int *capacity, const char *filename);
void free_contact_memory(struct SonarContact **contacts, int *size, int *capacity);

int main(void) {
    int size = 0;
    int capacity = INITIAL_CAPACITY;
    
    // Allocate initial dynamic memory on the heap
    struct SonarContact *contacts = (struct SonarContact *)malloc(capacity * sizeof(struct SonarContact));
    if (contacts == NULL) {
        fprintf(stderr, "[ERR] Fatal: Memory allocation failed on startup.\n");
        return 1;
    }

    // Initialize static acoustic signature database
    struct AcousticSignature sig_library[MAX_SIGNATURES];
    init_signature_library(sig_library);

    // Auto-load existing records from binary file on startup (if present)
    load_contacts_from_file(&contacts, &size, &capacity, BINARY_FILE);

    int choice = 0;
    char input_buf[16];

    while (choice != 5) {
        display_menu();
        printf("Select Option (1-5): ");
        
        if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
            choice = atoi(input_buf);
        } else {
            choice = 0;
        }

        switch (choice) {
            case 1:
                add_contact(&contacts, &size, &capacity, sig_library, MAX_SIGNATURES);
                // Auto-save to disk after adding a new contact
                save_contacts_to_file(contacts, size, BINARY_FILE);
                break;
            case 2:
                display_contacts(contacts, size);
                break;
            case 3:
                calculate_fire_solution(contacts, size);
                break;
            case 4:
                save_contacts_to_file(contacts, size, BINARY_FILE);
                printf("[SYS] Contact records manually persisted to %s.\n", BINARY_FILE);
                break;
            case 5:
                // Final auto-save before exit
                save_contacts_to_file(contacts, size, BINARY_FILE);
                printf("[SYS] Shutting down Sonar Fire-Control System...\n");
                break;
            default:
                printf("\n[ERR] Invalid selection. Enter a number between 1 and 5.\n");
        }
    }

    // Clean up heap allocation before exit
    free_contact_memory(&contacts, &size, &capacity);
    return 0;
}

// Pre-populates default vessel acoustic profiles
void init_signature_library(struct AcousticSignature *lib) {
    //vessel 1
    strncpy((lib + 0)->vessel_class, "Akula-class Submarine", 32);
    (lib + 0)->base_freq_hz = 120.0f;
    (lib + 0)->is_threat = 1;

    //vessel 2
    strncpy((lib + 1)->vessel_class, "Cargo Container Ship", 32);
    (lib + 1)->base_freq_hz = 60.0f;
    (lib + 1)->is_threat = 0;

    //vessel 3
    strncpy((lib + 2)->vessel_class, "Arleigh Burke Destroyer", 32);
    (lib + 2)->base_freq_hz = 210.0f;
    (lib + 2)->is_threat = 1;

    //vessel 4
    strncpy((lib + 3)->vessel_class, "Research Vessel", 32);
    (lib + 3)->base_freq_hz = 90.0f;
    (lib + 3)->is_threat = 0;

    //vessel 5
    strncpy((lib + 4)->vessel_class, "Fishing Vessel", 32);
    (lib + 4)->base_freq_hz = 75.0f;
    (lib + 4)->is_threat = 0;

    //vessel 6
    strncpy((lib + 5)->vessel_class, "Passenger Ship", 32);
    (lib + 5)->base_freq_hz = 150.0f;
    (lib + 5)->is_threat = 0;
}

// Finds closest matching signature using pointer arithmetic and fabs()
int match_signature(const struct AcousticSignature *lib, int lib_size, float freq) {
    int best_index = 0;
    float min_diff = fabs(freq - lib->base_freq_hz);

    for (int i = 1; i < lib_size; i++) {
        float diff = fabs(freq - (lib + i)->base_freq_hz);
        if (diff < min_diff) {
            min_diff = diff;
            best_index = i;
        }
    }
    return (min_diff <=MATCH_TOLERANCE_HZ) ? best_index : -1;
}

// Dynamically inserts contact and resizes buffer using realloc when capacity is reached
void add_contact(struct SonarContact **contacts, int *size, int *capacity, const struct AcousticSignature *lib, int lib_size) {
    if (*size >= *capacity) {
        int new_capacity = (*capacity) * 2;
        struct SonarContact *temp = (struct SonarContact *)realloc(*contacts, new_capacity * sizeof(struct SonarContact));
        if (temp == NULL) {
            fprintf(stderr, "[ERR] Memory reallocation failed! Keeping existing list.\n");
            return;
        }
        *contacts = temp;
        *capacity = new_capacity;
        printf("[MEM] Dynamic capacity expanded to %d entries.\n", *capacity);
    }

    struct SonarContact *new_entry = *contacts + *size;
    char input_buf[32];

    printf("\n--- ACOUSTIC FREQUENCY SCANNER ---\n");
    new_entry->contact_id = 101 + *size;

    printf("Enter Detected Frequency (Hz): ");
    if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
        new_entry->detected_freq = atof(input_buf);
    }

    printf("Enter Bearing Angle (0.0 to 359.9 degrees): ");
    if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
        new_entry->bearing_deg = atof(input_buf);
    }

    printf("Enter Estimated Distance (Nautical Miles): ");
    if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
        new_entry->range_nm = atof(input_buf);
    }

    int match_idx = match_signature(lib, lib_size, new_entry->detected_freq);
    if (match_idx == -1){
        printf("[SYS] Unknown Vessel: no matching profile found.\n");
        strncpy(new_entry->matched_class, "Unknown Vessel", 32);
        new_entry->doppler_shift = 0.0f;
        new_entry->threat_level = 0;
    }
    else {
        strncpy(new_entry->matched_class, (lib + match_idx)->vessel_class, 32);
    new_entry->doppler_shift = new_entry->detected_freq - (lib + match_idx)->base_freq_hz;

    if ((lib + match_idx)->is_threat) {
        if (new_entry->doppler_shift > 0.0f) {
            new_entry->threat_level = 3; // Critical
        } else {
            new_entry->threat_level = 2; // Medium
        }
    } else {
        new_entry->threat_level = 1;     // Low
    }

    (*size)++;
    printf("[SYS] Contact #%d logged successfully as %s.\n", new_entry->contact_id, new_entry->matched_class);
}
}

// Displays contacts in tabular tactical layout
void display_contacts(const struct SonarContact *contacts, int size) {
    if (size == 0) {
        printf("\n[SYS] No active sonar contacts detected in immediate sector.\n");
        return;
    }

    printf("\n========================================================================================\n");
    printf("                               TACTICAL SONAR CONTACT ROSTER                            \n");
    printf("========================================================================================\n");
    printf("%-6s | %-24s | %-9s | %-8s | %-8s | %-12s\n", 
           "ID", "Matched Class", "Freq (Hz)", "Shift", "Range", "Threat Level");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < size; i++) {
        const struct SonarContact *c = contacts + i;
        char threat_str[16];
        if (c->threat_level == 3) strncpy(threat_str, "CRITICAL", 16);
        else if (c->threat_level == 2) strncpy(threat_str, "MEDIUM", 16);
        else strncpy(threat_str, "LOW", 16);

        printf("#%-5d | %-24s | %-9.1f | %-+8.1f | %-6.1f NM | %-12s\n",
               c->contact_id, c->matched_class, c->detected_freq, 
               c->doppler_shift, c->range_nm, threat_str);
    }
    printf("========================================================================================\n");
}

// Computes torpedo intercept run time and lead firing solution
void calculate_fire_solution(const struct SonarContact *contacts, int size) {
    if (size == 0) {
        printf("\n[SYS] Error: No targets available for fire-control calculation.\n");
        return;
    }

    display_contacts(contacts, size);
    char input_buf[16];
    int target_id = 0;

    printf("\nEnter Target ID to generate torpedo fire solution: ");
    if (fgets(input_buf, sizeof(input_buf), stdin) != NULL) {
        target_id = atoi(input_buf);
    }

    // Search for target by ID using pointer iteration
    const struct SonarContact *target = NULL;
    for (int i = 0; i < size; i++) {
        if ((contacts + i)->contact_id == target_id) {
            target = contacts + i;
            break;
        }
    }

    if (target == NULL) {
        printf("\n[ERR] Target ID #%d not found in active roster.\n", target_id);
        return;
    }

    // Fire control physics calculation:
    // Time (seconds) = (Range in NM / Torpedo Speed in knots) * 3600 seconds/hour
    float run_time_sec = (target->range_nm / TORPEDO_SPEED_KNOTS) * 3600.0f;
    
    // Adjust lead angle based on Doppler shift velocity factor
    float lead_angle_correction = target->doppler_shift * 0.15f; 
    float firing_bearing = target->bearing_deg + lead_angle_correction;

    // Normalize bearing within 0-360 degrees using fmod from math.h
    firing_bearing = fmod(firing_bearing + 360.0f, 360.0f);

    printf("\n====================================================\n");
    printf("            TORPEDO FIRE-CONTROL SOLUTION           \n");
    printf("====================================================\n");
    printf(" Target Contact    : #%d (%s)\n", target->contact_id, target->matched_class);
    printf(" Current Bearing   : %.1f°\n", target->bearing_deg);
    printf(" Target Range      : %.2f Nautical Miles\n", target->range_nm);
    printf(" Doppler Status    : %+.1f Hz (%s)\n", target->doppler_shift, 
           target->doppler_shift > 0 ? "CLOSING IN" : "OPENING RANGE");
    printf("----------------------------------------------------\n");
    printf(" RECOMMENDED SOLUTION:\n");
    printf(" -> Torpedo Launch Bearing : %.1f°\n", firing_bearing);
    printf(" -> Estimated Impact Time  : %.1f seconds\n", run_time_sec);
    printf("====================================================\n");
}

// Persists active contact records to binary file using fwrite
void save_contacts_to_file(const struct SonarContact *contacts, int size, const char *filename) {
    FILE *fp = fopen(filename, "wb"); // Write Binary mode
    if (fp == NULL) {
        fprintf(stderr, "[ERR] Failed to open file %s for writing.\n", filename);
        return;
    }

    // First write the total active contact count (int)
    fwrite(&size, sizeof(int), 1, fp);

    // Then write the entire dynamic array of structs in a single block
    if (size > 0) {
        fwrite(contacts, sizeof(struct SonarContact), size, fp);
    }

    fclose(fp);
}

// Loads contact records from binary file using fread and reallocates memory
void load_contacts_from_file(struct SonarContact **contacts, int *size, int *capacity, const char *filename) {
    FILE *fp = fopen(filename, "rb"); // Read Binary mode
    if (fp == NULL) {
        // Non-fatal: File doesn't exist yet on first run
        return;
    }

    int loaded_size = 0;
    // Read the stored count
    if (fread(&loaded_size, sizeof(int), 1, fp) != 1) {
        fclose(fp);
        return;
    }

    // Expand capacity if loaded size exceeds current capacity
    if (loaded_size > *capacity) {
        int new_capacity = loaded_size * 2;
        struct SonarContact *temp = (struct SonarContact *)realloc(*contacts, new_capacity * sizeof(struct SonarContact));
        if (temp != NULL) {
            *contacts = temp;
            *capacity = new_capacity;
        }
    }

    // Read the array records directly into heap memory
    size_t items_read = fread(*contacts, sizeof(struct SonarContact), loaded_size, fp);
    *size = (int)items_read;

    fclose(fp);
    printf("[SYS] Successfully loaded %d contact records from %s.\n", *size, filename);
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void get_safe_string(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        } else {
            clear_input_buffer();
        }
    }
}

void display_menu(void) {
    printf("\n====================================================\n");
    printf("    NAVY SUBMARINE SONAR & FIRE-CONTROL SYSTEM    \n");
    printf("====================================================\n");
    printf(" 1. Scan Acoustic Signature & Log Contact\n");
    printf(" 2. Display Active Sonar Contact Roster\n");
    printf(" 3. Compute Torpedo Intercept Solution\n");
    printf(" 4. Manually Save Contacts to Binary File\n");
    printf(" 5. Exit System\n");
    printf("====================================================\n");
}

void free_contact_memory(struct SonarContact **contacts, int *size, int *capacity) {
    if (contacts != NULL && *contacts != NULL) {
        free(*contacts);
        *contacts = NULL;
        *size = 0;
        *capacity = 0;
        printf("[MEM] Dynamic memory successfully freed.\n");
    }
}


