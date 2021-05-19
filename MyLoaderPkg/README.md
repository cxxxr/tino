```sh
# for mac
$ mkdir $HOME/dev
$ cd $HOME/dev
$ git clone git@github.com:cxxxr/tino
$ git clone git@github.com:tianocore/edk2
$ cd edk2
$ git checkout edk2-stable202011
$ git submodule update --init
$ source edksetup.sh
$ cp ../tino/target.txt ./Conf/target.txt
$ build
```
