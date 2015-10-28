#include<string.h>
#include<iostream>
#include<stdio.h>

#include "XdrFilter.h"

using namespace united_dpi;

static void*
test_new(void* junk)
{
	test_t		*test;

	assert(junk == NULL);

	test = (test_t*)malloc(sizeof(test_t));

	test->op = TEST_OP_UNINITIALIZED;
	test->val1 = NULL;
	test->val2 = NULL;

	return (void*) test;
}

static void
test_free(void* value)
{
	test_t	*test = (test_t *)value;

	if (test->val1)
		XdrFilter::stnode_free(test->val1);
	if (test->val2)
		XdrFilter::stnode_free(test->val2);

	free(test);
}

//留着在实际解析中在trans_to_buffer再new
#if 0
static void*
buffer_new(void* junk)
{
	buffer_t		*stfuncrec;

	assert(junk == NULL);

	stfuncrec = (buffer_t*)malloc(sizeof(buffer_t));

	stfuncrec->len = 0;
	stfuncrec->param = NULL;

	return (void*) stfuncrec;
}
#endif

static void
buffer_free(void* value)
{
    if(value)
    {
    	buffer_t	*stfuncrec = (buffer_t*)value;
    	if(stfuncrec->param) free(stfuncrec->param);
    	free(stfuncrec);
    }
}

//------string
static void*
string_new(void* string)
{
	//assert(string);
	char* s = (char*)malloc(strlen((char*)string)+1);
	strcpy(s, (char*)string);
	
	return (void*)s;
}

static void
string_free(void* value)
{
	free(value);
}

static void*
range_new(void* junk)
{
	range_t		*range;

	assert(junk == NULL);

	range = (range_t*)malloc(sizeof(range_t));
    range->proto = FIELD_UNKNOW;
    range->start_pos = 0;    
    range->len = 0;

	return (void*) range;
}

static void
range_free(void* value)
{
	range_t	*range = (range_t*)value;
	free(range);
}

sttype_t* XdrFilter::sttype_array;
int(*XdrFilter::op_ref_table)[2];

XdrFilter::XdrFilter()
    : root(NULL)
{
    //注册所有的实体节点的类型，顺序不能乱
	static sttype_t type_list[STTYPE_NUM_TYPES] = 
	{
        {STTYPE_UNINITIALIZED,"UNINITIALIZED",NULL,NULL},
        {STTYPE_TEST, "TEST", test_new, test_free},
        {STTYPE_UNPARSED, "UNPARSED", string_new, string_free},
        {STTYPE_STRING, "STRING", string_new, string_free},            
        {STTYPE_BUFFER, "BUFFER", NULL, buffer_free}, //buffer类型在实际的解析中new
        {STTYPE_FIELD, "FIELD", NULL, NULL}, //向其注册int型的id
        {STTYPE_FVALUE, "FVALUE", NULL, NULL},
        {STTYPE_INTEGER, "INTEGER", NULL, NULL},
        {STTYPE_RANGE, "RANGE", range_new, range_free},
        {STTYPE_FUNCTION, "FUNCTION", NULL, buffer_free},
    };

    sttype_array = type_list;

    static int op_ref[][2] =
    {
        {TOKEN_TEST_EQ,TEST_OP_EQ},
        {TOKEN_TEST_NE,TEST_OP_NE},
        {TOKEN_TEST_GT,TEST_OP_GT},
        {TOKEN_TEST_GE,TEST_OP_GE},
        {TOKEN_TEST_LT,TEST_OP_LT},
        {TOKEN_TEST_LE,TEST_OP_LE},
        {TOKEN_TEST_BITWISE_AND,TEST_OP_BITWISE_AND},
        {TOKEN_TEST_CONTAINS,TEST_OP_CONTAINS},
        {TOKEN_TEST_MATCHES,TEST_OP_MATCHES},
        {TOKEN_TEST_NOT,TEST_OP_NOT},
        {TOKEN_TEST_AND,TEST_OP_AND},
        {TOKEN_TEST_OR,TEST_OP_OR}
    };
    op_ref_table = op_ref;

    proto_map["unknow"] = FIELD_UNKNOW;
    proto_map["ip"] = FIELD_IP;
    proto_map["ip.addr"] = FIELD_IP_ADDR;
    proto_map["ip.src"] = FIELD_IP_SRC_ADDR;
    proto_map["ip.dst"] = FIELD_IP_DST_ADDR;
    proto_map["ip.hl_len"] = FIELD_IP_HEAD_LEN;
    proto_map["ip.len"] = FIELD_IP_LEN;
    proto_map["ip.protocol"] = FIELD_IP_PROTOCOL;
    proto_map["tcp"] = FIELD_TCP;
    proto_map["tcp.port"] = FIELD_TCP_PORT;
    proto_map["tcp.srcport"] = FIELD_TCP_SRC_PORT;
    proto_map["tcp.dstport"] = FIELD_TCP_DST_PORT;
    proto_map["udp"] = FIELD_UDP;
    proto_map["udp.port"] = FIELD_UDP_PORT;
    proto_map["udp.srcport"] = FIELD_UDP_SRC_PORT;
    proto_map["udp.dstport"] = FIELD_UDP_DST_PORT;
    
}

