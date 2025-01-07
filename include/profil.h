#ifndef PROFIL_H
#define PROFIL_H

#include <stdint.h>

typedef enum {
    NONE,
    ONE,
    TWO,
    THREE
}numberOfResistance;

typedef enum {
    AILE,
    PREPREG,
    CUSTOM
}profilName;

typedef struct {
    uint8_t temperature;
    uint64_t duration;
    numberOfResistance activeResistance;
}phase;

typedef struct {
   phase phase1;
   phase phase2;
   phase phase3;
}cookingProfil;

// Enum to define states
typedef enum {
    STATE_IDLE,
    STATE_ERROR,
    STATE_PROFIL_SELECT,
    STATE_PHASE1,
    STATE_PHASE2,
    STATE_PHASE3,
    COOLING
} cookingState;

// Function declarations
void idle_state(void);
void profil_select_state(void);
void phase1_state(void);
void phase2_state(void);
void phase3_state(void);


void profil_init(cookingProfil * self, uint8_t Temp1, uint64_t duration1, numberOfResistance power1, uint8_t Temp2, uint64_t duration2, numberOfResistance power2, uint8_t Temp3, uint64_t duration3, numberOfResistance power3);


#endif
