# Senioriae blog

从好久以前就一直在咕咕的一个blog.

以后也会咕咕下去的 (!) x (超小声(被打死((((((

## 目录结构

作为整个repo的一部分的:

- `src`: blog的源文件;
- `etc`: 需要copy到输出的站点下的文件;
- `dev`: 各种需要的中间源码文件.

编译输出的:

- `out`: 输出的站点;
- `index`: 生成的对每个文件的中间缓存.

## 自定义filter

源文件目录下可以用`filter.lua`来自定义一些预处理步骤, 以及哪些文件和子目录是需要处理的.

`filter.lua`需要返回一个table, 表示这个目录的信息, 可以有如下字段, 它们都是可选的:

- `article, subdirs, unindiced : string[]`: 分别是需要处理的文章, 子目录, 和不加入索引的文章的路径列表,
  需要是相对repo的相对路径或者绝对路径. 默认为空;
- `build_args : string`: 构建的参数, 参数需要以`--template=dev/`结尾, 用于让生成器加入不同的模板信息.
  默认为`default_build_args`.

可以使用这些函数和变量:

- 运行`filter.lua`时传递给它的参数是这个目录相对repo的路径;
- `default_build_args: string`: 默认的构建参数;
- `list_dirs(path: string): { path: string, type: "file" | "dir" }[]`: 列出`path`下的所有文件和子目录;
- `need_rebuild(src: string, dst: string?): boolean`: 检查`src`是否需要重新构建, `dst`为`src`的输出文件,
  如果不提供则会根据`src`生成.

