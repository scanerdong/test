#ifndef _relay_msg_h_
#define _relay_msg_h_ 
#include <cstddef>
#include <sys/time.h>
#include <stdint.h>

namespace united_dpi
{

typedef struct _streaderinfo
{
    uint64_t reader_succ_pkg;;
    uint64_t reader_succ_flow;
    uint64_t reader_fail_pkg;;
    uint64_t reader_fail_flow;
    uint64_t decoder_fail_pkg;
    uint64_t decoder_fail_flow;
    uint64_t send_fail_flow;
    uint64_t send_fail_pkg;
    _streaderinfo(){memset(this, 0x0, sizeof(*this));}
    _streaderinfo& add(_streaderinfo& r)
    {
        reader_succ_pkg += r.reader_succ_pkg; 
        reader_succ_flow += r.reader_succ_flow; 
        reader_fail_pkg += r.reader_fail_pkg; 
        reader_fail_flow += r.reader_fail_flow; 
        send_fail_flow += r.send_fail_flow; 
        send_fail_pkg += r.send_fail_pkg; 
        decoder_fail_pkg += r.decoder_fail_pkg; 
        decoder_fail_flow += r.decoder_fail_flow; 
        return *this;
    }
}STReaderInfo;

    
typedef struct _stworkinfo
{
    uint64_t worker_succ_pkg;
    uint64_t worker_succ_flow;
    _stworkinfo(){memset(this, 0x0, sizeof(*this));}
    _stworkinfo& add(_stworkinfo& r)
    {
        worker_succ_pkg += r.worker_succ_pkg; 
        worker_succ_flow += r.worker_succ_flow; 
        return *this;
    }

}STWorkerInfo;

struct stat_dis_info{

	stat_dis_info():_dis_pkts(0)
		,_dis_byts(0)
		,_err_pkts(0)
		,_lst_pkts(0){
	}
	uint64_t  _dis_pkts;
	uint64_t  _dis_byts;
	uint64_t  _err_pkts;
	uint64_t  _lst_pkts;

};



struct raw_buf{

	raw_buf():_raw_buf_len(0){
	}

	size_t _raw_buf_len;
	enum { RAW_MSG_MAX = 3200 };
	char _raw_buf[RAW_MSG_MAX];
};

enum MSG_DIR{
	MSG_DIR_UP=0,
	MSG_DIR_DN,
	MSG_DIR_MAX,
};

#define U16_MAX_VALUE 0xffff
#define U32_MAX_VALUE 0xffffffff

//ip port teid are little endian
struct CommInfo{
	CommInfo() : u8IpAddrType(1),
		u32OutSrcIpAddr(U32_MAX_VALUE),u32OutDstIpAddr(U32_MAX_VALUE),
		u16OutSrcPort(U16_MAX_VALUE),u16OutDstPort(U16_MAX_VALUE),
		u32Teid(U32_MAX_VALUE),
		u32InSrcIp(U32_MAX_VALUE),u32InDstIp(U32_MAX_VALUE),
		u16InSrcPort(U16_MAX_VALUE),u16InDstPort(U16_MAX_VALUE),
		u8L4Protocal(0xff),u32DataFlow(0),
		u32IpFragPkgs(0),u8IpFragInfo(0),u16InnerIpPos(0),u16InnerL4Pos(0),u16AppDataPos(0)
	{}

	//解第一层
	uint8_t  u8IpAddrType; //1,ipv4 2,ipv6
	uint32_t u32OutSrcIpAddr; // outer layer source ip address little endian
	uint32_t u32OutDstIpAddr; //
	uint16_t u16OutSrcPort;
	uint16_t u16OutDstPort;
	uint32_t u32Teid;

	//解第二层
	uint32_t u32InSrcIp; //inner layer source ip address ( little endian )
	uint32_t u32InDstIp;
	uint16_t u16InSrcPort;
	uint16_t u16InDstPort;
	uint8_t  u8L4Protocal; //传输层的协议号 0x6:tcp 0x11:udp
	uint32_t u32DataFlow; //from inner layer ip packge
	uint32_t u32IpFragPkgs; //IP分片数，以内层IP包的分片为准

	uint8_t u8IpFragInfo;
	//dongying 添加指向应用层数据的pos
	uint16_t u16InnerIpPos;
	uint16_t u16InnerL4Pos; //内层tcp/udp包的偏移位置
	uint16_t u16AppDataPos; //应用层数据相对于raw_buf的起始位置
};

struct new_raw_buf{

	new_raw_buf():_raw_buf_len(0),_raw_buf(NULL){
	}
	new_raw_buf(size_t len, char* buf):_raw_buf_len(len),_raw_buf(buf){
	}

	size_t _raw_buf_len;
	char* _raw_buf;
};

struct relay_msg_base
{      relay_msg_base():_msg_dir(MSG_DIR_DN)
		,_raw_buf(sizeof(_alloc_buf._raw_buf),_alloc_buf._raw_buf)
		,_is_filled(false)
		,_apphead_len(0){
	}
	virtual ~relay_msg_base(){
		}
	struct timeval _ts;
	MSG_DIR	_msg_dir;
	raw_buf	_alloc_buf;
	new_raw_buf	_raw_buf;
	
	//当重组了新的IP包后，新的IP包通过new_raw_buf传出
	new_raw_buf _new_raw_buf;
	CommInfo commInfo;
	uint32_t _apphead_len;
	bool	_is_filled;
};

}
#endif

