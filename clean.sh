# !usr/bin/bash
rm -rf my_license output_pc output_projector

if [ -s debug_pc.json ]
then
	rm debug_pc.json
fi

if [ -s debug_projector.json ]
then
	rm debug_projector.json
fi

if [ -s license-manager-pc-debug ]
then
	rm license-manager-pc-debug
fi

if [ -s license-manager-projector-debug ]
then
	rm license-manager-projector-debug
fi

if [ -s license-manager-pc ]
then
	rm license-manager-pc
fi

if [ -s license-manager-projector ]
then
	rm license-manager-projector
fi

if [ -s trial-test-debug ]
then
	rm trial-test-debug
fi

if [ -s trial-test ]
then
	rm trial-test
fi

if [ -s debug-trial.json ]
then
	rm debug-trial.json
fi

hostname=`hostname`
user=`whoami`
file_name="$hostname-$user.json"
file_name=`echo "$file_name" | awk '{print tolower($0)}'`

if [ -s ./src/generator/status/pc_status/"$file_name" ]
	then
		
		mv ./src/generator/status/pc_status/"$file_name" \
		   ./src/generator/status/pc_status/status.json
fi

source="src/projector_licensing/projector_serial.txt"
file_name=`awk 'NR==1' "$source"`

if [ -s ./src/generator/status/projector_status/"$file_name.json" ]
then
	mv ./src/generator/status/projector_status/"$file_name.json" \
	   ./src/generator/status/projector_status/status.json
fi

trial_file="C:/Users/$user/.check"
if [ -s "$trial_file" ]
then
	rm "$trial_file"
fi