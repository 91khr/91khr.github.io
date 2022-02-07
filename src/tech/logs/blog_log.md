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

# 2020-05-24

commit log:

> Fixed description, add TOC

~~按照Senioria的习惯, 这个应该和上面那个合并才对~~

修了description的显示, 不然index里的和文章里的是不一致的.

加了目录功能, 暂时没有什么美化的念头(aka. 咕咕咕咕咕咕)

---

虽然表现出来只有一点更改, 但是做的工作不亚于上次的大规模重构呢(趴
其实上次也是这样, 界面的更改并不算很大,
但是背后重构`build.cpp`来加catalog功能的代码和对index架构的更改着实费了很大的一番力气
(其实是因为一开始的架构并不正确, 花了很长时间修不正确的架构)

算是部分理解了听到"加个按钮"之后的心情吧(苦笑(((

# 2020-08-05

commit log:

> Added custom prompt, article & friends link

加了个友链, 为了文章改了下目录结构... ><

---

Senioria... 太咕... qwq
没有什么精神去努力... 虽然不知道为什么><
还是那句话... Senioria因为自闭失去的东西太多了... (sigh

蛮好奇那些静态页面生成器是怎么做索引的...
难道也要每篇文章都扫一遍? ><

# 2021-08-22

commit log:

> Fixed style
>
> btw, add invokation to build.cpp

稍微改了改修了修样式之类的, 希望能更阳间一点 qwq
也顺便让`build.cpp`会自动运行了.

唔嗯... 除掉高考咕掉的一年, 还咕了高考之后的大半个暑假... qwq >\_<
想做tags支持, 但是tags支持的工作量... 想想就好大:

- 在各种依靠collection的地方添加tags对应的collection;
- 在filter里加上扫描, 处理和写入tags的代码;
- 正确处理各种ignored和hidden.

所以还是咕到下一次吧, 无论下一次是什么时候> \< (

不过还是加了tags的branch.
说起来, 这是senioria第一次用比较认真的态度加branch?

# 2022-02-08

commit log:

> Periodric update...

因为其实是好久以来的一堆修改叠在一起的... 所以其实也并没有什么好说的/好做的...
只能说似乎senioria这段时间里一直在咕咕... (sigh... (超小声(

其实这个commit也没改什么... 大概只是改了改友链相关, 以及稍微修了修一些历史代码的样子... (超小声(

(sigh... (超小声()))
想要做点什么, 却又似乎一直不知道为什么就是在咕咕咕... (sigh... (超小声(
活没有干, 各种该写的东西似乎都没有写的样子... > \< (超小声(

无论如何... 慢慢来吧... qwq... (超小声(

如果可能的话... 也还是想大改一下build system的... (超小声(
不过顺便说一句, 果然配置这种东西是越改越好用的呢 x x (超小声(