XdrFilter::~XdrFilter()
{
    token_list.clear();
    proto_map.clear();

    if(root)
    {
        stnode_free(root);
    }
}


stnode_t* XdrFilter::stnode_new(sttype_id_t type_id, void* data)
{
	sttype_t	*type;
	stnode_t	*node;

	node = (stnode_t*)malloc(sizeof(stnode_t));

	if (type_id == STTYPE_UNINITIALIZED) {
		node->type = NULL;
		node->data = NULL;
	}
	else {
		type = &sttype_array[type_id];
		assert(type);
		node->type = type;
		if (type->func_new) {
			node->data = type->func_new(data);
		}
		else {
			node->data = data;
		}
	}
    

	return node;
}

void XdrFilter::stnode_free(stnode_t *node)
{
	if (node->type) {
		if (node->type->func_free) {
			node->type->func_free(node->data);
		}
	}
	else {
		assert(!node->data);
	}
	free(node);
}

void XdrFilter::stnode_new_data(stnode_t *node, void* data)
{
		if (node->type->func_new) {
			node->data = node->type->func_new(data);
		}
		else {
			node->data = data;
		}
    
}

void XdrFilter::stnode_free_data(stnode_t *node)
{
	if (node->type) {
		if (node->type->func_free) {
			node->type->func_free(node->data);
            node->data = NULL;
		}
	}
	else {
		assert(!node->data);
	}
}

buffer_t* XdrFilter::trans_to_buffer(const char* data)
{
    const unsigned int len = strlen(data);
    if((len+1)%3!=0 || len < 2) return NULL;
    unsigned int i = 0, j = 0;
    char* buf = (char*)malloc((len+1)/3);
    
    while(i<len){
        char tmp[2];
        if(data[i]>='0' && data[i]<='9') tmp[0] = (char)(data[i]-'0');
        else if(data[i]>='a' && data[i]<='f') tmp[0] = (char)(data[i]-'a'+10);
        else if(data[i]>='A' && data[i]<='F') tmp[0] = (char)(data[i]-'A'+10);
        else {free(buf); return NULL;}

        if(data[i+1]>='0' && data[i+1]<='9') tmp[1] = (char)(data[i+1]-'0');
        else if(data[i+1]>='a' && data[i+1]<='f') tmp[1] = (char)(data[i+1]-'a'+10);
        else if(data[i+1]>='A' && data[i+1]<='F') tmp[1] = (char)(data[i+1]-'A'+10);
        else {free(buf); return NULL;}

        buf[j] = ((tmp[0]<<4) | tmp[1]);
        j++;
        
        i+=3;
        if(i<len && i%3==0 && data[i-1]!=','){
            free(buf);return NULL;
        }
    }

    buffer_t* bf = (buffer_t*)malloc(sizeof(buffer_t));
    bf->len = (len+1)/3;
    bf->param = buf;
    printf("data:%s\n", data);
    for(i=0;i<bf->len;i++) printf("%02x",buf[i]);
    
    return bf;
}

