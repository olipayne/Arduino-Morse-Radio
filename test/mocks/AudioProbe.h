#ifndef AUDIO_PROBE_H
#define AUDIO_PROBE_H

#include <vector>

struct AudioEvent {
  bool toneOn;
  unsigned long timestamp;
};

namespace AudioProbe {
void clear();
const std::vector<AudioEvent>& events();
}

#endif
