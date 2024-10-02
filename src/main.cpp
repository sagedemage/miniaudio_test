#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>

#include <iostream>

int main() {
    const float volume = 0.5F;
    const char *audio_file = "audio/test.mp3";

    std::cout << "---------------------------------" << std::endl;
    std::cout << "| Play mp3 file with miniaudio. |" << std::endl;
    std::cout << "---------------------------------" << std::endl;

    // Initialize engine using it's default configuration
    ma_result result;
    ma_engine engine;

    result = ma_engine_init(NULL, &engine);
    if (result != MA_SUCCESS) {
        std::cerr << "Unable to initialize the engine!" << std::endl;
        return result;
    }

    // Load audio file
    ma_sound sound;
    // MA_SOUND_FLAG_DECODE: decode the sound before storing it in memory
    result = ma_sound_init_from_file(&engine, audio_file, MA_SOUND_FLAG_DECODE,
                                     NULL, NULL, &sound);
    if (result != MA_SUCCESS) {
        std::cerr << "Unable to load the audio file!" << std::endl;
        return result;
    }

    // set volume
    ma_sound_set_volume(&sound, volume);

    ma_sound_set_looping(&sound, static_cast<ma_bool32>(true));

    // Play audio file
    ma_sound_start(&sound);

    ma_bool32 is_playing = ma_sound_is_playing(&sound);

    if (is_playing == false) {
        std::cerr << "Sound is not playing!" << std::endl;
    }

    ma_bool32 is_looping = ma_sound_is_looping(&sound);

    if (is_looping == false) {
        std::cerr << "Sound is not looping!" << std::endl;
    }

    // Game loop
    while (true) {
    }

    // Free resources
    ma_engine_uninit(&engine);
}
