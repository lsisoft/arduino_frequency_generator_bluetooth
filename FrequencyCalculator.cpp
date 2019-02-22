#include "FrequencyCalculator.h"

#include <math.h>
#include <HardwareSerial.h>

namespace {
    static const int nscalers = 5;
    static constexpr unsigned int scalers[nscalers] = {1, 8, 64, 256, 1024};
//    int master = pow(2, 24);

    int log_master = 24;
    double log_scalers[5];
    int log_reg = 16;
}

FrequencyCalculator::FrequencyCalculator() {
    for (int i = 0; i < nscalers; ++i)
        log_scalers[i] = (int) (log(scalers[i]) / log(2.));
}

FrequencyCalculator::RegCountDuty FrequencyCalculator::frequency(double freq, double duty) const {
    double log_freq = log(freq) / log(2.);
    double possible_log_scaler = log_master - log_reg - log_freq;

    Serial.print("log_reg ");
    Serial.println(log_reg);
    Serial.print(" freq ");
    Serial.println(freq);
    Serial.print(" log_freq ");
    Serial.println(log_freq);
    Serial.print(" possible_log_scaler ");
    Serial.println(possible_log_scaler);

    int idx = 0;
    while (idx < nscalers - 1 && possible_log_scaler > log_scalers[idx]) {
        ++idx;

        Serial.print("idx ");
        Serial.print(idx);
        Serial.print(" log_scaler ");
        Serial.print(log_scalers[idx]);
        Serial.print(" scaler ");
        Serial.print(scalers[idx]);
        Serial.println();
    }


    Serial.print("final idx ");
    Serial.println(idx);

    unsigned int count_reg = (int)round(pow(2, 24 - log_scalers[idx] - log_freq));

//    double gen_frequency = pow(2, 24 - log_scalers[idx]) / count_reg;

    Serial.print("duty ");
    Serial.println(duty);

    unsigned int duty_reg = (int)round(count_reg * duty);

    Serial.print("count_reg ");
    Serial.print(count_reg);
    Serial.print(" duty_reg ");
    Serial.println(duty_reg);

    return RegCountDuty{count_reg, duty_reg, scalers[idx]};
}