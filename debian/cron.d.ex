#
# Regular cron jobs for the opensim package
#
0 4	* * *	root	[ -x /usr/bin/opensim_maintenance ] && /usr/bin/opensim_maintenance