int XdrFilter::scan(const char* pchSrc)
{
    int len = strlen(pchSrc);
    unsigned int pos=0,i=0;
    token_node token;
    token_node ast_token[]= //最长的特定字符放到最前面
    {
        {TOKEN_TEST_EQ,TEST_OP_EQ,"==",5},
        {TOKEN_TEST_NE,TEST_OP_NE,"!=",5},
        {TOKEN_TEST_GE,TEST_OP_GE,">=",5},
        {TOKEN_TEST_LE,TEST_OP_LE,"<=",5},
        {TOKEN_TEST_OR,TEST_OP_OR,"||",1},
        {TOKEN_TEST_AND,TEST_OP_AND,"&&",1},
        {TOKEN_TEST_NOT,TEST_OP_NOT,"!",2},
        {TOKEN_TEST_BITWISE_AND,TEST_OP_BITWISE_AND,"&",6},
        {TOKEN_LPAREN,TEST_OP_UNINITIALIZED,"(",0},
        {TOKEN_RPAREN,TEST_OP_UNINITIALIZED,")",0},
        {TOKEN_LBRACKET,TEST_OP_UNINITIALIZED,"[",6},
        {TOKEN_RBRACKET,TEST_OP_UNINITIALIZED,"]",6},
        {TOKEN_COLON,TEST_OP_UNINITIALIZED,":",6},
        {TOKEN_TEST_GT,TEST_OP_GT,">",5},
        {TOKEN_TEST_LT,TEST_OP_LT,"<",5},
        {TOKEN_TEST_CONTAINS,TEST_OP_CONTAINS,"contains ",5},
        {TOKEN_TEST_MATCHES,TEST_OP_MATCHES,"matches ",5}
    };
    char flags[]={' ','\t','=','!','>','<','|','&',':','(',')','[',']','\0','\n'};
    while(pos<len)
    {
        //跳过前面的空格
        while(pchSrc[pos]==' ' || pchSrc[pos]=='\t' || pchSrc[pos]=='\n') pos++;
        if(pos>=len) break;
        
        for(i=0;i<sizeof(ast_token)/sizeof(token_node);i++)
        {
            if(strncmp(pchSrc+pos, ast_token[i].value.c_str(), ast_token[i].value.size())==0){
                token_list.push_front(ast_token[i]);
                pos+=ast_token[i].value.size();
                break;//find: jump out for loop
            }
        }

        //not found special token
        if(i==sizeof(ast_token)/sizeof(token_node)){
            if(pchSrc[pos] == '\"'){//is string?
                const char* substr;
                if((substr=strstr(pchSrc+pos+1,"\"")) != NULL){
                    token.type = TOKEN_STRING;
                    token.value.assign(pchSrc+pos+1, substr-(pchSrc+pos+1));
                    token.priority = 0;
                    
                    token_list.push_front(token);
                    pos+=substr-(pchSrc+pos+1)+2;
                }else return -2;
            }else{
                unsigned int start = pos;
                while(pos<len){
                    for(i=0;i<sizeof(flags);i++){
                        
                        if(flags[i]==pchSrc[pos]){
                            break;
                        }
                    }

                    if(i<sizeof(flags)) break;
                    pos++;
                }

                if(pos == start) return -2;

                token.type = TOKEN_UNPARSED;
                token.value.assign(pchSrc+start, pos-start);
                token.priority = 0;
                
                token_list.push_front(token);
            }
            
        }
    }
/*
    list<token_node>::iterator ite;
    for(ite = token_list.begin();ite!=token_list.end();ite++){
        cout<<" " <<(*ite).value;
    }
    
    for(ite = token_list.begin();ite!=token_list.end();ite++){
        cout<<" " <<ite->priority;
    }
*/
    if(token_list.size()>0) return 0;
    else return -1;
}

int XdrFilter::num_operands(int op)
{
	switch(op) {
		case TEST_OP_UNINITIALIZED:
			break;
		case TEST_OP_EXISTS:
		case TEST_OP_NOT:
			return 1;
		case TEST_OP_AND:
		case TEST_OP_OR:
		case TEST_OP_EQ:
		case TEST_OP_NE:
		case TEST_OP_GT:
		case TEST_OP_GE:
		case TEST_OP_LT:
		case TEST_OP_LE:
		case TEST_OP_BITWISE_AND:
		case TEST_OP_CONTAINS:
		case TEST_OP_MATCHES:
			return 2;
	}
	return -1;
}

