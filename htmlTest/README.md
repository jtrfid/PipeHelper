基于节点的HTML文本解析器的设计和实现
------------------------------------
by Liigo http://blog.csdn.net/liigo/article/details/6153829


众所周知，HTML是结构化文档(Structured Document)，由诸多标签（&lt;p&gt;等）嵌套形成的著名的文档对象模型（DOM, Document Object Model），是显而易见的树形多层次结构。如果带着这种思路看待HTML、编写HTML解析器，无疑将导致问题复杂化。不妨从另一视角俯视HTML文本，视其为一维线状结构：诸多单一节点的顺序排列。仔细审视任何一段HTML文本，以左右尖括号（&lt;和&gt;）为边界，会发现HTML文本被天然地分割为：一个标签（Tag），接一段普通文字，再一个标签，再一段普通文字…… 如下图所示：

![HTML: Tags + Texts](http://hi.csdn.net/attachment/201101/19/11443_1295450955dZ04.png)

标签有两种，开始标签（如&lt;p&gt;）和结束标签（&lt;/p&gt;），它们和普通文字一起，顺序排列，共同构成了HTML文本的全部。

为了再次简化编程模型，我(liigo)继续将“开始标签”“结束标签”“普通文字”三者统一抽象归纳为“节点”（HtmlNode），相应的，“节点”有三种类型，要么是开始标签，要么是结束标签，要么是普通文字。现在，HTML在我们眼里更加单纯了，它就是“节点”的线性顺序组合，是一维的“节点”数组。如下图所示：HTML文本 = 节点1 + 节点2 + 节点3 + …… 

![HTML Nodes(tags/texts)](http://hi.csdn.net/attachment/201101/19/11443_12954566710NLB.png)
