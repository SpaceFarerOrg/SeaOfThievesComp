#include "AudioSystem.h"
#include <SFML\System\String.hpp>
#include "Math.h"
#include "Application.h"

CAudioSystem & CAudioSystem::GetInstance()
{
	static CAudioSystem instance;

	return instance;
}

void CAudioSystem::LoadAllAudio()
{
	LoadSound(ESound::ShipCrash, "shipCrash");
	LoadSound(ESound::Sell, "earnMoney");
	LoadSound(ESound::Loot, "treasureThump");

	LoadMusic(EMusic::BgMusicOne, "song");
	LoadMusic(EMusic::BgMusicTwo, "song2");
	LoadMusic(EMusic::StressMusic, "stress");
	LoadMusic(EMusic::WinMusic, "winSong");
}

void CAudioSystem::StopAllMusic()
{
	for (sf::Music& song : myMusicBuffer)
	{
		song.stop();
		song.setLoop(false);
	}

	myCurrentlyPlayingSong = EMusic::None;
}

EMusic CAudioSystem::GetCurrentlyPlayedSong() const
{
	return myCurrentlyPlayingSong;
}

void CAudioSystem::PlayMusic(EMusic aSong, bool aLoop)
{
	if (myMusicBuffer[(size_t)aSong].getStatus() == sf::SoundSource::Status::Playing)
		return;

	myLastPlayedSong = aSong;

	for (sf::Music& music : myMusicBuffer)
	{
		music.setLoop(false);
		music.stop();
	}

	if (aLoop)
	{
		myMusicBuffer[(size_t)aSong].setLoop(true);
	}

	myMusicBuffer[(size_t)aSong].play();
	myCurrentlyPlayingSong = aSong;
}

void CAudioSystem::PlaySound(ESound aSound)
{
	mySoundBank[(size_t)aSound].play();
}

void CAudioSystem::Update()
{
	float masterVolume = CApplication::GetVolume();
	for (sf::Music& song : myMusicBuffer)
	{
		song.setVolume(masterVolume);
	}
	for (sf::Sound& sound : mySoundBank)
	{
		sound.setVolume(masterVolume);
	}

	if (myCurrentlyPlayingSong == EMusic::None)
		return;

	myTimeSilent.restart();

	if (myMusicBuffer[(size_t)myCurrentlyPlayingSong].getStatus() != sf::SoundSource::Status::Playing)
	{
		myCurrentlyPlayingSong = EMusic::None;
	}

}

float CAudioSystem::GetTimeSilent() const
{
	return myTimeSilent.getElapsedTime().asSeconds();
}

bool CAudioSystem::NoMusicPlaying() const
{
	return myCurrentlyPlayingSong == EMusic::None;
}

void CAudioSystem::RandomizeSongBetween(EMusic aFirst, EMusic aLast, bool aShouldExcludeLastPlayed)
{
	short first = (short)aFirst;
	short last = (short)aLast;

	short newSong;

	while (true)
	{
		newSong = Math::GetRandomInRange(first, last);

		if ((EMusic)newSong != myLastPlayedSong)
		{
			break;
		}
	}

	StopAllMusic();

	PlayMusic((EMusic)newSong);
}

void CAudioSystem::LoadMusic(EMusic aMusic, const char * aPath)
{
	sf::String path = "audio/";
	path += aPath;
	path += ".ogg";

	myMusicBuffer[(size_t)aMusic].openFromFile(path);
}

void CAudioSystem::LoadSound(ESound aSound, const char * aPath)
{
	sf::String path = "audio/";
	path += aPath;
	path += ".ogg";

	mySoundBuffers[(size_t)aSound].loadFromFile(path);
	mySoundBank[(size_t)aSound].setBuffer(mySoundBuffers[(size_t)aSound]);
}

CAudioSystem::CAudioSystem()
{
	myCurrentlyPlayingSong = EMusic::None;
}
