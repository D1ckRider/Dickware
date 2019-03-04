#include "Player.h"

void RadioPlayer::PlayStreamFromURL(wchar_t * url)
{
	if (!Inited)
		Init();
	spPlayer->put_URL(url);
}

void RadioPlayer::ResumePlayer()
{
	if (!Inited)
		return;
	spControls->play();
}

void RadioPlayer::PausePlayer()
{
	if (!Inited)
		return;
	spControls->pause();
}

void RadioPlayer::SetVolume(int volume)
{
	spSettings->put_volume(volume);
}

void RadioPlayer::Release()
{
	spPlayer.Release();
	CoUninitialize();
}

RadioPlayer::~RadioPlayer()
{
	Release();
}

HRESULT RadioPlayer::Init()
{
	HRESULT res;
	CoInitialize(NULL);
	res = spPlayer.CoCreateInstance(__uuidof(WindowsMediaPlayer), 0, CLSCTX_INPROC_SERVER);
	if (SUCCEEDED(res))
	{
		Inited = true;
		spPlayer->get_controls(&spControls);
		spPlayer->get_settings(&spSettings);
	}
	return res;
}