void
XdrFilter::sttype_test_set1(stnode_t *node, test_op_t op, stnode_t *val1)
{
	test_t	*test;

	test = (test_t*)node->data;

	assert(num_operands(op) == 1);
	test->op = op;
	test->val1 = val1;
}

void
XdrFilter::sttype_test_set2(stnode_t *node, test_op_t op, stnode_t *val1, stnode_t *val2)
{
	test_t	*test;

	test = (test_t*)node->data;

	assert(num_operands(op) == 2);
	test->op = op;
	test->val1 = val1;
	test->val2 = val2;
}

void
XdrFilter::sttype_test_set2_args(stnode_t *node, stnode_t *val1, stnode_t *val2)
{
	test_t	*test;

	test = (test_t*)node->data;

	if (num_operands(test->op) == 1) {
		assert(val2 == NULL);
	}
	test->val1 = val1;
	test->val2 = val2;
}

void
XdrFilter::sttype_test_get(stnode_t *node, test_op_t *p_op, stnode_t **p_val1, stnode_t **p_val2)
{
	test_t	*test;

	test = (test_t*)node->data;

	if (p_op)
		*p_op = test->op;
	if (p_val1)
		*p_val1 = test->val1;
	if (p_val2)
		*p_val2 = test->val2;
}

void XdrFilter::sttype_range_set(stnode_t *node, stnode_t *val1, stnode_t *val2, stnode_t *val3)
{
	range_t	*range;
    
    assert(val1->type->id==STTYPE_UNPARSED || val1->type->id==STTYPE_STRING);
    assert(val2->type->id==STTYPE_UNPARSED || val2->type->id==STTYPE_STRING);
    assert(val3->type->id==STTYPE_UNPARSED || val3->type->id==STTYPE_STRING);

    range = (range_t*)node->data;
    string str_pro = (char*)val3->data;
    std::map<string,int>::iterator it = proto_map.find(str_pro);
    assert(it!=proto_map.end());
    
    range->proto = (field_protocol_t)it->second;
    range->start_pos = strtol((char*)val2->data, 0, 0);    
    range->len = strtol((char*)val1->data, 0, 0);
    
    XdrFilter::stnode_free(val1);
    XdrFilter::stnode_free(val2);
    XdrFilter::stnode_free(val3);
}

