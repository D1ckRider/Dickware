#include "ConvarSpoofer.h"
#include "..\valve_sdk\sdk.hpp"

bool m_bReady = false;
std::map<std::string, SpoofedConvar*> m_pSpoofeds;

void C_ConvarSpoofer::Init() 
{
	m_pSpoofeds.clear();
	m_bReady = false;
}

void C_ConvarSpoofer::Release() 
{
	m_bReady = false;

	for (auto it = m_pSpoofeds.begin(); it != m_pSpoofeds.end(); it++)
		delete it->second;

	m_pSpoofeds.clear();
}

void C_ConvarSpoofer::Add(const char *name) { m_pSpoofeds.insert(std::make_pair(name, nullptr)); }

void C_ConvarSpoofer::Spoof()
{
	for (auto it = m_pSpoofeds.begin(); it != m_pSpoofeds.end(); it++) 
		it->second = new SpoofedConvar(it->first.c_str());


	m_bReady = true;
}

SpoofedConvar* C_ConvarSpoofer::Get(const char *name) 
{
	auto find = m_pSpoofeds.find(name);
	if (find != m_pSpoofeds.end())
		return find->second;

	return NULL;
}

bool C_ConvarSpoofer::Ready() {	return m_bReady; }


