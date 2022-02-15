#!/bin/bash
#.others cannot be an extension

usagemessage="use directory(optional) inputfile as argument and inputfile has to be in same directory as this script" 
workdir='pwd'

# wrong number of argument
if [ $# -gt 2 ] || [ $# -eq 0 ]; then 
	echo "$usagemessage"
	exit 
fi

# input validation 
if [ "$#" -eq 2 ]; then 
	filename=$2
	workdir=$1
	if [ ! -d $workdir ]; then
		echo "directory not found"
		exit
	fi
	if [ ! -f $filename ]; then
		echo "wrong filename"
		exit
	fi
fi
if [ "$#" -eq 1 ]; then 
	filename=$1
	if [ ! -f "$filename" ]; then
		echo "wrong filename"
		exit
	fi
fi

#prepare output dir
rm -rf ./output_directory
mkdir output_directory
outdir=./output_directory

#save ignorable extensions to array arr
n=0
while IFS= read -r line
do
	line=${line::-1}
	n=$((n+1))
	arr[$n]=$line
done <$filename

#save all file paths to array allfiles
allfiles=( `find $workdir -type f` )


ignorecnt=0
for((i=0;i<${#allfiles[*]};i++))
do
	ignorable=0
	for((j=0;j<${#arr[*]};j++))
	do
		if [[ ${allfiles[$i]} == *.${arr[$j]} ]]; then
			ignorable=1
		fi
	done
	# not ignorable
	if [ $ignorable -eq 0 ]; then
		#echo ${allfiles[$i]}
		relpath=${allfiles[$i]}
		#echo $relpath
		absolutepath=(`readlink -f $relpath`)
		#echo $absolutepath
		justfilename=${relpath##*/}
		#echo $justfilename
		
		if [[ $justfilename == *.* ]]; then
			# has extension
			extension="${justfilename##*.}"
			
			mkdir -p $outdir/$extension
			txtfilename="$outdir/$extension/desc_$extension.txt"
			touch $txtfilename
			
			if [[ ! -f $outdir/$extension/$justfilename ]]; then
				cp $absolutepath $outdir/$extension	
				echo $relpath>>$txtfilename
			fi
		
		else
			# no extension
			
			mkdir -p "$outdir/others"
			txtfilename="$outdir/others/desc_others.txt"
			touch $txtfilename
			if [[ ! -f "$outdir/others/$justfilename" ]]; then
				cp $absolutepath "$outdir/others"	
				echo $relpath>>$txtfilename
			fi
		fi
	else 
		ignorecnt=$((ignorecnt+1))
	fi
done

# start work for csv
csvfile="output.csv"
if [[ -f $csvfile ]]; then
	rm $csvfile
fi
touch $csvfile
echo "file_type,no_of_files" >> $csvfile

logtexts=( `find $outdir -type f -name 'desc_*.txt'` )
for((i=0;i<${#logtexts[*]};i++))
do
# dirname ar basename use kore filename ar foldername ber kora jay
	DIR="$(dirname "${logtexts[$i]}")"
	DIR=${DIR##*/}
	foldername="$(basename "$DIR")"
	count=`wc -l < ${logtexts[$i]}`
	echo "$foldername,$count" >> $csvfile
done
echo "ignored,$ignorecnt" >> $csvfile




