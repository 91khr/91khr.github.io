---
title: "大概是篇Vim入门教程(1): 基本操作"
...

虽然其实已经有了很多很好的Vim教程了... 但是... senioria也来试着写写吧...

首先, 放一些senioria当年入门的时候看的教程:

- `vimtutor`, 这个是随Vim安装的一篇小教程脚本,
  在*NIX系统上可以在shell中键入命令`vimtutor`启动, 在Windows上(大概)可以在Vim的安装目录下找到`vimtutor.bat`.
- [coolshell上的教程](https://coolshell.cn/articles/5426.html)

[vim速查表什么的大概到处都可以找到](https://cn.bing.com/search?q=vim+cheat+sheet), 所以senioria就不放了(((

安装的话, 绝大多数*NIX发行版的软件源里应该都有gvim或者类似的包;
Windows上, [官网](https://www.vim.org/download.php)里有一个32位的版本,
[github](https://github.com/vim/vim-win32-installer/releases)上有64位的.
一般来说, 除非实在没办法下载, 否则都建议用64位版本:
32位版本无法链接64位的python, lua之类的dll[^1],
所以要不就也用它们的32位版本, 要不就装一个vim用的版本, 一个自己用的版本...

[^1]: 事实上是无法链接所有任何64位dll, 这里只是举了两个例子.

---

在介绍Vim之前, senioria想先回忆一下黑魂的操作:

