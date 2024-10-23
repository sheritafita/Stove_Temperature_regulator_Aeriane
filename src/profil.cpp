#include"profil.h"




void profil_init (cookingProfil * self, uint8_t Temp1, uint64_t duration1, numberOfResistance power1, uint8_t Temp2, uint64_t duration2, numberOfResistance power2, uint8_t Temp3, uint64_t duration3, numberOfResistance power3)
{
#if 1
    self->phase1.temperature = Temp1;
    self->phase1.duration = duration1*3600000; // hour to ms
    self->phase1.activeResistance = power1;   
    self->phase2.temperature = Temp2;
    self->phase2.duration = duration2*3600000; // hour to ms
    self->phase2.activeResistance=power2;
    self->phase3.temperature = Temp3;
    self->phase3.duration = duration3*3600000; // hour to ms
    self->phase3.activeResistance=power3;
#else
self->phase1.temperature = Temp1;
    self->phase1.duration = duration1; 
    self->phase1.activeResistance = power1;   
    self->phase2.temperature = Temp2;
    self->phase2.duration = duration2; 
    self->phase2.activeResistance=power2;
    self->phase3.temperature = Temp3;
    self->phase3.duration = duration3; 
    self->phase3.activeResistance=power3;
#endif
}



