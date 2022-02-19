#!/bin/sh

# -o: output file

help() {
	printf '%s\n' 'Usage: comp.sh -o outfile infile.bf'
	printf '%s\n' 'Compile infile.bf into executable outfile'
}

BFC="$(command -v bfc 2>/dev/null)"
if [ -z "$BFC" ]; then
	if [ -f bfc ]; then
		BFC="./bfc"
	else
		echo "No bfc command found" >&2
		exit 1
	fi
fi


ofile=""
ifile=""

while [ $# -gt 0 ]; do
	case "$1" in
		-h|--help)
			help
			exit 0
			;;
		-o)
			shift
			ofile="$1"
			;;
		*)
			ifile="$1"
			;;
	esac
	shift
done

[ -z "$ofile" ] && { echo "must specify output file" >&2; exit 1; }
[ -z "$ifile" ] && { echo "must specify input file" >&2; exit 1; }

TMP_ASM="$(mktemp --suffix='.asm')"
TMP_O="$(mktemp --suffix='.o')"

$BFC -o "$TMP_ASM" "$ifile"
as -o "$TMP_O" "$TMP_ASM"
ld -o "$ofile" "$TMP_O"

rm "$TMP_ASM" "$TMP_O"
