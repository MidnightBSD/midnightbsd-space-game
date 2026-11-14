#include "sound.h"

#include <algorithm>
#include <array>
#include <cassert>

int main() {
    std::array<Uint8, 64> stream{};
    std::fill(stream.begin(), stream.end(), 0xff);

    mixaudio(nullptr, stream.data(), static_cast<int>(stream.size()));

    assert(std::all_of(stream.begin(), stream.end(), [](Uint8 sample) {
        return sample == 0;
    }));
    return 0;
}
