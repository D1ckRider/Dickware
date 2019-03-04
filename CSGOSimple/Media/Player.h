#pragma once
#include <atlbase.h>
#include <wmp.h>
class RadioPlayer
{
public:
	void PlayStreamFromURL(wchar_t* url);
	void ResumePlayer();
	void PausePlayer();
	void SetVolume(int volume);
	void Release();
	~RadioPlayer();
private:
	HRESULT Init();
	bool Inited = false;
	CComPtr<IWMPPlayer> spPlayer;  // Smart pointer to IWMPPlayer interface.
	IWMPControls* spControls;
	IWMPSettings* spSettings;
};