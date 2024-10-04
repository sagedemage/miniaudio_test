#define MA_NO_VORBIS
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio/miniaudio.h>
#include <miniaudio/extras/miniaudio_libvorbis.h>

#include <iostream>

void DataCallback(ma_device* p_device, void* p_output, const void* p_input,
                  ma_uint32 frame_count) {
    // In playback mode copy data to pOutput. In capture mode read data from
    // pInput. In full-duplex mode, both pOutput and pInput will be valid and
    // you can move data from pInput into pOutput. Never process more than
    // frameCount frames.
    ma_decoder* p_decoder = static_cast<ma_decoder*>(p_device->pUserData);
    if (p_decoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(p_decoder, p_output, frame_count, NULL);

    (void)p_input;
}

static ma_result MaDecodingBackendInitLibvorbis(
    void* p_user_data, ma_read_proc on_read, ma_seek_proc on_seek,
    ma_tell_proc on_tell, void* p_read_seek_tell_user_data,
    const ma_decoding_backend_config* p_config,
    const ma_allocation_callbacks* p_allocation_callbacks,
    ma_data_source** pp_backend) {
    ma_result result;
    ma_libvorbis* p_vorbis;

    (void)p_user_data;

    p_vorbis = static_cast<ma_libvorbis*>(
        ma_malloc(sizeof(*p_vorbis), p_allocation_callbacks));
    if (p_vorbis == NULL) {
        return MA_OUT_OF_MEMORY;
    }

    result =
        ma_libvorbis_init(on_read, on_seek, on_tell, p_read_seek_tell_user_data,
                          p_config, p_allocation_callbacks, p_vorbis);
    if (result != MA_SUCCESS) {
        ma_free(p_vorbis, p_allocation_callbacks);
        return result;
    }

    *pp_backend = p_vorbis;

    return MA_SUCCESS;
}

static ma_result MaDecodingBackendInitFileLibvorbis(
    void* p_user_data, const char* p_file_path,
    const ma_decoding_backend_config* p_config,
    const ma_allocation_callbacks* p_allocation_callbacks,
    ma_data_source** pp_backend) {
    ma_result result;
    ma_libvorbis* p_vorbis;

    (void)p_user_data;

    p_vorbis = static_cast<ma_libvorbis*>(
        ma_malloc(sizeof(*p_vorbis), p_allocation_callbacks));
    if (p_vorbis == NULL) {
        return MA_OUT_OF_MEMORY;
    }

    result = ma_libvorbis_init_file(p_file_path, p_config,
                                    p_allocation_callbacks, p_vorbis);
    if (result != MA_SUCCESS) {
        ma_free(p_vorbis, p_allocation_callbacks);
        return result;
    }

    *pp_backend = p_vorbis;

    return MA_SUCCESS;
}

static void MaDecodingBackendUninitLibvorbis(
    void* p_user_data, ma_data_source* p_backend,
    const ma_allocation_callbacks* p_allocation_callbacks) {
    ma_libvorbis* p_vorbis = static_cast<ma_libvorbis*>(p_backend);

    (void)p_user_data;

    ma_libvorbis_uninit(p_vorbis, p_allocation_callbacks);
    ma_free(p_vorbis, p_allocation_callbacks);
}

int main() {
    const float volume = 0.5F;
    const char* audio_file = "audio/test.ogg";

    std::cout << "---------------------------------" << std::endl;
    std::cout << "| Play ogg file with miniaudio. |" << std::endl;
    std::cout << "---------------------------------" << std::endl;

    // Initialize engine using it's default configuration
    ma_result result;
    ma_engine engine;
    ma_resource_manager resource_manager;
    ma_resource_manager_config resource_manager_config;

    static ma_decoding_backend_vtable g_ma_decoding_backend_vtable_libvorbis = {
        MaDecodingBackendInitLibvorbis, MaDecodingBackendInitFileLibvorbis,
        NULL, /* onInitFileW() */
        NULL, /* onInitMemory() */
        MaDecodingBackendUninitLibvorbis};

    ma_decoding_backend_vtable* p_custom_backend_v_tables[] = {
        &g_ma_decoding_backend_vtable_libvorbis};

    ma_decoder_config decoder_config;
    decoder_config = ma_decoder_config_init_default();
    decoder_config.pCustomBackendUserData = NULL;
    decoder_config.ppCustomBackendVTables = p_custom_backend_v_tables;
    decoder_config.customBackendCount = sizeof(p_custom_backend_v_tables) /
                                        sizeof(p_custom_backend_v_tables[0]);

    resource_manager_config = ma_resource_manager_config_init();
    resource_manager_config.ppCustomDecodingBackendVTables =
        p_custom_backend_v_tables;
    resource_manager_config.customDecodingBackendCount =
        sizeof(p_custom_backend_v_tables) /
        sizeof(p_custom_backend_v_tables[0]);
    resource_manager_config.pCustomDecodingBackendUserData = NULL;

    result =
        ma_resource_manager_init(&resource_manager_config, &resource_manager);

    if (result != MA_SUCCESS) {
        std::cerr << "Unable to initialize the resource manager!" << std::endl;
        return result;
    }

    ma_engine_config engine_config;
    engine_config = ma_engine_config_init();
    engine_config.pResourceManager = &resource_manager;

    result = ma_engine_init(&engine_config, &engine);
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

    if (!is_playing) {
        std::cerr << "Sound is not playing!" << std::endl;
    }

    ma_bool32 is_looping = ma_sound_is_looping(&sound);

    if (!is_looping) {
        std::cerr << "Sound is not looping!" << std::endl;
    }

    // Game loop
    while (true) {
    }

    // Free resources
    ma_engine_uninit(&engine);

    return 0;
}
