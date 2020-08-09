---
title: Senioria的代码规范
description: (其实是给自己看的x
author: Senioria
...

# C++

~~(其实很多比较早的代码都没有遵循这些~~

- 头文件/目录/模块/命名空间: `snake_case`
- 类/概念: `CamelCase`
- 常量/宏: `ALL_CAPITAL`
- 函数: `snake_case`
- 成员: `snake_case`
- 局部变量/lambda: `nodelim`或者`snake_case`

模块和命名空间之类的东西一般会尽可能用复数, 比如`parsers`;
诸如`alisa_parser`这种特化的东西会用单数.

类的名称一般遵循对象+动词用作职业的名词这个形式, 比如`AlisaParser`;
或者类似`ParserInfo`这样的职业名词(作形容词)+名词的形式.
有些用作命名空间的类(比如`environments`)遵循命名空间的格式.

概念被用于描述一个对象所应该具有的性质, 正如其名称所述
(因此, 用于拓展的接口之类的会被做成类/接口/元类, 要求用户继承),
因此名称是用于描述状态的词, 比如`Sized`, `SizeChangable`.

常量的名称大概是名词.
一个特例是header guard, 会遵循头文件名大写+`_INCLUDED`的名字.

函数大概是Qt规则, 即动词+对象; get/set在不产生歧义的时候可能会被省略.
成员会尽量凑成名词/名词性短语.

局部变量会放飞自我(((

---

具体的规范的话:

- 大括号换行
- 控制语句的括号前空一格, `for`语句和各种带init的控制语句的分号后空格
- 一般来说, 二元(和三元)运算符的参数和运算符之间空格, 数组下标访问除外,
  一元运算符不空格
- 括号内不空格
- 函数名和返回类型写在同一行, 如果有换行必要, 尽量换类型的行
- header guard的`#endif`后加注释表明guard, 命名空间的回括号后加上`End namespace xxx`,
  对于匿名命名空间, 是`End anonymous namespace`

