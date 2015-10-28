#!/bin/sh

#$1=version_type: 0=software; 1=firmware; 2=unknow
#$2=back_version eg. om_12, fpga_12
#$3=exec_version eg. om_13, fpga_13
#$4=def_version  eg. om_00, fpga_00

#software_compress_filename
S_C_F=om_default.tgz
#firmware_compress_filename
F_C_F=fpga_default.tgz
#exec result: return value
R_V=0

if [ $1 -eq 0 ]; then
	if cp -f $S_C_F ver/$S_C_F && cd ver && tar -zxvf $S_C_F && rm -f $S_C_F && chmod +x $4;  then
		if cd ../ && rm -f $2 $3;  then 
			mv -f ver/$4 $4 && chmod +x $4
		else
			cd ver && rm -f * && R_V=1
		fi
	else
		cd ver && rm -f * && R_V=2
	fi
elif [ $1 -eq 1 ]; then
	if cp -f $F_C_F  ver/$F_C_F && cd ver && tar -zxvf $F_C_F && rm -f $F_C_F && chmod +r ${4}_if.bit; then 
		if cd ../ && rm -f ${2}* ${3}*; then    
			mv -f ver/${4}_if.bit ${4}_if.bit && chmod +r ${4}_if.bit
		else
			cd ver && rm -f * && R_V=3
		fi
	else
		cd ver && rm -f * && R_V=4
	fi
else
   R_V=5
fi

echo return_value is $R_V
exit $R_V


 
