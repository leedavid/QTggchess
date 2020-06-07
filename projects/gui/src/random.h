#pragma once

#include <algorithm>
#include <random>
#include <string>
#include "mutex.h"
#include <QString>

namespace Chess {
	class Random {
	public:
		static Random& Get();
		double GetDouble(double max_val);
		float GetFloat(float max_val);
		double GetGamma(double alpha, double beta);
		// Both sides are included.
		int GetInt(int min, int max);
		//std::string GetString(int length);
		QString GetString(int length);
		bool GetBool();
		template <class RandomAccessIterator>
		void Shuffle(RandomAccessIterator s, RandomAccessIterator e);

	private:
		Random();

		Mutex mutex_;
		std::mt19937 gen_ GUARDED_BY(mutex_);
	};

	template <class RandomAccessIterator>
	void Random::Shuffle(RandomAccessIterator s, RandomAccessIterator e) {
		Mutex::Lock lock(mutex_);
		std::shuffle(s, e, gen_);
	}

}