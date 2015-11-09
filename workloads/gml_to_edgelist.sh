cat <( grep -o "source [0-9]\{1,\}" $1 | grep -o "[0-9]\{1,\}" ) <(grep -o "target [0-9]\{1,\}" $1 | grep -o "[0-9]\{1,\}") | sort -n | uniq | nl -v 0 | awk '{ print $2 " " $1}' > .tmp_map; \
paste -d' ' <( grep -o "source [0-9]\{1,\}" $1 | grep -o "[0-9]\{1,\}" ) <(grep -o "target [0-9]\{1,\}" $1 | grep -o "[0-9]\{1,\}") > .tmp_edgelist; \
tail -n 1 .tmp_map | awk '{ V = $2 + 1; print V }' > $2; \
awk 'NR==FNR {map[$1]=$2; next}{print map[$1],map[$2]}' .tmp_map .tmp_edgelist >> $2; rm .tmp_map .tmp_edgelist
