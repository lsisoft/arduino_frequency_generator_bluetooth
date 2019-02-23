#include "FrequencyCalculator.h"

#include "Logging.h"

#include <math.h>
#include <HardwareSerial.h>
#include <USBAPI.h>


namespace {
    const int nscalers = 5;
    constexpr unsigned int scalers[nscalers] = {1, 8, 64, 256, 1024};

    int log_master = 23;
    int log_reg = 16;
}

FrequencyCalculator::FrequencyCalculator() {
    for (int i = 0; i < nscalers; ++i)
        log_scalers[i] = (int) (log(scalers[i]) / log(2.));
}

FrequencyCalculator::RegCountDuty FrequencyCalculator::frequency(double freq, double duty) const {
    double log_freq = log(freq) / log(2.);
    double possible_log_scaler = log_master - log_reg - log_freq;

//    LogDebug("log_reg %u freq %u log_freq %lue-3 (d) possible_log_scaler %lue-3",
//             log_reg, (unsigned int) freq,
//             (unsigned long) (log_freq * 1000), (unsigned long) (possible_log_scaler * 1000));

    int idx = 0;
    while (idx < nscalers - 1 && possible_log_scaler >= log_scalers[idx])
        ++idx;

//    LogDebug("idx %d log_scaler %lue-3 scaler %d ",
//             idx, (unsigned long) (log_scalers[idx] * 1000), scalers[idx]);

    unsigned long count_reg = (unsigned long) round(pow(2, log_master - log_scalers[idx] - log_freq));
    if (count_reg > 65535) {
        LogDebug("ERROR count_reg %lu", count_reg);
        count_reg = 65535;
    }

    if (count_reg < 2) {
        LogInfo("WARN: frequency higher then 4mhz requested, unable to generate on FastPWM, generating 4Mhz")
        count_reg = 2;
    }

#ifdef __DEBUG__
    char str_frequency[10];
    double gen_frequency = pow(2, log_master - log_scalers[idx]) / count_reg;
    dtostrf(gen_frequency, 7, 2, str_frequency);

    LogDebug("gen_frequency %s", str_frequency);
#endif

    unsigned int duty_reg = min(1, (unsigned int) round(count_reg * duty));

    return RegCountDuty{count_reg, duty_reg, scalers[idx]};
}