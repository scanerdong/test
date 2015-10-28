#ifndef _DFILTER_H
#define _DFILTER_H

#include <list>
#include <map>
#include <string>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>


#include "relay_msg.h"
using namespace united_dpi;


using namespace std;

//----------原表达式分解成token时用---------------
typedef enum {
	TOKEN_UNPARSED, //未解析的字段
	TOKEN_STRING,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_COLON,
	//TOKEN_COMMA,
	//TOKEN_HYPHEN,
	TOKEN_TEST_EQ,
	TOKEN_TEST_NE,
	TOKEN_TEST_GT,
	TOKEN_TEST_GE,
	TOKEN_TEST_LT,
	TOKEN_TEST_LE,
	TOKEN_TEST_BITWISE_AND,
	TOKEN_TEST_CONTAINS,
	TOKEN_TEST_MATCHES,
	TOKEN_TEST_NOT,
	TOKEN_TEST_AND,
	TOKEN_TEST_OR
}token_type;

//当sttype_id_t为STTYPE_TEST时指定下面的操作类型
typedef enum {
	TEST_OP_UNINITIALIZED,
	TEST_OP_EXISTS,
	TEST_OP_NOT,
	TEST_OP_AND,
	TEST_OP_OR,
	TEST_OP_EQ,
	TEST_OP_NE,
	TEST_OP_GT,
	TEST_OP_GE,
	TEST_OP_LT,
	TEST_OP_LE,
	TEST_OP_BITWISE_AND,
	TEST_OP_CONTAINS,
	TEST_OP_MATCHES
} test_op_t;

typedef struct {
	token_type type;
	test_op_t op;
	string value;
	int priority;
}token_node;
//-----------------------------------------


//------------------创建节点时用-----------------------

//定义实体节点的类型
typedef enum {
	STTYPE_UNINITIALIZED,
	STTYPE_TEST, //为此类型表示上面还有操作类型
	STTYPE_UNPARSED,
	STTYPE_STRING,	
	STTYPE_BUFFER,//用类似0a:0c:f4:00:35这种形式来表示一段buffer
	STTYPE_FIELD,
	STTYPE_FVALUE,
	STTYPE_INTEGER,
	STTYPE_RANGE,
	STTYPE_FUNCTION,
	STTYPE_NUM_TYPES
} sttype_id_t;

typedef void*        (*STTypeNewFunc)(void*);
typedef void            (*STTypeFreeFunc)(void*);

/* Type information */
typedef struct {
	sttype_id_t		id;
	const char		*name;
	STTypeNewFunc		func_new; //根据id调用创建数据的指针
	STTypeFreeFunc		func_free;
} sttype_t;

/** Node (type instance) information */
typedef struct {
	sttype_t	*type;
	void*	data; //指定多种结构体指针
	int32_t		value;//int型直接保存到value中，以免过多转换
} stnode_t;

//--------------field-------------------------------------
//定义实体节点为STTYPE_FIELD的类型
typedef enum {
	FIELD_UNKNOW,
		
	FIELD_IP,
	FIELD_IP_ADDR,
	FIELD_IP_SRC_ADDR,
	FIELD_IP_DST_ADDR,
	FIELD_IP_HEAD_LEN,
	FIELD_IP_LEN,
	FIELD_IP_PROTOCOL,
	
	FIELD_TCP,
	FIELD_TCP_PORT,
	FIELD_TCP_SRC_PORT,
	FIELD_TCP_DST_PORT,
	
	FIELD_UDP,
	FIELD_UDP_PORT,
	FIELD_UDP_SRC_PORT,
	FIELD_UDP_DST_PORT,
} field_protocol_t;

//----------下面是stnode_t.data可能出现的类型-----------------------------------------
//若data指向test_t，表示该节点下面是一个子节点
typedef struct {
	test_op_t	op;
	stnode_t	*val1;
	stnode_t	*val2;
} test_t;

//---------buffer
typedef struct {
	uint32_t len;
	void* param;
} buffer_t;

//---------range
typedef struct {
	field_protocol_t proto;

	uint32_t start_pos;
	uint32_t len;
} range_t;



class DFilter{
public:
	DFilter();
	~DFilter();
	int scan(const char* pchSrc); //将原表达式分解成token list
	int make_ast();
	int semantic_check(stnode_t* node);
	bool exec_msg(stnode_t* node, relay_msg_base& msg); //执行过滤
	
	int num_operands(int op);
	void sttype_test_set1(stnode_t *node, test_op_t op, stnode_t *val1);
	void sttype_test_set2(stnode_t *node, test_op_t op, stnode_t *val1, stnode_t *val2);
	void sttype_test_set2_args(stnode_t *node, stnode_t *val1, stnode_t *val2);
	void sttype_test_get(stnode_t *node, test_op_t *p_op, stnode_t **p_val1, stnode_t **p_val2);
	
	void sttype_range_set(stnode_t *node, stnode_t *val1, stnode_t *val2, stnode_t *val3);
	
	static stnode_t* stnode_new(sttype_id_t type_id, void* data);
	static void stnode_free(stnode_t *node);
	static void stnode_new_data(stnode_t *node, void* data);
	static void stnode_free_data(stnode_t *node);
	static buffer_t* trans_to_buffer(const char* data);

	
	stnode_t* root;
private:
	bool is_eq(stnode_t *arg1, stnode_t *arg2, relay_msg_base& msg);
	
	std::list<token_node> token_list;
	static sttype_t* sttype_array; //array which length have STTYPE_NUM_TYPES 
	static int(*op_ref_table)[2];
	std::map<string, int> proto_map;
};
#endif
