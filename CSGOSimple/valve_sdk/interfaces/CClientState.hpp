#pragma once

#include <cstdint>
#include "..//..//helpers/utils.hpp"

#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1
#define MAX_FLOWS 2

// Created with ReClass.NET by KN4CK3R
#pragma pack(push, 1)
class INetChannelInfo
{
public:

	char pad_0000[24];						//0x0000
	int m_nOutSequenceNr;					//0x0018
	int m_nInSequenceNr;					//0x001C
	int m_nOutSequenceNrAck;				//0x0020
	int m_nOutReliableState;				//0x0024
	int m_nInReliableState;					//0x0028
	int m_nChokedPackets;					//0x002C
	char pad_0030[108];						//0x0030
	int m_Socket;							//0x009C
	int m_StreamSocket;						//0x00A0
	int m_MaxReliablePayloadSize;			//0x00A4
	char pad_00A8[100];						//0x00A8
	float last_received;					//0x010C
	float connect_time;						//0x0110
	char pad_0114[4];						//0x0114
	int m_Rate;								//0x0118
	char pad_011C[4];						//0x011C
	float m_fClearTime;						//0x0120
	char pad_0124[16688];					//0x0124
	char m_Name[32];						//0x4254
	unsigned int m_ChallengeNr;				//0x4274
	float m_flTimeout;						//0x4278
	char pad_427C[32];						//0x427C
	float m_flInterpolationAmount;			//0x429C
	float m_flRemoteFrameTime;				//0x42A0
	float m_flRemoteFrameTimeStdDeviation;	//0x42A4
	int m_nMaxRoutablePayloadSize;			//0x42A8
	int m_nSplitPacketSequence;				//0x42AC
	char pad_42B0[40];						//0x42B0
	bool m_bIsValveDS;						//0x42D8
	char pad_42D9[65];						//0x42D9

	virtual const char  *GetName(void) const = 0;	// get channel name
	virtual const char  *GetAddress(void) const = 0; // get channel IP address as string
	virtual float		GetTime(void) const = 0;	// current net time
	virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut(void) const = 0;	// true if timing out
	virtual bool		IsPlayback(void) const = 0;	// true if demo playback

	virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int			GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int flow, int *received, int *total) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int *pnLatencyMsecs, int *pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float *pflFrameTime, float *pflFrameTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
}; //Size: 0x431A

class INetChannel : public INetChannelInfo
{
public:
	virtual	~INetChannel(void) {};

	virtual void	SetDataRate(float rate) = 0;
	virtual bool	RegisterMessage(void *msg) = 0;
	virtual bool	StartStreaming(unsigned int challengeNr) = 0;
	virtual void	ResetStreaming(void) = 0;
	virtual void	SetTimeout(float seconds) = 0;
	virtual void	SetDemoRecorder(void *recorder) = 0;
	virtual void	SetChallengeNr(unsigned int chnr) = 0;

	virtual void	Reset(void) = 0;
	virtual void	Clear(void) = 0;
	virtual void	Shutdown(const char *reason) = 0;

	virtual void	ProcessPlayback(void) = 0;
	virtual bool	ProcessStream(void) = 0;
	virtual void	ProcessPacket(struct netpacket_s* packet, bool bHasHeader) = 0;

	/*virtual bool	SendNetMsg(bool bForceReliable = false, bool bVoice = false) = 0;
#ifdef POSIX*/
	FORCEINLINE bool SendNetMsg(uint8_t const &msg, bool bForceReliable = false, bool bVoice = false) { return SendNetMsg(*((uint8_t *)&msg), bForceReliable, bVoice); }
//#endif
	virtual bool	SendData(bf_write &msg, bool bReliable = true) = 0;
	virtual bool	SendFile(const char *filename, unsigned int transferID) = 0;
	virtual void	DenyFile(const char *filename, unsigned int transferID) = 0;
	virtual void	RequestFile_OLD(const char *filename, unsigned int transferID) = 0;	// get rid of this function when we version the
	virtual void	SetChoked(void) = 0;
	virtual int		SendDatagram(bf_write *data) = 0;
	virtual bool	Transmit(bool onlyReliable = false) = 0;

	virtual void	GetRemoteAddress(void) const = 0;
	virtual void *GetMsgHandler(void) const = 0;
	virtual int				GetDropNumber(void) const = 0;
	virtual int				GetSocket(void) const = 0;
	virtual unsigned int	GetChallengeNr(void) const = 0;
	virtual void			GetSequenceData(int &nOutSequenceNr, int &nInSequenceNr, int &nOutSequenceNrAck) = 0;
	virtual void			SetSequenceData(int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck) = 0;

	virtual void	UpdateMessageStats(int msggroup, int bits) = 0;
	virtual bool	CanPacket(void) const = 0;
	virtual bool	IsOverflowed(void) const = 0;
	virtual bool	IsTimedOut(void) const = 0;
	virtual bool	HasPendingReliableData(void) = 0;

