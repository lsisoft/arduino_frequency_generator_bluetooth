#pragma once

class FrequencyCalculator {
    double log_scalers[5];

public:
    FrequencyCalculator();

    struct RegCountDuty {
        unsigned int count_reg, duty_reg, scaler;
    };

    RegCountDuty frequency(double freq, double duty) const;
};