int XdrFilter::make_ast()
{
    std::list<token_node> op_stack;
    std::list<stnode_t*> node_stack;
    token_node token;

    while(token_list.size()>0)
    {
        token = token_list.back();
        token_list.pop_back();
        switch(token.type)
        {
            case TOKEN_LPAREN: //遇"("直接入操作符栈
                op_stack.push_back(token);
            break; 
            
            case TOKEN_RPAREN: //遇")"，计算到操作符栈的最近一个"("为止
            {
                token_node pre_op = op_stack.back();
                op_stack.pop_back();
                
                while(pre_op.type != TOKEN_LPAREN)
                {
                    int n = num_operands(pre_op.op);
                    if(n < 0) return -1;
                    else if(n==1){
                        //新建一个test_t类型的节点,然后取节点栈里一个值，与之结合成新结点
                        stnode_t* arg1=node_stack.back();
                        node_stack.pop_back();
                        
                        stnode_t* new_st = stnode_new(STTYPE_TEST, NULL);
                        sttype_test_set1(new_st, pre_op.op, arg1);
                        node_stack.push_back(new_st);
                    }else if(n==2){
                        //新建一个test_t类型的节点,然后取节点栈里一个值，与之结合成新结点
                        stnode_t* arg2=node_stack.back();
                        node_stack.pop_back();
                        stnode_t* arg1=node_stack.back();
                        node_stack.pop_back();
                        
                        stnode_t* new_st = stnode_new(STTYPE_TEST, NULL);
                        sttype_test_set2(new_st, pre_op.op, arg1, arg2);
                        node_stack.push_back(new_st);
                    }
                    
                    pre_op = op_stack.back();
                    op_stack.pop_back();
                }
                
                //运行到这里，说明已经到"("处了
                assert(pre_op.type == TOKEN_LPAREN);
            }
            break; 
            
            case TOKEN_LBRACKET:
                op_stack.push_back(token);
            break; 
            
            case TOKEN_RBRACKET:
                //op stack should be -- "[" ":"
                //node stack should be--"protocal" "start position" "length"
                {
                    assert(op_stack.back().type == TOKEN_COLON);
                    op_stack.pop_back();
                    assert(op_stack.back().type == TOKEN_LBRACKET);
                    op_stack.pop_back();
                    
                    stnode_t* arg1=node_stack.back();
                    node_stack.pop_back();
                    stnode_t* arg2=node_stack.back();
                    node_stack.pop_back();
                    stnode_t* arg3=node_stack.back();
                    node_stack.pop_back();
                    
                    stnode_t* new_st = stnode_new(STTYPE_RANGE, NULL);
                    sttype_range_set(new_st, arg1, arg2, arg3);
                    node_stack.push_back(new_st);
                }
            break; 
            
            case TOKEN_COLON:
                op_stack.push_back(token);
            break; 
            
            case TOKEN_UNPARSED:
            {
                stnode_t* new_st = stnode_new(STTYPE_UNPARSED, (void*)token.value.c_str());
                node_stack.push_back(new_st);

            }
            break; 
            
            case TOKEN_STRING:
            {
                stnode_t* new_st = stnode_new(STTYPE_STRING, (void*)token.value.c_str());
                node_stack.push_back(new_st);

            }
            break; 

            case TOKEN_TEST_EQ:
            case TOKEN_TEST_NE:
            case TOKEN_TEST_GT:
            case TOKEN_TEST_GE:
            case TOKEN_TEST_LT:
            case TOKEN_TEST_LE:
            case TOKEN_TEST_BITWISE_AND:
            case TOKEN_TEST_CONTAINS:
            case TOKEN_TEST_MATCHES:
            case TOKEN_TEST_NOT:
            case TOKEN_TEST_AND:
            case TOKEN_TEST_OR:
                while(!op_stack.empty() && token.priority <= op_stack.back().priority)
                {
                    token_node pre_op = op_stack.back();
                    op_stack.pop_back();
                    //判断之前的操作符是单目还是双目
                    //assert(op_ref_table[pre_op.type-5][0] == pre_op.type-5);
                    int n = num_operands(pre_op.op);
                    if(n < 0) return -1;
                    else if(n==1){
                        //新建一个test_t类型的节点,然后取节点栈里一个值，与之结合成新结点
                        stnode_t* arg1=node_stack.back();
                        node_stack.pop_back();
                        
                        stnode_t* new_st = stnode_new(STTYPE_TEST, NULL);
                        sttype_test_set1(new_st, pre_op.op, arg1);
                        node_stack.push_back(new_st);
                    }else if(n==2){
                        //新建一个test_t类型的节点,然后取节点栈里一个值，与之结合成新结点
                        stnode_t* arg2=node_stack.back();
                        node_stack.pop_back();
                        stnode_t* arg1=node_stack.back();
                        node_stack.pop_back();
                        
                        stnode_t* new_st = stnode_new(STTYPE_TEST, NULL);
                        sttype_test_set2(new_st, pre_op.op, arg1, arg2);
                        node_stack.push_back(new_st);
                    }

                }

                op_stack.push_back(token);
            break; 
            
        }
    }

    //退出时清理op_stack
    while(!op_stack.empty())
    {
        token_node pre_op = op_stack.back();
        op_stack.pop_back();
        
        int n = num_operands(pre_op.op);
        if(n < 0) return -1;
        else if(n==1){
            //新建一个test_t类型的节点,然后取节点栈里一个值，与之结合成新结点
            stnode_t* arg1=node_stack.back();
            node_stack.pop_back();
            
            stnode_t* new_st = stnode_new(STTYPE_TEST, NULL);
            sttype_test_set1(new_st, pre_op.op, arg1);
            node_stack.push_back(new_st);
        }else if(n==2){
            //新建一个test_t类型的节点,然后取节点栈里一个值，与之结合成新结点
            stnode_t* arg2=node_stack.back();
            node_stack.pop_back();
            stnode_t* arg1=node_stack.back();
            node_stack.pop_back();
            
            stnode_t* new_st = stnode_new(STTYPE_TEST, NULL);
            sttype_test_set2(new_st, pre_op.op, arg1, arg2);
            node_stack.push_back(new_st);
        }
                
    }

    if(node_stack.size()!=1) return -2;

    root=node_stack.back();
    node_stack.pop_back();
    
    return 0;
}

