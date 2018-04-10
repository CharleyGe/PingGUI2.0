#include "pinger.h"
#include <QDebug>
#include <QRegExp>


Pinger::Pinger():
    icmp_data(NULL)
{
    WSADATA wsaData;
    pPingReply = new PingReply();
    //版本设置
    if(WSAStartup(MAKEWORD(2,1), &wsaData)!=0){
        qDebug()<<"WSAStartup failed"<<endl;
        ExitProcess(STATUS_FAILED);
    }
}

Pinger::~Pinger()
{
    qDebug()<<"~Pinger()"<<endl;
    WSACleanup();
    if(NULL != icmp_data)
    {
        free(icmp_data);
        icmp_data = NULL;
    }
    //让ping按钮可用，stop按钮不可用
    emit btnEnable();
}

void Pinger::run()
{
    m_isCanRun = true; //标记为可以运行
    ping();  //调用ping开始ping
}

void Pinger::ping()
{
    emitFlag = false;
    statistic = 0;
    Times=0;
    SOCKET sockRaw;
    struct sockaddr_in dest,from;
    struct hostent *hp;
    int bread, datasize, times=pingTimes;
    int fromlen=sizeof(from);
    int timeout=timeOut;
    char *dest_ip;
    unsigned int addr=0;
    USHORT seq_no=0;
    QString pattern("^(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|[1-9]\\."
                    "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                    "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)\\."
                    "(1\\d{2}|2[0-4]\\d|25[0-5]|[1-9]\\d|\\d)$");
    QRegExp rx(pattern);

    sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);

    if(sockRaw==INVALID_SOCKET){
        qDebug()<<"WSASocket() failed"<<endl;
        ExitProcess(STATUS_FAILED);
    }

    bread=setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO,(char*)&timeout, sizeof(timeout));
    if(bread==SOCKET_ERROR){
        qDebug()<<"Failed to set recv timeout"<<endl;
        //ExitProcess(STATUS_FAILED);
        return;
    }
    timeout=timeOut;
    bread=setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO,(char*)&timeout, sizeof(timeout));
    if(bread==SOCKET_ERROR){
        qDebug()<<"failed to set send timeout"<<endl;
        //ExitProcess(STATUS_FAILED);
        return;
    }
    bread=setsockopt(sockRaw, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(TTL));
    if(bread==SOCKET_ERROR){
        qDebug()<<"failed to set ttl"<<endl;
        return;
    }
    memset(&dest,0,sizeof(dest));

    std::string stdAddr = pingAddr.toStdString();
    const char* charAddr = stdAddr.c_str();

    //hp=gethostbyname("www.baidu.com");//此处传入目的地址
    hp = gethostbyname(charAddr);

    if(!hp){
       // qDebug()<<"IP Address"<<endl;
        bool match = rx.exactMatch(pingAddr);  //判断IP值是否合法
        if(!match)  //合法
        {
            qDebug()<<"emitFlag==false"<<endl;
            //qDebug()<<"IP address is invalid."<<endl;
            //调用showinformationmessagebox
            showInformaionMessageBox::show("error","Please input correct IP Address.");
            emit addrInvalidUpdateStatus();   //地址无效更新状态栏
            return;
        }
    }
    //通过正则校验开启发送结果信号
    emitFlag = true;

    if(hp->h_aliases[0])
    {
        hostName = hp->h_aliases[0];
    }else{
        hostName = hp->h_name;
    }
    if((!hp)&&(addr==INADDR_NONE)){
        fromAddr = hostName;
        qDebug()<<"Unable to resolve IP address or hostname"<<endl;
        errorFlag = TRUE;
        error = (enum errorType)2;
        transferData();  //输出出错信息
        return;
    }

    fromAddr = inet_ntoa(*(struct in_addr*)hp->h_addr_list[0]);  //ping不通时也在表格中显示
    if(hp!=NULL)
        memcpy(&(dest.sin_addr),hp->h_addr,hp->h_length);
    else
        dest.sin_addr.s_addr=addr;

    if(hp)
        dest.sin_family=hp->h_addrtype;
    else
        dest.sin_family=AF_INET;

    dest_ip=inet_ntoa(dest.sin_addr);

    datasize = packSize + sizeof(IcmpHeader);

    icmp_data = (char*)malloc(packSize + sizeof(IcmpHeader));
    char recvbuf[256] = { "\0" };

    if(!icmp_data){
        qDebug()<<"malloc failed"<<endl;
        ExitProcess(STATUS_FAILED);
    }

    memset(icmp_data, 0, datasize);
    fill_icmp_data(icmp_data, datasize);

    for( count=0;count<times;++count){

        //正确终止一个线程
        QMutexLocker locker(&m_lock);
        if(!m_isCanRun)
        {
           emit pingCanceled();
           return;
        }

        int bwrote;
        ULONG ulSendTimestamp = GetTickCountCalibrate();


        ((IcmpHeader*)icmp_data)->i_cksum=0;
        ((IcmpHeader*)icmp_data)->timestamp = ulSendTimestamp;

        ((IcmpHeader*)icmp_data)->i_seq=seq_no++;
        ((IcmpHeader*)icmp_data)->i_cksum=checksum((USHORT*)icmp_data, datasize);

        bwrote=sendto(sockRaw, icmp_data,datasize,0,(struct sockaddr*)&dest,sizeof(dest));
        if(bwrote==SOCKET_ERROR){
            if(WSAGetLastError()==WSAETIMEDOUT){
                qDebug()<<"Request time out"<<endl;
                //调用showinformationmessagebox
                //showInformaionMessageBox::show("timeout","Request time out");
                errorFlag = TRUE;
                error = (enum errorType)1;
                transferData(); //输出出错信息
                continue;
            }
            qDebug()<<"sendto failed"<<endl;
            //showInformaionMessageBox::show("error","sendto failed.");
            errorFlag = TRUE;
            error = (enum errorType)2;
            transferData();
            continue;
        }
        if(bwrote<datasize){
            qDebug()<<"Wrote "<<bwrote<<" bytes"<<endl;
        }
        bread=recvfrom(sockRaw,recvbuf,256,0,(struct sockaddr*)&from,&fromlen);
        if(bread==SOCKET_ERROR){
            if(WSAGetLastError()==WSAETIMEDOUT){
                qDebug()<<"Request timed out."<<endl;
                //showInformaionMessageBox::show("timeout","Request timed out");
                errorFlag = TRUE;
                error = (enum errorType)1;
                transferData();  //输出出错信息
                continue;
            }
            qDebug()<<"recvfrom failed"<<endl;
            //showInformaionMessageBox::show("error","recvfrom failed");
            errorFlag = TRUE;
            error = (enum errorType)3;
            transferData();  //输出出错信息
            return;
        }
        if(!decode_resp(recvbuf,bread,&from))
            statistic++;
        Sleep(200);
    }
        //destip,Sendt,Received,Lost-->desr_ip/times/statistic/times-statistic/(float)(times-statistic)/times*100

        qDebug()<<endl<<"Ping statistics for "<<dest_ip<<endl;
        qDebug()<<"Packet: Sent = "<<times<<" Received = "<<statistic<<" Lost = "<<times-statistic<<"("<<(times-statistic)*100/times<<"% loss)"<<endl;
        qDebug()<<QString("Minimum = %1ms, Maximum = %2ms, Average=%3ms").arg(Minimum).arg(Maximum).arg((Maximum+Minimum)/2)<<endl;
        emit sendResultData(count,statistic);  //发送统计信息
        //return statistic;
}

