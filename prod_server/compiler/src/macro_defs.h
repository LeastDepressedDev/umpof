#pragma once
#define grab_t std::chrono::steady_clock::now()
#define dif_t(t) ((float) (grab_t - (t)).count())/1000000
#define dif_s(t) std::to_string(dif_t(t)) + "ms"