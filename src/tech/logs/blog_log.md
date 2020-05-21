---
title: Blog Log
author: Senioria
description: 对这个blog的一点说明和计划
...

~~有预感这些logs迟早都会成为日记~~
(~~其实已经是了~~)

# 2020-05-21

昨天520呢, 没去要糖好可惜(逃

今天实际上没有提交><

---

Review了一下[`build.cpp`](/build.cpp)的代码, 感觉... 好乱...
不愧是Senioria一个下午边查文档边用C++糊出来的东西(逃

但是... 因为[介绍里说过](../../about.html), 所以似乎只能用C++来糊(lua糊就更乱了呢).
所以打算以后重构.

下次重构`build.cpp`打算做的事情:

- 把index的生成换成直接生成html代码(这个需要`filter.lua`配合)
- 加入对catalog的支持(对每个目录, 读取它下面的`index.md`, 并根据index的模板来生成对应的文档)
  这实际上也解决了到目前为止index.html仍然有很大的手写成分的问题(((

然后, 重构index界面也在日程之中了. 整体的界面风格不打算变,
具体的风格的话... 想加点lisp元素... 但是直接加会有点丑所以还在设计中...

# 2020-05-23

commit log:

> Add code block & basic catalog support

诶居然这么快就摸完了呀(((

---

[上次](#section)[^1]画的饼都做完了w Scheme风格的index排版有点丑... 但是看起来还不错w

少女感什么的... 色弱的Senioria做不到... qwq

[^1]: 明明就在上面x

