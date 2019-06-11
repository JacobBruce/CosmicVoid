#pragma once
#include <soloud.h>
#include <soloud_wav.h>
#include "ReadWrite.h"
#include "Resource.h"

class SoundManager
{
private:

    SoLoud::Soloud _soloud;
    std::vector<SoLoud::Wav> _sounds;
    int _lastForced;

public:

    SoundManager() : _lastForced(-1) {
        _soloud.init();
        _sounds.reserve(99);
    }

    ~SoundManager() {
        _soloud.deinit();
    }

    void LoadWavFile(const std::string& file) {
        _sounds.emplace_back();
        _sounds.back().load(file.c_str());
    }

    int PlaySound(uint32_t sound_index, float volume=1.0f) {
        return _soloud.play(_sounds[sound_index], volume);
    }

    void StopSound(uint32_t sound_index) {
        return _sounds[sound_index].stop();
    }

    int ForcePlay(uint32_t sound_index, float volume=1.0f) {
        if (_lastForced >= 0) StopSound(_lastForced);
        _lastForced = sound_index;
        return PlaySound(sound_index, volume);
    }

    SoLoud::Wav& GetSound(uint32_t sound_index) {
        return _sounds[sound_index];
    }

    void StopAllSounds(uint32_t sound_index) {
        return _soloud.stopAll();
    }

    float GetGlobalVolume() {
        return _soloud.getGlobalVolume();
    }

    void SetGlobalVolume(float volume) {
        return _soloud.setGlobalVolume(volume);
    }

    void LoadSounds(const std::string sound_list) {
        std::vector<std::string> soundFiles(ReadFileLines(sound_list));
        _sounds.reserve(soundFiles.size());

        for (const std::string& file : soundFiles)
        {
            LoadWavFile(file);
        }
    }
};