	virtual void	SetFileTransmissionMode(bool bBackgroundMode) = 0;
	virtual void	SetCompressionMode(bool bUseCompression) = 0;
	virtual unsigned int RequestFile(const char *filename) = 0;
	virtual float	GetTimeSinceLastReceived(void) const = 0;	// get time since last received packet in seconds

	virtual void	SetMaxBufferSize(bool bReliable, int nBytes, bool bVoice = false) = 0;

	virtual bool	IsNull() const = 0;
	virtual int		GetNumBitsWritten(bool bReliable) = 0;
	virtual void	SetInterpolationAmount(float flInterpolationAmount) = 0;
	virtual void	SetRemoteFramerate(float flFrameTime, float flFrameTimeStdDeviation) = 0;

	// Max # of payload bytes before we must split/fragment the packet
	virtual void	SetMaxRoutablePayloadSize(int nSplitSize) = 0;
	virtual int		GetMaxRoutablePayloadSize() = 0;

	virtual int		GetProtocolVersion() = 0;
};

class CClockDriftMgr
{
public:
    float m_ClockOffsets[16];   //0x0000
    uint32_t m_iCurClockOffset; //0x0044
    uint32_t m_nServerTick;     //0x0048
    uint32_t m_nClientTick;     //0x004C
}; //Size: 0x0050

class CEventInfo
{
public:

	uint16_t classID;			//0x0000 0 implies not in use
	char pad_0002[2];			//0x0002
	float fire_delay;			//0x0004 If non-zero, the delay time when the event should be fired ( fixed up on the client )
	char pad_0008[4];			//0x0008
	ClientClass *pClientClass;	//0x000C
	void *pData;				//0x0010 Raw event data
	char pad_0014[48];			//0x0014
}; //Size: 0x0044

// This struct is most likely wrong
// Only fields that I know to be valid are:
// - m_NetChannel
// - m_nCurrentSequence
// - m_nDeltaTick
// - m_nMaxClients
// - viewangles
class CClientState
{
public:
    void ForceFullUpdate()
    {
        m_nDeltaTick = -1;
		//*reinterpret_cast<int*>(std::uintptr_t(this) + 0x174) = -1;
    }

	char pad_0000[148];				//0x0000
	INetChannel *m_NetChannel;		//0x0094
	char pad_0098[8];				//0x0098
	uint32_t m_nChallengeNr;		//0x00A0
	char pad_00A4[100];				//0x00A4
	uint32_t m_nSignonState;		//0x0108
	char pad_010C[8];				//0x010C
	float m_flNextCmdTime;			//0x0114
	uint32_t m_nServerCount;		//0x0118
	uint32_t m_nCurrentSequence;	//0x011C
	char pad_0120[84];				//0x0120
	uint32_t m_nDeltaTick;			//0x0174
	bool m_bPaused;					//0x0178
	char pad_0179[7];				//0x0179
	uint32_t m_nViewEntity;			//0x0180
	uint32_t m_nPlayerSlot;			//0x0184
	char m_szLevelName[260];		//0x0188
	char m_szLevelNameShort[80];	//0x028C
	char m_szGroupName[80];			//0x02DC
	char pad_032С[92];				//0x032С
	uint32_t m_nMaxClients;			//0x0388 
	char pad_038C[18820];			//0x038C 
	float m_flLastServerTickTime;	//0x4D10
	bool insimulation;				//0x4D14 
	char pad_4D15[3];				//0x4D15  
	uint32_t oldtickcount;			//0x4D18 
	float m_tickRemainder;			//0x4D1C
	float m_frameTime;				//0x4D20 
	uint32_t lastoutgoingcommand;	//0x4D24 
	uint32_t chokedcommands;		//0x4D28 
	uint32_t last_command_ack;		//0x4D2C
	uint32_t command_ack;			//0x4D30 
	uint32_t m_nSoundSequence;		//0x4D34
	char pad_4D38[80];				//0x4D38
	QAngle viewangles;				//0x4D88 
	char pad_4D94[208];				//0x4D94
	CEventInfo *events;				//0x4E64 
};

#pragma pack(pop)

static_assert(FIELD_OFFSET(CClientState, m_NetChannel)       == 0x0094, "Wrong struct offset");
static_assert(FIELD_OFFSET(CClientState, m_nCurrentSequence) == 0x011C, "Wrong struct offset");
static_assert(FIELD_OFFSET(CClientState, m_nDeltaTick)       == 0x0174, "Wrong struct offset");
static_assert(FIELD_OFFSET(CClientState, m_nMaxClients)      == 0x0388, "Wrong struct offset");
static_assert(FIELD_OFFSET(CClientState, viewangles)         == 0x4D88, "Wrong struct offset");