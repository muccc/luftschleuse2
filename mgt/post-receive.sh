
TMP=$(mktemp -d -p ~gitosis/tmp/)
git clone file:///srv/gitosis/repositories/keyholder.git $TMP/repo

for keyholder in $(ls $TMP/repo/keys/*.pub);
do
  echo "Validating ${keyholder}"
  ssh-keygen -l -f "${keyholder}"
done

for keyholder in $(ls $TMP/repo/keys/*.pub);
do
  cat $keyholder >> $TMP/merged.pub
done

scp $TMP/merged.pub root@luftschleuse:/home/authorized_keys
rm -rf $TMP