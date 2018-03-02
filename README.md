# GA

## Overview
C++で実装されたGAプログラム

0/1データと実数値データを扱えるようにしています．

また，GAアルゴリズムに関しても「一点交叉」「二点交叉」「一様交叉」「ブレンド交叉」を実装しています．

（注意点）0/1データに対してブレンド交叉は使えないようにしています．

アルゴリズムは完成していますが，テストをまだ行っていない状態です．

なにか問題があった場合、もしお気づきになられたら修正 or 連絡をお願い致します。

## Usage
```
$ cmake CMakeLists
$ make
$ ./GA
```
これで一応テストプログラムは動作します．

## Requirement
C++11

cmake 3.6.2

## License
Copyright © 2016 T_Sumida Distributed under the MIT License.
