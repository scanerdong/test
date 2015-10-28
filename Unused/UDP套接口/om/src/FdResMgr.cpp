/***************************************************************************
 *   
 *                                NTS��˾��Ȩ����
 *              CopyRight NTS Technology (ChengDu) Co., Ltd.   All Rights Reserved
 *
***************************************************************************/
/**
 * @filename : FdResMgr.cpp
 * @version : 1.0.0
 * @brief : ʵ���ļ���������Դ������
 *   
 * @author : zhangx
 * @date : 2011��10��28�� 9:35:28
 * @history :
 *   
 */

/************************���ò���*****************************/
#include "FdResMgr.h"


/************************ǰ����������***********************/


/************************���岿��*****************************/


/************************ʵ�ֲ���*****************************/
//CFdResMgr
_enResult CFdResMgr::Init()
{	
    OM_INFO("Init CFdResMgr.\n");

	m_thisMutexId = ADD_MUTEX();
   
	return SUCC;
}

_enResult CFdResMgr::Destroy()
{           
    OM_INFO("Destroy CFdResMgr.\n");
    
    m_clFdDesList.clear();
    DEL_MUTEX(m_thisMutexId);
    
	return SUCC;
}

_enResult CFdResMgr::Reset()
{
	return SUCC;
}

_enResult CFdResMgr::Run()
{
	return SUCC;
}

_enResult CFdResMgr::Dump(FILE* pf)
{	  
    CFdDesList::iterator li;
    
    AUTO_MUTEX(m_thisMutexId);
    
    DUMP_PRINT(pf, "CFdResMgr::Dump there are %d fd(s) in system, They are: \r\n", m_clFdDesList.size());
    for(li = m_clFdDesList.begin(); li != m_clFdDesList.end(); li++)
    {
        DUMP_PRINT(pf, "Fd 0x%x, %s\r\n", (*li).m_i32Fd, (*li).m_strDes.c_str());
    }
    
    return SUCC;
}

_enResult CFdResMgr::RegisterRes(int32 i32Fd, const string& strDes)
{
    CFdDesNode clFdDesNode(i32Fd, strDes);

    AUTO_MUTEX(m_thisMutexId);
    m_clFdDesList.push_back(clFdDesNode);

    return SUCC;
}

_enResult CFdResMgr::DeregisterRes(int32 i32Fd)
{
    CFdDesList::iterator li;
    
    AUTO_MUTEX(m_thisMutexId);
    
    for(li = m_clFdDesList.begin(); li != m_clFdDesList.end(); li++)
    {
        if((*li).m_i32Fd == i32Fd)
        {
            m_clFdDesList.erase(li);
            break;
        }
    }

    return SUCC;
}


