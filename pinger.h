#ifndef PINGER_H
#define PINGER_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>   //可以修改TTL
#include <string>
#include <time.h>
#include <QThread>
#include <QString>
#include <QMutex>
#include "showinformationmessagebox.h"



#define STATUS_FAILED 0xFFFF
#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0

#define ICMP_MIN 8

#define DEF_PACKET_NUMBER 4
#define MAX_PACKET 1024


#pragma pack(push)
#pragma pack (4)

typedef struct iphdr{
    BYTE h_len;
    BYTE tos;
    USHORT total_len;
    USHORT ident;
    USHORT frag_and_flags;
    BYTE ttl;
    BYTE proto;
    USHORT checksum;
    ULONG sourceIP;
    ULONG destIP;
}IpHeader;

//ICMP header
typedef struct icmphdr {
    BYTE i_type;
    BYTE i_code;
    USHORT i_cksum;
    USHORT i_id;
    USHORT i_seq;
    ULONG timestamp;
}IcmpHeader;

struct PingReply
{
    USHORT m_usSeq;
    DWORD m_dwRoundTripTime;
    DWORD m_dwBytes;
    DWORD m_dwTTL;
};

#pragma pack(pop)

enum errorType{IP_TTL_EXPIRED_TRANSIT,IP_REQ_TIMED_OUT,Invalid_host_address,RecvFrom_Failed};

class Pinger : public QThread
{
    Q_OBJECT

signals:
    void sendData(const QString &info);
    void sendTime(const QString &timestamp);
    void sendHostName(const QString &hostNametoUI);
    void sendBytes(const int &bytestoUI);
    void sendTTL(const QString &ttlData);
    void sendErrMsg(const QString &errMsg);
    void updatePingRequests(const int &Pingtimes);
    void sendResultData(const int &SentData, const int &RecvData);
    void pingCanceled();
    void btnEnable();
    void addrInvalidUpdateStatus();
public:
    Pinger();
    //virtual ~Pinger();
    ~Pinger();
    void ping();
    void run();
    void fill_icmp_data(char *, int);
    USHORT checksum(USHORT *, int);
    int decode_resp(char *, int, struct sockaddr_in *);
    void setPingAddr(const QString &value);
    void setPackSize(const int &value);
    void setTimeOut(const int &value);
    void setPingTimes(const int &value);
    void setResolveHostName(const int &check);
    void setTTL(const int &ttl);
    ULONG GetTickCountCalibrate();
    void transferData();
private:
    int iPosIndex = 0;
    int timeOut;
    ULONG tmStamp;
    QString pingAddr;
    QString hostName;
    QString fromAddr;
    QMutex m_lock;
    bool m_isCanRun;
    int packSize = 0;
    int pingTimes = 10;
    char *icmp_data;
    PingReply *pPingReply;
    BOOL errorFlag=FALSE;
    errorType error;
    BOOL resolveHostName;
    int TTL;
    int Times;
    int count=0, statistic=0;
    bool emitFlag=false;  //是否ping通过可以发送结果信息
    int Minimum=0, Maximum=0, Average=0, Sum=0;
    bool timeFlag=true;
public slots:
    void stopImmediately();

};

#endif // PINGER_H
