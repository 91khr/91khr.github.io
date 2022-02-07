---
title: Senioria的朋友们
description: 就是一些朋友的blog之类的?(
---

(小声: title好想写Senioriae

唔感觉什么都不说也有点不好... qwq
~~嗯这就说了点什么了x~~

这里用的人称大概都是Senioria用来称呼彼等的人称代词.
(是随机排序的 x (超小声(被打死((((((

<style>
.friend-list > ul {
    padding-left: 0;
}
.friend-list > ul > li {
    list-style: none;
    margin: 1em auto;
    background-color: #faebd752;
    border: 0.2ch solid gray;
}
.friend-list > ul > li >:first-child {
    padding: 0.5ch 0.7ch 0 0.7ch;
    margin: 0 0 0.5em;
    border-bottom: 1px solid gray;
}
.friend-list > ul > li >:first-child > a:first-child {
    font-size: 1.05em;
}
.friend-list > ul > li >:nth-child(2) {
    margin-top: 0;
}
.friend-list > ul > li >:nth-child(n+2) {
    margin: auto 0.7ch;
}
.friend-list > ul > li >:last-child {
    padding-bottom: 0.5ch;
    margin-bottom: 0;
}
</style>

## 还可用的链接

:::{ .friend-list }

- [Tirpitz](https://tirpitz.live)

  是最好的姐姐 w

- [锦心](https://lhcfl.github.io/)

  可爱的leprechuan, 想要抱紧紧揉揉揉揉揉脑袋, ~~用Seniorious戳戳奈子x~~. (超小声(

  希望能有一起吃羊肉卷, 喝加了蜂蜜的热牛奶的一天 >&lt; (超小声(

- [Coelacanthus](https://blog.coelacanthus.moe)

  > ~~c10s强强, c10s假假 (超小声(被打死(~~

  (但c10s可爱是真的, 嗯 x (超小声(

- [Clansty Icaria](https://Clansty.com)

  Clansty姐强强, 赞美Clansty姐 x (超小声(

- [秋雨落](https://blog.rain.cx)

  是可爱的橘猫 x (超小声(

- [Tomiya](https://tommy0103.github.io)

  是日常发情的可爱猫猫 x (超小声(被打死(((((((((((((

:::

## 已经淡出的链接...

::: { .friend-list }

- [清浔](https://cmath.cc) (似乎不可用了... 等什么时候去更新 qaq (超小声()))

- [moi](https://rain.moimo.me) (虽然不可用, 但友链不能删>&lt;)

:::


<script>
// Shuffle the friend list
for (let friList of document.getElementsByClassName('friend-list'))
{
    let fri = friList.children[0];
    fri.replaceChildren(...Array.from(fri.children)
        .map(n => [Math.random(), n])
        .sort((a, b) => a[0] < b[0])
        .map(n => n[1]));
}
</script>

