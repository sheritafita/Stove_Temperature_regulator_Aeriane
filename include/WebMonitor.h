


#ifndef __WEBMONITOR_H__
#define __WEBMONITOR_H__

#include <Arduino.h>
void initWifi();
void RssiDisplay();
void ReconnectIfWifiDisconnected();
void startWebMonitoring();
void sendEmail(const String &errorMessage, const String &cookProfile, bool cookError);
void sendProfilData(const String &profilName,
                    float target_temperature,
                    float measured_temperature,
                    int actual_phase,
                    int number_of_phase,
                    int profil_duration,
                    float T1_temp,
                    int T1_duration,
                    int nR1,
                    float T2_temp,
                    int T2_duration,
                    int nR2,
                    float T3_temp,
                    int T3_duration,
                    int nR3,
                    bool setPointReached);

#ifdef	__cplusplus

extern "C" {
#endif


#ifdef	__cplusplus
}
#endif

#endif