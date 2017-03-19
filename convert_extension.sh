re='(.*)\.(c)'
for hello in *.c; do
if [[ $hello =~ $re ]]; then
  hello2=${BASH_REMATCH[1]}.cpp
fi
mv $hello "$hello2"
done