//do semantic check, and turn TOKEN_TEST_UNPARSED type to field/buffer/int
int XdrFilter::semantic_check(stnode_t* node)
{
    switch(node->type->id){
        case STTYPE_UNINITIALIZED:
            return -1; 

        case STTYPE_TEST:
        {
            test_t* test_node = (test_t*)(node->data);

            if(test_node->val1 != NULL){
                if(semantic_check(test_node->val1) < 0)
                    return -1;
            }
            
            if(test_node->val2 != NULL){
                if(semantic_check(test_node->val2) < 0)
                    return -1;
            }

        }
        break; 

        case STTYPE_UNPARSED:
        {
            //is a field?
            char* pchData = (char*)node->data;
            string str_pro = pchData;
            std::map<string,int>::iterator it = proto_map.find(str_pro);
            if(it!=proto_map.end()){//free之前的unpased类型的数据，替换成新的field类型(int)
            
                stnode_free_data(node);
                node->type = &sttype_array[STTYPE_FIELD];
                node->value = it->second;
            }else{
                //既可以转成integer又可以转成buffer，暂时优先转成integer
                //is integer?
                char* endptr;
                node->value = strtol(pchData,&endptr,0);
                if(*endptr == '\0'){
                    stnode_free_data(node);
                    node->type = &sttype_array[STTYPE_INTEGER];
                }else{
                    //is a buffer? buffer format(16-based): 1f,53,74,99,ae
                    buffer_t* buf = trans_to_buffer(pchData);
                    if(buf){
                        stnode_free_data(node);
                        node->type = &sttype_array[STTYPE_BUFFER];
                        node->data = buf;
                    }else{
                        return -2;
                    }
                }
                
            }
            

        }
        break; 

        case STTYPE_BUFFER:
        break; 

        case STTYPE_STRING:
        {
        }
        break; 

        case STTYPE_FIELD:
        break; 

        case STTYPE_FVALUE:
            break; 

        case STTYPE_INTEGER:
            break; 

        case STTYPE_RANGE:
            break; 

        case STTYPE_FUNCTION:
            break; 
        default:
            return -3;
    }
    return 0;
}

bool XdrFilter::exec_msg(stnode_t* node, relay_msg_base& msg) //执行过滤
{
    switch(node->type->id){
        case STTYPE_UNINITIALIZED:
            return true; 

        case STTYPE_TEST:
        {
            test_t* test_node = (test_t*)(node->data);
            switch(test_node->op)
            {
                case TEST_OP_NOT:
                {
                    if(exec_msg(test_node->val1,msg) == true) return false;
                    else return true;
                }
                break;

                case TEST_OP_AND:
                {
                    return (exec_msg(test_node->val1,msg) == true && exec_msg(test_node->val2,msg) == true);
                }
                break;

                case TEST_OP_OR:
                {
                    return (exec_msg(test_node->val1,msg) == true || exec_msg(test_node->val2,msg) == true);
                }
                break;

                case TEST_OP_EQ:
                {
                    return is_eq(test_node->val1,test_node->val2,msg);
                }
                break;
            
                default:
                    return false;
            }

            if(test_node->val1 != NULL){
                if(semantic_check(test_node->val1) < 0)
                    return -1;
            }
            
            if(test_node->val2 != NULL){
                if(semantic_check(test_node->val2) < 0)
                    return -1;
            }

        }
        break; 

        case STTYPE_FIELD:
        {
            switch(node->value){
                case FIELD_IP:
                    return true;
                case FIELD_TCP:
                    return msg.commInfo.u8L4Protocal == 0x6;
                case FIELD_UDP:
                    return msg.commInfo.u8L4Protocal == 0x11;
                default:
                    return false;
            }
        }
        break; 

        default:
            return false;
    }
}

