#include "FrequencyCalculator.h"
#include "Logging.h"

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

    LOG("log_reg %u freq %u log_freq %u (d) possible_log_scaler %d (d)",
        log_reg, (unsigned int) freq, (unsigned int) log_freq * 100, (unsigned int) possible_log_scaler * 100);

    int idx = 0;
    while (idx < nscalers - 1 && possible_log_scaler > log_scalers[idx]) {
        ++idx;

        LOG("idx %d log_scaler %d scaler %d ", idx, log_scalers[idx], scalers[idx]);
    }

    LOG("final idx %d", idx);

    unsigned int count_reg = (int) round(pow(2, 24 - log_scalers[idx] - log_freq));

#ifdef DEBUG
    double gen_frequency = pow(2, 24 - log_scalers[idx]) / count_reg;

    LOG("gen_frequency %d", gen_frequency);
#endif

    LOG("duty %d (d)", (int) (duty * 100));

    unsigned int duty_reg = (int) round(count_reg * duty);

    LOG("count_reg %ud duty_reg %ud", count_reg, duty_reg);

    return RegCountDuty{count_reg, duty_reg, scalers[idx]};
}