int Pinger::decode_resp(char *buf,int bytes,struct sockaddr_in *from){
    IpHeader *iphdr;
    IcmpHeader *icmphdr;
    unsigned short iphdrlen;

    iphdr=(IpHeader *)buf;

    iphdrlen=(iphdr->h_len & 0x0f)*4;

    icmphdr=(IcmpHeader*)(buf+iphdrlen);
    tmStamp = GetTickCountCalibrate()-icmphdr->timestamp;

    if(bytes<iphdrlen+ICMP_MIN){
        qDebug()<<"Too few bytes"<<endl;
    }

    if(icmphdr->i_type !=ICMP_ECHOREPLY){
        qDebug()<<"non-echo type"<<endl;
        return 1;
    }
    if(icmphdr->i_id!=(USHORT)GetCurrentProcessId()){
        qDebug()<<"someone else's packet"<<endl;
        return 1;
    }
    pPingReply->m_usSeq = icmphdr->i_seq;
    pPingReply->m_dwRoundTripTime = tmStamp;
    pPingReply->m_dwBytes = bytes - iphdrlen - sizeof(IcmpHeader);
    pPingReply->m_dwTTL = iphdr->ttl;
    fromAddr=QString(inet_ntoa(from->sin_addr));
    transferData();
    return 0;
}