bool XdrFilter::is_eq(stnode_t *arg1, stnode_t *arg2, relay_msg_base& msg)
{
    switch(arg1->type->id){
        case STTYPE_FIELD:
        {
            switch(arg1->value){
                case FIELD_IP:
                    return true;//debug
                case FIELD_IP_ADDR:
                    return (msg.commInfo.u32InDstIp == arg2->value || msg.commInfo.u32InSrcIp == arg2->value);
                case FIELD_IP_SRC_ADDR:
                    return (msg.commInfo.u32InSrcIp == arg2->value);
                case FIELD_IP_DST_ADDR:                    
                    return (msg.commInfo.u32InDstIp == arg2->value);
                case FIELD_IP_HEAD_LEN:
                    return true;
                case FIELD_IP_LEN:
                    return msg._raw_buf._raw_buf_len - msg.commInfo.u16InnerIpPos == arg2->value;
                case FIELD_IP_PROTOCOL:
                    return msg.commInfo.u8L4Protocal == arg2->value;
                case FIELD_TCP:
                    return msg.commInfo.u8L4Protocal == 0x6;
                case FIELD_TCP_PORT:
                    return (msg.commInfo.u8L4Protocal==0x6
                        &&(msg.commInfo.u16InSrcPort==arg2->value || msg.commInfo.u16InDstPort==arg2->value));
                case FIELD_TCP_SRC_PORT:
                    return (msg.commInfo.u8L4Protocal==0x6
                        &&msg.commInfo.u16InSrcPort==arg2->value);
                case FIELD_TCP_DST_PORT:
                    return (msg.commInfo.u8L4Protocal==0x6
                        &&msg.commInfo.u16InDstPort==arg2->value);
                case FIELD_UDP:
                    return msg.commInfo.u8L4Protocal == 0x11;
                case FIELD_UDP_PORT:
                    return (msg.commInfo.u8L4Protocal==0x11
                        &&(msg.commInfo.u16InSrcPort==arg2->value || msg.commInfo.u16InDstPort==arg2->value));
                case FIELD_UDP_SRC_PORT:
                    return (msg.commInfo.u8L4Protocal==0x11
                        &&msg.commInfo.u16InSrcPort==arg2->value);
                case FIELD_UDP_DST_PORT:
                    return (msg.commInfo.u8L4Protocal==0x11
                        &&msg.commInfo.u16InDstPort==arg2->value);
            }
        }
        break; 

        case STTYPE_RANGE:
        {
            range_t* range = (range_t*)arg1->data;
            unsigned int pos = 0;
            switch(range->proto){
                case FIELD_IP:
                    pos = msg.commInfo.u16InnerIpPos;
                    break;
                case FIELD_TCP:
                    if(msg.commInfo.u8L4Protocal!=0x6) return false;
                    pos = msg.commInfo.u16InnerL4Pos;
                    break;
                case FIELD_UDP:
                    if(msg.commInfo.u8L4Protocal!=0x11) return false;
                    pos = msg.commInfo.u16InnerL4Pos;
                    break;
                default:
                    return false;
            }

            switch(arg2->type->id){
                case STTYPE_STRING:
                {
                    char* data = (char*)arg2->data;
                    if(msg._raw_buf._raw_buf_len - 
                        (pos+range->start_pos+range->len) < 0) return false;
                    if(strncmp(data,msg._raw_buf._raw_buf+pos+range->start_pos,range->len)==0)
                    {
                        return true;
                    }else
                    {return false;}
                }
                break;

                case STTYPE_BUFFER:
                {
                    buffer_t* buf = (buffer_t*)arg2->data;
                    
                    if(buf->len!=range->len||msg._raw_buf._raw_buf_len - 
                        (pos+range->start_pos+range->len) < 0) return false;
                    if(memcmp(buf->param,msg._raw_buf._raw_buf+pos+range->start_pos,range->len)==0)
                    {
                        return true;
                    }else
                    {return false;}
                }
                break;

                default:
                    return false;
            }
        }
        break;

        default:
            return false;
    }
    return true;
}

