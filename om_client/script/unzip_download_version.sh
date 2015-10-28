#!/bin/sh

#$1=new_version eg. om_12, fpga_12
#$2=old_version eg. om_11, fpga_11
#$3=version_type: 0=software; 1=firmware; 2=unknow

#software_compress_filename
S_C_F=om.tgz
#firmware_compress_filename
F_C_F=fpga.tgz
#exec result: return value
R_V=0

if [ $3 -eq 0 ]; then
	if cd ver && tar -zxvf $S_C_F && rm -f $S_C_F && chmod +x $1;  then
		if mv -f $1 ../$1 && cd ../ && chmod +x $1;  then 
		     rm -f $2;
		else
			 cd ../ && rm -f $1 && cd ver && rm -f * && R_V=1
		fi
	else
	  cd ver && rm -f * && R_V=2
	fi
elif [ $3 -eq 1 ]; then
	if cd ver && tar -zxvf $F_C_F && rm -f $F_C_F && chmod +r ${1}_if.bit; then 
		if mv -f ${1}_if.bit ../${1}_if.bit && cd ../ && chmod +r ${1}_if.bit; then    
		     rm -f ${2}*
		else
		   cd ../ && rm -f ${1}* && cd ver && rm -f * && R_V=3
		fi
	else
	  cd ver && rm -f * && R_V=4
	fi
else
   R_V=5
fi

echo return_value is $R_V
exit $R_V


 
