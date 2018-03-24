#pragma once
#include <array>
#include <SFML\Audio\Sound.hpp>
#include <SFML\Audio\SoundBuffer.hpp>
#include <SFML\Audio\Music.hpp>

enum class EMusic
{
	BgMusicOne,
	BgMusicTwo,
	StressMusic,
	WinMusic,
	Count,
	None,
};

enum class ESound
{
	ShipCrash,
	Sell,
	Loot,
	Count,
	None,
};

class CAudioSystem
{
public:
	static CAudioSystem & GetInstance();

	void LoadAllAudio();

	void PlayMusic(EMusic aSong, bool aLoop = false);
	void StopAllMusic();

	EMusic GetCurrentlyPlayedSong() const;

	void PlaySound(ESound aSound);

	void Update();

	bool NoMusicPlaying() const;

	void RandomizeSongBetween(EMusic aFirst, EMusic aLast);
private:
	void LoadMusic(EMusic aMusic, const char* aPath);
	void LoadSound(ESound aSound, const char* aPath);

	CAudioSystem();

	EMusic myCurrentlyPlayingSong;

	std::array<sf::Sound, (size_t)ESound::Count> mySoundBank;
	std::array<sf::SoundBuffer, (size_t)ESound::Count> mySoundBuffers;

	std::array<sf::Music, (size_t)EMusic::Count> myMusicBuffer;

};