### 初期セットアップ

```sh
$ mkdir $HOME/dev
$ cd $HOME/dev
$ git clone git@github.com:cxxxr/tino
$ git clone -b edk2-stable202011 git@github.com:tianocore/edk2
$ cd edk2
$ git submodule update --init
$ source edksetup.sh
$ cp ../tino/target.txt ./Conf/target.txt
$ make -C /home/user/dev/edk2/BaseTools/Source/C
$ build
```

### ビルドと起動

```sh
$ cd $HOME/dev/edk2
$ source edksetup.sh
$ build
$ cd $HOME/dev/tino
$ ./run.sh $HOME/dev/edk2/Build/MyLoaderX64/DEBUG_CLANG38/X64/Loader.efi
```
