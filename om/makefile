################################################################################
# CopyRight NTS Technology (ChengDu) Co., Ltd
################################################################################
# 文件名称：makefile
# 功    能: 提供om_wdg和om的编译配置，包括包含文件、输出文件及中间文
#           件；编译警告条件为Wall
# 版    本: v0.2
# 编写作者:	vic
# 编写日期:	20010/09/06
# 说    明:	交叉编译环境为powerpc-e300c3-linux-gcc，如果需要在虚拟机上试运行，请
#           将CC = $(PPCGCCBINPATH)/powerpc-e300c3-linux-gcc改为GCC = gcc
# 修改历史:	
# 修改日期    修改人  修改描述
################################################################################

#//////////////////////////////////////////////////////////////////////
#
#配置当前编译出的软件版本号。该版本号为4位BCD码，其格式和每位含义如下：
#               0           0             0          0
#          硬件版本号 软件大版本号 软件小版本号 软件调试版本号
#软件升级时:
#OM_WDG需要修改HW_VER_NO, SW_DEF_VER_NO和FW_DEF_VER_NO
#OM需要修改HW_VER_NO, SW_VER_NO和SW_MINI_VER_NO
#//////////////////////////////////////////////////////////////////////
#硬件版本号
HW_VER_NO=1

#软件版本号
SW_VER_NO=1
SW_MINI_VER_NO=04

#FPGA版本号
FW_VER_NO=1

#软件缺省版本号
#(表示没有start.config文件时的缺省启动版本)
SW_DEF_VER_NO=1

#FPGA缺省版本号
#(表示没有start.config文件时的缺省启动版本)
FW_DEF_VER_NO=1

#软件版本号全名
OM_SW_VER_NO=0x$(HW_VER_NO)$(SW_VER_NO)$(SW_MINI_VER_NO)

#FPGA模拟读取版本号全名
OM_FW_VER_NO=0x$(HW_VER_NO)$(FW_VER_NO)00

#///////////////////////////////////////////////////////////////////////
#
# 输出结果文件的名称定义
#
#///////////////////////////////////////////////////////////////////////
ITEMNAME = om
VERSION = $(HW_VER_NO)$(SW_VER_NO)
#WDGMODULENAME = $(ITEMNAME)_wdg
OMMODULENAME = $(ITEMNAME)_$(VERSION)

#///////////////////////////////////////////////////////////////////////
#
# 源文件根路径、中间文件路径、结果文件路径定义
#
#///////////////////////////////////////////////////////////////////////
MAKEFILEPATH = .
HOMEDIR = $(MAKEFILEPATH)
OBJPATH	= $(HOMEDIR)/obj
LIBPATH	= $(HOMEDIR)/lib
OUTPATH	= $(HOMEDIR)/out
SRCPATH = $(HOMEDIR)/src
INCLUDE = $(HOMEDIR)/include
DPDKINCLUDE = $(HOMEDIR)/dpdk/include
DPDKLIBPATH = $(HOMEDIR)/dpdk/lib

#INCLUDE += -I/usr/sqlite-x86/database/3.8.3.1/include
#DDINCLUDE = $(HOMEDIR)/dd_include
#DSPINCLUDE = $(HOMEDIR)/dsp_include
#DSPSRC = $(HOMEDIR)/dsp_src

#///////////////////////////////////////////////////////////////////////
#
# 编译器路径及其命令名称定义
#
#///////////////////////////////////////////////////////////////////////
#PPCGCCPATH = /opt/arm/nxp_toolchain
#PPCGCCBINPATH	=	$(PPCGCCPATH)/bin
CC = g++  -std=c++11
ECHO = /usr/bin/echo
#OBJCOPY = $(PPCGCCBINPATH)/arm-vfp-linux-gnu-objcopy
#GCC_PATH = /opt/arm/nxp_toolchain/bin
#CC = $(GCC_PATH)/arm-vfp-linux-gnu-g++
#///////////////////////////////////////////////////////////////////////
#
# 编译选项定义
#
#///////////////////////////////////////////////////////////////////////
CFLAGS  = -g3
CFLAGS += -Wall -Wshadow
#CFLAGS += -O3
CFLAGS += -D _GNU_SOURCE -D_BSD_SOURCE -D__FAVOR_BSD
CFLAGS +=  -pthread  -march=corei7  -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2
CFLAGS += -DHW_VER_NO=$(HW_VER_NO)
CFLAGS += -DSW_DEF_VER_NO=$(SW_DEF_VER_NO)
CFLAGS += -DFW_DEF_VER_NO=$(FW_DEF_VER_NO)
CFLAGS += -DOM_SW_VER_NO=$(OM_SW_VER_NO)
CFLAGS += -DOM_FW_VER_NO=$(OM_FW_VER_NO)

