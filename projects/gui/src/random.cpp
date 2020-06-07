#include "random.h"
#include <random>

namespace Chess {

    Random::Random() : gen_(std::random_device()()) {}

    Random& Random::Get() {
        static Random rand;
        return rand;
    }

    int Random::GetInt(int min, int max) {
        Mutex::Lock lock(mutex_);
        std::uniform_int_distribution<> dist(min, max);
        return dist(gen_);
    }

    bool Random::GetBool() { return GetInt(0, 1) != 0; }

    double Random::GetDouble(double maxval) {
        Mutex::Lock lock(mutex_);
        std::uniform_real_distribution<> dist(0.0, maxval);
        return dist(gen_);
    }

    float Random::GetFloat(float maxval) {
        Mutex::Lock lock(mutex_);
        std::uniform_real_distribution<> dist(0.0, maxval);
        return float(dist(gen_));
    }

    QString Random::GetString(int length) {
        QString result;
        for (int i = 0; i < length; ++i) {
            result += 'a' + GetInt(0, 25);
        }
        return result;
    }

    double Random::GetGamma(double alpha, double beta) {
        Mutex::Lock lock(mutex_);
        std::gamma_distribution<double> dist(alpha, beta);
        return dist(gen_);
    }

}  // namespace lczero
