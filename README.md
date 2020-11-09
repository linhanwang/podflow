# podflow

## 设计思路
用一个DAG来代表计算流，图中edge代表数据，node代表一个计算逻辑，这样表达多输入多输出节点比较自然。

edge中存的数据用一个void*表示，node中存的计算逻辑用std::function<void(std::vector<void*>&, std::vector<void*>&)>
表示。第一个std::vector<void*>参数代表输入数据，由节点输入edge带的void*拼凑出来，第二个std::vector<void*>参数
代表输出数据，由节点输出edge带的void*拼凑出来。podflow的用户在构建图的时候通过传入函数来设置计算逻辑，
函数中用户自行static_cast实际的数据类型。

## todo
计算逻辑的抽象感觉大概完成了，师兄可以结合业务检查一下是否可行。库的易用性很堪忧，数据生命期管理也没做，
性能不知道符不符合业务场景的要求等等。因为花了两天时间也没有想到好的易用接口封装方法，先把代码发给师兄检查
计算逻辑的可行性，如果没问题再想易用性等问题。

## 参考
参考了tensorflow 1.13的部分代码，如
tensorflow/core/graph/graph.h，
tensorflow/core/graph/graph.cc,
tensorflow/core/graph/algorithm.h，
tensorflow/core/graph/algorithm.cc。