#///////////////////////////////////////////////////////////////////////
#
# 链接选项定义 -s是删除所有的调试信息
#
#///////////////////////////////////////////////////////////////////////
#LDFLAGS  = -static
#LDFLAGS += -s
#-Wl,-lrte_pdump -Wl,-lrte_ip_frag -Wl,-lrte_gro -Wl,-lrte_meter -Wl,-lrte_sched -Wl,-lrte_jobstats -Wl,-lrte_metrics -Wl,-lrte_bitratestats -Wl,-lrte_latencystats -Wl,-lrte_efd -Wl,--whole-archive -Wl,-lrte_hash -Wl,-lrte_vhost -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_net -Wl,-lrte_ethdev -Wl,-lrte_cryptodev -Wl,-lrte_eventdev -Wl,-lrte_mempool -Wl,-lrte_mempool_ring -Wl,-lrte_ring -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_reorder -Wl,-lrte_kni -Wl,-lrte_mempool_stack -Wl,-lrte_pmd_af_packet -Wl,-lrte_pmd_ark -Wl,-lrte_pmd_avp -Wl,-lrte_pmd_bnxt -Wl,-lrte_pmd_bond -Wl,-lrte_pmd_cxgbe -Wl,-lrte_pmd_e1000 -Wl,-lrte_pmd_ena -Wl,-lrte_pmd_enic -Wl,-lrte_pmd_fm10k -Wl,-lrte_pmd_failsafe -Wl,-lrte_pmd_i40e -Wl,-lrte_pmd_ixgbe -Wl,-lrte_pmd_kni -Wl,-lrte_pmd_lio -Wl,-lrte_pmd_nfp -Wl,-lrte_pmd_null -Wl,-lrte_pmd_qede -Wl,-lrte_pmd_ring -Wl,-lrte_pmd_sfc_efx -Wl,-lrte_pmd_thunderx_nicvf -Wl,-lrte_pmd_virtio -Wl,-lrte_pmd_vhost -Wl,-lrte_pmd_vmxnet3_uio -Wl,-lrte_pmd_aesni_mb -Wl,-lrte_pmd_null_crypto -Wl,-lrte_pmd_qat -Wl,-lcrypto -Wl,-lrte_pmd_crypto_scheduler -Wl,-lrte_pmd_skeleton_event -Wl,-lrte_pmd_sw_event -Wl,-lrte_pmd_octeontx_ssovf -Wl,--no-whole-archive -Wl,-lrt -Wl,-lm -Wl,-lnuma -Wl,-ldl
DPDK_LIBS_LD_FLAG = -pthread  -march=corei7  -DRTE_MACHINE_CPUFLAG_SSE -DRTE_MACHINE_CPUFLAG_SSE2 -DRTE_MACHINE_CPUFLAG_SSE3 -DRTE_MACHINE_CPUFLAG_SSSE3 -DRTE_MACHINE_CPUFLAG_SSE4_1 -DRTE_MACHINE_CPUFLAG_SSE4_2 -L$(DPDKLIBPATH) -Wl,-lrte_pipeline -Wl,-lrte_table -Wl,-lrte_port -Wl,-lrte_pdump -Wl,-lrte_distributor -Wl,-lrte_ip_frag -Wl,-lrte_meter -Wl,-lrte_sched -Wl,-lrte_lpm -Wl,--whole-archive -Wl,-lrte_acl -Wl,--no-whole-archive -Wl,-lrte_jobstats -Wl,-lrte_metrics -Wl,-lrte_bitratestats -Wl,-lrte_latencystats -Wl,-lrte_power -Wl,-lrte_timer -Wl,-lrte_efd -Wl,-lrte_cfgfile -Wl,--whole-archive -Wl,-lrte_hash -Wl,-lrte_vhost -Wl,-lrte_kvargs -Wl,-lrte_mbuf -Wl,-lrte_net -Wl,-lrte_ethdev -Wl,-lrte_cryptodev -Wl,-lrte_eventdev -Wl,-lrte_mempool -Wl,-lrte_mempool_ring -Wl,-lrte_ring -Wl,-lrte_eal -Wl,-lrte_cmdline -Wl,-lrte_reorder -Wl,-lrte_kni -Wl,-lrte_mempool_stack -Wl,-lrte_pmd_af_packet -Wl,-lrte_pmd_ark -Wl,-lrte_pmd_avp -Wl,-lrte_pmd_bnxt -Wl,-lrte_pmd_bond -Wl,-lrte_pmd_cxgbe -Wl,-lrte_pmd_e1000 -Wl,-lrte_pmd_ena -Wl,-lrte_pmd_enic -Wl,-lrte_pmd_fm10k -Wl,-lrte_pmd_i40e -Wl,-lrte_pmd_ixgbe -Wl,-lrte_pmd_kni -Wl,-lrte_pmd_lio -Wl,-lrte_pmd_nfp -Wl,-lrte_pmd_null -Wl,-lrte_pmd_qede -Wl,-lrte_pmd_ring -Wl,-lrte_pmd_sfc_efx -Wl,-lrte_pmd_tap -Wl,-lrte_pmd_virtio -Wl,-lrte_pmd_vhost -Wl,-lrte_pmd_vmxnet3_uio -Wl,-lrte_pmd_null_crypto -Wl,-lrte_pmd_crypto_scheduler -Wl,-lrte_pmd_skeleton_event -Wl,-lrte_pmd_sw_event -Wl,-lrte_pmd_octeontx_ssovf -Wl,--no-whole-archive -Wl,-lrt -Wl,-lm -Wl,-lnuma -Wl,-ldl
LDFLAGS += -Wall $(DPDK_LIBS_LD_FLAG)
LIBS = -lpthread 