USHORT Pinger::checksum(USHORT *buffer, int size){
    unsigned long cksum=0;
    while(size>1){
        cksum+=*buffer++;
        size-=sizeof(USHORT);
    }

    if(size){
        cksum+=*(UCHAR*)buffer;
    }

    cksum=(cksum>>16)+(cksum&0xffff);
    cksum+=(cksum>>16);
    return (USHORT)(~cksum);
}

void Pinger::fill_icmp_data(char *icmp_data, int datasize)
{
    IcmpHeader *icmp_hdr;
    char *datapart;

    icmp_hdr=(IcmpHeader*)icmp_data;

    icmp_hdr->i_type=ICMP_ECHO;
    icmp_hdr->i_code=0;
    icmp_hdr->i_id=(USHORT)GetCurrentProcessId();
    icmp_hdr->i_cksum=0;
    icmp_hdr->i_seq=0;

    datapart=icmp_data+sizeof(IcmpHeader);

    memset(datapart,'E',datasize-sizeof(IcmpHeader));
}

ULONG Pinger::GetTickCountCalibrate()
{
    static ULONG s_ulFirstCallTick = 0;
    static LONGLONG s_ullFirstCallTickMS = 0;

    SYSTEMTIME systemtime;
    FILETIME filetime;
    GetLocalTime(&systemtime);
    SystemTimeToFileTime(&systemtime, &filetime);
    LARGE_INTEGER liCurrentTime;
    liCurrentTime.HighPart = filetime.dwHighDateTime;
    liCurrentTime.LowPart = filetime.dwLowDateTime;
    LONGLONG llCurrentTimeMS = liCurrentTime.QuadPart / 10000;

    if (s_ulFirstCallTick == 0)
    {
        s_ulFirstCallTick = GetTickCount();
    }
    if (s_ullFirstCallTickMS == 0)
    {
        s_ullFirstCallTickMS = llCurrentTimeMS;
    }

    return s_ulFirstCallTick + (ULONG)(llCurrentTimeMS - s_ullFirstCallTickMS);
}

void Pinger::transferData()
{
    Times++;
    emit sendData(fromAddr);
    emit updatePingRequests(Times);   //更新Ping请求状态
    if(resolveHostName==TRUE)  //是否需要将ip地址转换为hostName
    {
        emit sendHostName(hostName);
    }
    if(errorFlag)
    {
        errorFlag = FALSE;  //把标志还原为false
        switch (error) {
        case IP_TTL_EXPIRED_TRANSIT:
            emit sendErrMsg("ttl expire transit");
            break;
        case IP_REQ_TIMED_OUT:
            emit sendErrMsg("request timed out");
            break;
        case Invalid_host_address:
            emit sendErrMsg("invalid host address");
            break;
        case RecvFrom_Failed:
            emit sendErrMsg("recvFrom failed");
            break;
        default:
            break;
        }
    }else
    {
        QString timeStamp = QString::number(pPingReply->m_dwRoundTripTime);
        if(timeFlag)
        {
            Minimum = Maximum = pPingReply->m_dwRoundTripTime;
            timeFlag = false;
        }
        if(pPingReply->m_dwRoundTripTime<Minimum)
            Minimum = pPingReply->m_dwRoundTripTime;
        if(pPingReply->m_dwRoundTripTime>Maximum)
            Maximum = pPingReply->m_dwRoundTripTime;
        QString TTLData = QString::number(pPingReply->m_dwTTL);
        emit sendBytes(pPingReply->m_dwBytes);
        emit sendTime(timeStamp);
        emit sendTTL(TTLData);
    }
}

void Pinger::setPingAddr(const QString &value)
{
    pingAddr = value;
}

void Pinger::setTimeOut(const int &value)
{
    timeOut = value;
}

void Pinger::stopImmediately()
{
    QMutexLocker locker(&m_lock);
    m_isCanRun = false;
    //在发送结果前请确保count和statistic已经初始化
    if(emitFlag==true)
    {
        qDebug()<<"emitFlag==True"<<endl;
        emit sendResultData(count,statistic);  //发送统计信息
    }
}

void Pinger::setPingTimes(const int &value)
{
    pingTimes = value;
}

void Pinger::setPackSize(const int &value)
{
    packSize = value;
}

void Pinger::setResolveHostName(const int &check)
{
    resolveHostName = check;
}

void Pinger::setTTL(const int &ttl)
{
    TTL = ttl;
}
