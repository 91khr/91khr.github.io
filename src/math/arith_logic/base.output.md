---
title: 基本定义
description: 这里的逻辑系统中的一些基本定义
...

这一章包含了senioria在形式化中使用的基本定义, 主要是各种逻辑符号和表达式.

<pre class="Agda"><a id="103" class="Keyword">module</a> <a id="110" href="base.html" class="Module">base</a> <a id="115" class="Keyword">where</a>

<a id="122" class="Keyword">open</a> <a id="127" class="Keyword">import</a> <a id="134" href="Data.Nat.html" class="Module">Data.Nat</a> <a id="143" class="Keyword">using</a> <a id="149" class="Symbol">(</a><a id="150" href="Agda.Builtin.Nat.html#192" class="Datatype">ℕ</a><a id="151" class="Symbol">)</a>
</pre>
首先是基本的逻辑式子的定义, 基本就是定义了一下单个变量, 以及各个逻辑联结词.
之所以使用自然数而非字符串和符号... 是因为符号不会, 而字符串写起来有点过于费力了... (超小声(

<pre class="Agda"><a id="261" class="Keyword">infixr</a> <a id="268" class="Number">20</a> <a id="271" href="base.html#482" class="InductiveConstructor Operator">_⇔_</a>
<a id="275" class="Keyword">infixr</a> <a id="282" class="Number">21</a> <a id="285" href="base.html#455" class="InductiveConstructor Operator">_⇒_</a>
<a id="289" class="Keyword">infixl</a> <a id="296" class="Number">22</a> <a id="299" href="base.html#409" class="InductiveConstructor Operator">_∨_</a>
<a id="303" class="Keyword">infixl</a> <a id="310" class="Number">23</a> <a id="313" href="base.html#382" class="InductiveConstructor Operator">_∧_</a>
<a id="317" class="Keyword">infix</a> <a id="323" class="Number">24</a> <a id="326" href="base.html#436" class="InductiveConstructor Operator">¬_</a>
<a id="329" class="Keyword">infix</a> <a id="335" class="Number">25</a> <a id="338" href="base.html#366" class="InductiveConstructor Operator">`_</a>

<a id="342" class="Keyword">data</a> <a id="Term"></a><a id="347" href="base.html#347" class="Datatype">Term</a> <a id="352" class="Symbol">:</a> <a id="354" href="Agda.Primitive.html#326" class="Primitive">Set</a> <a id="358" class="Keyword">where</a>
  <a id="Term.`_"></a><a id="366" href="base.html#366" class="InductiveConstructor Operator">`_</a> <a id="369" class="Symbol">:</a> <a id="371" href="Agda.Builtin.Nat.html#192" class="Datatype">ℕ</a> <a id="373" class="Symbol">→</a> <a id="375" href="base.html#347" class="Datatype">Term</a>
  <a id="Term._∧_"></a><a id="382" href="base.html#382" class="InductiveConstructor Operator">_∧_</a> <a id="386" class="Symbol">:</a> <a id="388" href="base.html#347" class="Datatype">Term</a> <a id="393" class="Symbol">→</a> <a id="395" href="base.html#347" class="Datatype">Term</a> <a id="400" class="Symbol">→</a> <a id="402" href="base.html#347" class="Datatype">Term</a>
  <a id="Term._∨_"></a><a id="409" href="base.html#409" class="InductiveConstructor Operator">_∨_</a> <a id="413" class="Symbol">:</a> <a id="415" href="base.html#347" class="Datatype">Term</a> <a id="420" class="Symbol">→</a> <a id="422" href="base.html#347" class="Datatype">Term</a> <a id="427" class="Symbol">→</a> <a id="429" href="base.html#347" class="Datatype">Term</a>
  <a id="Term.¬_"></a><a id="436" href="base.html#436" class="InductiveConstructor Operator">¬_</a> <a id="439" class="Symbol">:</a> <a id="441" href="base.html#347" class="Datatype">Term</a> <a id="446" class="Symbol">→</a> <a id="448" href="base.html#347" class="Datatype">Term</a>
  <a id="Term._⇒_"></a><a id="455" href="base.html#455" class="InductiveConstructor Operator">_⇒_</a> <a id="459" class="Symbol">:</a> <a id="461" href="base.html#347" class="Datatype">Term</a> <a id="466" class="Symbol">→</a> <a id="468" href="base.html#347" class="Datatype">Term</a> <a id="473" class="Symbol">→</a> <a id="475" href="base.html#347" class="Datatype">Term</a>
  <a id="Term._⇔_"></a><a id="482" href="base.html#482" class="InductiveConstructor Operator">_⇔_</a> <a id="486" class="Symbol">:</a> <a id="488" href="base.html#347" class="Datatype">Term</a> <a id="493" class="Symbol">→</a> <a id="495" href="base.html#347" class="Datatype">Term</a> <a id="500" class="Symbol">→</a> <a id="502" href="base.html#347" class="Datatype">Term</a>
</pre>