#///////////////////////////////////////////////////////////////////////
#
# 头文件包含路径定义
#
#///////////////////////////////////////////////////////////////////////
INCFLAGS  = -I$(INCLUDE) -I$(DPDKINCLUDE)

#INCFLAGS += -I$(DDINCLUDE)
#INCFLAGS += -I$(DSPINCLUDE)

#///////////////////////////////////////////////////////////////////////
#
# OBJ文件列表定义
# 因为全局对象构建之前需要先初始化全局单例对象链表，因此object.o必须放在最后
#
#///////////////////////////////////////////////////////////////////////
OMOBJS := $(patsubst $(SRCPATH)/%.cpp, $(OBJPATH)/%.o, $(wildcard $(SRCPATH)/*.cpp))
			
#///////////////////////////////////////////////////////////////////////
#
# 链接命令定义
#
#///////////////////////////////////////////////////////////////////////
all : $(OMMODULENAME)

$(OMMODULENAME) : $(OMOBJS) 
	$(CC) -o $(OUTPATH)/$@ $^ $(LIBS) $(LDFLAGS)
	@echo $(OM_SW_VER_NO)
	@echo $(OM_FW_VER_NO)
#	@echo $(OM_SW_VER_NO) > ver.ini
#	$(OBJCOPY) --add-section .om_ver=ver.ini $(OUTPATH)/$(OMMODULENAME)
#	@rm -rf ver.ini

clean :
	rm $(OBJPATH)/*.o
	rm $(OBJPATH)/*.d

#///////////////////////////////////////////////////////////////////////
#
# 编译命令定义
#
#///////////////////////////////////////////////////////////////////////
$(OBJPATH)/%.o : $(SRCPATH)/%.cpp
	$(CC) -c -o $@ $< $(INCFLAGS) $(CFLAGS)
	
#///////////////////////////////////////////////////////////////////////
#
# 和添加头文件依赖相关的编译命令
# 
#///////////////////////////////////////////////////////////////////////
$(OBJPATH)/%.d : $(SRCPATH)/%.cpp
	@rm -rf $@
	@echo "Generate: "$@
	@${CC} -MM $< $(INCFLAGS) > $@.$$
	@sed 's,$(basename $(notdir $<)).o:,$(OBJPATH)/$(basename $(notdir $<)).o $@:,g' < $@.$$ > $@
	@rm -rf $@.$$
		
include ${OMOBJS:.o=.d}
