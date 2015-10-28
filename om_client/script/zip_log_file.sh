#!/bin/sh

#$1=log_type: 0=alarm_log, 1=user_log, 2=system_log
#alarm_log_compress_filename
A_C_F=alarm_log.tgz
#user_log_compress_filename
U_C_F=user_log.tgz
#system_log_compress_filename
S_C_F=system_log.tgz
#exec result:return value
R_V=0

if [ $1 -eq 0 ]; then
	if cd /log; then
		tar -czvf $A_C_F alarm_log_[1-6].log
	else
		R_V=1
	fi
elif [ $1 -eq 1 ]; then
	if cd /log; then
		tar -czvf $U_C_F user_log_[1-6].log
	else
		R_V=2
	fi
elif [ $1 -eq 2 ]; then
	if cd /log; then
		tar -czvf $S_C_F system_log_[1-6].log
	else
		R_V=3
	fi
else
	R_V=4
fi

echo retuen value is  $R_V
exit $R_V






