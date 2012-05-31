rm ./compiler.sh compiler.sh.tar.gz*
wget http://krita.org/builder/compiler.sh.tar.gz
tar -xzvf ./compiler.sh.tar.gz
rm ./compiler.sh.tar.gz
./compiler.sh | tee ./calligra-builder.log
